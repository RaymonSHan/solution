
#include "mr2dll.h"

MYINT ShouldQuit = 0;       // global quit sign, set to nozero for quit

RESULT InitSocket(void)
{
  WSADATA wsaData;

  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != NO_ERROR)
    return RESULT_WSA_ERROR;
  else
    return RESULT_OK;
}

template <class T> RESULT LockList<T>::SetList(T &p)
{
  LOCK_CRIT(Lock);
  List.push_back(p);
  UNLOCK_CRIT(Lock);
  return RESULT_OK;
}

template <class T> RESULT LockList<T>::GetList(T &p)
{
  RESULT ret = RESULT_OK;

  LOCK_CRIT(Lock);
  if (List.empty())
  {
    ret = RESULT_EMPTY;
  }
  else
  {
    p = List.front();
    List.pop_front();
  }
  UNLOCK_CRIT(Lock);
  return ret;
}

template <class T> RESULT LockList<T>::Remove(T &it)
{
  RESULT ret = RESULT_OK;

  LOCK_CRIT(Lock);
  List.remove(it);
  UNLOCK_CRIT(Lock);
  return RESULT_OK;
}

#define IS_EQUAL(field, len)       if (tprop->field[0] && strncmp(tprop->field, pprop->field, len)) return RESULT_NOT_EQUAL;

RESULT CompareProp(STUMsgProperty2 *tprop, STUMsgProperty2 *pprop)
{
  IS_EQUAL(m_szSourceUserID, MR2_MAXLEN_ADDR);
  IS_EQUAL(m_szSourceAppID, MR2_MAXLEN_ADDR);
  IS_EQUAL(m_szPkgID, MR2_MAXLEN_PKGID);
  IS_EQUAL(m_szUserData1, MR2_MAXLEN_USERDATA);
  IS_EQUAL(m_szUserData1, MR2_MAXLEN_USERDATA);
//  if (!tprop->m_szCorrPkgID[0]) return RESULT_OK;
  if (!strcmp(tprop->m_szCorrPkgID, "<EMPTY>") && !pprop->m_szCorrPkgID[0]) return RESULT_OK;
  if (!strcmp(tprop->m_szCorrPkgID, "<NOEMPTY>") && pprop->m_szCorrPkgID[0]) return RESULT_OK;
  IS_EQUAL(m_szCorrPkgID, MR2_MAXLEN_PKGID);
  return RESULT_OK;
}

template <class T> RESULT LockList<T>::GetList(T &it, STUMsgProperty2 *pprop)
// for psSemaThread only
{
  STUMsgProperty2* tprop;
  psSemaThread pthread;
  std::list<psSemaThread>::iterator pit, piter;

  for (pit = List.begin(); pit != List.end();)
  {
    pthread = *pit;
    piter = pit++;   // see http://blog.sina.com.cn/s/blog_782496390100rtyp.html
    tprop = &pthread->threadProperty;
    if (CompareProp(tprop, pprop) == RESULT_OK)
    {
      List.erase(piter);
      it = pthread;
      return RESULT_OK;
    }
  }
  return RESULT_NOT_EQUAL;
}

ProcessMr::ProcessMr()
{
  memset(&ConnectSocket, 0, sizeof(ConnectSocket));
  time( &tNowTime );
  initResult = RESULT_OK;
  SendAfterConnect = true;

  DEFINE_SEMA(SendSign, 0, MAX_THREAD);
  DEFINE_CRIT(SendCrit);                  // for user thread use, do send serial
  DEFINE_SEMA(ScheSign, 0, MAX_RECVLIST * 16);
}

RESULT ProcessMr::InitConnect(const char* psAppID, const char* psAppPasswd, 
                              const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, int iThreadCount)
{
  STUMsgProperty2 prop;

  conninfo = pArrConnInfo;
  memset(&prop, 0, sizeof(STUMsgProperty2));
  strncpy_s(prop.m_szDestAppID, psAppID, MR2_MAXLEN_ADDR);

  GenPacket(connectPacket, PACKET_LINK, NULL, 0, &prop);
//   memset(&connectPacket, 0, sizeof(sSemaPacket));
//   connectPacket.packetHead.headID = PACKET_LINK;
//   connectPacket.packetHead.headLength = sizeof(STUMsgHead) + sizeof(STUMsgProperty2);
//   connectPacket.packetHead.headMulti = 0;
//   connectPacket.packetHead.headUser = 0;

  UNLOCK_STEP(Step, 0);
  for (int i = 0; i < iArrConnInfoCount; i++)
  {
    hThreadRecv[i] = (HANDLE)_beginthreadex(NULL, 0, ::RecvThread, this, 0, NULL);
  }
  //  wait for all recv thread finish init
  WAIT_STEP(Step, iArrConnInfoCount, MAX_RECEIVE + THREAD_LOOP);
  hThreadLoop = (HANDLE)_beginthreadex(NULL, 0, ::LoopThread, this, 0, NULL);  
  hThreadSche = (HANDLE)_beginthreadex(NULL, 0, ::ScheThread, this, 0, NULL);
  hThreadSend = (HANDLE)_beginthreadex(NULL, 0, ::SendThread, this, 0, NULL);  

  return RESULT_OK;
}

