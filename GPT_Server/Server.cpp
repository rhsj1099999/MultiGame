//#include <iostream>
//#include <WinSock2.h>
//#include <Windows.h>
//#include <ws2tcpip.h>
//#include <vector>
//#include <thread>
//
//using namespace std;
//
//#pragma comment(lib, "ws2_32.lib")
//
//#define DEFAULT_PORT 12345
//#define MAX_CLIENTS 100
//#define BUFFER_SIZE 1024
//
//// 클라이언트 정보 구조체
//struct ClientInfo {
//    SOCKET socket;
//    SOCKADDR_IN addr;
//};
//
//// IOCP 관련 구조체
//struct IOContext {
//    OVERLAPPED overlapped;
//    WSABUF dataBuf;
//    char buffer[BUFFER_SIZE];
//    DWORD bytesTransferred;
//    DWORD flags;
//};
//
//// 전역 변수
//HANDLE completionPort;
//std::vector<ClientInfo> clients;
//
//// 함수 선언
//void WorkerThread();
//void SendClientCountToAll();
//
//int main() {
//    // 윈속 초기화
//    WSADATA wsaData;
//    WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//    // IOCP 생성
//    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//
//    // 소켓 생성
//    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
//
//    unsigned long On = 1;
//    ioctlsocket(listenSocket, FIONBIO, &On);
//    bool bEnable = true;
//    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bEnable, sizeof(bEnable));
//    setsockopt(listenSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&bEnable, sizeof(bEnable));
//
//
//    SOCKADDR_IN serverAddr;
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//    serverAddr.sin_port = htons(DEFAULT_PORT);
//    bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
//    listen(listenSocket, SOMAXCONN);
//
//    // 완료 포트에 소켓 바인딩
//    CreateIoCompletionPort((HANDLE)listenSocket, completionPort, 0, 0);
//
//    vector<thread> vecThreads;
//
//    // Worker 스레드 생성
//    for (int i = 0; i < 5; ++i) 
//    {
//        vecThreads.push_back(thread([=]() {WorkerThread(); }));
//        //HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
//        //CloseHandle(hThread);
//    }
//
//    // 클라이언트 연결 대기
//    std::cout << "서버: 클라이언트 연결 대기 중..." << std::endl;
//    while (true) 
//    {
//        SOCKET clientSocket = {};
//        SOCKADDR_IN ClientAddress;
//        __int32 AddrLen = sizeof(ClientAddress);
//
//        while (true)
//        {
//            clientSocket = accept(listenSocket, NULL, NULL);
//
//            if (WSAGetLastError() == WSAEWOULDBLOCK)
//            {
//                continue;
//            }
//            else
//            {
//                break;
//            }
//        }
//
//
//        // 클라이언트 정보 저장
//        ClientInfo clientInfo;
//        clientInfo.socket = clientSocket;
//        //getpeername(clientSocket, (SOCKADDR*)&clientInfo.addr, NULL);
//        clients.push_back(clientInfo);
//
//        // 완료 포트에 소켓 바인딩
//        CreateIoCompletionPort((HANDLE)clientSocket, completionPort, 0, 0);
//
//        // 클라이언트에게 현재 연결된 클라이언트 수 정보 보내기
//        SendClientCountToAll();
//    }
//
//    // 소켓 닫기
//    closesocket(listenSocket);
//
//    // 윈속 해제
//    WSACleanup();
//
//    return 0;
//}
//
//void WorkerThread() 
//{
//    while (true) 
//    {
//        DWORD bytesTransferred;
//        ULONG_PTR completionKey;
//        LPOVERLAPPED overlapped;
//
//        BOOL result = GetQueuedCompletionStatus(completionPort, &bytesTransferred, &completionKey, &overlapped, INFINITE);
//
//        if (!result || bytesTransferred == 0) {
//            // 오류 처리
//            continue;
//        }
//
//        // 클라이언트에게 현재 연결된 클라이언트 수 정보 보내기
//        SendClientCountToAll();
//    }
//
//    return;
//}
//
//void SendClientCountToAll() {
//    // 현재 연결된 클라이언트 수 정보 구하기
//    int numClients = clients.size();
//
//    // WSABUF 설정
//    WSABUF DataBuf;
//    DataBuf.buf = (char*)&numClients;
//    DataBuf.len = sizeof(numClients);
//
//    // 모든 클라이언트에게 정보 전송
//    for (const auto& client : clients) {
//        WSASend(client.socket, &DataBuf, 1, NULL, 0, NULL, NULL);
//    }
//}








