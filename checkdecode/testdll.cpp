// testdll.cpp : 定義主控台應用程式的進入點。
//

#include <stdio.h>
#include <Windows.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


typedef char* (WINAPI T_GetCheck)(char*);

T_GetCheck* GetCheck;

int main(int argc, char* argv[])
{
	
	HINSTANCE m_checkDll = ::LoadLibrary("checkdll.dll") ;
	if (m_checkDll == NULL)
	{
		printf("error: can not find (mrapi.dll)");
		return -1 ;
	}
  GetCheck = (T_GetCheck *)::GetProcAddress(m_checkDll, "GetCheck") ;
	if (GetCheck == NULL)
	{
		printf("do not found dll\r\n");
		::FreeLibrary(m_checkDll) ;
		m_checkDll = NULL ;
		return -1 ;
  }
  char *buf = GetCheck(argv[1]);

}

