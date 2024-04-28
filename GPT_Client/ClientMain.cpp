//#include <iostream>
//#include <WinSock2.h>
//#include <Windows.h>
//#include <ws2tcpip.h>
//#include <vector>
//#include <thread>
//
//#pragma comment(lib, "ws2_32.lib")
//
//#define DEFAULT_PORT 12345
//#define BUFFER_SIZE 1024
//
//using namespace std;
//
//// IOCP ���� ����ü
//struct IOContext {
//    OVERLAPPED overlapped;
//    WSABUF dataBuf;
//    char buffer[BUFFER_SIZE];
//    DWORD bytesTransferred;
//    DWORD flags;
//};
//
//// ���� ����
//HANDLE completionPort;
//
//// �Լ� ����
//void WorkerThread();
//
//int main() {
//    // ���� �ʱ�ȭ
//    WSADATA wsaData;
//    WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//    // IOCP ����
//    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//
//    // Worker ������ ����
//    //for (int i = 0; i < 2; ++i) {
//    //    HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
//    //    CloseHandle(hThread);
//    //}
//    vector<thread> vecThreads;
//
//    for (int i = 0; i < 5; ++i)
//    {
//        vecThreads.push_back(thread([=]() {WorkerThread(); }));
//        //HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
//        //CloseHandle(hThread);
//    }
//
//    // ���� ����
//    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
//    unsigned long On = 1;
//    ioctlsocket(clientSocket, FIONBIO, &On);
//    bool bEnable = true;
//    setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bEnable, sizeof(bEnable));
//    setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&bEnable, sizeof(bEnable));
//
//
//    if (clientSocket == INVALID_SOCKET) {
//        std::cerr << "WSASocket() failed: " << WSAGetLastError() << std::endl;
//        WSACleanup();
//        return 1;
//    }
//
//    // ���� ���� ����
//
//
//
//    SOCKADDR_IN serverAddr;
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_port = htons(DEFAULT_PORT);
//    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
//
//    // ������ ���� (IOCP ��)
//    int result = connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
//    if (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
//        std::cerr << "connect() failed: " << WSAGetLastError() << std::endl;
//        closesocket(clientSocket);
//        WSACleanup();
//        return 1;
//    }
//
//    std::cout << "Ŭ���̾�Ʈ: ������ ����Ǿ����ϴ�." << std::endl;
//
//    // ���� �Ϸ� ���� Ȯ��
//    DWORD bytesTransferred;
//    ULONG_PTR completionKey;
//    LPOVERLAPPED overlapped;
//    result = GetQueuedCompletionStatus(completionPort, &bytesTransferred, &completionKey, &overlapped, INFINITE);
//    if (!result) {
//        std::cerr << "GetQueuedCompletionStatus() failed: " << GetLastError() << std::endl;
//        closesocket(clientSocket);
//        WSACleanup();
//        return 1;
//    }
//
//    // ������ ����
//    IOContext* ioContext = (IOContext*)overlapped;
//    std::cout << "���� ����� Ŭ���̾�Ʈ ��: " << *((int*)ioContext->dataBuf.buf) << std::endl;
//
//    // Ŭ���̾�Ʈ ���� �ݱ�
//    closesocket(clientSocket);
//
//    // ���� ����
//    WSACleanup();
//
//    return 0;
//}
//
//void WorkerThread() {
//    while (true) {
//        DWORD bytesTransferred;
//        ULONG_PTR completionKey;
//        LPOVERLAPPED overlapped;
//
//        BOOL result = GetQueuedCompletionStatus(completionPort, &bytesTransferred, &completionKey, &overlapped, INFINITE);
//
//        if (!result || bytesTransferred == 0) {
//            // ���� ó��
//            continue;
//        }
//
//        // Ŭ���̾�Ʈ���Լ� �����͸� �޾��� �� ó���� ����
//    }
//
//    return;
//}






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
//#define BUFFER_SIZE 1024
//
//// IOCP ���� ����ü
//struct IOContext {
//    OVERLAPPED overlapped;
//    WSABUF dataBuf;
//    char buffer[BUFFER_SIZE];
//    DWORD bytesTransferred;
//    DWORD flags;
//};
//
//// ���� ����
//HANDLE completionPort;
//
//// �Լ� ����
//DWORD WINAPI WorkerThread(LPVOID lpParam);
//
//int main() {
//    // ���� �ʱ�ȭ
//    WSADATA wsaData;
//    WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//    // IOCP ����
//    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//
//    // Worker ������ ����
//    for (int i = 0; i < 2; ++i) {
//        HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
//        CloseHandle(hThread);
//    }
//
//    // ���� ����
//    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
//    if (clientSocket == INVALID_SOCKET) {
//        std::cerr << "WSASocket() failed: " << WSAGetLastError() << std::endl;
//        WSACleanup();
//        return 1;
//    }
//
//    // ���� ���� ����
//    SOCKADDR_IN serverAddr;
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_port = htons(DEFAULT_PORT);
//    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
//
//    // ������ ���� (IOCP ��)
//    int result = connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
//    if (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
//        std::cerr << "connect() failed: " << WSAGetLastError() << std::endl;
//        closesocket(clientSocket);
//        WSACleanup();
//        return 1;
//    }
//
//    std::cout << "Ŭ���̾�Ʈ: ������ ����Ǿ����ϴ�." << std::endl;
//
//    // ���� �Ϸ� ���� Ȯ��
//    DWORD bytesTransferred;
//    ULONG_PTR completionKey;
//    LPOVERLAPPED overlapped;
//    result = GetQueuedCompletionStatus(completionPort, &bytesTransferred, &completionKey, &overlapped, INFINITE);
//    if (!result) {
//        std::cerr << "GetQueuedCompletionStatus() failed: " << GetLastError() << std::endl;
//        closesocket(clientSocket);
//        WSACleanup();
//        return 1;
//    }
//
//    // ������ ����
//    IOContext* ioContext = (IOContext*)overlapped;
//    std::cout << "���� ����� Ŭ���̾�Ʈ ��: " << *((int*)ioContext->dataBuf.buf) << std::endl;
//
//    // Ŭ���̾�Ʈ ���� �ݱ�
//    closesocket(clientSocket);
//
//    // ���� ����
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
//            // ���� ó��
//            continue;
//        }
//
//        // Ŭ���̾�Ʈ���Լ� �����͸� �޾��� �� ó���� ����
//    }
//
//    return 0;
//}


