#pragma once
#include "SFML/Network.hpp"
#include "NetworkMessage.h"
#include <vector>

using Checksum = unsigned int;
using uint32 = unsigned int;
using uint16 = unsigned short;

using std::vector;
using std::string;
using std::optional;

using sf::UdpSocket;
using sf::IpAddress;


class NetworkRole {
public:
	NetworkRole() = default;
	virtual ~NetworkRole() = default;

	virtual bool waitForMessage() = 0;
	virtual void sendMessage(NetworkMessage* message, E::NETWORK_MSG messageType) = 0;

	virtual int countSetBits(const void* pData, size_t sizeofData) = 0;
	virtual Checksum getChecksum(const void* pData, size_t sizeofData) = 0;
	virtual Package getPackage(const void* pData, int sizeofData) = 0;
	virtual bool isPackageValid(const Package& pack, Package* pOutPackage = nullptr, uint16* pOutDataSize = nullptr) = 0;
	virtual bool getPackageTypeAndData(const Package& pack, uint16& msgType, Package& unpackedData) = 0;

private:
};
