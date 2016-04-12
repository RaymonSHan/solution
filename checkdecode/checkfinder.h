#pragma once
#include "rcommon.h"

#include "cv.h"
#include <highgui.h>

#define MAX_RECT                  200
#define MAX_QR_CODE               2048

#define DEFAULT_ADAPTIVE_TH_SIZE  25
#define DEFAULT_ADAPTIVE_TH_DELTA 10


int seq_cmp_by_clockwise(const void *_a, const void *_b, void *_cog);
RESULT SortRect(CvPoint2D32f* point, int number);
void TranslatePoint(CvPoint2D32f* conner, CvPoint2D32f& result, float a, float b);
void TranslateRect(CvPoint2D32f* conner, CvRect& result, CvRect source, CvRect paper);

typedef struct ShareInfo
{
  double area;
  double length;
  int    l2count;
  double l2area;
  double l2length;
  int    l3count;
  double l3area;
  double l3length;
  CvRect MaskRect;
  CvRect RoiRect;
  CvPoint DisplayPoint;
  char  CheckResult;
}ShareInfo, *pShareInfo;


class CheckDecode
{
private:
  IplImage *ImageSrc;
  IplImage *Image1c;
//  LOCKER(status);

  CvRect PaperSize;
  int NowMask;
  ShareInfo Info[MAX_RECT];
  CvPoint2D32f MaxRect[4];       // for largest contour placed in screen
  CvPoint DisplaySize;

  RESULT LoadImage(char* filename);
  RESULT LoadImage(void* image, long size);
  RESULT PrepareImage(bool gaussian);
  RESULT FindPattern(void);
  RESULT FIndMaxRect(void);
  RESULT GetLevelInfo(CvSeq* contour, pShareInfo info, bool doarea, bool doloop = true);
  RESULT FindCheck(int order);
  RESULT DetectCheck(int order);
  RESULT DisplayResult(void);

  RESULT GetQrCode(void);
  unsigned char QrResult[MAX_QR_CODE];

  RESULT ShowImage(IplImage* img = 0, char* name = 0);

public:
  CheckDecode();
  ~CheckDecode();
  RESULT SetPaper(int width, int height);
  RESULT LoadMask(CvRect* clist, long size);
  RESULT LoadMask(int* xplace, long xsize, int* yplace, int ysize);
  RESULT Process(char* filename);
  RESULT GetResult(char* checked);
  RESULT ProcessAndDisplay(char* filename);
};



#ifdef __cplusplus
extern "C" {
#endif 

char* WINAPI GetCheck(char* filename);

#ifdef CHECKDLL
JNIEXPORT jstring JNICALL Java_com_nasoft_diper_qrutil_ImageScanForC_GetCheck
  (JNIEnv *, jclass, jstring);
#endif CHECKDLL

#ifdef __cplusplus
}
#endif 