ProcessMr::~ProcessMr(void)
{
  return;
}

RESULT ProcessMr::GenPacket(sSemaPacket &packet, long packetType, const char* psPkg, int iPkgLen, STUMsgProperty2* pMsgPropery)
{
  memset(&packet, 0, sizeof(sSemaPacket));
  if (psPkg && (packetType == PACKET_SEND || packetType == PACKET_USERONLY))
  {
    packet.pkgData = (char*)psPkg;
    packet.pkgLength = iPkgLen;
    packet.packetHead.headUser = iPkgLen;
  }
  switch (packetType)
  {
  case PACKET_HELLO :
  case PACKET_USERONLY :
    packet.packetHead.headLength = sizeof(STUMsgHead);        // MUST be 0x10
    break;
  case PACKET_LINK :
  case PACKET_SEND :
    packet.packetHead.headLength = sizeof(STUMsgHead) + sizeof(STUMsgProperty2);
    if (pMsgPropery)
    {
      memcpy(&packet.packetProperty, pMsgPropery, sizeof(STUMsgProperty2));
    }
    break;
  case PACKET_MULTISEND :
  case PACKET_STATUS :
  default:
    return RESULT_ERR;
  }

  packet.packetHead.headID = packetType;
  return RESULT_OK;
}

void ProcessMr::RecvThread(void)
{
  int ret;
  const STUConnInfo2* addrs;
  sOneConnect* infos;
  sockaddr_in connectAddr;
  MYINT order = InterInc(&ConnectSocket.connectNumber) - 1;
  char* remainchar;
  int remainsize;
  int processed;

  RESULT result;

  sSemaPacket processPacket;

  char buffer [BUFFER_SIZE];
  int length = BUFFER_SIZE;


  addrs = conninfo + order;
  connectAddr.sin_family = AF_INET;
  connectAddr.sin_addr.s_addr = inet_addr(addrs->m_szMRIP);
  connectAddr.sin_port = htons(addrs->m_usMRPort);

  infos = &ConnectSocket.connectInfo[order];
  infos->connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (infos->connectSocket == INVALID_SOCKET) 
    initResult = RESULT_WSA_ERROR;

  LOCK_STEP(Step, order);
  while (initResult == RESULT_OK && !ShouldQuit)          // if error ahead, do NOTHING
  {
    do 
    {
      ret = connect(infos->connectSocket, (SOCKADDR*) &connectAddr, sizeof(connectAddr));
      if (ret != SOCKET_ERROR)
      {
        infos->status = STATUS_OK;
// normally send first packet after connect
        if (SendAfterConnect)
        {
          send(infos->connectSocket, (char*)&connectPacket.packetHead, 
                sizeof(STUMsgHead) + sizeof(STUMsgProperty2), 0);
        }
// normally end
      }
      UNLOCK_STEP(Step, order + 1);   // connect CLIENT in order, one by one
      if (ShouldQuit)
      {
        infos->status = STATUS_CLOSE;
        break;
      }
      if (ret == SOCKET_ERROR)
      {
        Sleep(15*1000);
      }
    } 
    while (ret == SOCKET_ERROR);    // reconnect again

    memset(&processPacket, 0, sizeof(sSemaPacket));
    memset(&lastRecvProp, 0, sizeof(sSemaPacket));
    memset(&lastSendProp, 0, sizeof(sSemaPacket));

    do
    {
      ret = recv(infos->connectSocket, buffer, length, 0);
      remainchar = buffer;
      remainsize = ret;

      while (remainsize > 0)    // have received more, another packet have begin
      {
        processed = 0;
        result = ProcessPacket(remainchar, remainsize, processed, &processPacket);
        if (result == RESULT_NOT_SUPPORT)   // error, close socket
        {
continue;     // test for continue
          infos->status = STATUS_CLOSE;
          closesocket(infos->connectSocket);
          infos->connectSocket = 0;
          ret = 0;
          break;        // to reconnect socket
        }
        if (result == RESULT_OK)    // packet completed
        {
          if (processPacket.packetHead.headID == PACKET_SEND)   // record last packet prop
          {
            memcpy(&lastRecvProp.packetProperty, &processPacket.packetProperty, sizeof(STUMsgProperty2));
          }
          if (processPacket.packetHead.headID == PACKET_USERONLY) // copy last packet prop to new packet
          {
            memcpy(&processPacket.packetProperty, &lastRecvProp.packetProperty, sizeof(STUMsgProperty2));
          }
          RecvList.SetList(processPacket);
          UNLOCK_SEMA(ScheSign);      // begin sche thread
          memset(&processPacket, 0, sizeof(sSemaPacket));
        }
        remainchar += processed;
        remainsize -= processed;
      }
    } 
    while (ret > 0 /* or no error */);   // normal received, loop again
  }
  return;
}

