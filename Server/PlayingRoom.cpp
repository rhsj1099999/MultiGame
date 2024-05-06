#include "stdafx.h"
#include "PlayingRoom.h"

CPlayingRoom::CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
	m_arrClients[0] = pThreeSession[0];
	m_arrClients[1] = pThreeSession[1];
	m_arrClients[2] = pThreeSession[2];
}

CPlayingRoom::~CPlayingRoom()
{
}

void CPlayingRoom::Init(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
    m_InsertedIndices.resize(15);

	m_arrClients[0] = pThreeSession[0];
	m_arrClients[1] = pThreeSession[1];
	m_arrClients[2] = pThreeSession[2];

	m_iPlayingOrder = rand() % 1;

	m_arrClients[m_iPlayingOrder]->eClientState = ClientSession::ClientState::TURNON;

    int Dummy = 0;

    for (int i = 0; i < CLIENT3; i++)
    {
        //PREDATA::OrderType TempOrder = (i == m_iPlayingOrder) ? PREDATA::OrderType::TURNON : PREDATA::OrderType::TURNOFF;

        //ClientSession* pSession = m_arrClients[i];

        //CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());

        //bool bTempMessageSend = false;
        //if (pSession->CQPtr->GetSize() == 0)
        //    bTempMessageSend = true;

        //pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
        //(
        //    sizeof(int),
        //    TempOrder
        //));
        //pSession->CQPtr->Enqueqe_Instance<__int32>(Dummy);

        //if (bTempMessageSend == true)
        //{
        //    DWORD recvLen = 0;
        //    DWORD flag = 0;
        //    pSession->wsaBuf_Send.buf = (char*)pSession->CQPtr->GetFrontPtr();
        //    pSession->wsaBuf_Send.len = pSession->CQPtr->GetSize();
        //    if (WSASend((pSession)->soc, &pSession->wsaBuf_Send, 1, &recvLen, flag, &(pSession)->Overlapped_Send, NULL) == SOCKET_ERROR)
        //    {
        //        if (WSAGetLastError() != WSA_IO_PENDING)
        //        {
        //            closesocket(pSession->soc);
        //        }
        //    }
        //}

        ClientSession* pSession = m_arrClients[i];

        MySend<int>(pSession, m_iPlayingOrder, PREDATA::OrderType::TURNCHANGED);

        //CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());

        //bool bTempMessageSend = false;
        //if (pSession->CQPtr->GetSize() == 0)
        //    bTempMessageSend = true;

        //pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
        //(
        //    sizeof(int),
        //    PREDATA::OrderType::TURNCHANGED
        //));
        //pSession->CQPtr->Enqueqe_Instance<__int32>(m_iPlayingOrder);

        //if (bTempMessageSend == true)
        //{
        //    DWORD recvLen = 0;
        //    DWORD flag = 0;
        //    pSession->wsaBuf_Send.buf = (char*)pSession->CQPtr->GetFrontPtr();
        //    pSession->wsaBuf_Send.len = pSession->CQPtr->GetSize();
        //    if (WSASend((pSession)->soc, &pSession->wsaBuf_Send, 1, &recvLen, flag, &(pSession)->Overlapped_Send, NULL) == SOCKET_ERROR)
        //    {
        //        //if (WSAGetLastError() != WSA_IO_PENDING)
        //        //{
        //        //    closesocket(pSession->soc);
        //        //}
        //        if (WSAGetLastError() != WSAEWOULDBLOCK)
        //        {
        //            SR1_MSGBOX("EWOULDBLOCK In Send Server");
        //        }
        //    }
        //}

    }
}

void CPlayingRoom::Tick()
{


}
