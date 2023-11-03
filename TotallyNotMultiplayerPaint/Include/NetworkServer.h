#pragma once
#include "NetworkRole.h"
#include "NetworkMessage.h"
#include <unordered_map>
#include <iostream>

using std::unordered_map;
using std::cout;
using std::endl;

struct UnconnectedClient {
	optional<IpAddress> userIp;
	uint16 userPort;
	string userName;
	string userPass;
	bool creatingAccount;
};

struct Client {
	optional<IpAddress> userIp;
	uint16 userPort;
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

	void handlePackage(Package& unpackedData, const uint16& msgType, const Client& messageSender);
	bool clientIsAlreadyConnecting(const Client& messageSender);

	inline bool isRunning() { return m_isRunning; };

private:
	vector<Client> m_userList;
	vector<UnconnectedClient> m_incomingClients;
	unordered_map<string, string> m_registeredUsers;
	bool m_isRunning;

	UdpSocket socket;

	
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