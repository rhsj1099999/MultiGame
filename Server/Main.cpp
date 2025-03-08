#include "stdafx.h"
#include <cstdlib>
#include "MainInstance.h"
#include "Timer.h"
#include <csignal> // for signal


CMainInstance* _mainInstance = nullptr;

void DestroyMainInstance(int signal)
{
    std::cout << "Received signal: " << signal << std::endl;

    _mainInstance->Stop();

    delete _mainInstance;
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
    
    _mainInstance = CMainInstance::GetInstance();

    _mainInstance->Init();

    while (true)
    {
        _mainInstance->Tick();
    }

    return 0;
}



