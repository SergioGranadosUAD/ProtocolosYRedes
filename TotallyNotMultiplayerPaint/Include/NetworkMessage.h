#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

#define MESSAGE_TYPE_VAR unsigned short

namespace E {
	enum NETWORK_MSG {
		kERROR = 0,
		kPING,
		kCONNECT, // Done
		kDISCONNECTION, // Done
		kCHAT,
		kCREATE_SHAPE, // Done
		kUNDO_MESSAGE,
		
	};

	enum SHAPE_TYPE {
		kLINE = 0,
		kRECTANGLE,
		kCIRCLE,
		kFREEDRAW
	};
}

using std::string;
using std::vector;
using std::to_string;
using sf::Color;
using Package = vector<char>;

class NetworkMessage
{
public:
	NetworkMessage() = default;
	virtual ~NetworkMessage() = default;
	virtual Package packData() = 0;
};

class MsgConnect : public NetworkMessage
{
public:
	MsgConnect()
	{
		m_msgData = "CONNECTED";
	}
	MsgConnect(string loginMode, string username, string password)
	{
		m_msgData = loginMode + " " + username + " " + password;
	}

	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kCONNECT;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	static bool unpackData(void* pSrcData, void* pDestData, size_t numBytes)
	{
		if (numBytes != sizeof(m_msgData))
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);

		return true;
	}

public:
	string m_msgData;
};

class MsgDisconnected : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kDISCONNECTION;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes)
	{
		if (numBytes != m_msgData.size())
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
		return true;
	}

public:
	const string m_msgData = "DISCCONNECTED";
};

class MsgCreateShape : public NetworkMessage
{
public:
	MsgCreateShape() = default;
	MsgCreateShape(Package data) {
		unpackData(&m_msgData, data.data(), data.size());
	};
	MsgCreateShape(E::SHAPE_TYPE shapeType, float iniPosX, float iniPosY, float finPosX, float finPosY, Color colorID, unsigned int messageID = 0)
	{
		m_msgData.shapeType = shapeType;
		m_msgData.initialPosX = iniPosX;
		m_msgData.initialPosY = iniPosY;
		m_msgData.finalPosX = finPosX;
		m_msgData.finalPosY = finPosY;
		m_msgData.colorID = colorID;
		m_msgData.messageID = messageID;
	};
	virtual ~MsgCreateShape() = default;

	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kCREATE_SHAPE;
		Package data;
		data.resize(sizeof(m_msgData) + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), &m_msgData, sizeof(m_msgData));
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes)
	{
		if (numBytes != sizeof(m_msgData))
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
		return true;
	}

public:
	struct ShapeData
	{
		E::SHAPE_TYPE shapeType;
		float initialPosX;
		float initialPosY;
		float finalPosX;
		float finalPosY;
		Color colorID;
		unsigned int messageID;
		unsigned short senderID = 0;
	} m_msgData;
};

class MsgUndo : public NetworkMessage
{
public:
	MsgUndo()
	{
		m_msgData = 0;
	}
	MsgUndo(unsigned int packageID)
	{
		m_msgData = packageID;
	}
	
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kUNDO_MESSAGE;
		Package data;
		data.resize(sizeof(m_msgData) + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), &m_msgData, sizeof(m_msgData));
		return data;
	}

	static bool unpackData(void* pSrcData, void* pDestData, size_t numBytes)
	{
		if (numBytes != sizeof(m_msgData))
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
		return true;
	}

public:
	unsigned int m_msgData;
	
};

class MsgChat : public NetworkMessage
{
public:
	MsgChat() = default;
	MsgChat(string message)
	{
		m_msgData = message;
	}
	MsgChat(string senderName, string message)
	{
		m_msgData = "<" + senderName + "> " + message;
	}

	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kCHAT;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes)
	{
		string test(static_cast<char*>(pDestData));
		test.resize(numBytes);
		if (numBytes < sizeof(char))
		{
			return false;
		}

		//memcpy(pDestData, pSrcData, numBytes);
		pSrcData = &test;
		return true;
	}

public:
	string m_msgData;
};

class MsgPing : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kPING;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes)
	{
		if (numBytes != m_msgData.size())
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
		return true;
	}

public:
	const string m_msgData = "PING";
};