void ProcessMr::LoopThread(void)
{
  LOCK_STEP(Step, MAX_RECEIVE + THREAD_LOOP);
  // init thread
  UNLOCK_STEP(Step, MAX_RECEIVE + THREAD_SCHE);

  while (initResult == RESULT_OK && !ShouldQuit)          // if error ahead, do NOTHING
  {
    time( &tNowTime );
    Sleep(500);
  }


}

void ProcessMr::ScheThread(void)
{
  std::list<sSemaPacket>::iterator it, iter;
  sSemaPacket packet;
  psSemaThread pthread;
  RESULT result;

  LOCK_STEP(Step, MAX_RECEIVE + THREAD_SCHE);
  // init thread
  UNLOCK_STEP(Step, MAX_RECEIVE + THREAD_SEND);

  while (initResult == RESULT_OK && !ShouldQuit)          // if error ahead, do NOTHING
  {
    LOCK_SEMA(ScheSign);
    //  add wait loop for release more sign

    if (ThreadList.List.empty())
    {
      continue;
    }
    for (it = RecvList.List.begin(); it != RecvList.List.end();)
    {
      packet = *it;
      iter = it++;   // see http://blog.sina.com.cn/s/blog_782496390100rtyp.html

      if (tNowTime - packet.recvTime > MAX_TIMEWAIT)
      {
        RecvList.List.erase(iter);
        continue;
      }

      result = ThreadList.GetList(pthread, &packet.packetProperty);   // this line remove the current thread from list
      if (result != RESULT_OK) 
        continue;
      if (pthread->recvUserData && pthread->recvUserSize)
      {
        if (pthread->recvUserSize >= packet.pkgLength - packet.recvLength)
        {
          memcpy(pthread->recvUserData, packet.pkgData + packet.recvLength, packet.pkgLength - packet.recvLength);
          pthread->recvUserSize = packet.pkgLength;
          iter->recvLength = packet.pkgLength;
          packet.recvLength = packet.pkgLength;
          RecvList.List.erase(iter);
        }
        else
        {
          memcpy(pthread->recvUserData, packet.pkgData + packet.recvLength, pthread->recvUserSize);
          pthread->recvUserSize = pthread->recvUserSize;
          iter->recvLength += pthread->recvUserSize;
          packet.recvLength += pthread->recvUserSize;
        }
//  should change, for use user buffer, but user buffer may less than necessary
      }
      else
      {
        // use system buffer
        pthread->recvUserData = packet.pkgData;
        pthread->recvUserSize = packet.pkgLength;
        RecvList.List.erase(iter);
      }
      UNLOCK_SEMA(pthread->hSemaRecv);
    }
  }
  return;
}

