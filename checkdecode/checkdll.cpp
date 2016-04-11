// dllmain.cpp : 定義 DLL 應用程式的進入點。
#include "checkdll.h"

#define MAX_REQ     16

char AllCheck[MAX_REQ][MAX_RECT];
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

char* jstringTostring(JNIEnv* env, jstring jstr)
{
  char* rtn = NULL;
  jclass clsstring = env->FindClass("java/lang/String");
  jstring strencode = env->NewStringUTF("utf-8");
  jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
  jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
  jsize alen = env->GetArrayLength(barr);
  jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
  if (alen > 0)
  {
    rtn = (char*)malloc(alen + 1);

    memcpy(rtn, ba, alen);
    rtn[alen] = 0;
  }
  env->ReleaseByteArrayElements(barr, ba, 0);
  return rtn;
}

jstring stoJstring(JNIEnv* env, const char* pat)
{
  jclass strClass = env->FindClass("Ljava/lang/String;");
  jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
  jbyteArray bytes = env->NewByteArray(strlen(pat));
  env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
  jstring encoding = env->NewStringUTF("utf-8");
  return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
}


char* WINAPI GetCheck(char* filename)
{
  CheckDecode decode;
  nowreq = (nowreq + 1) % MAX_REQ;      // lazy for not use InterLock
  char* checked = &AllCheck[nowreq][0];
  RESULT result;
  result = decode.Process(filename);
  if (result == RESULT_OK)
  {
    result = decode.GetResult(checked);
    return checked;
  }
  else
  {
    return NULL;
  }
};

JNIEXPORT jstring JNICALL Java_com_nasoft_diper_qrutil_ImageScanForC_GetCheck
  (JNIEnv * env, jclass jclassname, jstring filename)
{
  char* filein = jstringTostring(env, filename);
  char* output = GetCheck(filein);
  if (filein) free(filein);
  return stoJstring(env, output);
};
