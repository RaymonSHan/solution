// stdafx.h : 可在此標頭檔中包含標準的系統 Include 檔，
// 或是經常使用卻很少變更的
// 專案專用 Include 檔案
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 從 Windows 標頭排除不常使用的成員
// Windows 標頭檔:
#include <windows.h>

#include "jni.h"
#include <stdio.h>
#include "cv.h"
#include <highgui.h>
#include "rcommon.h"

extern "C" {
	char* WINAPI TechOcr(char* imgtype, char* filename);
	JNIEXPORT jstring JNICALL Java_net_gbicc_xbrl_otc_util_ImageScanForC_TechOcr
        (JNIEnv *, jclass, jstring, jstring);
}

#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "giflib.lib")
#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "openjpeg.lib")
#pragma comment(lib, "libtiff.lib")
#pragma comment(lib, "liblept.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "libtesseract.lib")

#ifdef _DEBUG
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#else _DEBUG
#pragma comment(lib, "opencv_core2411.lib")
#pragma comment(lib, "opencv_imgproc2411.lib")
#pragma comment(lib, "opencv_highgui2411.lib")
#endif

// TODO: 在此參考您的程式所需要的其他標頭
