// stdafx.h : 可在此標頭檔中包含標準的系統 Include 檔，
// 或是經常使用卻很少變更的
// 專案專用 Include 檔案
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 從 Windows 標頭排除不常使用的成員
// Windows 標頭檔:
#include <windows.h>
#include <iostream>

#include "jni.h"
#include <stdio.h>
#include "cv.h"
#include <highgui.h>


#include "allheaders.h"
#include "baseapi.h"
#include "basedir.h"
#include "renderer.h"
#include "strngs.h"
#include "tprintf.h"
#include "openclwrapper.h"
#include "osdetect.h"

#include "rcommon.h"

extern "C" {
	// for C user, MUST delete[] the return;
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

class TechOcr;
class OnceOcr;

#define MAX_CHAR      64
#define MAX_DETECT    100

typedef RESULT (*DetectDatum)(OnceOcr*, CvRect&, void*);

RESULT BusinessLicenseDatum(OnceOcr *ocr, CvRect &rect, void *futureuse);

typedef struct SourceItem {
	CvRect rectName;
	CvRect rectValue;
	tesseract::PageSegMode nameMode;
	tesseract::PageSegMode ValueMode;
	char shouldBe[MAX_CHAR];
	DetectDatum isDatumFunc;
	char isDatum;		// 4 bit for 4 conner
	bool isOutput;
}SourceItem, *pSourceItem;

typedef struct TargetItem {
	CvRect rectName;
	CvRect rectValue;
	char *strName;
	char *strValue;
}TargetItem, *pTargetItem;

class OnceOcr {
private:
	friend     RESULT BusinessLicenseDatum(OnceOcr *ocr, CvRect &rect, void *futureuse);
	Pix       *targetImage;
	UINT       targetNumber;
	TargetItem targetItem[MAX_CHAR];
	tesseract::TessBaseAPI* api;
public:
	RESULT InitOnceOcr();
	RESULT ExitOnceOcr();
	RESULT MapRect(UINT num);
	RESULT DoOcr();
	RESULT OutputOcr();
};

class ModelOcr {
private:
	UINT       sourctNumber;
	SourceItem *sourceItem;
public:
	ModelOcr();
	~ModelOcr();
	RESULT AddOcrItem(CvRect &name, CvRect &value, char* shouldbe = NULL, bool isoutput = true,
		tesseract::PageSegMode nmode = tesseract::PSM_SINGLE_WORD,
		tesseract::PageSegMode vmode = tesseract::PSM_SINGLE_LINE);
	RESULT AddOcrItem(DetectDatum isdatum, char datum = 0, 
		tesseract::PageSegMode nmode = tesseract::PSM_SINGLE_WORD);
	RESULT GenerateTarget(Pix *image, OnceOcr* &once);
};

class TechOcr {
public:
	ModelOcr BusinessLicenseOcr;

};
// TODO: 在此參考您的程式所需要的其他標頭
