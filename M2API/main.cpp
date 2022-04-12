/*
* blackdragonx61 / Mali
* 12.04.2022
*/

#include <iostream>
#include "M2API.h"

int main()
{
	M2API p("SHOWMETHEMONEY", "127.0.0.1", 13061);

	// Admin Login
	{
		BUFFER_VEC vecBuffer(sizeof(TPacketGCPhase) + sizeof(TPacketGCHandshake) + 32);
		bool recv_success{ p.SendAndGet(PACKET_SERVER::HEADER_CG_TEXT, vecBuffer, p.GetPassword()) };

		if (recv_success) {
			auto tempHead = vecBuffer.data();
			tempHead += sizeof(TPacketGCPhase) + sizeof(TPacketGCHandshake); // for first connection

			auto resText = Decode<std::string>(tempHead);

			if (!resText.compare("UNKNOWN"))
				printf("ADMIN LOGIN SUCCESS.\n");
			else
				printf("ADMIN LOGIN FAILED: %s\n", resText.c_str());
		}
		else {
			printf("[Admin Login]Cannot get data.\n");
		}
	}
	
	// USER_COUNT
	{
		BUFFER_VEC vecBuffer(32);
		bool recv_success{ p.SendAndGet(PACKET_SERVER::HEADER_CG_TEXT, vecBuffer, "USER_COUNT")};

		if (recv_success) {
			auto tempHead = vecBuffer.data();

			auto resText = Decode<std::string>(tempHead);

			printf("USER_COUNT Result: %s\n", resText.c_str());
		}
		else {
			printf("[USER_COUNT]Cannot get data.\n");
		}
	}

	// IS_SERVER_UP
	{
		BUFFER_VEC vecBuffer(8);
		bool recv_success{ p.SendAndGet(PACKET_SERVER::HEADER_CG_TEXT, vecBuffer, "IS_SERVER_UP") };

		if (recv_success) {
			auto tempHead = vecBuffer.data();

			auto resText = Decode<std::string>(tempHead);

			printf("IS_SERVER_UP Result: %s\n", resText.c_str());
		}
		else {
			printf("[IS_SERVER_UP]Cannot get data.\n");
		}
	}

	// NOTICE
	{
		BUFFER_VEC vecBuffer(16);
		bool recv_success{ p.SendAndGet(PACKET_SERVER::HEADER_CG_TEXT, vecBuffer, "NOTICE This is simple notice.") };

		if (!recv_success) {
			printf("[NOTICE]Cannot get data.\n");
		}
	}
	
	// HEADER_CG_STATE_CHECKER
	{
		BUFFER_VEC vecBuffer(128);
		bool recv_success{ p.SendAndGet(PACKET_SERVER::HEADER_CG_STATE_CHECKER, vecBuffer) };

		if (recv_success) {
			auto tempHead = vecBuffer.data();

			auto bHeader = Decode<BYTE>(tempHead);
			auto nSize = Decode<int>(tempHead);

			std::vector<TChannelStatus> collect;
			collect.reserve(nSize);

			for (int i = 0; i < nSize; i++)
				collect.emplace_back(Decode<TChannelStatus>(tempHead));

			auto bSuccess = Decode<BYTE>(tempHead);

			printf("HEADER_CG_STATE_CHECKER Result(%d): \n", nSize);
			for (const auto& v : collect)
				printf("\tPORT: %d -> STATUS: %d\n", v.nPort, v.bStatus);
		}
		else {
			printf("[HEADER_CG_STATE_CHECKER]Cannot get data.\n");
		}
	}

	printf("\nFinished, Press a key.\n");
	std::cin.get();

	return EXIT_SUCCESS;
}
