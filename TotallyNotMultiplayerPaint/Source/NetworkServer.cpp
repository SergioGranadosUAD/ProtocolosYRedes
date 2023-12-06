#include "../Include/NetworkServer.h"

#define DEFAULT_PORT 50001

NetworkServer::NetworkServer() :
	m_isRunning(true)
{
	socket.setBlocking(false);
	socket.bind(DEFAULT_PORT);
	m_pingCooldown.restart();
}

bool NetworkServer::waitForMessage()
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
	case E::kPING:
	{
		MsgPing* msgObject = reinterpret_cast<MsgPing*>(message);
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

Checksum NetworkServer::getChecksum(const void* pData, int sizeofData)
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

void NetworkServer::syncUser(const Client& messageSender)
{
	for (int i = 0; i < m_boardSyncStorage.size(); ++i)
	{
		PackageInformation packageInfo = m_boardSyncStorage[i];

		Package unpackedData = packageInfo.pack;
		uint16 msgType = packageInfo.msgType;
		uint32 packageID = packageInfo.packageID;

		sendShape(unpackedData, packageID, messageSender, msgType, true);
	}
}

void NetworkServer::disconnectUser(const Client& messageSender)
{
	removeUserFromList(messageSender);
	MsgDisconnected message;
	sendMessage(&message, E::kDISCONNECTION, messageSender);
}

void NetworkServer::connectUser(const string& username, const string& password, const Client& messageSender)
{
	++m_clientIDCount;
	Client newClient;
	newClient.userIp = messageSender.userIp;
	newClient.userPort = messageSender.userPort;
	newClient.userName = username;
	newClient.userPass = password;
	newClient.clientID = m_clientIDCount;
	newClient.pingTimer.restart();
	m_userList.push_back(newClient);

	MsgConnect message;
	sendMessage(&message, E::kCONNECT, newClient);
	cout << "Usuario conectado:" << newClient.userIp.value() << ":" << newClient.userPort << ", ID: " << newClient.clientID << endl;

	syncUser(messageSender);
	return;
}

bool NetworkServer::isUserValid(const string& username, const string& password)
{
	for (auto& dict : m_registeredUsers)
	{
		if (dict.first == username && dict.second == password)
		{
			return true;
		}
	}

	return false;
}

bool NetworkServer::findUsername(const string& username)
{
	for (auto& dict : m_registeredUsers)
	{
		if (dict.first == username)
		{
			return true;
		}
	}

	return false;
}

void NetworkServer::sendMessageToAllUsers(NetworkMessage* message, E::NETWORK_MSG& typeToSend)
{
	for (auto& client : m_userList)
	{
		sendMessage(message, typeToSend, client);
	}
}

void NetworkServer::saveMessageToSyncList(const Package& unpackedData, const uint16& msgType, const uint32& userID)
{
	++m_messageIDCount;
	PackageInformation dataToStore;
	dataToStore.pack = unpackedData;
	dataToStore.msgType = msgType;
	dataToStore.packageID = m_messageIDCount;
	dataToStore.senderID = userID;
	m_boardSyncStorage.push_back(dataToStore);
}

bool NetworkServer::sendPing()
{
	MsgPing msg;
	E::NETWORK_MSG msgType = E::kPING;
	sendMessageToAllUsers(&msg, msgType);
	m_pingCooldown.restart();
}

void NetworkServer::checkForTimeout()
{
	for (auto& client : m_userList)
	{
		if (client.pingTimer.getElapsedTime().asMilliseconds() > 10000)
		{
			cout << "User timed out" << endl;
			disconnectUser(client);
		}
	}
}

void NetworkServer::sendShape(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage)
{
	MsgCreateShape message;
	MsgCreateShape::ShapeData realData;
	message.unpackData(unpackedData.data(), &realData, unpackedData.size());

	E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);
	message.m_msgData = realData;
	message.m_msgData.messageID = packageID;

	if (isSyncMessage)
	{
		sendMessage(&message, typeToSend, messageSender);
	}
	else 
	{
		sendMessageToAllUsers(&message, typeToSend);
	}
}

