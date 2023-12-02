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

void NetworkServer::syncUser(PackageInformation& packageInfo, Client& messageSender)
{
	Package unpackedData = packageInfo.pack;
	uint16 msgType = packageInfo.msgType;
	uint32 packageID = packageInfo.packageID;

	switch (msgType)
	{
	case E::kCREATE_LINE:
	{
		//saveMessageToSyncList(unpackedData, msgType);
		sendLine(unpackedData, packageID, messageSender, msgType, true);
	}
	break;
	case E::kCREATE_RECTANGLE:
	{
		//saveMessageToSyncList(unpackedData, msgType);
		sendRectangle(unpackedData, packageID, messageSender, msgType, true);
	}
	break;
	case E::kCREATE_CIRCLE:
	{
		//saveMessageToSyncList(unpackedData, msgType);
		sendCircle(unpackedData, packageID, messageSender, msgType, true);
	}
	break;
	case E::kCREATE_FREEDRAW:
	{
		//saveMessageToSyncList(unpackedData, msgType);
		sendFreedraw(unpackedData, packageID, messageSender, msgType, true);
	}
	break;
	}
}

void NetworkServer::signUpUser(const Client& messageSender, const bool& signUp)
{
	std::cout << "Un usuario se esta conectando..." << endl;

	if (clientIsAlreadyConnecting(messageSender))
	{
		MsgDisconnected message;
		sendMessage(&message, E::kDISCONNECTION, messageSender);
		return;
	}
	UnconnectedClient newClient;
	newClient.userIp = messageSender.userIp;
	newClient.userPort = messageSender.userPort;



	newClient.creatingAccount = signUp;
	m_incomingClients.push_back(newClient);

	MsgUsernameRequest message;
	sendMessage(&message, E::kUSERNAME_REQUEST, messageSender);
}

UnconnectedClient* NetworkServer::isInIncomingList(const Client& messageSender)
{
	for (auto& client : m_incomingClients)
	{
		if (client.userIp.value() == messageSender.userIp.value() && client.userPort == messageSender.userPort)
		{
			return &client;
		}
	}
	return nullptr;
}

void NetworkServer::deleteUserInIncomingList(const UnconnectedClient& messageSender)
{
	for (auto it = m_incomingClients.rbegin(); it != m_incomingClients.rend(); ++it)
	{
		auto& cl = *it;
		if (cl.userIp.value() == messageSender.userIp.value() && cl.userPort == messageSender.userPort)
		{
			m_incomingClients.erase((it + 1).base());
			break;
		}
	}
}

void NetworkServer::disconnectUser(const Client& messageSender)
{
	MsgDisconnected message;
	sendMessage(&message, E::kDISCONNECTION, messageSender);
}

void NetworkServer::connectUser(const UnconnectedClient* messageSender)
{
	++m_clientIDCount;
	Client newClient;
	newClient.userIp = messageSender->userIp;
	newClient.userPort = messageSender->userPort;
	newClient.userName = messageSender->userName;
	newClient.clientID = m_clientIDCount;
	m_userList.push_back(newClient);

	MsgConnected message;
	sendMessage(&message, E::kCONNECTION_SUCCESSFUL, newClient);
	cout << "Usuario conectado:" << newClient.userIp.value() << ":" << newClient.userPort << ", ID: " << newClient.clientID << endl;
	return;
}

bool NetworkServer::isUserValid(const UnconnectedClient* messageSender)
{
	for (auto& dict : m_registeredUsers)
	{
		if (dict.first == messageSender->userName && dict.second == messageSender->userPass)
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

void NetworkServer::sendMessageToAllUsers(NetworkMessage* message, E::NETWORK_MSG& typeToSend, const Client& messageSender)
{
	for (auto& client : m_userList)
	{
		if ((client.userIp.value() != messageSender.userIp.value() && client.userPort != messageSender.userPort) ||
			(client.userIp.value() == messageSender.userIp.value() && client.userPort != messageSender.userPort))
		{
			cout << "Sending shape information to user " << client.userIp.value() << ":" << client.userPort << endl;
			sendMessage(message, typeToSend, client);
		}
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

bool NetworkServer::checkForNewMessage()
{
	float timeSinceLastMessage = m_newMessageTimer.getElapsedTime().asSeconds();
	m_newMessageTimer.restart();

	if (timeSinceLastMessage > 1)
	{
		return true;
	}
	return false;
}

void NetworkServer::sendLine(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage)
{
	MsgCreateLine message;
	MsgCreateLine::LineData realData;
	message.unpackData(unpackedData.data(), &realData, unpackedData.size());

	cout << "Shape created by user." << endl;
	E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);
	message.m_msgData = realData;
	message.m_msgData.messageID = packageID;

	if (isSyncMessage)
	{
		sendMessage(&message, typeToSend, messageSender);
	}
	else 
	{
		sendMessageToAllUsers(&message, typeToSend, messageSender);
	}
}

void NetworkServer::sendRectangle(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage)
{
	MsgCreateRectangle message;
	MsgCreateRectangle::RectangleData realData;
	message.unpackData(unpackedData.data(), &realData, unpackedData.size());

	cout << "Shape created by user." << endl;
	E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);
	message.m_msgData = realData;
	message.m_msgData.messageID = packageID;

	if (isSyncMessage)
	{
		sendMessage(&message, typeToSend, messageSender);
	}
	else
	{
		sendMessageToAllUsers(&message, typeToSend, messageSender);
	}
}

void NetworkServer::sendCircle(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage)
{
	MsgCreateCircle message;
	MsgCreateCircle::CircleData realData;
	message.unpackData(unpackedData.data(), &realData, unpackedData.size());

	cout << "Shape created by user." << endl;

	E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);
	message.m_msgData = realData;
	message.m_msgData.messageID = packageID;

	if (isSyncMessage)
	{
		sendMessage(&message, typeToSend, messageSender);
	}
	else
	{
		sendMessageToAllUsers(&message, typeToSend, messageSender);
	}
}

void NetworkServer::sendFreedraw(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage)
{
	MsgCreateFreedraw message;
	MsgCreateFreedraw::FreedrawData realData;
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
		sendMessageToAllUsers(&message, typeToSend, messageSender);
	}
}

