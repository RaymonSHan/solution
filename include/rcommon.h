#pragma once

#include "windows.h"

#ifdef WIN64
#define     MYINT						        __int64
#define     RESULT                  __int64
#define     InterExgAdd				      InterlockedExchangeAdd64
#define     InterCmpExg	     			  InterlockedCompareExchange64
#define     InterExg				     	  InterlockedExchange64
#define     InterDec			     		  InterlockedDecrement64
#define     InterInc					      InterlockedIncrement64
#else WIN64
#define     MYINT						        long
#define     RESULT  		            long
#define     InterExgAdd	            InterlockedExchangeAdd
#define     InterCmpExg	            InterlockedCompareExchange
#define     InterExg                InterlockedExchange
#define     InterDec                InterlockedDecrement
#define     InterInc                InterlockedIncrement
#endif WIN64		

#define     RESULT_OK               0
#define     RESULT_FREE             1
#define     RESULT_CONTINUE         2           // packet have NOT complete
#define     RESULT_NULL             3           // no user data, system use only
#define     RESULT_NOT_SUPPORT      4
#define     RESULT_NOT_EQUAL        5

#define     RESULT_ERR              11
#define     RESULT_BUSY             12


#define     RESULT_WSA_ERROR        1001
#define     RESULT_DLLLOAD_ERROR    2001
#define     RESULT_THREAD_ERROR     3001
#define     RESULT_EMPTY            200001

#define     MARK_NOT_IN_PROCESS			(MYINT)0xa0
#define     MARK_IN_PROCESS				  (MYINT)0xa1
#define     MARK_UNDEFINE           (MYINT)0xcccccccc

#define     DEFINE_LOCK(lock)                                   \
  static volatile MYINT lock = MARK_NOT_IN_PROCESS;
#define     DECLARE_CLASS_LOCK(lock)                            \
  static volatile MYINT lock;
#define     DEFINE_CLASS_LOCK(cla, lock)                        \
  volatile MYINT cla::lock = MARK_NOT_IN_PROCESS;
