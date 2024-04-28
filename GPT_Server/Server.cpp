#include <iostream>
#include <vector>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT 27015
#define MAX_CLIENTS 10

struct ClientInfo {
    SOCKET socket;
    sockaddr_in addr;
};

std::vector<ClientInfo> g_clients;

DWORD WINAPI WorkerThread(LPVOID lpParam);

int main() {
    WSADATA wsaData;
    SOCKET listenSocket;
    sockaddr_in serverAddr;
    HANDLE hThread;
    DWORD dwThreadId;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Create a SOCKET for listening for incoming connection requests.
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to the server address and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    if (bind(listenSocket, reinterpret_cast
        sockaddr* > (&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Start listening on the socket
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Create worker thread
    hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, &dwThreadId);
    if (hThread == NULL) {
        std::cerr << "CreateThread failed with error: " << GetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Accept incoming connections and process them asynchronously
    while (1) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        // Add client info to the list
        if (g_clients.size() < MAX_CLIENTS) {
            ClientInfo clientInfo;
            clientInfo.socket = clientSocket;
            int addrLen = sizeof(clientInfo.addr);
            getpeername(clientSocket, reinterpret_cast
                cpp
                Copy code
                <sockaddr*>(&clientInfo.addr), &addrLen);
            g_clients.push_back(clientInfo);
        }
        else {
            std::cerr << "Max clients reached. Connection rejected." << std::endl;
            closesocket(clientSocket);
        }
    }

    // Cleanup
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}

DWORD WINAPI WorkerThread(LPVOID lpParam) {
    while (1) {
        // Send client count to all clients
        ULONG_PTR clientCount = g_clients.size();
        for (const auto& client : g_clients) {
            send(client.socket, reinterpret_cast<const char*>(&clientCount), sizeof(clientCount), 0);
        }
        Sleep(1000); // Sleep for 1 second
    }

    return 0;