void ProcessMr::SendThread(void)
{
  LOCK_STEP(Step, MAX_RECEIVE + THREAD_SEND);
  // init thread
  UNLOCK_STEP(Step, STEP_END);
  int i;
  bool sameprop;

  sSemaPacket packet;
  RESULT result;

  GenPacket(lastSendProp, PACKET_USERONLY, NULL, 0, NULL);
//   memset(&lastSendProp, 0, sizeof(sSemaPacket));
//   lastSendProp.packetHead.headID = PACKET_USERONLY;
//   lastSendProp.packetHead.headLength = sizeof(STUMsgHead);
//   lastSendProp.packetHead.headMulti = 0;
  while (initResult == RESULT_OK && !ShouldQuit)          // if error ahead, do NOTHING
  {
    LOCK_SEMA(SendSign);    // wait user program send data
    LOCK_CRIT(SendCrit);    // only one thread do send, do NOT use CRIT, but DoubleSendList may have only one
    result = SendList.GetList(packet);
    if (result == RESULT_OK)
    {
      packet.sendResult = RESULT_WSA_ERROR;

      if (memcmp(&lastSendProp.packetProperty, &packet.packetProperty, sizeof(STUMsgProperty2)))
      {
        memcpy(&lastSendProp.packetProperty, &packet.packetProperty, sizeof(STUMsgProperty2));
        sameprop = false;
      }
      else
      {
        lastSendProp.packetHead.headUser = packet.packetHead.headUser;
        sameprop = true;
      }

      for (i = 0; i < ConnectSocket.connectNumber; i++)
      {
        if (ConnectSocket.connectInfo[i].status != STATUS_OK) continue;

        SOCKET so = ConnectSocket.connectInfo[i].connectSocket;

//      char testpkg[200];    // test sample
//      send(so, testpkg, 3, 0);
//      send(so, ((char*)&packet.packetHead)+3, sizeof(STUMsgHead) + sizeof(STUMsgProperty2) +3 , 0);      // detect send
//      send(so, packet.pkgData, packet.pkgLength-6, 0);                            // detect send
        
//      send(so, (char*)&packet.packetHead, sizeof(STUMsgHead) + sizeof(STUMsgProperty2)+3, 0);      // detect send
//      send(so, packet.pkgData, packet.pkgLength-3, 0);                            // detect send

//      send(so, (char*)&packet.packetHead, sizeof(STUMsgHead) + sizeof(STUMsgProperty2)-3, 0);      // detect send
//      memcpy(&testpkg[3], packet.pkgData, packet.pkgLength);
//      send(so, testpkg, packet.pkgLength+3, 0);                            // detect send

        // if same prop, send small head
        if (!sameprop)
        {
          send(so, (char*)&packet.packetHead, sizeof(STUMsgHead) + sizeof(STUMsgProperty2), 0);      // different head
        }
        else
        {
          send(so, (char*)&lastSendProp.packetHead, sizeof(STUMsgHead), 0);      // detect send
        }
//        send(so, (char*)&packet.packetHead, sizeof(STUMsgHead) + sizeof(STUMsgProperty2), 0);      // same head
        send(so, packet.pkgData, packet.pkgLength, 0);                            // detect send
        packet.sendResult = RESULT_OK;
      }
    }
    UNLOCK_CRIT(SendCrit);
    UNLOCK_SEMA(packet.hSemaSend);
  }
  return;
}

IntoClassFunction(ProcessMr, RecvThread);
IntoClassFunction(ProcessMr, LoopThread);
IntoClassFunction(ProcessMr, ScheThread);
IntoClassFunction(ProcessMr, SendThread);

RESULT ProcessMr::IsLinkOK(void)
{
  for (int i = 0; i < ConnectSocket.connectNumber; i++)
  {
    if (ConnectSocket.connectInfo[i].status == STATUS_OK)
      return RESULT_OK;
  }
  return RESULT_WSA_ERROR;
}

int ProcessMr::Mr2Send(const char* psPkg, int iPkgLen, STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  sSemaPacket packet;

//   packet.pkgData = (char*)psPkg;
//   packet.pkgLength = iPkgLen;
//   packet.nowLength = 0;
//   packet.packetHead.headID = PACKET_SEND;
//   packet.packetHead.headLength = MAX_HEAD;
//   packet.packetHead.headMulti = 0;
//   packet.packetHead.headUser = iPkgLen;
//   memcpy(&packet.packetProperty, pMsgPropery, sizeof(STUMsgProperty2));
  GenPacket(packet, PACKET_SEND, psPkg, iPkgLen, pMsgPropery);
  return Mr2Send(&packet, iMillSecTimeo);
}

int ProcessMr::Mr2Send(psSemaPacket packet, int iMillSecTimeo)
{
  DEFINE_SEMA(packet->hSemaSend, 0, MAX_SEMA);
  SendList.SetList(*packet);

  UNLOCK_SEMA(SendSign);
  LOCK_SEMA(packet->hSemaSend);
  FREE_SEMA(packet->hSemaSend);
  return 0;
}

