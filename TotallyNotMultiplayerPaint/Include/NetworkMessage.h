#pragma once
#include <vector>

using std::vector;
using Package = vector<char>;

class NetworkMessage
{
public:
	NetworkMessage() = default;
	virtual ~NetworkMessage() = default;
	virtual Package packData() = 0;
};