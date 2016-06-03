#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

// for opencv
#include <stdio.h>
#include "cv.h"
#include <highgui.h>

// for tesseract
#include "allheaders.h"
#include "baseapi.h"
#include "basedir.h"
#include "renderer.h"
#include "strngs.h"
#include "tprintf.h"
#include "openclwrapper.h"
#include "osdetect.h"

#include "rcommon.h"

#define DEFAULT_LANGURE			"chi_sim"
#define DEFALUT_COLOR			CV_RGB(221, 134, 212)
#define DEFALUT_POINT_COLOR		CV_RGB(112, 222, 66)

#define DEFAULT_WIDTH			3

// following function is for translate data between leptonica and opencv
Pix* trPixCreateFromIplImage(IplImage *img);

// following function is major for opencv
IplImage* trCloneImg1c(IplImage *src);
IplImage* trRotateImage(IplImage* src, int angle, bool clockwise);
CvSeq* trCreateHoughLines(IplImage *src, CvMemStorage* storage);

// following function is major for tesseract
tesseract::TessBaseAPI* trInitTessAPI(void);
void trExitTessAPI(tesseract::TessBaseAPI *api);

// above function is inter use
// following function is declare outside
void trDrawBoxs(IplImage *img, Boxa *boxa, CvScalar *color = &DEFALUT_COLOR, int width = DEFAULT_WIDTH);
void trDrawLines(IplImage *img, CvSeq *lines, bool drawpoint, 
	CvScalar *color = &DEFALUT_POINT_COLOR, int width = DEFAULT_WIDTH, CvScalar *pointcolor = &DEFALUT_POINT_COLOR);
void trShowImage(char *name, IplImage *img, 
	Boxa *boxa = NULL, CvScalar *color = &DEFALUT_COLOR, int width = DEFAULT_WIDTH);


// should read 
// http://www.open-open.com/lib/view/open1363156299203.html
// http://blog.csdn.net/lmj623565791/article/details/23960391

