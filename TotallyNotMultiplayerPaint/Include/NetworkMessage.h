#pragma once
#include <vector>
#include <string>

#define MESSAGE_TYPE_VAR unsigned short

namespace E {
	enum NETWORK_MSG {
		kERROR = 0,
		kLOGIN_REQUEST, // Done
		kSIGNUP_REQUEST,
		kUSERNAME_REQUEST, // Done
		kUSERNAME_SENT, // Done
		kPASSWORD_REQUEST, // Done
		kPASSWORD_SENT, // Done
		kCONNECTION_SUCCESSFUL, // Done
		kDISCONNECTION, // Done
		kCREATE_LINE, // Done
		kCREATE_RECTANGLE, // Done
		kCREATE_CIRCLE, // Done
		kCREATE_FREEDRAW,
		kSYNC_USER
	};
}

using std::string;
using std::vector;
using Package = vector<char>;

class NetworkMessage
{
public:
	NetworkMessage() = default;
	virtual ~NetworkMessage() = default;
	virtual Package packData() = 0;
	virtual bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) = 0;
};

class MsgConnectRequest : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kLOGIN_REQUEST;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != m_msgData.size())
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);

		return true;
	}

public:
	const string m_msgData = "CONNECT";
};

class MsgSignupRequest : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kLOGIN_REQUEST;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != m_msgData.size())
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);

		return true;
	}

public:
	const string m_msgData = "SIGNUP";
};

class MsgUsernameRequest : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kUSERNAME_REQUEST;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != m_msgData.size())
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	const string m_msgData = "USER";
};

class MsgUsernameSent : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kUSERNAME_SENT;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		/*if (numBytes != sizeof(m_msgData.size()))
		{
			return false;
		}*/

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	string m_msgData;
};

class MsgPasswordRequest : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kPASSWORD_REQUEST;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != m_msgData.size())
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	string m_msgData = "PASSWORD";
};

class MsgPasswordSent : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kPASSWORD_SENT;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		/*if (numBytes != sizeof(m_msgData.size()))
		{
			return false;
		}*/

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	string m_msgData;
};

class MsgConnected : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kCONNECTION_SUCCESSFUL;
		Package data;
		data.resize(m_msgData.size() + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), m_msgData.data(), m_msgData.size());
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != m_msgData.size())
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	const string m_msgData = "CONNECTED";
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

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != m_msgData.size())
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	const string m_msgData = "DISCCONNECTED";
};

class MsgCreateLine : public NetworkMessage
{
public:
	MsgCreateLine() = default;
	MsgCreateLine(Package data) {
		unpackData(&m_msgData, data.data(), data.size());
	};
	MsgCreateLine(float iniPosX, float iniPosY, float finPosX, float finPosY, unsigned short colorID)
	{
		m_msgData.initialPosX = iniPosX;
		m_msgData.initialPosY = iniPosY;
		m_msgData.finalPosX = finPosX;
		m_msgData.finalPosY = finPosX;
		m_msgData.colorID = finPosX;
	};
	virtual ~MsgCreateLine() = default;

	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kCREATE_LINE;
		Package data;
		data.resize(sizeof(m_msgData) + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), &m_msgData, sizeof(m_msgData));
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != sizeof(m_msgData))
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	struct LineData
	{
		float initialPosX;
		float initialPosY;
		float finalPosX;
		float finalPosY;
		unsigned short colorID;
	} m_msgData;
};

class MsgCreateRectangle : public NetworkMessage
{
public:
	MsgCreateRectangle() = default;
	MsgCreateRectangle(Package data) {
		unpackData(&m_msgData, data.data(), data.size());
	};
	MsgCreateRectangle(float iniPosX, float iniPosY, float finPosX, float finPosY, unsigned short colorID)
	{
		m_msgData.initialPosX = iniPosX;
		m_msgData.initialPosY = iniPosY;
		m_msgData.finalPosX = finPosX;
		m_msgData.finalPosY = finPosX;
		m_msgData.colorID = finPosX;
	};
	virtual ~MsgCreateRectangle() = default;

	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kCREATE_RECTANGLE;
		Package data;
		data.resize(sizeof(m_msgData) + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), &m_msgData, sizeof(m_msgData));
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != sizeof(m_msgData))
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	struct RectangleData
	{
		float initialPosX;
		float initialPosY;
		float finalPosX;
		float finalPosY;
		unsigned short colorID;
	} m_msgData;
};

class MsgCreateCircle : public NetworkMessage
{
public:
	MsgCreateCircle() = default;
	MsgCreateCircle(Package data) {
		unpackData(&m_msgData, data.data(), data.size());
	};
	MsgCreateCircle(float iniPosX, float iniPosY, float finPosX, float finPosY, unsigned short colorID)
	{
		m_msgData.initialPosX = iniPosX;
		m_msgData.initialPosY = iniPosY;
		m_msgData.finalPosX = finPosX;
		m_msgData.finalPosY = finPosX;
		m_msgData.colorID = finPosX;
	};
	virtual ~MsgCreateCircle() = default;

	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kCREATE_CIRCLE;
		Package data;
		data.resize(sizeof(m_msgData) + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), &m_msgData, sizeof(m_msgData));
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != sizeof(m_msgData))
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	struct CircleData
	{
		float initialPosX;
		float initialPosY;
		float finalPosX;
		float finalPosY;
		unsigned short colorID;
	} m_msgData;
};

class MsgCreateFreedraw : public NetworkMessage
{
public:
	Package packData() override
	{
		MESSAGE_TYPE_VAR MSGTYPE = E::kCREATE_FREEDRAW;
		Package data;
		data.resize(sizeof(m_msgData) + sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data(), &MSGTYPE, sizeof(MESSAGE_TYPE_VAR));
		memcpy(data.data() + sizeof(MESSAGE_TYPE_VAR), &m_msgData, sizeof(m_msgData));
		return data;
	}

	bool unpackData(void* pSrcData, void* pDestData, size_t numBytes) override
	{
		if (numBytes != sizeof(m_msgData))
		{
			return false;
		}

		memcpy(pDestData, pSrcData, numBytes);
	}

public:
	struct FreedrawData
	{
		size_t vectorSize;
		//vector<float> pointPositions;
		unsigned short colorID;
	} m_msgData;
};