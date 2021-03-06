// dllmain.cpp : 定義 DLL 應用程式的進入點。
#include "checkdll.h"
#include "checkmore.h"

#define MAX_REQ     16

char AllCheck[MAX_REQ][MAX_RECT+MAX_RECT / 4];
int nowreq;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
    nowreq = 0;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
};

char* WINAPI GetCheck(char* filename)
{
//   CheckDecode decode, decoderot;
  CheckMore decode;

  nowreq = (nowreq + 1) % MAX_REQ;      // lazy for not use InterLock
  char* checked = &AllCheck[nowreq][0];
  RESULT result;
  result = decode.Process(filename, checked, false);
//   if (result != RESULT_OK)
//   {
//     result = decoderot.Process(filename, checked, true);
//   }

  return checked;
};

JNIEXPORT jstring JNICALL Java_com_nasoft_diper_qrutil_ImageScanForC_GetCheck
  (JNIEnv * env, jclass jclassname, jstring filename)
{
   const char* strfilename;  
   strfilename = env->GetStringUTFChars(filename, false);  
   if(strfilename == NULL)
   {  
       return NULL; /* OutOfMemoryError already thrown */  
   }  
   char* checked = GetCheck((char*)strfilename);
   env->ReleaseStringUTFChars(filename, strfilename);  

   jstring returnstr = env->NewStringUTF(checked);
   return returnstr;





//   char* tmpstr = "return string succeeded";  
//   jstring rtstr = env->NewStringUTF(tmpstr);  
//   return rtstr;  

  /*
  char* filein = jstringTostring(env, filename);
  char* output = GetCheck(filein);
  if (filein) free(filein);
  return stoJstring(env, output);
  */
//  return filename;
};
