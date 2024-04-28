#include "CoreLib.h"

void WorkerEntry(HANDLE hHandle, WSABUF* pOut)
{
    while (true)
    {
        while (true)
        {
            DWORD Bytes = 0;
            ClientSession* pSession = nullptr;
            LPOVERLAPPED pOverlap = nullptr;

            bool bRet = GetQueuedCompletionStatus(hHandle, &Bytes, (ULONG_PTR*)&pSession, (LPOVERLAPPED*)&pOverlap, INFINITE);

            if (bRet == FALSE || Bytes == 0)
            {
                //Close Socket
                continue;
            }

            switch (pSession->eType)
            {
            case READ:
            {
                WSABUF DataBuf;
                DataBuf.buf = pSession->recvBuffer;
                DataBuf.len = BUFSIZE;

                DWORD recvLen = 0;
                DWORD flag = 0;
                WSARecv(pSession->soc, &DataBuf, 1, &recvLen, &flag, (LPOVERLAPPED)&pOverlap, NULL);
            }
            break;

            case WRITE:
            {
            }
            break;

            case QUEUEWATING:
            {
                WSABUF DataBuf;
                DataBuf.buf = pSession->recvBuffer;
                DataBuf.len = BUFSIZE;

                DWORD recvLen = 0;
                DWORD flag = 0;
                WSASend(pSession->soc, &DataBuf, 1, &recvLen, flag, (LPOVERLAPPED)&pOverlap, nullptr);
            }
            break;


            default:
                break;
            }
        }
    }
}
