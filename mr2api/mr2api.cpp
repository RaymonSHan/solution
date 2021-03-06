// mr2api.cpp : 定義 DLL 應用程式的匯出函式。
//

#include "mr2dll.h"

HINSTANCE                       m_FDEPdll;

DEFINEPROCESS_ALL(Mr2Init);
DEFINEPROCESS_C(Mr2Init2);
DEFINEPROCESS_ALL(Mr2CreatePkgID);
DEFINEPROCESS_ALL(Mr2Send);
DEFINEPROCESS_ALL(Mr2Receive1);
DEFINEPROCESS_C(Mr2Receive1_FreeBuf);
DEFINEPROCESS_C(Mr2Receive2);
DEFINEPROCESS_ALL(Mr2Receive3);
DEFINEPROCESS_ALL(Mr2IsLinkOK);
DEFINEPROCESS_ALL(Mr2GetVersion);
DEFINEPROCESS_ALL(Mr2RegRecvCondition);
DEFINEPROCESS_ALL(Mr2Destroy);
DEFINEPROCESS_C(Mr2MultiDestSend);
DEFINEPROCESS_ALL(Mr2GetPeerUserStat);

RESULT OTCDLL_FDEPInit(char* dllPath)
{
  m_FDEPdll = ::LoadLibrary(dllPath) ;
	if (m_FDEPdll == NULL)
    return RESULT_DLLLOAD_ERROR;

  GETPROCESS_ALL(Mr2Init);
  GETPROCESS_C(Mr2Init2);
  GETPROCESS_ALL(Mr2CreatePkgID);
  GETPROCESS_ALL(Mr2Send);
  GETPROCESS_ALL(Mr2Receive1);
  GETPROCESS_C(Mr2Receive1_FreeBuf);
  GETPROCESS_C(Mr2Receive2);
  GETPROCESS_ALL(Mr2Receive3);
  GETPROCESS_ALL(Mr2IsLinkOK);
  GETPROCESS_ALL(Mr2GetVersion);
  GETPROCESS_ALL(Mr2RegRecvCondition);
  GETPROCESS_ALL(Mr2Destroy);
  GETPROCESS_C(Mr2MultiDestSend);
  GETPROCESS_ALL(Mr2GetPeerUserStat);
  return RESULT_OK;
}

RESULT OTCDLL_FDEPExit()
{
  ::FreeLibrary(m_FDEPdll);
  return RESULT_OK;
}

void* WINAPI Mr2Init(const char* psAppID, const char* psAppPasswd, 
                         OnReceiveCallBack2 onReceive,const STUConnInfo2* pArrConnInfo, 
                         int iArrConnInfoCount, void* pvUserData)
{
  if (AppIDUseFDEP(psAppID)) 
    return FDEP_C(Mr2Init)(psAppID, psAppPasswd, onReceive, pArrConnInfo, iArrConnInfoCount, pvUserData);
  else
    return MAIN_C(Mr2Init)(psAppID, psAppPasswd, onReceive, pArrConnInfo, iArrConnInfoCount, pvUserData);
}

void WINAPI Mr2Init2(void** ppHandle, const char* psAppID, const char* psAppPasswd, 
                         STUMsgProperty2* pOnRecvMsgPropery,OnReceiveCallBack2 onReceive, 
                         const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
                         void* pvUserData, int iThreadCount)
{
  if (AppIDUseFDEP(psAppID)) 
    return FDEP_C(Mr2Init2)(ppHandle, psAppID, psAppPasswd, pOnRecvMsgPropery, onReceive,
                            pArrConnInfo, iArrConnInfoCount, pvUserData, iThreadCount);
  else
    return MAIN_C(Mr2Init2)(ppHandle, psAppID, psAppPasswd, pOnRecvMsgPropery, onReceive,
                            pArrConnInfo, iArrConnInfoCount, pvUserData, iThreadCount);
}

