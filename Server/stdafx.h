#pragma once

using namespace std;

#define SR1_MSGBOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Fail_"), MB_OK)
#define SR1_MSGBOX2(MESSAGE) MessageBox(0, MESSAGE, TEXT("Fail_"), MB_OK)

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <list>
#include <mutex>
#include <map>
#include <queue>


#include <CoreLib.h>
#pragma comment(lib, "CoreLib.lib")

#include <Windows.h>