Client* NetworkServer::getClientData(const Client& messageSender)
{
	for (int i = 0; i < m_userList.size(); i++)
	{
		if (messageSender.userIp.value() == m_userList[i].userIp.value() &&
			messageSender.userPort == m_userList[i].userPort)
		{
			return &m_userList[i];
		}
	}
	return 0;
}

uint32 NetworkServer::removeLatestPackageFromList(const uint32& clientID)
{
	uint32 packageID = 0;
	for (auto it = m_boardSyncStorage.rbegin(); it != m_boardSyncStorage.rend(); ++it)
	{
		auto& cl = *it;
		if (cl.senderID == clientID)
		{
			packageID = cl.packageID;
			m_boardSyncStorage.erase((it + 1).base());
			break;
		}
	}
	return packageID;
}

void NetworkServer::removeUserFromList(const Client& messageSender)
{
	for (auto it = m_userList.rbegin(); it != m_userList.rend(); ++it)
	{
		auto& cl = *it;
		if (cl.userIp.value() == messageSender.userIp.value() &&
			cl.userPort == messageSender.userPort)
		{
			m_userList.erase((it + 1).base());
			break;
		}
	}
}

void NetworkServer::handlePackage(Package& unpackedData, const uint16& msgType, Client& messageSender)
{
	switch (msgType) {
	case E::kCONNECT:
	{
		connectionRequest(unpackedData, messageSender);
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
				m_userList.erase(m_userList.begin() + i);
			}
		}
	}
	break;
	case E::kCREATE_SHAPE:
	{
		uint32 clientID = getClientData(messageSender)->clientID;
		saveMessageToSyncList(unpackedData, msgType, clientID);
		sendShape(unpackedData, m_messageIDCount, messageSender, msgType, false);
	}
	break;
	case E::kUNDO_MESSAGE:
	{
		uint32 clientID = getClientData(messageSender)->clientID;
		uint32 removedPackageID = removeLatestPackageFromList(clientID);

		MsgUndo msg(removedPackageID);
		E::NETWORK_MSG typeToSend = E::kUNDO_MESSAGE;
		sendMessageToAllUsers(&msg, typeToSend);
	}
	break;
	case E::kCHAT:
	{
		string chatMessage(unpackedData.data());
		chatMessage.resize(unpackedData.size());
		MsgChat msg(getClientData(messageSender)->userName, chatMessage);
		E::NETWORK_MSG typeToSend = E::kCHAT;
		sendMessageToAllUsers(&msg, typeToSend);
	}
	break;
	case E::kPING:
	{
		getClientData(messageSender)->pingTimer.restart();
	}
	break;
	default:
	{
		cout << "Tipo de mensaje invalido." << endl;
	}
	}
}

void NetworkServer::connectionRequest(Package& unpackedData, Client& messageSender)
{
	string realData;
	realData.insert(realData.begin(), unpackedData.begin(), unpackedData.end());
	stringstream sstream(realData);

	string loginMode;
	string username;
	string password;

	sstream >> loginMode >> username >> password;

	if (loginMode == "l")
	{
		loginUser(username, password, messageSender);
	}
	else if (loginMode == "s")
	{
		registerUser(username, password, messageSender);
	}
	else
	{
		disconnectUser(messageSender);
	}

	
}

void NetworkServer::loginUser(const string& username, const string& password, Client& messageSender)
{
	if (isUserValid(username, password))
	{
		connectUser(username, password, messageSender);
	}
	else
	{
		disconnectUser(messageSender);
	}
}

void NetworkServer::registerUser(const string& username, const string& password, Client& messageSender)
{
	if (!findUsername(username))
	{
		m_registeredUsers.insert({ username, password });
		connectUser(username, password, messageSender);
	}
	else
	{
		disconnectUser(messageSender);
	}
}