#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <vector>
#include <thread>
#pragma comment(lib, "ws2_32.lib") 

typedef struct
{
    WSAOVERLAPPED Overlapped;
    SOCKET Socket;
    WSABUF wsaBuf;
    char Buffer[1024];
    DWORD Flags;
} PER_IO_DATA, * LPPER_IO_DATA;

static DWORD WINAPI ClientWorkerThread(LPVOID lpParameter)
{
    HANDLE hCompletionPort = (HANDLE)lpParameter;
    DWORD NumBytesRecv = 0;
    ULONG CompletionKey;
    LPPER_IO_DATA PerIoData;

    while (GetQueuedCompletionStatus(hCompletionPort, &NumBytesRecv, (PULONG_PTR)&CompletionKey, (LPOVERLAPPED*)&PerIoData, INFINITE))
    {
        if (!PerIoData)
            continue;

        if (NumBytesRecv == 0)
        {
            std::cout << "Server disconnected!\r\n\r\n";
        }
        else
        {
            // use PerIoData->Buffer as needed...
            std::cout << std::string(PerIoData->Buffer, NumBytesRecv);

            PerIoData->wsaBuf.len = sizeof(PerIoData->Buffer);
            PerIoData->Flags = 0;

            if (WSARecv(PerIoData->Socket, &(PerIoData->wsaBuf), 1, &NumBytesRecv, &(PerIoData->Flags), &(PerIoData->Overlapped), NULL) == 0)
                continue;

            if (WSAGetLastError() == WSA_IO_PENDING)
                continue;
        }

        closesocket(PerIoData->Socket);
        delete PerIoData;
    }

    return 0;
}

int main(void)
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
        HANDLE hThread = CreateThread(NULL, 0, ClientWorkerThread, hCompletionPort, 0, NULL);
        CloseHandle(hThread);
    }

    SOCKET Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (Socket == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN SockAddr;
    SockAddr.sin_family = AF_INET;
    //SockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, "127.0.0.1", &SockAddr.sin_addr);
    SockAddr.sin_port = htons(8888);

    CreateIoCompletionPort((HANDLE)Socket, hCompletionPort, 0, 0);

    if (WSAConnect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
        return 0;

    PER_IO_DATA* pPerIoData = new PER_IO_DATA;
    ZeroMemory(pPerIoData, sizeof(PER_IO_DATA));

    pPerIoData->Socket = Socket;
    pPerIoData->Overlapped.hEvent = WSACreateEvent();
    pPerIoData->wsaBuf.buf = pPerIoData->Buffer;
    pPerIoData->wsaBuf.len = sizeof(pPerIoData->Buffer);

    DWORD dwNumRecv;
    if (WSARecv(Socket, &(pPerIoData->wsaBuf), 1, &dwNumRecv, &(pPerIoData->Flags), &(pPerIoData->Overlapped), NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            delete pPerIoData;
            return 0;
        }
    }

    while (TRUE)
        Sleep(1000);

    shutdown(Socket, SD_BOTH);
    closesocket(Socket);

    WSACleanup();
    return 0;
}