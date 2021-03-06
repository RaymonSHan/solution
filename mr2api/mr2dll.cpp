
#include "mr2dll.h"

#ifdef _DEBUG
#define     FDEPDLLPATH             "Z:\\HostWrite\\otc-tech\\mr2api\\mrapi.dll"
#else  _DEBUG
#define     FDEPDLLPATH             ".\\mrapi_fdep.dll"
#endif _DEBUG

#define     MR2LOGPATH              ".\\mrapi_test.log"

#define     TOSTRING(x)            _TOSTRING(x)
//ConnectStruct mConnectStruct[MAX_THREAD];


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
  RESULT result = OTCDLL_Main(ul_reason_for_call);
  if (result != RESULT_OK)
    return FALSE;
  else
    return TRUE;
}

RESULT OTCDLL_Main(DWORD reason)
{
  DEFINE_COUNT(EntryCount);

  __TRY
  switch (reason)
  {
    case DLL_PROCESS_ATTACH:
      __DO (OTCDLL_ProcessInit());
      if (OTC_ShouldLoadFDEP())
        __DO (OTCDLL_FDEPInit(FDEPDLLPATH));
      break;
	  case DLL_THREAD_ATTACH:
//      result = OTCDLL_ThreadInit(EntryCount);
//      INC(EntryCount);
      break;
    case DLL_THREAD_DETACH:
//      DEC(EntryCount);  // only by order
//      result = OTCDLL_ThreadExit();
      break;
    case DLL_PROCESS_DETACH:
      if (OTC_ShouldLoadFDEP())
        __DO (OTCDLL_FDEPExit());
      __DO (OTCDLL_ProcessExit());
      break;
    default:
      return RESULT_DLLLOAD_ERROR;
  }
  __CATCH
}

BOOL OTC_ShouldLoadFDEP(void)
{
  static MYINT should = MARK_UNDEFINE;
  if (should == MARK_UNDEFINE)
    should = TRUE;                  // should = realloadjudeg();
  return should;
}


RESULT OTCDLL_ProcessInit()
{
  WSADATA wsaData;

//  ZeroMemory(mConnectStruct, sizeof(mConnectStruct));
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != NO_ERROR)
    return RESULT_WSA_ERROR;
  else
    return RESULT_OK;
}

RESULT OTCDLL_ProcessExit()
{
  WSACleanup();
  return RESULT_OK;
}

RESULT OTCDLL_ThreadInit(MYINT count)
{
  return RESULT_OK;
}

RESULT OTCDLL_ThreadExit()
{
  return RESULT_OK;
}

RESULT OTCDLL_Log(char* buffer, int len)
{
#define BUFFERSIZE  64

	FILE *handle = NULL;
  char timebuf[BUFFERSIZE];
  char CR[] = "\r\n";
  char BLANK[] = "  ";

  RESULT result = fopen_s(&handle, MR2LOGPATH, "ab+");
	if (result == RESULT_OK)
	{
    _strtime_s( timebuf, BUFFERSIZE );
    fwrite(timebuf, 8, 1, handle);
    fwrite(BLANK, sizeof(BLANK)-1, 1, handle);

		fwrite(buffer, len, 1, handle);
    fwrite(CR, sizeof(CR)-1, 1, handle);
		fclose(handle);
	}
  return RESULT_OK;
}

/*
void main() {
  //----------------------
  // Initialize Winsock
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != NO_ERROR)
    printf("Error at WSAStartup()\n");

  //----------------------
  // Create a SOCKET for connecting to server

  //----------------------
  // The sockaddr_in structure specifies the address family,
  // IP address, and port of the server to be connected to.
  sockaddr_in clientService; 
  clientService.sin_family = AF_INET;
  clientService.sin_addr.s_addr = inet_addr( "127.0.0.1" );
  clientService.sin_port = htons( 27015 );

  //----------------------
  // Connect to server.
  if ( connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) {
    printf( "Failed to connect.\n" );
    WSACleanup();
    return;
  }

  printf("Connected to server.\n");
  WSACleanup();
  return;
}
*/
