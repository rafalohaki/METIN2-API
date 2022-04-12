#include "M2API.h"

M2API::M2API(const char* pw, const char* ip, WORD port) :
	m_Password(pw),
	m_IP(ip),
	m_Port(port),
	m_SocketFd(INVALID_SOCKET),
	m_WSA(std::make_unique<WSADATA>()),
	m_ConnectResult(SOCKET_ERROR)
{
	if (WSAStartup(MAKEWORD(2, 2), m_WSA.get()) != ERROR_SUCCESS) {
		printf("WSAStartup Failed. Error Code: %d\n", WSAGetLastError());
		m_WSA = nullptr;
		return;
	}
	
	m_SocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_SocketFd == INVALID_SOCKET) {
		printf("Can't create socket! Error Code: %d\n", WSAGetLastError());
		return;
	}

	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(GetIP().c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(GetPort());

	m_ConnectResult = connect(m_SocketFd, (struct sockaddr*)&server, sizeof(server));
	if (m_ConnectResult == SOCKET_ERROR) {
		printf("Connect fail! Error Code: %d\n", WSAGetLastError());
		return;
	}
}

M2API::~M2API()
{
	if (m_SocketFd != INVALID_SOCKET) {
		closesocket(m_SocketFd);
	}

	if (m_WSA) {
		WSACleanup();
	}
}

bool M2API::SendAndGet(PACKET_SERVER p, BUFFER_VEC& vecBuffer, const std::string& msg)
{
	if (m_ConnectResult == SOCKET_ERROR)
		return false;

	std::string message = (char)p + msg;
	if (p == PACKET_SERVER::HEADER_CG_TEXT)
		message += '\n';

	int iResult = send(m_SocketFd, message.c_str(), static_cast<int>(message.size()), 0);
	if (iResult == SOCKET_ERROR) {
		printf("Send Failed! Error Code: %d\n", WSAGetLastError());
		return false;
	}

	iResult = recv(m_SocketFd, vecBuffer.data(), static_cast<int>(vecBuffer.size()), 0);
	if (iResult == SOCKET_ERROR) {
		printf("Recv Failed! Error Code: %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

const std::string& M2API::GetPassword() const
{
	return m_Password;
}

const std::string& M2API::GetIP() const
{
	return m_IP;
}

WORD M2API::GetPort() const
{
	return m_Port;
}
