#include "../Include/NetworkServer.h"

#define DEFAULT_PORT 25565

NetworkServer::NetworkServer() :
	m_isRunning(true)
{
	socket.setBlocking(false);
	socket.bind(DEFAULT_PORT);
}

bool NetworkServer::waitForMessage()
{
	Package incomingPackage;
	incomingPackage.resize(2048);
	size_t bytesReceived;
	optional<IpAddress> sender;
	uint16 senderPort;

	if (socket.receive(incomingPackage.data(), incomingPackage.size(), bytesReceived, sender, senderPort) != sf::Socket::Status::Done)
	{
		return false;
	}

	Client messageSender;
	messageSender.userIp = sender;
	messageSender.userPort = senderPort;

	Package receivedPackage;
	receivedPackage.resize(bytesReceived);
	memcpy(receivedPackage.data(), incomingPackage.data(), bytesReceived);

	Package realPackage;
	if (!isPackageValid(receivedPackage, &realPackage))
	{
		return false;
	}

	uint16 msgType = E::kERROR;
	Package unpackedData;

	getPackageTypeAndData(realPackage, msgType, unpackedData);

	handlePackage(unpackedData, msgType, messageSender);
	return true;
}

void NetworkServer::sendMessage(NetworkMessage* message, E::NETWORK_MSG messageType)
{
}

