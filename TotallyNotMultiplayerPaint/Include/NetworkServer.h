#pragma once
#include "NetworkRole.h"
#include "NetworkMessage.h"
#include <unordered_map>

using std::unordered_map;

struct client {
	string username;
	IpAddress userIp;
};

class NetworkServer : public NetworkRole
{
public:
	NetworkServer() = default;
	virtual ~NetworkServer() = default;

	void waitForMessage() override;
	void sendMessage(NetworkMessage* message, E::NETWORK_MSG messageType) override;

private:
	vector<client> m_userList;
	unordered_map<string, string> m_registeredUsers;

	UdpSocket socket;
};