
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN


#include <windows.h>

#include <WinSock2.h>																											//
#include <Ws2tcpip.h>
#include <mswsock.h>	
#include <time.h>
#include <process.h>

#include "jni.h"


#define DLLOUT_EXPORTS

#include "rcommon.h"
#include "mr2api.h"
#include "mr2main.h"


#pragma comment (lib, "ws2_32.lib")	


RESULT OTCDLL_Main(DWORD reason);

BOOL OTC_ShouldLoadFDEP(void);

RESULT OTCDLL_ProcessInit();
RESULT OTCDLL_ProcessExit();
RESULT OTCDLL_ThreadInit(MYINT count);
RESULT OTCDLL_ThreadExit();
RESULT OTCDLL_FDEPInit(char*);
RESULT OTCDLL_FDEPExit();

RESULT OTCDLL_Log(char* buffer, int len);