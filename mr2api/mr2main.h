
#pragma once

using namespace std;

#ifdef _DEBUG
#define MAX_THREAD                  6 
#define MAX_RECEIVE                 2
#define MAX_CONNECT                 2
#define MAX_RECVLIST                4
#define MAX_TIMEWAIT                600         // received packet in list, but not user recv found. in second
#define MAX_HANDLE                  8
#define MAX_PACKETSIZE              1024        // max size of pkgsize for user message
#else  _DEBUG
#define MAX_THREAD                  64
#define MAX_RECEIVE                 32
#define MAX_CONNECT                 8
#define MAX_RECVLIST                64
#define MAX_TIMEWAIT                20
#define MAX_HANDLE                  8
#define MAX_PACKETSIZE              1024*512
#endif _DEBUG

#if MAX_THREAD < MAX_RECEIVE + 3                // MAX_THREAD MUST LARGE THAN MAX_RECEIVE 3 or MORE
  #error MAX_THREAD - MAX_RECEIVE > 3
#endif

#define THREAD_LOOP                 1
#define THREAD_SCHE                 2
#define THREAD_SEND                 3

#define MIN_HEAD                    (sizeof(STUMsgHead)) // size of common head
#define MAX_HEAD                    (MIN_HEAD + sizeof(STUMsgProperty2))
#define BUFFER_SIZE                 2048
#define MAX_SEMA                    64


typedef struct sOneConnect                      // save one connect client status
{
  MYINT             status;
  SOCKET            connectSocket;
}sOneConnect, *psOneConnect;

#define STATUS_OK                   0x2000
#define STATUS_BAD                  0x2001
#define STATUS_BUSY                 0x2002
#define STATUS_CLOSE                0x2003

typedef struct sAllConnect                      // save all status
{
  volatile MYINT    connectNumber;
  sOneConnect       connectInfo[MAX_CONNECT];
}sAllConnect, *psAllConnect;

typedef struct sSemaThread 
{
  char*             recvUserData;
  long              recvUserSize;
  DECLARE_SEMA(hSemaRecv);
  STUMsgProperty2   threadProperty;
}sSemaThread, *psSemaThread;

#define PACKET_HELLO                1
#define PACKET_LINK                 2
#define PACKET_SEND                 3
#define PACKET_MULTISEND            4
#define PACKET_USERONLY             5
#define PACKET_STATUS               6

#define MAX_PACKETTYPE              7
typedef struct STUMsgHead
{
  long              headID;
  long              headLength;
  long              headMulti;
  long              headUser;
}STUMsgHead, *pSTUMsgHead;

typedef struct sSemaPacket
{
  char*             pkgData;                    // pointer to user data
  MYINT             pkgLength;                  // USER data length, have been processed
  MYINT             nowLength;                  // TOTAL length from head, have been processed
  time_t            recvTime;                   // for recv, time recvd
  MYINT             recvLength;                 // for user buffer short than packet
  STUMsgHead        packetHead;            
  STUMsgProperty2   packetProperty;             // the two must together
  DECLARE_SEMA(hSemaSend);                      // wait this, sign it after send thread finish
  RESULT            sendResult;
}sSemaPacket, *psSemaPacket;

unsigned int __stdcall RecvThread(PVOID pM);
unsigned int __stdcall LoopThread(PVOID pM);
unsigned int __stdcall ScheThread(PVOID pM);
unsigned int __stdcall SendThread(PVOID pM);

#define IntoClassFunction(func)                                                       \
  unsigned int __stdcall func(PVOID pM)                                               \
  {                                                                                   \
    ProcessMr* pMr = (ProcessMr*) pM;                                                 \
    pMr->func();                                                                      \
    return 0;                                                                         \
  }

template <class T> class LockList
{
private:
  DECLARE_CRIT(Lock);
public:
  std::list<T> List;
  LockList() { DEFINE_CRIT(Lock); };
  ~LockList() { FREE_CRIT(Lock); };

  RESULT SetList(T &p);
  RESULT GetList(T &p);
  RESULT GetList(T &p, STUMsgProperty2 *prop);      // for psSemaThread only
  RESULT Remove(T &it);
};

class ProcessMr
{
private:
 	time_t tNowTime;

  LockList<psSemaThread> ThreadList;
  LockList<sSemaPacket> SendList;
  LockList<sSemaPacket> RecvList;

  sAllConnect ConnectSocket;
  //SOCKET connectSocket;
  HANDLE hThreadRecv[MAX_RECEIVE];
  HANDLE hThreadLoop;
  HANDLE hThreadSche;
  HANDLE hThreadSend;


  ThreadStep Step;
  RESULT     initResult;                // if error in init, set to error
  DECLARE_SEMA(SendSign);
  DECLARE_CRIT(SendCrit);
  DECLARE_SEMA(ScheSign);

  const STUConnInfo2* conninfo;         // temp save in init
  sSemaPacket connectPacket;            // save the Init STU info, set to PACKET_LINK

  sSemaPacket lastRecvProp;        // send & recv have different Prop, record the status of last packet
  sSemaPacket lastSendProp;


public:
  ProcessMr();
  RESULT InitConnect(const char* psAppID, const char* psAppPasswd, 
                     const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, int iThreadCount);
  RESULT ProcessPacket(char* buffer, int length, int &processed, psSemaPacket packet);
  ~ProcessMr(void);

  RESULT IsLinkOK(void);
  int Mr2Send(const char* psPkg, int iPkgLen, STUMsgProperty2* pMsgPropery, int iMillSecTimeo);
  int ProcessMr::Mr2Receive(char** ppsPkg, int* piOutPkgLen, STUMsgProperty2* pMsgPropery, int iMillSecTimeo);

  void RecvThread(void);
  void LoopThread(void);
  void ScheThread(void);
  void SendThread(void);
};


typedef struct AppIDKey
{
  char                AppID[MR2_MAXLEN_ADDR];
}AppIDKey, *pAppIDKey;

typedef struct SocketMap
{
  SOCKET              ConnectSocket;
  MYINT               blank1;
  MYINT               blank2;
  MYINT               blank3;
  MYINT               blank4;
  STUMsgProperty2     MsgStruct;
}SocketMap, *pSocketMap;

typedef std::map<std::string, SocketMap*> MapStruct;
typedef std::map<std::string, SocketMap*>::iterator MapItem;


inline BOOL GlobalUseFDEP() { return TRUE; };         // if false, never use fdep; if true, detect more.
inline BOOL AppIDUseFDEP(const char *appid) { return GlobalUseFDEP() ? FALSE : FALSE; };
inline BOOL MemoryUseFDEP(void *mem) { return GlobalUseFDEP() ? FALSE : FALSE; };
inline BOOL HandleIDUseFDEP(void* pHandle) { return GlobalUseFDEP() ? FALSE : FALSE; };