int ProcessMr::Mr2Receive(char** ppsPkg, int* piOutPkgLen, STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  sSemaThread mthread;
  psSemaThread pthread = &mthread;

  if (*ppsPkg && *piOutPkgLen)
  {
    mthread.recvUserData = *ppsPkg;      // use user buffer
    mthread.recvUserSize = *piOutPkgLen;
  }
  else
  {
    mthread.recvUserData = 0;      // use system buffer, set to 0
    mthread.recvUserSize = 0;
  }
  memcpy(&mthread.threadProperty, pMsgPropery, sizeof(STUMsgProperty2));
  DEFINE_SEMA(mthread.hSemaRecv, 0, 1);        // only set once !!!!! should check when Release
  ThreadList.SetList(pthread);

  UNLOCK_SEMA(ScheSign);      // begin sche thread
  LOCK_SEMA(mthread.hSemaRecv);
  
  // if timeout, remove thread

  *ppsPkg = mthread.recvUserData;
  *piOutPkgLen = mthread.recvUserSize;
//  ThreadList.Remove(pthread);     remove should in sche, just after found match thread

  return 0;
}


/*
typedef struct sSemaPacket
{
  const char*       pkgData;                    // pointer to user data
  MYINT             pkgLength;                  // USER data length, have been processed
  MYINT             nowLength;                  // TOTAL length from head, have been processed
  time_t            recvTime;                   // for recv, time recvd
  STUMsgHead        packetHead;            
  STUMsgProperty2   packetProperty;             // the two must together
  DECLARE_SEMA(hSemaSend);                      // wait this, sign it after send thread finish
  RESULT            sendResult;
}sSemaPacket, *psSemaPacket;
*/

// if packet complete, return RESULT_OK, 
RESULT ProcessMr::ProcessPacket(char* buffer, int length, int &processed, psSemaPacket packet)
{
  char* nowstart;
  nowstart = (char*)&packet->packetHead;
  nowstart += packet->nowLength;                 // the place in last packet for head
  processed = 0;
  int processuser, processhead;

  int realheadsize;
  int realusersize;
  int packettype;

  if (length + packet->nowLength <= MIN_HEAD)     // head have not finish, do not konw head size and user size
  {
    memcpy(nowstart, buffer, length);
    processed += length;
    packet->nowLength += length;
    return RESULT_CONTINUE;
  }
  if (packet->nowLength < MIN_HEAD)   // first time process min head
  {
    processhead = MIN_HEAD - packet->nowLength;
    memcpy(nowstart, buffer, processhead);
    processed += processhead;
    packet->nowLength = MIN_HEAD;
    nowstart += processhead;

    buffer += processhead;
    length -= processhead;
  }
  realheadsize = packet->packetHead.headLength;
  realusersize = packet->packetHead.headUser;
  packettype = packet->packetHead.headID;

  if ((realheadsize != MAX_HEAD && realheadsize != MIN_HEAD) || 
      realusersize > MAX_PACKETSIZE ||
      packettype >= MAX_PACKETTYPE)
  {
    return RESULT_NOT_SUPPORT;    // now receive packet have fix size head
  }
  if (packet->nowLength < realheadsize || realheadsize == MIN_HEAD)   // should copy head into STUMsgHead & STUMsgProperty2
  {
    processhead = min(realheadsize - packet->nowLength, length);
    memcpy(nowstart, buffer, processhead);
    processed += processhead;
    packet->nowLength += processhead;

    buffer += processhead;
    length -= processhead;
    if (packet->nowLength < realheadsize)   // have not finished
    {
      return RESULT_CONTINUE;
    }
    if (realusersize)
    {
      packet->pkgData = (char*)malloc(realusersize);
    }
    else    // head only, this packet finish
    {
      return RESULT_NULL;
    }
  }
  if (packet->pkgLength + length < realusersize)    // have user data, but not finish
  {
    memcpy(packet->pkgData + packet->pkgLength, buffer, length);
    packet->pkgLength += length;
    packet->nowLength += length;
    processed += length;
    return RESULT_CONTINUE;
  }
  processuser = realusersize - packet->pkgLength;
  processed += processuser;    // to end of this packet
  memcpy(packet->pkgData + packet->pkgLength, buffer, processuser);
  packet->pkgLength += processuser;
  packet->nowLength += processuser;
  packet->recvTime = tNowTime;
  return RESULT_OK;
}


void* WINAPI MAIN_C(Mr2Init)(const char* psAppID, const char* psAppPasswd, 
                      OnReceiveCallBack2 onReceive,const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
  void* pvUserData)
{
  return 0;
}

