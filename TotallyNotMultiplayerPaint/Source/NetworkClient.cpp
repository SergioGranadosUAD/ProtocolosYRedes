#include "../Include/NetworkClient.h"

NetworkClient::NetworkClient() :
	m_connected(false),
	m_serverPort(0)
{
	socket.setBlocking(false);
}

bool NetworkClient::waitForMessage()
{
	return false;
}

bool NetworkClient::waitForMessage(Package* messageData, uint16* messageType)
{
	Package incomingPackage;
	incomingPackage.resize(8192);
	size_t bytesReceived;
	optional<IpAddress> sender;
	uint16 senderPort;

	if (socket.receive(incomingPackage.data(), incomingPackage.size(), bytesReceived, sender, senderPort) != sf::Socket::Status::Done)
	{
		return false;
	}

	Package receivedPackage;
	receivedPackage.resize(bytesReceived);
	memcpy(receivedPackage.data(), incomingPackage.data(), bytesReceived);

	Package realPackage;
	if (!isPackageValid(receivedPackage, &realPackage))
	{
		return false;
	}

	//uint16 msgType = E::kERROR;
	//Package unpackedData;

	getPackageTypeAndData(realPackage, *messageType, *messageData);
	return true;
}

void NetworkClient::sendMessage(NetworkMessage* message, E::NETWORK_MSG messageType)
{
	Package connectData;
	switch (messageType) {
	case E::kCONNECT:
	{
		MsgConnect* msgObject = reinterpret_cast<MsgConnect*>(message);
		connectData = msgObject->packData();
	}
		break;
	case E::kDISCONNECTION:
	{
		MsgDisconnected* msgObject = reinterpret_cast<MsgDisconnected*>(message);
		connectData = msgObject->packData();
	}
		break;
	case E::kCREATE_SHAPE:
	{
		MsgCreateShape* msgObject = reinterpret_cast<MsgCreateShape*>(message);
		connectData = msgObject->packData();
	}
		break;
	case E::kUNDO_MESSAGE:
	{
		MsgUndo* msgObject = reinterpret_cast<MsgUndo*>(message);
		connectData = msgObject->packData();
	}
		break;
	case E::kCHAT:
	{
		MsgChat* msgObject = reinterpret_cast<MsgChat*>(message);
		connectData = msgObject->packData();
	}
	break;
	}
	Package finalPackage = getPackage(connectData.data(), connectData.size());

	if (socket.send(finalPackage.data(), finalPackage.size(), m_serverIP.value(), m_serverPort) != sf::Socket::Status::Done) {
		return;
	}
}

int NetworkClient::countSetBits(const void* pData, size_t sizeofData)
{
	int setBits = 0;
	const char* pDataInBytes = reinterpret_cast<const char*>(pData);
	for (int c = 0; c < sizeofData; ++c)
	{
		for (int b = 0; b < 8; ++b)
		{
			setBits += (pDataInBytes[c] & (1 << b)) ? 1 : 0;
		}
	}

	return setBits;
}

Checksum NetworkClient::getChecksum(const void* pData, size_t sizeofData)
{
	static unsigned int firm = 0xAAAA0000;
	int numBitsData = countSetBits(pData, sizeofData);
	int byteCount = countSetBits(sizeofData);
	return numBitsData + byteCount + firm;
}

bool NetworkClient::isPackageValid(const Package& pack, Package* pOutPackage, uint16* pOutDataSize)
{
	if (pack.empty() || pack.size() < (sizeof(uint16) + sizeof(Checksum)))
	{
		return false;
	}

	Checksum packChecksum = 0;
	uint16 dataSize = 0;
	Package packData;

	if (pOutPackage == nullptr)
	{
		pOutPackage = &packData;
	}

	memcpy(&packChecksum, &pack[0], sizeof(packChecksum));
	memcpy(&dataSize, &pack[sizeof(packChecksum)], sizeof(dataSize));

	if (dataSize < 1)
	{
		return false;
	}

	if (pOutDataSize)
	{
		*pOutDataSize = dataSize;
	}

	pOutPackage->resize(dataSize);
	memcpy(pOutPackage->data(), &pack[sizeof(packChecksum) + sizeof(dataSize)], dataSize);

	return(getChecksum(pOutPackage->data(), pOutPackage->size()) == packChecksum);
}

Package NetworkClient::getPackage(const void* pData, int sizeofData)
{
	Package newPackage;
	Checksum packageChecksum = getChecksum(pData, sizeofData);
	uint16 dataSize = sizeofData;

	newPackage.resize(sizeofData + sizeof(uint16) + sizeof(packageChecksum));

	memcpy(&newPackage[0], &packageChecksum, sizeof(packageChecksum));
	memcpy(&newPackage[sizeof(packageChecksum)], &dataSize, sizeof(dataSize));
	memcpy(&newPackage[sizeof(packageChecksum) + sizeof(dataSize)], pData, dataSize);

	return newPackage;
}

bool NetworkClient::getPackageTypeAndData(const Package& pack, uint16& msgType, Package& unpackedData)
{
	if (pack.size() < sizeof(MESSAGE_TYPE_VAR))
	{
		return false;
	}

	memcpy(&msgType, pack.data(), sizeof(MESSAGE_TYPE_VAR));

	if (!(pack.size() > sizeof(MESSAGE_TYPE_VAR)))
	{
		return true;
	}

	auto finalDataSize = pack.size() - sizeof(MESSAGE_TYPE_VAR);
	unpackedData.resize(finalDataSize);
	memcpy(unpackedData.data(), pack.data() + sizeof(MESSAGE_TYPE_VAR), finalDataSize);

	return true;
}

void NetworkClient::setServerIP(string ip)
{
	m_serverIP = IpAddress::resolve(ip);
}

void NetworkClient::setServerPort(unsigned short port)
{
	m_serverPort = port;
}

void NetworkClient::setConnection(bool connectionStatus)
{
	m_connected = connectionStatus;
}
