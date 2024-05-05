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

	m_iPlayingOrder = rand() % CLIENT1;

	m_arrClients[m_iPlayingOrder]->eClientState = ClientSession::ClientState::TURNON;

    int Dummy = 0;

    for (int i = 0; i < CLIENT1; i++)
    {

        PREDATA::OrderType TempOrder = (i == m_iPlayingOrder) ? PREDATA::OrderType::TURNON : PREDATA::OrderType::TURNOFF;

        ClientSession* pSession = m_arrClients[i];

        CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());

        //if (TempOrder != PREDATA::OrderType::TURNON)
        //{
        //    SR1_MSGBOX("NotOrder");
        //}

        pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
        (
            sizeof(int),
            TempOrder
        ));
        pSession->CQPtr->Enqueqe_Instance<__int32>(Dummy);

        //DWORD Sendlen = {};
        //DWORD Flag = {};
        ////pSession->Lock_WSASend
        ////(
        ////    &pSession->wsaBuf_Send, 1, &Sendlen, Flag, &pSession->Overlapped_Send, NULL
        ////);
        //if (WSASend((pSession)->soc, &pSession->wsaBuf_Send, 1, &Sendlen, Flag, &(pSession)->Overlapped_Send, NULL) == SOCKET_ERROR)
        //{
        //    if (WSAGetLastError() != WSA_IO_PENDING)
        //    {
        //        closesocket(pSession->soc);
        //    }
        //}
    }
}

void CPlayingRoom::Tick()
{

    PREDATA::OrderType TempOrder = PREDATA::OrderType::ROTATEANGLE;

    //for (size_t i = 0; i < 3; i++)
    //{
    //    if (i == m_iPlayingOrder)
    //    {
    //        continue;
    //    }


    //    //Send CurrAngle
    //    ClientSession* pSession = m_arrClients[i];

    //    CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());

    //    pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
    //    (
    //        sizeof(int),
    //        TempOrder
    //    ));
    //    pSession->CQPtr->Enqueqe_Instance<float>(m_fCurrAngle);

    //    DWORD Sendlen = {};
    //    DWORD Flag = {};

    //    pSession->Lock_WSASend
    //    (
    //        &pSession->wsaBuf_Send, 1, &Sendlen, Flag, &pSession->Overlapped_Send, NULL
    //    );
    //}


}
