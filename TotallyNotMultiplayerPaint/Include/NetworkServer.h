#pragma once
#include "NetworkRole.h"
#include "NetworkMessage.h"
#include <unordered_map>
#include <iostream>
#include <sstream>

using std::unordered_map;
using std::stringstream;
using std::cout;
using std::endl;
using sf::Clock;

struct Client 
{
	optional<IpAddress> userIp;
	uint16 userPort;
	string userName;
	string userPass;
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
	void connectionRequest(Package& unpackedData, Client& messageSender);
	void loginUser(const string& username, const string& password, Client& messageSender);
	void registerUser(const string& username, const string& password, Client& messageSender);
	void syncUser(PackageInformation& packageInfo, Client& messageSender);
	void disconnectUser(const Client& messageSender);
	void connectUser(const string& username, const string& password, const Client& messageSender);
	bool isUserValid(const string& username, const string& password);
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