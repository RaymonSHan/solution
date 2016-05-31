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

class TechOcrClass;
class OnceOcrClass;
struct TargetItem;

#define VERSION       "TechOcr V0.1"
#define MAX_CHAR      64
#define MAX_DETECT    100
#define RECT_ZERO     cvRect(0, 0, 0, 0);
#define DEFAULT_LAN   "chi_sim"
#define ENLAGRE_X     16
#define ENLARGE_Y     24
#define MIN_WORD_SIZE 30

typedef RESULT (*DetectDatum)(OnceOcrClass*, Box*, void*);

RESULT BusinessLicenseDatum(OnceOcrClass *ocr, Box *box, void *futureuse);
RESULT GB2312toUTF8(char* gb2312, long gsize, char*& utf8, long &usize);
RESULT UTF8toGB2312(char* utf8, long usize, char*& gb2312, long &gsize);
CvPoint TransformPoint(const CvPoint point, const CvMat *matrix);
CvRect TransformRect(const CvRect rect, const CvMat *matrix, int enx = 0, int eny = 0);
void AddToJson(std::string &str, char *chars);

RESULT BoxInRect(Box *box, CvRect *rect);
CvRect DetectWord(Pixa *pixa, CvRect *rect);

// RESULT ShowImage(IplImage *img, char *name, int num, Boxa *boxa, int itemnum, TargetItem* item);

typedef struct SourceItem {
	CvRect rectName;
	CvRect rectValue;
	tesseract::PageSegMode modeName;
	tesseract::PageSegMode modeValue;
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

class OnceOcrClass {
private:
	friend     RESULT BusinessLicenseDatum(OnceOcrClass *ocr, Box *box, void *futureuse);
	tesseract::TessBaseAPI *api;
	Pix       *targetImage;
	MYINT      targetNumber;
	TargetItem targetItem[MAX_CHAR];
	Pixa *pixW;
	Boxa *boxW;
	CvMat *warpMat;
	char *fileName;

public:
	RESULT InitOnceOcr(Pix *image, char *filename);
	RESULT ExitOnceOcr();
	RESULT TranWordInRect(Pixa *pixa, CvRect *rect, char *&str);
	RESULT TranAllInRect(CvRect *rect, tesseract::PageSegMode mode, char *&str);

	RESULT MapRect(MYINT num, SourceItem *source);
	RESULT DoOcr(MYINT num, SourceItem *source);
	RESULT ShowImage(MYINT num, char *name);
	RESULT OutputOcr(MYINT num, SourceItem *source, char *&output);		// should delete[] outside the function
};

class ModelOcrClass {
private:
	volatile MYINT      sourceNumber;
	SourceItem sourceItem[MAX_DETECT];
public:
//	ModelOcr();
//	~ModelOcr();
	RESULT AddOcrItem(CvRect &name, CvRect &value, char *shouldbe = NULL, bool isoutput = true,
		tesseract::PageSegMode nmode = tesseract::PSM_SINGLE_WORD,
		tesseract::PageSegMode vmode = tesseract::PSM_SINGLE_WORD);
	RESULT AddOcrItem(CvRect &name, DetectDatum isdatum, char datum = 0, char *shouldbe = NULL, bool isoutput = true,
		tesseract::PageSegMode nmode = tesseract::PSM_SINGLE_CHAR);
	RESULT GenerateTarget(Pix *image, OnceOcrClass* &once, char *filename, char *&output);
};

class TechOcrClass {
public:
	ModelOcrClass BusinessLicenseOcr;
public:
	RESULT InitBusinessLicense(void);
	RESULT DoBusinessLicense(Pix *image, char *&output, char *filename);

};

char* WINAPI OcrTest(char *filename);