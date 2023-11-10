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
	switch (packageInfo.msgType)
	{
	case E::kCREATE_LINE:
	{
		MsgCreateLine m;
		MsgCreateLine::LineData realData = *(reinterpret_cast<MsgCreateLine::LineData*>(packageInfo.pack.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		cout << "Shape created by user." << endl;
		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(packageInfo.msgType);

		MsgCreateLine message(realData.initialPosX,
			realData.initialPosY,
			realData.finalPosX,
			realData.finalPosY,
			realData.colorID);

		cout << "Sending shape information to user " << messageSender.userIp.value() << ":" << messageSender.userPort << endl;
		sendMessage(&message, typeToSend, messageSender);
	}
	break;
	case E::kCREATE_RECTANGLE:
	{
		MsgCreateRectangle m;
		MsgCreateRectangle::RectangleData realData = *(reinterpret_cast<MsgCreateRectangle::RectangleData*>(packageInfo.pack.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		cout << "Shape created by user." << endl;
		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(packageInfo.msgType);
		MsgCreateRectangle message(realData.initialPosX,
			realData.initialPosY,
			realData.finalPosX,
			realData.finalPosY,
			realData.colorID);
		
		cout << "Sending shape information to user " << messageSender.userIp.value() << ":" << messageSender.userPort << endl;
		sendMessage(&message, typeToSend, messageSender);
	}
	break;
	case E::kCREATE_CIRCLE:
	{
		MsgCreateCircle m;
		MsgCreateCircle::CircleData realData = *(reinterpret_cast<MsgCreateCircle::CircleData*>(packageInfo.pack.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		cout << "Shape created by user." << endl;
		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(packageInfo.msgType);
		MsgCreateCircle message(realData.initialPosX,
			realData.initialPosY,
			realData.finalPosX,
			realData.finalPosY,
			realData.colorID);
		
		cout << "Sending shape information to user " << messageSender.userIp.value() << ":" << messageSender.userPort << endl;
		sendMessage(&message, typeToSend, messageSender);
	}
	break;
	case E::kCREATE_FREEDRAW:
	{
		MsgCreateFreedraw m;
		MsgCreateFreedraw::FreedrawData realData;

		memcpy(&realData.colorID, &packageInfo.pack[0], sizeof(unsigned short));
		memcpy(&realData.vectorSize, &packageInfo.pack[sizeof(unsigned short)], sizeof(unsigned int));
		realData.pointPositions.resize(realData.vectorSize);
		memcpy(realData.pointPositions.data(), &packageInfo.pack[sizeof(unsigned short) + sizeof(unsigned int)], sizeof(float) * realData.vectorSize);

		cout << "Shape created by user." << endl;
		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(packageInfo.msgType);
		MsgCreateFreedraw message(realData.colorID, realData.vectorSize, realData.pointPositions);

		cout << "Sending shape information to user: " << messageSender.userIp.value() << messageSender.userPort << endl;
		sendMessage(&message, typeToSend, messageSender);
	}
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
	Client newClient;
	newClient.userIp = messageSender->userIp;
	newClient.userPort = messageSender->userPort;
	m_userList.push_back(newClient);

	MsgConnected message;
	sendMessage(&message, E::kCONNECTION_SUCCESSFUL, newClient);
	cout << "Usuario conectado:" << newClient.userIp.value() << ":" << newClient.userPort << endl;
	return;
}

bool NetworkServer::isUserValid(const UnconnectedClient* messageSender)
{
	for (auto dict : m_registeredUsers)
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
	for (auto dict : m_registeredUsers)
	{
		if (dict.first == username)
		{
			return true;
		}
	}

	return false;
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
		PackageInformation dataToStore;
		dataToStore.pack = unpackedData;
		dataToStore.msgType = msgType;
		m_boardSyncStorage.push_back(dataToStore);

		MsgCreateLine m;
		MsgCreateLine::LineData realData = *(reinterpret_cast<MsgCreateLine::LineData*>(unpackedData.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		cout << "Shape created by user." << endl;
		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);

		MsgCreateLine message(realData.initialPosX,
			realData.initialPosY,
			realData.finalPosX,
			realData.finalPosY,
			realData.colorID);
		for (auto& client : m_userList)
		{
			if ((client.userIp.value() != messageSender.userIp.value() && client.userPort != messageSender.userPort) ||
				(client.userIp.value() == messageSender.userIp.value() && client.userPort != messageSender.userPort))
			{
				cout << "Sending shape information to user " << client.userIp.value() << ":" << client.userPort << endl;
				sendMessage(&message, typeToSend, client);
			}
		}
	}
	break;
	case E::kCREATE_RECTANGLE:
	{
		PackageInformation dataToStore;
		dataToStore.pack = unpackedData;
		dataToStore.msgType = msgType;
		m_boardSyncStorage.push_back(dataToStore);

		MsgCreateRectangle m;
		MsgCreateRectangle::RectangleData realData = *(reinterpret_cast<MsgCreateRectangle::RectangleData*>(unpackedData.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		cout << "Shape created by user." << endl;
		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);
		MsgCreateRectangle message(realData.initialPosX,
			realData.initialPosY,
			realData.finalPosX,
			realData.finalPosY,
			realData.colorID);
		for (auto& client : m_userList)
		{
			if ((client.userIp.value() != messageSender.userIp.value() && client.userPort != messageSender.userPort) ||
				(client.userIp.value() == messageSender.userIp.value() && client.userPort != messageSender.userPort))
			{
				cout << "Sending shape information to user " << client.userIp.value() << ":" << client.userPort << endl;
				sendMessage(&message, typeToSend, client);
			}
		}
	}
	break;
	case E::kCREATE_CIRCLE:
	{
		PackageInformation dataToStore;
		dataToStore.pack = unpackedData;
		dataToStore.msgType = msgType;
		m_boardSyncStorage.push_back(dataToStore);

		MsgCreateCircle m;
		MsgCreateCircle::CircleData realData = *(reinterpret_cast<MsgCreateCircle::CircleData*>(unpackedData.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		cout << "Shape created by user." << endl;

		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);
		MsgCreateCircle message(realData.initialPosX,
			realData.initialPosY,
			realData.finalPosX,
			realData.finalPosY,
			realData.colorID);
		for (auto& client : m_userList)
		{
			if ((client.userIp.value() != messageSender.userIp.value() && client.userPort != messageSender.userPort) ||
				(client.userIp.value() == messageSender.userIp.value() && client.userPort != messageSender.userPort))
			{
				cout << "Sending shape information to user " << client.userIp.value() << client.userPort << endl;
				sendMessage(&message, typeToSend, client);
			}
		}
	}
	break;
	case E::kCREATE_FREEDRAW:
	{
		PackageInformation dataToStore;
		dataToStore.pack = unpackedData;
		dataToStore.msgType = msgType;
		m_boardSyncStorage.push_back(dataToStore);

		MsgCreateFreedraw m;
		MsgCreateFreedraw::FreedrawData realData;

		memcpy(&realData.colorID, &unpackedData[0], sizeof(unsigned short));
		memcpy(&realData.vectorSize, &unpackedData[sizeof(unsigned short)], sizeof(unsigned int));
		realData.pointPositions.resize(realData.vectorSize);
		memcpy(realData.pointPositions.data(), &unpackedData[sizeof(unsigned short) + sizeof(unsigned int)], sizeof(float)* realData.vectorSize);

		cout << "Shape created by user." << endl;
		E::NETWORK_MSG typeToSend = static_cast<E::NETWORK_MSG>(msgType);
		MsgCreateFreedraw message(realData.colorID, realData.vectorSize, realData.pointPositions);
		for (auto& client : m_userList)
		{
			if ((client.userIp.value() != messageSender.userIp.value() && client.userPort != messageSender.userPort) ||
				(client.userIp.value() == messageSender.userIp.value() && client.userPort != messageSender.userPort))
			{
				cout << "Sending shape information to user: " << client.userIp.value() << client.userPort << endl;
				sendMessage(&message, typeToSend, client);
			}
		}
	}
	break;
	case E::kSYNC_USER:
	{
		for (auto& package : m_boardSyncStorage)
		{
			cout << "Syncing user" << endl;
			syncUser(package, messageSender);
		}
	}
	break;
	default:
	{
		cout << "Tipo de mensaje invalido." << endl;
	}
	}
}
