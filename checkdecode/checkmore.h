#pragma once
#include "rcommon.h"

#include "cv.h"
#include <highgui.h>


class CheckMore
{
private:
  IplImage *ImageSrc;
  IplImage *Image1c;
  IplImage *ImageTran;
  IplImage *ImageTran1c;

  CvRect PaperSize;
  int NowCheck;
  ShareInfo CheckInfo[MAX_RECT];
  CvPoint2D32f MaxRect[4];       // for largest contour placed in screen

  CvScalar edgerp[4 + 4];            // record 4 point

  RESULT PrepareImage(char* filename);
  RESULT ClearState(void);
  RESULT FindMaxRect(void);
  RESULT WarpImage(void);

  RESULT GetLevelInfo(CvSeq* contour, pShareInfo info, bool doarea, bool doloop = true);
  RESULT DetectOneCheck(int order);
  RESULT DetectCheck(int order);
  RESULT GetResult(char* checked);

  RESULT DisplayResult(void);
  RESULT ShowImage(IplImage* img, char* name);

public:
  CheckMore();
  ~CheckMore();
  RESULT SetPaper(int width, int height);
  RESULT LoadMask(int* xplace, long xsize, int* yplace, int ysize);

  RESULT Process(char* filename, char* checkresult, bool display);
};


