#include "CoreLib.h"

void WorkerEntry(HANDLE hHandle, WSABUF* pOut)
{
    while (true)
    {
        DWORD Bytes = 0;
        Session* pSession = nullptr;
        OverlappedExtend* pOverLapEx = nullptr;

        bool bRet = GetQueuedCompletionStatus(hHandle, &Bytes, (ULONG_PTR*)&pSession, (LPOVERLAPPED*)&pOverLapEx, INFINITE);

        if (bRet == FALSE || Bytes == 0)
        {
            //Close Socket
            continue;
        }

        switch (pOverLapEx->eType)
        {
        case READ:
        {
            WSABUF wsaBuf;
            wsaBuf.buf = pSession->recvBuffer;
            wsaBuf.len = BUFSIZE;

            if (pOut != nullptr)
            {
                memcpy(pOut->buf, wsaBuf.buf, wsaBuf.len);
                pOut->len = wsaBuf.len;
            }


            DWORD recvLen = 0;
            DWORD flag = 0;
            WSARecv(pSession->soc, pOut, 1, &recvLen, &flag, &pOverLapEx->OverlappedEvent, NULL);
        }
        break;

        case WRITE:
        {
            int a = 10;
        }
        break;

        case QUEUEWATING:
        {
            WSABUF wsaBuf;

            wsaBuf.buf = pSession->recvBuffer;
            wsaBuf.len = BUFSIZE;

            DWORD recvLen = 0;
            DWORD flag = 0;
            WSASend(pSession->soc, &wsaBuf, 1, &recvLen, flag, &pOverLapEx->OverlappedEvent, nullptr);
            //WSARecv(pSession->soc, &wsaBuf, 1, &recvLen, &flag, &pOverLapEx->OverlappedEvent, NULL);
        }
        break;


        default:
            break;
        }
        //if (pOverLapEx->eType == READ)
        //{
        //    cout << "Data Recevied IOCP = " << Bytes << endl;
        //}
        //else
        //{
        //    //MessageBox()
        //}



        //메인스레드가 최초 1회 WSARecv를 해야한다고 함.

    }
}