void NetworkServer::sendMessage(NetworkMessage* message, E::NETWORK_MSG messageType, const Client& destination)
{
	Package connectData;
	switch (messageType) {
	case E::kLOGIN_REQUEST:
	{
		MsgConnectRequest* msgObject = reinterpret_cast<MsgConnectRequest*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kUSERNAME_REQUEST:
	{
		MsgUsernameRequest* msgObject = reinterpret_cast<MsgUsernameRequest*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kUSERNAME_SENT:
	{
		MsgUsernameSent* msgObject = reinterpret_cast<MsgUsernameSent*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kPASSWORD_REQUEST:
	{
		MsgPasswordRequest* msgObject = reinterpret_cast<MsgPasswordRequest*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kPASSWORD_SENT:
	{
		MsgPasswordSent* msgObject = reinterpret_cast<MsgPasswordSent*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kCONNECTION_SUCCESSFUL:
	{
		MsgConnected* msgObject = reinterpret_cast<MsgConnected*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kDISCONNECTION:
	{
		MsgDisconnected* msgObject = reinterpret_cast<MsgDisconnected*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kCREATE_LINE:
	{
		MsgCreateLine* msgObject = reinterpret_cast<MsgCreateLine*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kCREATE_CIRCLE:
	{
		MsgCreateCircle* msgObject = reinterpret_cast<MsgCreateCircle*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kCREATE_RECTANGLE:
	{
		MsgCreateRectangle* msgObject = reinterpret_cast<MsgCreateRectangle*>(message);
		connectData = msgObject->packData();
	}
	break;
	case E::kCREATE_FREEDRAW:
	{
		MsgCreateFreedraw* msgObject = reinterpret_cast<MsgCreateFreedraw*>(message);
		connectData = msgObject->packData();
	}
	break;
	}
	Package finalPackage = getPackage(connectData.data(), connectData.size());

	if (socket.send(finalPackage.data(), finalPackage.size(), destination.userIp.value(), destination.userPort) != sf::Socket::Status::Done) {
		return;
	}
}

int NetworkServer::countSetBits(const void* pData, size_t sizeofData)
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

Checksum NetworkServer::getChecksum(const void* pData, size_t sizeofData)
{
	static unsigned int firm = 0xAAAA0000;
	int numBitsData = countSetBits(pData, sizeofData);
	int byteCount = countSetBits(sizeofData);
	return numBitsData + byteCount + firm;
}

bool NetworkServer::isPackageValid(const Package& pack, Package* pOutPackage, uint16* pOutDataSize)
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

Package NetworkServer::getPackage(const void* pData, int sizeofData)
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

bool NetworkServer::getPackageTypeAndData(const Package& pack, uint16& msgType, Package& unpackedData)
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

bool NetworkServer::clientIsAlreadyConnecting(const Client& messageSender)
{
	for (auto& client : m_incomingClients) {
		if (messageSender.userIp.value() == client.userIp.value() && messageSender.userPort == client.userPort)
		{
			return true;
		}
	}
	return false;
}

void NetworkServer::handlePackage(Package& unpackedData, const uint16& msgType, const Client& messageSender)
{
	switch (msgType) {
	case E::kLOGIN_REQUEST:
	{
		if (clientIsAlreadyConnecting(messageSender))
		{
			MsgDisconnected message;
			sendMessage(&message, E::kDISCONNECTION, messageSender);
			return;
		}
		UnconnectedClient newClient;
		newClient.userIp = messageSender.userIp;
		newClient.userPort = messageSender.userPort;
		newClient.creatingAccount = false;
		m_incomingClients.push_back(newClient);

		MsgUsernameRequest message;
		sendMessage(&message, E::kUSERNAME_REQUEST, messageSender);
	}
	break;
	case E::kSIGNUP_REQUEST:
	{
		if (clientIsAlreadyConnecting(messageSender))
		{
			MsgDisconnected message;
			sendMessage(&message, E::kDISCONNECTION, messageSender);
			return;
		}
		UnconnectedClient newClient;
		newClient.userIp = messageSender.userIp;
		newClient.userPort = messageSender.userPort;
		newClient.creatingAccount = true;
		m_incomingClients.push_back(newClient);

		MsgUsernameRequest message;
		sendMessage(&message, E::kUSERNAME_REQUEST, messageSender);
	}
	break;
	case E::kUSERNAME_SENT:
	{
		string username;
		MsgUsernameSent msg;
		msg.unpackData(&username, unpackedData.data(), unpackedData.size());

		for (auto& client : m_incomingClients)
		{
			if (client.userIp.value() == messageSender.userIp.value() && client.userPort == messageSender.userPort)
			{
				client.userName = username;
			}
		}
	}
	break;
	case E::kPASSWORD_SENT:
	{
		string password;
		MsgPasswordSent msg;
		msg.unpackData(&password, unpackedData.data(), unpackedData.size());

		UnconnectedClient clientReference;

		for (int i = 0; i < m_incomingClients.size(); ++i)
		{
			if (m_incomingClients[i].userIp.value() == messageSender.userIp.value() && m_incomingClients[i].userPort == messageSender.userPort)
			{
				m_incomingClients[i].userPass = password;
				clientReference = m_incomingClients[i];
				m_incomingClients.erase(m_incomingClients.begin() + i);
				break;
			}
		}


		bool isCreatingAccount = clientReference.creatingAccount;

		for (auto dict : m_registeredUsers)
		{
			if (dict.first == clientReference.userName && dict.second == clientReference.userPass)
			{
				if (!isCreatingAccount)
				{
					Client newClient;
					newClient.userIp = clientReference.userIp;
					newClient.userPort = clientReference.userPort;
					m_userList.push_back(newClient);

					MsgConnected message;
					sendMessage(&message, E::kCONNECTION_SUCCESSFUL);
				}
				else
				{
					MsgDisconnected message;
					sendMessage(&message, E::kDISCONNECTION);
				}
				return;
			}
		}

		m_registeredUsers.insert({ clientReference.userName, clientReference.userPass });
		Client newClient;
		newClient.userIp = clientReference.userIp;
		newClient.userPort = clientReference.userPort;
		m_userList.push_back(newClient);

		MsgConnected message;
		sendMessage(&message, E::kCONNECTION_SUCCESSFUL);
	}
	break;
	case E::kDISCONNECTION:
	{
		for (int i = 0; i < m_userList.size(); i++)
		{
			if (messageSender.userIp.value() == m_userList[i].userIp.value() &&
				messageSender.userPort == m_userList[i].userPort)
			{
				cout << "Usuario desconectado: " << messageSender.userIp.value().toString() << ":" << messageSender.userPort << endl;
				m_userList.erase(m_userList.begin()+i);
			}
		}
	}
	break;
	case E::kCREATE_LINE:
	case E::kCREATE_RECTANGLE:
	case E::kCREATE_CIRCLE:
	case E::kCREATE_FREEDRAW:
	{
		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);
		MsgCreateLine message(unpackedData);
		for (auto& client : m_userList) 
		{
			sendMessage(&message, typeToSend, client);
		}
	}
	break;
	default:
	{
		cout << "Tipo de mensaje invalido." << endl;
	}
}