#define     LOCK(lock)                                          \
  while ( InterCmpExg(&lock, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS ); 
#define     UNLOCK(lock)                                        \
  lock = MARK_NOT_IN_PROCESS;

#define     DECLARE_SEMA(lock)                                  \
  HANDLE lock
#define     DEFINE_SEMA(lock, start, max)                       \
  lock = CreateSemaphore(NULL, start, max, NULL);
#define     LOCK_SEMA(lock)                                     \
  WaitForSingleObject(lock, INFINITE);
#define     UNLOCK_SEMA(lock)                                   \
  ReleaseSemaphore(lock, 1, NULL);
#define     FREE_SEMA(lock)                                     \
  CloseHandle(lock)

#define     DECLARE_CRIT(lock)                                  \
  CRITICAL_SECTION lock;
#define     DEFINE_CRIT(lock)                                   \
  InitializeCriticalSection(&lock);
#define     LOCK_CRIT(lock)                                     \
  EnterCriticalSection(&lock);
#define     UNLOCK_CRIT(lock)                                   \
  LeaveCriticalSection(&lock);  
#define     FREE_CRIT(lock)                                     \
  DeleteCriticalSection(&lock);  

#define IntoClassFunction(claname, func)                        \
  unsigned int __stdcall func(PVOID pM)                         \
  {                                                             \
    claname* pMr = (claname*) pM;                               \
    pMr->func();                                                \
    return 0;                                                   \
  }

#define     MAX_STEP                16
#define     MAX_PARA                16

#define     STEP_START              (-1)
#define     STEP_END                (-2)

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class ThreadStep
{
private:
  MYINT status;
  DECLARE_SEMA(Lock)[MAX_STEP];
public:
  ThreadStep(void) 
  {
    status = RESULT_OK;
    for (int i = 0; i < MAX_STEP; i++ ) 
      DEFINE_SEMA(Lock[i], 0, MAX_PARA); 
  };
  void LockStep(MYINT step)
  {
    if (status == RESULT_FREE || step >= MAX_STEP) return;
    if (step != STEP_START)
    {
      LOCK_SEMA(Lock[step]);
    }
  };
  void UnlockStep(MYINT step)
  {
    if (step != STEP_END && step < MAX_STEP)
    {
      UNLOCK_SEMA(Lock[step]);
    }
    else
    {
      for (int i = 0; i < MAX_STEP; i++ ) 
        FREE_SEMA(Lock[i]);
      status = RESULT_FREE;
    }
  }
};

#define     DECLARE_STEP(lock)                                  \
  ThreadStep lock;
#define     LOCK_STEP(lock, step)                               \
  lock.LockStep(step);
#define     UNLOCK_STEP(lock, step)                             \
  lock.UnlockStep(step);
#define     WAIT_STEP(lock, lstep, ustep)                       \
  lock.LockStep(lstep);                                         \
  lock.UnlockStep(ustep);
#define     FREE_STEP(lock)                                     \
  lock.UnlockStep(STEP_END);

template<class T> inline void SWAP(T& left, T& right)
{
  T temp;
  temp = left;
  left = right;
  right = temp;
}



#define     DEFINE_COUNT(count)     static volatile MYINT count = 0;
#define     INC(count)              InterInc(&count);
#define     DEC(count)              InterDec(&count);

#define    _TOSTRING(x)             #x
#define     TOSTRING(x)            _TOSTRING(x)

#define    _JOIN(x,y)               x ## y
#define     JOIN(x,y)              _JOIN(x,y)

#define    _TO_MARK(x)              _rm_ ## x
#define    _rm_MarkMax              0xffffffff

#define     BEGINCALL
#define     ENDCALL
#define     SETLINE

#define	  __TRY                                                 \
  RESULT ret_err = __LINE__;					                          \
  BEGINCALL
#define   __MARK(x)                                             \
  static RESULT _TO_MARK(x) = ret_err = __LINE__;		            \
  SETLINE
#define   DEF_MARK(x)						                                \
  static RESULT _TO_MARK(x);
#define   __MARK_(x)						                                \
  _TO_MARK(x) = ret_err = __LINE__;				                      \
  SETLINE
#define   __CATCH_BEGIN                                         \
  ENDCALL							                                          \
  return RESULT_OK;			                                        \
error_stop:							                                        \
  ENDCALL							                                          \
  if (ret_err == RESULT_OK) return RESULT_OK;
#define   __BETWEEN(x,y)                                        \
  if (ret_err >= _TO_MARK(x) && ret_err <= _TO_MARK(y))
#define   __BEFORE(x)                                           \
  if (ret_err < _TO_MARK(x))
#define   __AFTER(x)                                            \
  if (ret_err >= _TO_MARK(x))
#define   __CATCH_END                                           \
  return ret_err;
#define   __CATCH                                               \
  ENDCALL							                                          \
  return RESULT_OK;						                                  \
error_stop:							                                        \
  ENDCALL							                                          \
  return ret_err;
#define   __CATCH_(ret)						                              \
  ENDCALL							                                          \
  return ret;							                                      \
error_stop:							                                        \
  ENDCALL							                                          \
  return RESULT_OK;
#define   __TRY__                                               \
  BEGINCALL
#define   __CATCH__                                             \
  ENDCALL							                                          \
  return RESULT_OK;
#define   __RETURN_(ret)					                              \
  ENDCALL							                                          \
  return ret;
#define   __BREAK                                               \
  { goto error_stop; }
#define   __BREAK_OK                                            \
  { ret_err = RESULT_OK; goto error_stop; }


#define     MESSAGE_INFO                        (1 << 0)
#define     MESSAGE_DEBUG                       (1 << 1)
#define     MESSAGE_ERROR                       (1 << 2)
#define     MESSAGE_HALT                        (1 << 3)

// void      __MESSAGE_(INT level, const char * _Format, ...);
// void      __MESSAGE (INT level);
// void      __TRACE();
#define     __MESSAGE_                    // lazy for ignore
#define     __MESSAGE                     // lazy for ignore

// #define     TRACE                             __TRACE();

#define   __INFO(level, _Format,...) {				                  \
    __MESSAGE_(level, _Format,##__VA_ARGS__);			              \
  };

#define   __DO1(val, func) {					                          \
    SETLINE							                                        \
    val = func;							                                    \
    if (val == NEGONE) {				                              	\
      WSASetLastError(errno);				                          	\
      __BREAK;				                                          \
    }								                                            \
  };

#define   __DOc_(func, _Format,...) {				                    \
    SETLINE							                                        \
    if (func)							                                      \
      __INFO(MESSAGE_INFO, _Format,##__VA_ARGS__);		          \
  };
#define   __DOc(func) {						                              \
    SETLINE							                                        \
    func;							                                          \
  };

#define   __DO(func) {						                              \
    SETLINE							                                        \
    if (func) {																									\
      __MESSAGE(MESSAGE_DEBUG);																	\
      __BREAK;																									\
    }																														\
  };

#define   __DOb(func) {																					\
    SETLINE                                               			\
    func;                                                       \
    __BREAK;                                                    \
  };

#define   __DOe(func, err) {					                          \
    SETLINE							                                        \
    if (func) {							                                    \
      ERROR(err);						                                    \
      __MESSAGE(MESSAGE_DEBUG);					                        \
      __BREAK;							                                    \
    }								                                            \
  };