int WINAPI Mr2IsLinkOK(void* pHandle)
{
  if (HandleIDUseFDEP(pHandle))
    return FDEP_C(Mr2IsLinkOK)(pHandle);
  else
    return MAIN_C(Mr2IsLinkOK)(pHandle);
}

int WINAPI Mr2CreatePkgID(void* pHandle, char szPkgID[MR2_MAXLEN_PKGID])
{
  WRITELOG;
  return FDEP_C(Mr2CreatePkgID)(pHandle, szPkgID);
}

int WINAPI Mr2Send(void* pHandle, const char* psPkg, int iPkgLen, 
                       STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  if (HandleIDUseFDEP(pHandle))
    return FDEP_C(Mr2Send)(pHandle, psPkg, iPkgLen, pMsgPropery, iMillSecTimeo);
  else
    return MAIN_C(Mr2Send)(pHandle, psPkg, iPkgLen, pMsgPropery, iMillSecTimeo);
};

int WINAPI Mr2MultiDestSend(void* pHandle, const char* psPkg, int iPkgLen, 
                                STUMultiDestMsgProperty* pMsgPropery, int iMillSecTimeo)
{
  WRITELOG;
  return FDEP_C(Mr2MultiDestSend)(pHandle, psPkg, iPkgLen, pMsgPropery, iMillSecTimeo);
};

int WINAPI Mr2Receive1(void* pHandle, char** ppsPkg, int* piOutPkgLen, 
                           STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  if (HandleIDUseFDEP(pHandle))
    return FDEP_C(Mr2Receive1)(pHandle, ppsPkg, piOutPkgLen, pMsgPropery, iMillSecTimeo);
  else
    return MAIN_C(Mr2Receive1)(pHandle, ppsPkg, piOutPkgLen, pMsgPropery, iMillSecTimeo);
};

void WINAPI Mr2Receive1_FreeBuf(char* psPkg)
{
  if (MemoryUseFDEP(psPkg))
    return FDEP_C(Mr2Receive1_FreeBuf)(psPkg);
  else
    return MAIN_C(Mr2Receive1_FreeBuf)(psPkg);
};

int WINAPI Mr2Receive2(void* pHandle, char* psPkg, int* piOutPkgLen, int iBufLenIn, 
                           STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  if (HandleIDUseFDEP(pHandle))
    return FDEP_C(Mr2Receive2)(pHandle, psPkg, piOutPkgLen, iBufLenIn, pMsgPropery, iMillSecTimeo);
  else
    return MAIN_C(Mr2Receive2)(pHandle, psPkg, piOutPkgLen, iBufLenIn, pMsgPropery, iMillSecTimeo);
};

int WINAPI Mr2Receive3(void* pHandle, char** ppsPkg, int* piOutPkgLen, int* piErrSXCode, 
                           STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  WRITELOG;
  return FDEP_C(Mr2Receive3)(pHandle, ppsPkg, piOutPkgLen, piErrSXCode, pMsgPropery, iMillSecTimeo);
};

void WINAPI Mr2Destroy(void* pHandle)
{
  WRITELOG;
  return FDEP_C(Mr2Destroy)(pHandle);
};

void WINAPI Mr2GetVersion(char* psBufVersion, int iBufLen)
{
  WRITELOG;
  return FDEP_C(Mr2GetVersion)(psBufVersion, iBufLen);
};

int WINAPI Mr2RegRecvCondition(void* pHandle, STUMsgProperty2* pArrMsgPropery, int iArrayCount)
{
  WRITELOG;
  return FDEP_C(Mr2RegRecvCondition)(pHandle, pArrMsgPropery, iArrayCount);
};

int WINAPI Mr2GetPeerUserStat(void* pHandle, const char* psPeerUserID, int* piOutStat)
{
  WRITELOG;
  return FDEP_C(Mr2GetPeerUserStat)(pHandle, psPeerUserID, piOutStat);
};


