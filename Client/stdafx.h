// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <list>
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <CoreLib.h>

#pragma comment(lib, "CoreLib.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace std;
