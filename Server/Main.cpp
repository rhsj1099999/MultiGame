#include "stdafx.h"
#include <cstdlib>
#include "MainInstance.h"
#include "Timer.h"
#include <csignal> // for signal


CMainInstance* pInstance_Main = nullptr;

int* Temp = new int;

void DestroyMainInstance(int signal)
{
    std::cout << "Received signal: " << signal << std::endl;

    pInstance_Main->SetCanTick(false);

    while (true)
    {
        if (pInstance_Main->GetIsRunning() == false)
        {
            break;
        }
    }

    delete pInstance_Main;
}

int main()
{
    BOOL bHandler = SetConsoleCtrlHandler((PHANDLER_ROUTINE)DestroyMainInstance, TRUE);

    if (bHandler == false)
    {
        cout << "Ctrl False" << endl;
        return 0;
    }

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    signal(SIGABRT, DestroyMainInstance);
    
    pInstance_Main = CMainInstance::GetInstance();

    pInstance_Main->Init();

    while (true)
    {
        pInstance_Main->Tick();
    }

    return 0;
}