//#include <iostream>
//#include <WinSock2.h>
//#include <Windows.h>
//#include <vector>
//
//#pragma comment(lib, "ws2_32.lib")
//
//#define DEFAULT_PORT 12345
//#define MAX_CLIENTS 100
//#define BUFFER_SIZE 1024
//
//// 클라이언트 정보 구조체
//struct ClientInfo {
//    SOCKET socket;
//    SOCKADDR_IN addr;
//    OVERLAPPED overlapped;
//    WSABUF dataBuf;
//    char buffer[BUFFER_SIZE];
//    DWORD bytesTransferred;
//    DWORD flags;
//};
//
//// 전역 변수
//HANDLE completionPort;
//std::vector<ClientInfo> clients;
//
//// 함수 선언
//DWORD WINAPI WorkerThread(LPVOID lpParam);
//void SendClientCountToAll();
//
//int main() {
//    // 윈속 초기화
//    WSADATA wsaData;
//    WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//    // IOCP 생성
//    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//
//    // Worker 스레드 생성
//    for (int i = 0; i < 2; ++i) {
//        HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
//        CloseHandle(hThread);
//    }
//
//    // 소켓 생성
//    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
//    SOCKADDR_IN serverAddr;
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//    serverAddr.sin_port = htons(DEFAULT_PORT);
//    bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
//    listen(listenSocket, SOMAXCONN);
//
//    // 완료 포트에 소켓 바인딩
//    CreateIoCompletionPort((HANDLE)listenSocket, completionPort, 0, 0);
//
//    // 클라이언트 연결 대기
//    std::cout << "서버: 클라이언트 연결 대기 중..." << std::endl;
//    while (true) {
//        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
//        if (clientSocket == INVALID_SOCKET) {
//            std::cerr << "accept() failed: " << WSAGetLastError() << std::endl;
//            continue;
//        }
//
//        // 클라이언트 정보 저장
//        ClientInfo clientInfo;
//        clientInfo.socket = clientSocket;
//        clientInfo.dataBuf.buf = clientInfo.buffer;
//        clientInfo.dataBuf.len = BUFFER_SIZE;
//        ZeroMemory(&clientInfo.overlapped, sizeof(OVERLAPPED));
//        clients.push_back(clientInfo);
//
//        // 완료 포트에 소켓 바인딩
//        CreateIoCompletionPort((HANDLE)clientSocket, completionPort, (ULONG_PTR)&clients.back(), 0);
//
//        // 클라이언트에게 현재 연결된 클라이언트 수 정보 보내기
//        SendClientCountToAll();
//    }
//
//    // 소켓 닫기
//    closesocket(listenSocket);
//
//    // 윈속 해제
//    WSACleanup();
//
//    return 0;
//}
//
//DWORD WINAPI WorkerThread(LPVOID lpParam) {
//    while (true) {
//        DWORD bytesTransferred;
//        ULONG_PTR completionKey;
//        LPOVERLAPPED overlapped;
//
//        BOOL result = GetQueuedCompletionStatus(completionPort, &bytesTransferred, &completionKey, &overlapped, INFINITE);
//
//        if (!result || bytesTransferred == 0) {
//            // 오류 처리
//            continue;
//        }
//
//        // 클라이언트에게 현재 연결된 클라이언트 수 정보 보내기
//        SendClientCountToAll();
//    }
//
//    return 0;
//}
//
//void SendClientCountToAll() {
//    // 현재 연결된 클라이언트 수 정보 구하기
//    int numClients = clients.size();
//
//    // WSABUF 설정
//    WSABUF DataBuf;
//    DataBuf.buf = (char*)&numClients;
//    DataBuf.len = sizeof(numClients);
//
//    // 모든 클라이언트에게 정보 전송
//    for (auto& client : clients) {
//        WSASend(client.socket, &DataBuf, 1, NULL, 0, &(client.overlapped), NULL);
//    }
//}