#if defined(_WIN32)  || defined(_LINUX64_JDK32) || defined(_JDK32)
jint WINAPI Java_com_sscc_fdep_mrapi_Mr2Init(JNIEnv* env, jclass obj, jstring s1, jstring s2, jstring s3, jshort s4, jstring s5, jshort s6)
{
  WRITELOG;
  return FDEP_J(Mr2Init)(env, obj, s1, s2, s3, s4, s5, s6);
};
#endif
#if defined(_LINUX64_JDK64)  || defined(_AIX64_JDK64) || defined(_JDK64)
jlong WINAPI Java_com_sscc_fdep_mrapi_Mr2Init(JNIEnv , jclass obj, jstring s1, jstring s2, jstring s3, jshort s4, jstring s5, jshort s6)
{
  WRITELOG;
  return FDEP_J(Mr2Init)(env, obj, s1, s2, s3, s4, s5, s6);
};
#endif

jbyteArray WINAPI Java_com_sscc_fdep_mrapi_Mr2CreatePkgID(JNIEnv* env, jclass obj, jstring s1)
{
  WRITELOG;
  return FDEP_J(Mr2CreatePkgID)(env, obj, s1);
};

jstring WINAPI Java_com_sscc_fdep_mrapi_Mr2Send(JNIEnv* env, jclass obj, jbyteArray s1, jstring s2, jstring s3, jstring s4, 
                                                    jstring s5, jstring s6, jstring s7, jstring s8, jstring s9, jstring s10, 
                                                    jbyte s11, jbyte s12, jbyte s13, jbyte s14, jint s15)
{
  WRITELOG;
  return FDEP_J(Mr2Send)(env, obj, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15);
};

jbyteArray WINAPI Java_com_sscc_fdep_mrapi_Mr2Receive1(JNIEnv* env, jclass obj, jstring s1, jstring s2, jstring s3, 
                                                           jstring s4, jstring s5, jstring s6, jstring s7, jstring s8, jstring s9, jint s10)
{
  WRITELOG;
  return FDEP_J(Mr2Receive1)(env, obj, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10);
};

jbyteArray WINAPI Java_com_sscc_fdep_mrapi_Mr2Receive3(JNIEnv* env, jclass obj, jstring s1, jstring s2, jstring s3, 
                                                           jstring s4, jstring s5, jstring s6, jstring s7, jstring s8, jstring s9, jint s10)
{
  WRITELOG;
  return FDEP_J(Mr2Receive3)(env, obj, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10);
};

jint WINAPI Java_com_sscc_fdep_mrapi_Mr2IsLinkOK(JNIEnv* env, jclass obj, jstring s1)
{
  WRITELOG;
  return FDEP_J(Mr2IsLinkOK)(env, obj, s1);
};

jbyteArray WINAPI Java_com_sscc_fdep_mrapi_Mr2GetVersion(JNIEnv* env, jclass obj)
{
  WRITELOG;
  return FDEP_J(Mr2GetVersion)(env, obj);
};

jint WINAPI Java_com_sscc_fdep_mrapi_Mr2RegRecvCondition(JNIEnv* env, jclass obj, jstring s1, jstring s2, jstring s3, jstring s4, 
                                                             jstring s5, jstring s6, jstring s7, jstring s8, jstring s9)
{
  WRITELOG;
  return FDEP_J(Mr2RegRecvCondition)(env, obj, s1, s2, s3, s4, s5, s6, s7, s8, s9);
};

void WINAPI Java_com_sscc_fdep_mrapi_Mr2Destroy(JNIEnv* env, jclass obj, jstring s1)
{
  WRITELOG;
  return FDEP_J(Mr2Destroy)(env, obj, s1);
};

jint WINAPI Java_com_sscc_fdep_mrapi_Mr2GetPeerUserStat(JNIEnv* env, jclass obj, jstring s1, jstring s2)
{
  WRITELOG;
  return FDEP_J(Mr2GetPeerUserStat)(env, obj, s1, s2);
};
