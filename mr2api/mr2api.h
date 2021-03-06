#ifndef __SSCC_FDEPV4_MR2API_H__
#define __SSCC_FDEPV4_MR2API_H__

#ifdef __cplusplus
extern "C" {
#endif 

/*
#ifdef DLLOUT_EXPORTS
#define DLLOUT_API __declspec(dllexport)
#else
#define DLLOUT_API __declspec(dllimport)
#endif

for __stdcall, dllexport export function name like @funcname@paralength
for __cdecl, dllexport export function name like funcname
for def file, all function name like funcname. the MOST COMMON WIN32 DLL, use DEF file and __stdcall
*/

#define MR2_MSGFLAG_PERSIST           0x01   /*持久消息标志*/
#define MR2_MSGFLAG_COMPRESS          0x02   /*压缩标志*/
#define MR2_MSGFLAG_REPLYTOME         0X04   /*应答包自动推送到发送方标志*/


#define MR2_MAXLEN_ADDR               32     /*源地址或目的地址的最大长度 @2012.1.12*/
#define MR2_MAXLEN_PKGID              64     /*包ID的最大长度(目前版本中32位OS上长度为34,64位OS上长度为39或44)*/
#define MR2_MAXLEN_USERDATA           256    /*用户数据的最大长度*/

#define MR2_MAXLEN_MSGTYPE            8	     /*为了兼容旧的，长度与旧版一样*/

#define MR2_MAXLEN_IP                 16


/* MrSend, Mr2Browse, MrReceive1或MrReceive2函数返回的错误码.
 * 使用时一般只需要判断是否为0, 如果为0表示成功,否则表示失败.
 */
#define MR2_ERRCODE_OK                0
#define MR2_ERRCODE_PARAMERR         -1
#define MR2_ERRCODE_CONNERR          -2
#define MR2_ERRCODE_TIMEEXPIRED      -3
#define MR2_ERRCODE_TIMEOUT          -4
#define MR2_ERRCODE_NOMSG            -5
#define MR2_ERRCODE_BUFTOOSHORT      -6
#define MR2_ERRCODE_BUFTOOBIG        -7
#define MR2_ERRCODE_SYSERROR         -8
#define MR2_ERRCODE_COMMU_NOTALLOW   -9
#define MR2_ERRCODE_DEST_NOTONLINE   -10
#define MR2_ERRCODE_DEST_FULL        -11

/* 单发消息属性结构体 
 * 目的用户只有一个。
 */
typedef struct _tagSTUMsgProperty2
{
  char                m_szSourceUserID[MR2_MAXLEN_ADDR];  /* 源用户标识，必须是'\0'结尾的字符串。*/
  char                m_szSourceAppID[MR2_MAXLEN_ADDR];   /*源应用标识，必须是'\0'结尾的字符串。*/
  char                m_szDestUserID[MR2_MAXLEN_ADDR];    /*目的用户标识，必须是'\0'结尾的字符串。*/
  char                m_szDestAppID[MR2_MAXLEN_ADDR];     /*目的应用标识，必须是'\0'结尾的字符串。*/
  char                m_szPkgID[MR2_MAXLEN_PKGID];        /*包ID, 必须是'\0'结尾的字符串. 或者由用户使用MrCreatePkgID函数生成,或者填空*/
  char                m_szCorrPkgID[MR2_MAXLEN_PKGID];    /*相关包ID, 必须是'\0'结尾的字符串, 供用户使用*/
  char                m_szUserData1[MR2_MAXLEN_USERDATA]; /*用户数据1, 必须是'\0'结尾的字符串, 供用户使用*/
  char                m_szUserData2[MR2_MAXLEN_USERDATA]; /*用户数据2, 必须是'\0'结尾的字符串, 供用户使用*/
    
  unsigned char       m_ucFlag;      /*标志:可以是MR2_MSGFLAG_PERSIST或MR2_MSGFLAG_COMPRESS等标志的位或.*/
	unsigned char       m_ucBizType;   /*业务类型，当前取值有(将来会扩充):0-三方存管,10-银期转账,11-银基转账,12-资金划拨,13-信证报盘,14-电子对账,15-融资融券,16-基金盘后,17-转融通,18-B转H. */
	unsigned char       m_ucPriority;  /* 优先级， 5为最低，3为最高*/
	unsigned char       m_ucSensitiveLevel;    /* 敏感性级别，0为最低，255为最高*/
	char				        m_szMsgType[MR2_MAXLEN_MSGTYPE]; /*消息类型，目前只能取值: M(消息)或F(文件). 如果为F(文件),则在交换日志中永远不打印包内容.*/
} STUMsgProperty2;

/* 连接参数结构体 */
typedef struct _tagSTUConnInfo2
{
    char              m_szMRIP[MR2_MAXLEN_IP];            /*主用MR的IP. 必须是'\0'结尾的字符串. "127.0.0.1"*/
    unsigned short    m_usMRPort;                         /*主用MR的端口.    51231*/
} STUConnInfo2;

typedef struct _tagSTUUserAddr
{
	char m_szUserID[MR2_MAXLEN_ADDR];                      	/*用户标识，必须是'\0'结尾的字符串。*/
	char m_szAppID[MR2_MAXLEN_ADDR];                      	/*应用标识，必须是'\0'结尾的字符串。*/
} STUUserAddr;

/* 群发消息属性结构体。
 * 与单发消息属性不同之处：目的用户可以是多个；
 * m_pArrDestUserAddr-表示目的用户数组指针，m_iDestUserCount-表示目的用户个数。
 */
typedef struct _tagSTUMultiDestMsgProperty
{
	char                m_szSourceUserID[MR2_MAXLEN_ADDR];  /*源用户标识，必须是'\0'结尾的字符串。*/
	char                m_szSourceAppID[MR2_MAXLEN_ADDR];   /*源应用标识，必须是'\0'结尾的字符串。*/
	int                 m_iDestUserCount;		        			  /*目的用户个数*/
	STUUserAddr*        m_pArrDestUserAddr;					        /*目的用户结构数组指针*/
	char                m_szPkgID[MR2_MAXLEN_PKGID];        /*包ID, 必须是'\0'结尾的字符串. 或者由用户使用MrCreatePkgID函数生成,或者填空*/
	char                m_szCorrPkgID[MR2_MAXLEN_PKGID];    /*相关包ID, 必须是'\0'结尾的字符串, 供用户使用*/
	char                m_szUserData1[MR2_MAXLEN_USERDATA]; /*用户数据1, 必须是'\0'结尾的字符串, 供用户使用*/
	char                m_szUserData2[MR2_MAXLEN_USERDATA]; /*用户数据2, 必须是'\0'结尾的字符串, 供用户使用*/

	unsigned char       m_ucFlag;                           /*标志:可以是MR2_MSGFLAG_PERSIST或MR2_MSGFLAG_COMPRESS等标志的位或.*/
	unsigned char       m_ucBizType;                        /*业务类型，取值为上述各值，如11为银基转帐*/
	unsigned char       m_ucPriority;                       /* 优先级， 5为最低，3为最高*/
	unsigned char       m_ucSensitiveLevel;                 /* 敏感性级别，0为最低，255为最高*/
	char				m_szMsgType[MR2_MAXLEN_MSGTYPE];            /*消息类型，目前只能取值: M(消息)或F(文件). 如果为F(文件),则在交换日志中永远不打印包内容.*/
} STUMultiDestMsgProperty;


typedef int (*OnReceiveCallBack2)(const char* psPkg, int iPkgLen, const STUMsgProperty2* pMsgPropery, void* pvUserData);

// DLL 输出函数名
#define     EXPO_C(func)          func
#define     EXPO_J(func)          JOIN(Java_com_sscc_fdep_mrapi_, func)

// MC_ MJ_ 为自身处理函数
#define     MAIN_C(func)          JOIN(MC_, func)
#define     MAIN_J(func)          JOIN(MJ_, func)

// TC_ TJ_ 为函数指针类型，
#define     TYPE_C(func)          JOIN(TC_, func)
#define     TYPE_J(func)          JOIN(TJ_, func)

// FC_ FJ_ 为函数指针变量，用于存储原FDEP DLL的函数指针
#define     FDEP_C(func)          JOIN(FC_, func)
#define     FDEP_J(func)          JOIN(FJ_, func)

// JC_ JJ_ 为判断函数，返回RESULT_OK表示由自身处理，否则由FDEP处理
// #define     JUDG_C(func)          JOIN(JC_, func)
// #define     JUDG_J(func)          JOIN(JJ_, func)

/* void* Mr2Init(const char* psAppID, const char* psAppPasswd, 
                         OnReceiveCallBack2 onReceive,const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
                         void* pvUserData);
*/

#define     DEFINEPROCESS_C(func)                                                     \
  TYPE_C(func) * FDEP_C(func);
#define     DEFINEPROCESS_J(func)                                                     \
  TYPE_J(func) * FDEP_J(func);
#define     DEFINEPROCESS_ALL(func)                                                   \
  DEFINEPROCESS_C(func) DEFINEPROCESS_J(func)

#define     GETPROCESS_C(func)                                                        \
  FDEP_C(func) = (TYPE_C(func)*)::GetProcAddress(m_FDEPdll, TOSTRING(EXPO_C(func)));  \
  if (FDEP_C(func) == NULL) return RESULT_DLLLOAD_ERROR;
#define     GETPROCESS_J(func)                                                        \
  FDEP_J(func) = (TYPE_J(func)*)::GetProcAddress(m_FDEPdll, TOSTRING(EXPO_J(func)));  \
  if (FDEP_J(func) == NULL) return RESULT_DLLLOAD_ERROR;
#define     GETPROCESS_ALL(func)                                                      \
  GETPROCESS_C(func) GETPROCESS_J(func)

#define     WRITELOG              OTCDLL_Log(__FUNCTION__, sizeof(__FUNCTION__)-1)

#ifdef DLLOUT_EXPORTS
#define DEFINE_C_FUNC(ret, func, para)                                                \
  typedef ret (WINAPI TYPE_C(func)) para;                                             \
  ret WINAPI MAIN_C(func) para;                                                              \
  ret WINAPI EXPO_C(func) para;

#define DEFINE_J_FUNC(ret, func, para)                                                \
  typedef ret (WINAPI TYPE_J(func)) para;                                             \
  ret WINAPI MAIN_J(func) para;                                                              \
  ret WINAPI EXPO_J(func) para;

#else  // DLLOUT_EXPORTS
#define DEFINE_C_FUNC(ret, func, para)                                                \
  typedef ret (WINAPI TYPE_C(func)) para;                                             \
  ret WINAPI EXPO_C(func) para;

#define DEFINE_J_FUNC(ret, func, para)                                                \
  typedef ret (WINAPI TYPE_J(func)) para;                                             \
  ret WINAPI EXPO_J(func) para;

#endif // DLLOUT_EXPORTS

/*
typedef void* WINAPI (TC_Mr2Init)(const char* psAppID, const char* psAppPasswd, 
  OnReceiveCallBack2 onReceive,const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
  void* pvUserData);
// RESULT WINAPI JC_Mr2Init(const char* psAppID, const char* psAppPasswd, 
//   OnReceiveCallBack2 onReceive,const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
//   void* pvUserData);
void* WINAPI MC_Mr2Init(const char* psAppID, const char* psAppPasswd, 
  OnReceiveCallBack2 onReceive,const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
  void* pvUserData);
void* WINAPI Mr2Init(const char* psAppID, const char* psAppPasswd, 
  OnReceiveCallBack2 onReceive,const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
  void* pvUserData);
*/
DEFINE_C_FUNC(void*, Mr2Init, (const char* psAppID, const char* psAppPasswd, 
  OnReceiveCallBack2 onReceive,const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
  void* pvUserData));
DEFINE_C_FUNC(void, Mr2Init2, (void** ppHandle, const char* psAppID, const char* psAppPasswd,
	STUMsgProperty2* pOnRecvMsgPropery, OnReceiveCallBack2 onReceive,
	const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount,
	void* pvUserData, int iThreadCount));
DEFINE_C_FUNC(int, Mr2CreatePkgID, (void* pHandle, char szPkgID[MR2_MAXLEN_PKGID]));
DEFINE_C_FUNC(int, Mr2Send,(void* pHandle, const char* psPkg, int iPkgLen, STUMsgProperty2* pMsgPropery, int iMillSecTimeo));
DEFINE_C_FUNC(int, Mr2Receive1,(void* pHandle, char** ppsPkg, int* piOutPkgLen, STUMsgProperty2* pMsgPropery, int iMillSecTimeo));
DEFINE_C_FUNC(void, Mr2Receive1_FreeBuf,(char* psPkg));
DEFINE_C_FUNC(int, Mr2Receive2,(void* pHandle, char* psPkg, int* piOutPkgLen, int iBufLenIn, STUMsgProperty2* pMsgPropery, int iMillSecTimeo));
DEFINE_C_FUNC(int, Mr2Receive3,(void* pHandle, char** ppsPkg, int* piOutPkgLen, int* piErrSXCode, STUMsgProperty2* pMsgPropery, int iMillSecTimeo));
DEFINE_C_FUNC(int, Mr2IsLinkOK, (void* pHandle));
DEFINE_C_FUNC(void, Mr2GetVersion,(char* psBufVersion, int iBufLen));
DEFINE_C_FUNC(int, Mr2RegRecvCondition,(void* pHandle, STUMsgProperty2* pArrMsgPropery, int iArrayCount));  
DEFINE_C_FUNC(void, Mr2Destroy,(void* pHandle));
DEFINE_C_FUNC(int, Mr2MultiDestSend,(void* pHandle, const char* psPkg, int iPkgLen, STUMultiDestMsgProperty* pMsgPropery, int iMillSecTimeo));
DEFINE_C_FUNC(int, Mr2GetPeerUserStat,(void* pHandle, const char* psPeerUserID, int* piOutStat));

#if defined(_WIN32)  || defined(_LINUX64_JDK32) || defined(_JDK32)
DEFINE_J_FUNC(jint, Mr2Init, (JNIEnv *, jclass, jstring, jstring, jstring, jshort, jstring, jshort));
#endif
#if defined(_LINUX64_JDK64)  || defined(_AIX64_JDK64) || defined(_JDK64)
DEFINE_J_FUNC(jlong, Mr2Init, (JNIEnv *, jclass, jstring, jstring, jstring, jshort, jstring, jshort));
#endif
DEFINE_J_FUNC(jbyteArray, Mr2CreatePkgID, (JNIEnv*, jclass, jstring));
DEFINE_J_FUNC(jstring, Mr2Send, (JNIEnv*, jclass, jbyteArray, jstring, jstring, jstring, 
  jstring, jstring, jstring, jstring, jstring, jstring, jbyte, jbyte, jbyte, jbyte, jint));
DEFINE_J_FUNC(jbyteArray, Mr2Receive1, (JNIEnv *, jclass, jstring, jstring, jstring, 
  jstring, jstring, jstring, jstring, jstring, jstring, jint));
DEFINE_J_FUNC(jbyteArray, Mr2Receive3, (JNIEnv* , jclass, jstring, jstring, jstring, 
  jstring, jstring, jstring, jstring, jstring, jstring, jint));
DEFINE_J_FUNC(jint, Mr2IsLinkOK, (JNIEnv*, jclass, jstring));
DEFINE_J_FUNC(jbyteArray, Mr2GetVersion, (JNIEnv*, jclass));
DEFINE_J_FUNC(jint, Mr2RegRecvCondition, (JNIEnv*, jclass, jstring, jstring, jstring, jstring, 
  jstring, jstring, jstring, jstring, jstring));
DEFINE_J_FUNC(void, Mr2Destroy, (JNIEnv*, jclass, jstring));
DEFINE_J_FUNC(jint, Mr2GetPeerUserStat, (JNIEnv*, jclass, jstring, jstring));

#ifdef __cplusplus
}
#endif 

#endif  // __SSCC_FDEPV4_MR2API_H__




