#pragma once
#include "NetworkRole.h"

class NetworkClient : public NetworkRole
{
public:
	NetworkClient() = default;
	virtual ~NetworkClient() = default;

	void waitForMessage() override;
	void sendMessage(vector<char> data) override;
};