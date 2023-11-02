#pragma once
#include "SFML/Network.hpp"
#include "NetworkMessage.h"
#include <vector>

using Checksum = unsigned int;
using uint32 = unsigned int;
using uint16 = unsigned short;

using std::vector;
using std::string;

using sf::UdpSocket;
using sf::IpAddress;


class NetworkRole {
public:
	NetworkRole() = default;
	virtual ~NetworkRole() = default;

	virtual void waitForMessage() = 0;
	virtual void sendMessage(NetworkMessage* message) = 0;

	template <typename T>
	int countSetBits(T data);
	int countSetBits(const void* pData, size_t sizeofData);
	Checksum getChecksum(const void* pData, size_t sizeofData);
	Package getPackage(const void* pData, int sizeofData);
	bool isPackageValid(const Package& pack, Package* pOutPackage = nullptr, uint16* pOutDataSize = nullptr);
	bool getPackageTypeAndData(const Package& pack, uint16& msgType, Package& unpackedData);

private:
	UdpSocket socket;
};

template<typename T>
inline int NetworkRole::countSetBits(T data)
{
	int setBits = 0;
	for (int b = 0; b < sizeof(data) * 8; ++b) 
	{
		setBits += (data & (1 << b)) ? 1 : 0;
	}
	return setBits;
}
