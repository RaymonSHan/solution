// testdll.cpp : 定義主控台應用程式的進入點。
//

#include <stdio.h>
#include <Windows.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


typedef char* (WINAPI T_TechOcr)(char*, char*);

T_TechOcr* TechOcr;

int main(int argc, char* argv[])
{
	
	HINSTANCE m_checkDll = ::LoadLibrary("TechOcrDll.dll") ;
	if (m_checkDll == NULL)
	{
		printf("error: can not find (mrapi.dll)");
		return -1 ;
	}
	TechOcr = (T_TechOcr *)::GetProcAddress(m_checkDll, "TechOcr") ;
	if (TechOcr == NULL)
	{
		printf("do not found dll\r\n");
		::FreeLibrary(m_checkDll) ;
		m_checkDll = NULL ;
		return -1 ;
  }
  char *buf = TechOcr("a", "b");
  printf("%s\r\n", buf);

}