uint32 NetworkServer::getClientID(const Client& messageSender)
{
	for (int i = 0; i < m_userList.size(); i++)
	{
		if (messageSender.userIp.value() == m_userList[i].userIp.value() &&
			messageSender.userPort == m_userList[i].userPort)
		{
			return m_userList[i].clientID;
		}
	}
	return 0;
}

string NetworkServer::getClientName(const Client& messageSender)
{
	for (int i = 0; i < m_userList.size(); i++)
	{
		if (messageSender.userIp.value() == m_userList[i].userIp.value() &&
			messageSender.userPort == m_userList[i].userPort)
		{
			return m_userList[i].userName;
		}
	}
	return "Unknown";
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

void NetworkServer::handlePackage(Package& unpackedData, const uint16& msgType, Client& messageSender)
{
	switch (msgType) {
	case E::kLOGIN_REQUEST:
	{
		if (m_registeredUsers.empty())
		{
			disconnectUser(messageSender);
		}

		signUpUser(messageSender, false);
	}
	break;
	case E::kSIGNUP_REQUEST:
	{
		signUpUser(messageSender);
	}
	break;
	case E::kUSERNAME_SENT:
	{
		string username(unpackedData.data());
		username.resize(unpackedData.size());

		UnconnectedClient* pClient = isInIncomingList(messageSender);
		bool usernameExists = findUsername(username);

		if (pClient == nullptr || (usernameExists && pClient->creatingAccount))
		{
			disconnectUser(messageSender);
			return;
		}
		
		cout << "Nombre de usuario elegido por el cliente: " << username << endl;
		pClient->userName = username;

		MsgPasswordRequest message;
		sendMessage(&message, E::kPASSWORD_REQUEST, messageSender);
	}
	break;
	case E::kPASSWORD_SENT:
	{
		string password(unpackedData.data());
		password.resize(unpackedData.size());

		UnconnectedClient* pClient = isInIncomingList(messageSender);

		if (pClient == nullptr)
		{
			disconnectUser(messageSender);
		}

		pClient->userPass = password;

		bool isCreatingAccount = pClient->creatingAccount;

		bool validUser = isUserValid(pClient);
		
		if (!validUser && !isCreatingAccount)
		{
			cout << "No se encontro el usuario o la password" << endl;
			disconnectUser(messageSender);
			deleteUserInIncomingList(*pClient);
			return;
		}

		if (isCreatingAccount)
		{
			cout << "Registro exitoso" << endl;
			m_registeredUsers.insert({ pClient->userName, pClient->userPass });
			connectUser(pClient);
		}
		else
		{
			cout << "Inicio de sesion exitoso" << endl;
			connectUser(pClient);
		}

		deleteUserInIncomingList(*pClient);
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
	case E::kCREATE_LINE:
	{
		uint32 clientID = getClientID(messageSender);
		saveMessageToSyncList(unpackedData, msgType, clientID);
		sendLine(unpackedData, m_messageIDCount, messageSender, msgType, false);
	}
	break;
	case E::kCREATE_RECTANGLE:
	{
		uint32 clientID = getClientID(messageSender);
		saveMessageToSyncList(unpackedData, msgType, clientID);
		sendRectangle(unpackedData, m_messageIDCount, messageSender, msgType, false);
	}
	break;
	case E::kCREATE_CIRCLE:
	{
		uint32 clientID = getClientID(messageSender);
		saveMessageToSyncList(unpackedData, msgType, clientID);
		sendCircle(unpackedData, m_messageIDCount, messageSender, msgType, false);
	}
	break;
	case E::kCREATE_FREEDRAW:
	{
		uint32 clientID = getClientID(messageSender);
		saveMessageToSyncList(unpackedData, msgType, clientID);
		sendFreedraw(unpackedData, m_messageIDCount, messageSender, msgType, false);
	}
	break;
	case E::kSYNC_USER:
	{
		for (int i = 0; i < m_boardSyncStorage.size(); ++i)
		{
			syncUser(m_boardSyncStorage[i], messageSender);
		}
	}
	break;
	case E::kUNDO_MESSAGE:
	{
		uint32 clientID = getClientID(messageSender);
		uint32 removedPackageID = removeLatestPackageFromList(clientID);

		MsgUndo msg(removedPackageID);
		E::NETWORK_MSG typeToSend = E::kUNDO_MESSAGE;
		sendMessageToAllUsers(&msg, typeToSend, messageSender);
	}
	break;
	case E::kCHAT:
	{
		string chatMessage(unpackedData.data());
		chatMessage.resize(unpackedData.size());
		MsgChat msg(getClientName(messageSender), chatMessage);
		E::NETWORK_MSG typeToSend = E::kCHAT;
		sendMessageToAllUsers(&msg, typeToSend, messageSender);
	}
	break;
	default:
	{
		cout << "Tipo de mensaje invalido." << endl;
	}
	}
}
