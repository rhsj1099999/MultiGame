#include "stdafx.h"
#include "MainInstance.h"
#include "Timer.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    CMainInstance* pInstance_Main = CMainInstance::GetInstance();
    pInstance_Main->Init();

    while (true)
    {
        pInstance_Main->Tick();
    }

    delete pInstance_Main;

    return 0;
}