#include <iostream>  
#include <winsock2.h>  
#pragma comment(lib,"ws2_32.lib")  

typedef struct
{
    WSAOVERLAPPED Overlapped;
    SOCKET Socket;
    WSABUF wsaBuf;
    char Buffer[1024];
    DWORD BytesSent;
    DWORD BytesToSend;
} PER_IO_DATA, * LPPER_IO_DATA;


static DWORD WINAPI ServerWorkerThread(LPVOID lpParameter)
{
    HANDLE hCompletionPort = (HANDLE)lpParameter;
    DWORD NumBytesSent = 0;
    ULONG CompletionKey;
    LPPER_IO_DATA PerIoData;

    while (GetQueuedCompletionStatus(hCompletionPort, &NumBytesSent, (PULONG_PTR)&CompletionKey, (LPOVERLAPPED*)&PerIoData, INFINITE))
    {
        if (!PerIoData)
            continue;

        if (NumBytesSent == 0)
        {
            std::cout << "Client disconnected!\r\n\r\n";
        }
        else
        {
            PerIoData->BytesSent += NumBytesSent;
            if (PerIoData->BytesSent < PerIoData->BytesToSend)
            {
                PerIoData->wsaBuf.buf = &(PerIoData->Buffer[PerIoData->BytesSent]);
                PerIoData->wsaBuf.len = (PerIoData->BytesToSend - PerIoData->BytesSent);
            }
            else
            {
                PerIoData->wsaBuf.buf = PerIoData->Buffer;
                PerIoData->wsaBuf.len = strlen(PerIoData->Buffer);
                PerIoData->BytesSent = 0;
                PerIoData->BytesToSend = PerIoData->wsaBuf.len;
            }

            if (WSASend(PerIoData->Socket, &(PerIoData->wsaBuf), 1, &NumBytesSent, 0, &(PerIoData->Overlapped), NULL) == 0)
                continue;

            if (WSAGetLastError() == WSA_IO_PENDING)
                continue;
        }

        closesocket(PerIoData->Socket);
        delete PerIoData;
    }

    return 0;
}

int main()
{
    WSADATA WsaDat;
    if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0)
        return 0;

    HANDLE hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (!hCompletionPort)
        return 0;

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    for (DWORD i = 0; i < systemInfo.dwNumberOfProcessors; ++i)
    {
        HANDLE hThread = CreateThread(NULL, 0, ServerWorkerThread, hCompletionPort, 0, NULL);
        CloseHandle(hThread);
    }

    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN server;
    ZeroMemory(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(listenSocket, (SOCKADDR*)(&server), sizeof(server)) != 0)
        return 0;

    if (listen(listenSocket, 1) != 0)
        return 0;

    std::cout << "Waiting for incoming connection...\r\n";

    SOCKET acceptSocket;
    do
    {
        sockaddr_in saClient;
        int nClientSize = sizeof(saClient);
        acceptSocket = WSAAccept(listenSocket, (SOCKADDR*)&saClient, &nClientSize, NULL, NULL);
    } while (acceptSocket == INVALID_SOCKET);

    std::cout << "Client connected!\r\n\r\n";

    CreateIoCompletionPort((HANDLE)acceptSocket, hCompletionPort, 0, 0);

    LPPER_IO_DATA pPerIoData = new PER_IO_DATA;
    ZeroMemory(pPerIoData, sizeof(PER_IO_DATA));

    strcpy_s(pPerIoData->Buffer, "Welcome to the server!\r\n");

    pPerIoData->Overlapped.hEvent = WSACreateEvent();
    pPerIoData->Socket = acceptSocket;
    pPerIoData->wsaBuf.buf = pPerIoData->Buffer;
    pPerIoData->wsaBuf.len = strlen(pPerIoData->Buffer);
    pPerIoData->BytesToSend = pPerIoData->wsaBuf.len;

    DWORD dwNumSent;
    if (WSASend(acceptSocket, &(pPerIoData->wsaBuf), 1, &dwNumSent, 0, &(pPerIoData->Overlapped), NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            delete pPerIoData;
            return 0;
        }
    }

    while (TRUE)
        Sleep(1000);

    shutdown(acceptSocket, SD_BOTH);
    closesocket(acceptSocket);

    WSACleanup();
    return 0;
}