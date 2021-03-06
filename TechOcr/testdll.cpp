// testdll.cpp : 定義主控台應用程式的進入點。
//

#include <stdio.h>
#include <Windows.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

typedef char* (WINAPI T_TechOcr)(char*, char*);
typedef char* (WINAPI T_OcrTest)(char*);

T_TechOcr* TechOcr;
T_OcrTest* OcrTest;

long UTF8toGB2312(char* utf8, long usize, char*& gb2312, long &gsize) {
	long unicodelen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, usize, NULL, 0);
	WCHAR *strSrc = new WCHAR[unicodelen + 16];
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, usize, strSrc, unicodelen);

	gsize = WideCharToMultiByte(CP_ACP, 0, strSrc, unicodelen, NULL, 0, NULL, NULL);
	gb2312 = new char[gsize + 16];
	WideCharToMultiByte(CP_ACP, 0, strSrc, unicodelen, (LPSTR)gb2312, gsize, NULL, NULL);
	gb2312[gsize] = 0;
	delete[]strSrc;
	return 0;
}

 #define LOADFILE "z:\\solution\\files\\b\\b31.jpg"
// #define LOADFILE "z:\\solution\\files\\b\\b13.jpg"
// #define LOADFILE "z:\\solution\\files\\t.bmp"
// #define LOADFILE "z:\\solution\\files\\w2.jpg"
//  #define LOADFILE "z:\\solution\\files\\cl.jpg"
// #define LOADFILE "z:\\solution\\files\\p1.jpg"
// #define LOADFILE "z:\\solution\\files\\p\\p19.jpg"
// #define LOADFILE "z:\\solution\\files\\pn1.jpg"
// #define LOADFILE "z:\\solution\\files\\p\\p08.jpg"


int main(int argc, char* argv[]) {
	char *filename;
	char *funcname;

#ifdef _DEBUG
	filename = LOADFILE;
	funcname = argv[1];
#else  _DEBUG
	filename = argv[1];
	funcname = "OcrTest";
#endif _DEBUG

// following for all
	HINSTANCE m_checkDll = ::LoadLibrary("TechOcrDll.dll") ;
	if (m_checkDll == NULL)	{
		return -1 ;
	}
	if (!strcmp(funcname, "OcrTest")) {
		OcrTest = (T_OcrTest *)::GetProcAddress(m_checkDll, "OcrTest");
		if (OcrTest == NULL) {
			return -1;
		}
		char *ret = OcrTest(filename);
		return 0;
	}

	if (!strcmp(funcname, "TechOcr")) {
		TechOcr = (T_TechOcr *)::GetProcAddress(m_checkDll, "TechOcr");
		if (TechOcr == NULL) {
			printf("do not found dll\r\n");
			::FreeLibrary(m_checkDll);
			m_checkDll = NULL;
			return -1;
		}
		char *buf = 0;
		char *gb2312 = 0;
		long gsize;
		buf = TechOcr("SHOULDDISPLAY", filename);
		if (buf) {
			UTF8toGB2312(buf, strlen(buf), gb2312, gsize);
		}
	 	printf("%s\r\n", gb2312);

	}
}
