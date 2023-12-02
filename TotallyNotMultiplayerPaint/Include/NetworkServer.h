#pragma once
#include "NetworkRole.h"
#include "NetworkMessage.h"
#include <unordered_map>
#include <iostream>

using std::unordered_map;
using std::cout;
using std::endl;
using sf::Clock;

struct UnconnectedClient 
{
	optional<IpAddress> userIp;
	uint16 userPort;
	string userName;
	string userPass;
	bool creatingAccount;
};

struct Client 
{
	optional<IpAddress> userIp;
	uint16 userPort;
	string userName;
	uint32 clientID;
};

struct PackageInformation 
{
	Package pack;
	uint16 msgType;
	uint32 packageID;
	uint32 senderID;
};

class NetworkServer : public NetworkRole
{
public:
	NetworkServer();
	virtual ~NetworkServer() = default;

	bool waitForMessage() override;
	void sendMessage(NetworkMessage* message, E::NETWORK_MSG messageType) override;
	void sendMessage(NetworkMessage* message, E::NETWORK_MSG messageType, const Client& destination);

	template <typename T>
	int countSetBits(T data);
	int countSetBits(const void* pData, size_t sizeofData) override;
	Checksum getChecksum(const void* pData, size_t sizeofData) override;
	Package getPackage(const void* pData, int sizeofData) override;
	bool isPackageValid(const Package& pack, Package* pOutPackage = nullptr, uint16* pOutDataSize = nullptr) override;
	bool getPackageTypeAndData(const Package& pack, uint16& msgType, Package& unpackedData) override;

	void handlePackage(Package& unpackedData, const uint16& msgType, Client& messageSender);
	bool clientIsAlreadyConnecting(const Client& messageSender);
	void syncUser(PackageInformation& packageInfo, Client& messageSender);
	void signUpUser(const Client& messageSender, const bool& signUp = true);
	UnconnectedClient* isInIncomingList(const Client& messageSender);
	void deleteUserInIncomingList(const UnconnectedClient& messageSender);
	void disconnectUser(const Client& messageSender);
	void connectUser(const UnconnectedClient* messageSender);
	bool isUserValid(const UnconnectedClient* messageSender);
	bool findUsername(const string& username);
	void sendMessageToAllUsers(NetworkMessage* message, E::NETWORK_MSG& typeToSend, const Client& messageSender);
	void saveMessageToSyncList(const Package& unpackedData, const uint16& msgType, const uint32& userID);
	bool checkForNewMessage();
	void sendLine(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage);
	void sendRectangle(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage);
	void sendCircle(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage);
	void sendFreedraw(Package& unpackedData, const uint32& packageID, const Client& messageSender, const uint16& msgType, bool isSyncMessage);
	uint32 getClientID(const Client& messageSender);
	string getClientName(const Client& messageSender);
	uint32 removeLatestPackageFromList(const uint32& clientID);

	inline bool isRunning() { return m_isRunning; };

private:
	vector<Client> m_userList;
	vector<UnconnectedClient> m_incomingClients;
	vector<PackageInformation> m_boardSyncStorage;
	unordered_map<string, string> m_registeredUsers;
	bool m_isRunning;

	UdpSocket socket;

	Clock m_newMessageTimer;
	uint32 m_messageIDCount = 0;
	uint32 m_clientIDCount = 0;
};

template<typename T>
inline int NetworkServer::countSetBits(T data)
{
	int setBits = 0;
	for (int b = 0; b < sizeof(data) * 8; ++b)
	{
		setBits += (data & (1 << b)) ? 1 : 0;
	}
	return setBits;
}