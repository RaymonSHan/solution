#include "dlltest.h"

using namespace std;

#ifdef _DEBUG
#define MRAPI_DLL "c:\\mr2api\\debug\\mr2api.dll"
//#define MRAPI_DLL "Z:\\HostWrite\\otc-tech\\mr2api\\mrapi.dll"    // fdep
#else
#define MRAPI_DLL ".\\mrapi.dll"
#endif

DEFINEPROCESS_C(Mr2Init);
DEFINEPROCESS_C(Mr2Init2);
DEFINEPROCESS_C(Mr2CreatePkgID);
DEFINEPROCESS_C(Mr2Send);
DEFINEPROCESS_C(Mr2Receive1);
DEFINEPROCESS_C(Mr2Receive1_FreeBuf);
DEFINEPROCESS_C(Mr2Receive2);
DEFINEPROCESS_C(Mr2Receive3);
DEFINEPROCESS_C(Mr2IsLinkOK);
DEFINEPROCESS_C(Mr2GetVersion);
DEFINEPROCESS_C(Mr2RegRecvCondition);
DEFINEPROCESS_C(Mr2Destroy);
DEFINEPROCESS_C(Mr2MultiDestSend);
DEFINEPROCESS_C(Mr2GetPeerUserStat);

RESULT LoadDll()
{
	bool bRet = false ;
	
	HINSTANCE m_FDEPdll = ::LoadLibrary(MRAPI_DLL) ;
	if (m_FDEPdll == NULL)
	{
		printf("error: can not find (mrapi.dll)");
		return bRet ;
	}
  GETPROCESS_C(Mr2Init);
  GETPROCESS_C(Mr2Init2);
  GETPROCESS_C(Mr2CreatePkgID);
  GETPROCESS_C(Mr2Send);
  GETPROCESS_C(Mr2Receive1);
  GETPROCESS_C(Mr2Receive1_FreeBuf);
  GETPROCESS_C(Mr2Receive2);
  GETPROCESS_C(Mr2Receive3);
  GETPROCESS_C(Mr2IsLinkOK);
  GETPROCESS_C(Mr2GetVersion);
  GETPROCESS_C(Mr2RegRecvCondition);
  GETPROCESS_C(Mr2Destroy);
  GETPROCESS_C(Mr2MultiDestSend);
  GETPROCESS_C(Mr2GetPeerUserStat);

	return RESULT_OK ;
}

void MySleep(int iMillSecond)
{
	Sleep(iMillSecond);
}

//#define SELF_DEBUG
#define CONNECT_DEBUG

int _tmain(int argc, _TCHAR* argv[])
{
	if (LoadDll())
	{
		printf("load mrapi.dll error.\n");
		return -1;
	}
  void* handle;
  char psPkg[1000] = "asdfasr4w5 5ryrty dfg erb 45b6h45by rdgert e";
  char* recvbuffer;
  char userrecvbuffer[2000];
  int recvlength;
  STUConnInfo2 client;
  STUMsgProperty2 prop;

#ifdef CONNECT_DEBUG
  strcpy_s(client.m_szMRIP, MR2_MAXLEN_IP, "10.0.48.125");
  client.m_usMRPort = 65432;
#endif CONNECT_DEBUG
#ifdef SELF_DEBUG
  strcpy_s(client.m_szMRIP, MR2_MAXLEN_IP, "127.0.0.1");
  client.m_usMRPort = 9898;
#endif SELF_DEBUG

/*
  FDEP_C(Mr2Init2)(&handle, "otc-tech app", "otc-tech pwd", &prop, NULL, &client, 1, NULL, 1);
  do
  {
    FDEP_C(Mr2Receive1)(handle, &recvbuffer, &recvlength, &prop, 1000);
    printf("Received %s, length %d\r\n", recvbuffer, recvlength);
    FDEP_C(Mr2Receive1_FreeBuf)(recvbuffer);
  }
  while (true);
  return 0;
*/

  FDEP_C(Mr2Init2)(&handle, "otc-tech send", "otc-tech pwd", &prop, NULL, &client, 1, NULL, 1);
  FDEP_C(Mr2Send)(handle, psPkg, 444, &prop, 1000);

  FDEP_C(Mr2Receive2)(handle, userrecvbuffer, &recvlength, 2000, &prop, 1000);
  
  FDEP_C(Mr2Send)(handle, psPkg, 555, &prop, 1000);
  FDEP_C(Mr2Receive1)(handle, &recvbuffer, &recvlength, &prop, 1000);
  FDEP_C(Mr2Receive1_FreeBuf)(recvbuffer);


  FDEP_C(Mr2Send)(handle, psPkg, 334, &prop, 1000);
  FDEP_C(Mr2Receive1)(handle, &recvbuffer, &recvlength, &prop, 1000);
  FDEP_C(Mr2Receive1_FreeBuf)(recvbuffer);

  FDEP_C(Mr2Send)(handle, psPkg, 667, &prop, 1000);
  FDEP_C(Mr2Receive1)(handle, &recvbuffer, &recvlength, &prop, 1000);
  FDEP_C(Mr2Receive1_FreeBuf)(recvbuffer);

  FDEP_C(Mr2Send)(handle, psPkg, 345, &prop, 1000);
  FDEP_C(Mr2Receive1)(handle, &recvbuffer, &recvlength, &prop, 1000);
  FDEP_C(Mr2Receive1_FreeBuf)(recvbuffer);

  while (true)
  {
    Sleep(10000);
  }
	return 0;
}
