#ifndef __M2API_H__
#define __M2API_H__

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <memory>
#include <vector>

#pragma pack(1)
typedef struct packet_phase
{
	BYTE	header;
	BYTE	phase;
} TPacketGCPhase;

typedef struct packet_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketGCHandshake;

typedef struct SChannelStatus
{
	short nPort;
	BYTE bStatus;
} TChannelStatus;
#pragma pack()

enum class PACKET_SERVER
{
	HEADER_CG_TEXT = 64,
	HEADER_CG_MARK_LOGIN = 100,
	HEADER_CG_STATE_CHECKER = 206,
	HEADER_CG_KEY_AGREEMENT = 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_CG_PONG = 0xfe,
	HEADER_CG_HANDSHAKE = 0xff,
};

using BUFFER_VEC = std::vector<char>;

class M2API
{
public:
	M2API(const char* pw, const char* ip, WORD port);
	~M2API();

	bool SendAndGet(PACKET_SERVER p, BUFFER_VEC& vecBuffer, const std::string& msg = "");

	const std::string& GetPassword() const;
	const std::string& GetIP() const;
	WORD GetPort() const;

private:
	std::string m_Password;
	std::string m_IP;
	WORD m_Port;
	SOCKET m_SocketFd;
	std::unique_ptr<WSADATA> m_WSA;
	int m_ConnectResult;
};

template<typename T, typename std::enable_if<!std::is_same<T, const char*>::value>::type* = nullptr> inline
T Decode(char*& arr)
{
	auto& value = *reinterpret_cast<T*>(arr);
	arr += sizeof(T);
	return value;
}

template <> inline
std::string Decode<std::string>(char*& arr)
{
	std::string value(reinterpret_cast<const char*>(arr));
	if (value.empty() == false && value.back() == '\n')
		value.pop_back();
	arr += value.size();
	return value;
}

#endif