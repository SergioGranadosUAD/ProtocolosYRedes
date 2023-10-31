#include "../Include/NetworkRole.h"


int NetworkRole::countSetBits(const void* pData, size_t sizeofData)
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

Checksum NetworkRole::getChecksum(const void* pData, size_t sizeofData)
{
	static unsigned int firm = 0xAAAA0000;
	int numBitsData = countSetBits(pData, sizeofData);
	int byteCount = countSetBits(sizeofData);
}

Package NetworkRole::getPackage(const void* pData, int sizeofData)
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

bool NetworkRole::isPackageValid(const Package& pack)
{
	if (pack.empty() || pack.size() < (sizeof(uint16) + sizeof(Checksum)))
	{
		return false;
	}

	Checksum packChecksum = 0;
	uint16 dataSize = 0;
	Package packData;

	memcpy(&packChecksum, &pack[0], sizeof(packChecksum));
	memcpy(&dataSize, &pack[sizeof(packChecksum)], sizeof(dataSize));

	if (dataSize < 1)
	{
		return false;
	}

	packData.resize(dataSize);
	memcpy(&packData[0], &pack[sizeof(packChecksum) + sizeof(dataSize)], dataSize);

	return(getChecksum(packData.data(), packData.size()) == packChecksum);
}