void WINAPI MAIN_C(Mr2Init2)(void** ppHandle, const char* psAppID, const char* psAppPasswd, 
                      STUMsgProperty2* pOnRecvMsgPropery, OnReceiveCallBack2 onReceive, 
                      const STUConnInfo2* pArrConnInfo, int iArrConnInfoCount, 
                      void* pvUserData, int iThreadCount)
{
  RESULT result;
  ProcessMr *mr = NULL;

  if (iArrConnInfoCount)
  {
    mr = new ProcessMr();
    result = mr->InitConnect(psAppID, psAppPasswd, pArrConnInfo, iArrConnInfoCount, iThreadCount);
    if (result != RESULT_OK) 
    {
      delete mr;
      mr = NULL;
    }
  }
  if (ppHandle) *ppHandle = mr;
  return;
}

int WINAPI MAIN_C(Mr2Send)(void* pHandle, const char* psPkg, int iPkgLen, STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  ProcessMr *mr = (ProcessMr*) pHandle;
  RESULT result = mr->Mr2Send(psPkg, iPkgLen, pMsgPropery, iMillSecTimeo);
  return result;
}

int WINAPI MAIN_C(Mr2IsLinkOK)(void* pHandle)
{
  ProcessMr *mr = (ProcessMr*) pHandle;
  RESULT result = mr->IsLinkOK();
  if (result == RESULT_OK) return 1;
  else return 0;
}

int WINAPI MAIN_C(Mr2Receive1)(void* pHandle, char** ppsPkg, int* piOutPkgLen, STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  ProcessMr *mr = (ProcessMr*) pHandle;
  *ppsPkg = 0;
  *piOutPkgLen = 0;
  RESULT result = mr->Mr2Receive(ppsPkg, piOutPkgLen, pMsgPropery, iMillSecTimeo);
  return result;
}

void WINAPI MAIN_C(Mr2Receive1_FreeBuf)(char* psPkg)
{
  free(psPkg);
  return;
}

int WINAPI MAIN_C(Mr2Receive2)(void* pHandle, char* psPkg, int* piOutPkgLen, int iBufLenIn, 
                           STUMsgProperty2* pMsgPropery, int iMillSecTimeo)
{
  ProcessMr *mr = (ProcessMr*) pHandle;
  *piOutPkgLen = iBufLenIn;   //  for use same function
  RESULT result = mr->Mr2Receive(&psPkg, piOutPkgLen, pMsgPropery, iMillSecTimeo);
  return result;
}



/*
http://blog.csdn.net/morewindows/article/details/7577591

received_thread_loop
{
  prepare_recieve(
    recv();
  ); // do head and body process
  match_receive();   // match appid and other

  wait(thread.hSemaThreadReady);
  set_semi(thread.hSemaThreadRecv); 
  wait_semi(hSemaRecv);
}

dll_recv()
{
  setbufferpara();
  set_semi(thread.hSemaThreadReady);

  wait_semi(thread.hSemaThreadRecv);
  getdata();
  set_semi(hSemaRecv);
}

dll_recv_callback()
{
  wait_semi(thread.hSemaThreadRecv);
  getdata();
  set_semi(hSemaRecv);
  callback();
}

send_thread_loop
{
  wait_semi(hSemaSend);
  if timeout send_hello();
  else
    if same_mr2() send_head();
    send();
    set_semi(hSemaSendOK);
}

dll_send
{
  entry(cri_s);
  setdata();
  set_semi(hSemaSend);
  wait_semi(hSemaSendOK);
  leave(cri_s);
}





received_thread_loop
{
  prepare_recieve(
    recv();
  ); // do head and body process
  addto_list(packet);
  set_semi(hSemaRecv); 
}

sche_thread_loop
{
  wait_semi(hSemaRecv, hThreadReady);
  match_recv();
  set_semi(hSema[thread]);
}

dll_recv()
{
  wait_semi(hSema[thread]);
  MARK_IN_PROCESS
  set_semi(hThreadReady);
}

dll_recv_callback()
{
  wait_semi(hSema[thread]);
  MARK_IN_PROCESS
  set_semi(hThreadReady);
  callback();
}

send_thread_loop
{
  wait_semi(hSemaSend);
  if timeout send_hello();
  else
    if same_mr2() send_head();
    send();
    set_semi(hSemaSendOK);
}

dll_send
{
  entry(cri_s);
  setdata();
  set_semi(hSemaSend);
  wait_semi(hSemaSendOK);
  leave(cri_s);
}
*/
