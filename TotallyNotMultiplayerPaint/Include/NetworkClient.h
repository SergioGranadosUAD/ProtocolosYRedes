#pragma once
#include "NetworkRole.h"

class NetworkClient : public NetworkRole
{
public:
	NetworkClient();
	virtual ~NetworkClient() = default;

	bool waitForMessage() override;
	bool waitForMessage(Package* messageData, uint16* messageType);
	void sendMessage(NetworkMessage* message, E::NETWORK_MSG messageType) override;


	template <typename T>
	int countSetBits(T data);
	int countSetBits(const void* pData, size_t sizeofData) override;
	Checksum getChecksum(const void* pData, size_t sizeofData) override;
	Package getPackage(const void* pData, int sizeofData) override;
	bool isPackageValid(const Package& pack, Package* pOutPackage = nullptr, uint16* pOutDataSize = nullptr) override;
	bool getPackageTypeAndData(const Package& pack, uint16& msgType, Package& unpackedData) override;

	void setServerIP(string ip);
	void setServerPort(unsigned short port);
	void setConnection(bool connectionStatus);
	inline bool isConnected() { return m_connected; };
	
private:
	optional<IpAddress> m_serverIP;
	uint16 m_serverPort;
	bool m_connected;

	UdpSocket socket;
};

template<typename T>
inline int NetworkClient::countSetBits(T data)
{
	int setBits = 0;
	for (int b = 0; b < sizeof(data) * 8; ++b)
	{
		setBits += (data & (1 << b)) ? 1 : 0;
	}
	return setBits;
}