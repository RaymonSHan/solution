#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <stdio.h>

// for opencv
#include "cv.h"
#include <highgui.h>

#include "rcommon.h"

#define COLOR_BIT 4
#define COLOR_RSHIFT (8 - COLOR_BIT)
#define COLOR_RANGE (1 << COLOR_BIT)

char RangeChar(double num);


RESULT TrIplImageChannelChoiceMin(IplImage *src);

RESULT TrIplImageChannelChoiceMax(IplImage *src);

RESULT TrIplImageChannelChoice(IplImage *src, bool isr, bool isg, bool isb, uchar r = 0, uchar g = 0, uchar b = 0);

RESULT TrIplImageOneColor(IplImage *src, uchar r, uchar g, uchar b, uchar delta);
RESULT TrIplImageOneColor(IplImage *src, uchar g, uchar delta);
RESULT TrIplImageRateOneColor(IplImage *src, uchar r, uchar g, uchar b, uchar delta);


RESULT TrIplImageRgbToYuv(IplImage *src);
RESULT TrIplImageYuvToRgb(IplImage *src);

RESULT TrIplImageRgbToHsl(IplImage *src);
RESULT TrIplImageHslToRgb(IplImage *src);
RESULT TrIplImageDeltaColor(IplImage *src, uchar r, uchar g, uchar b);
RESULT TrIplImageDiv(IplImage *src, IplImage *dst);
RESULT TrIplImageOr(IplImage *src, IplImage *dst);
RESULT TrIplImageSub(IplImage *src, IplImage *dst);
RESULT TrIplImageNot(IplImage *src, bool isr, bool isg, bool isb);
RESULT TrIplImageCmp(IplImage *src, IplImage *dst);
RESULT TrIplImageBeOne(IplImage *src, IplImage *dst);


RESULT TrIplImageToWhite(IplImage *src, uchar delta);


RESULT TrIplImageSetOne(IplImage *src);


RESULT TrIplImageColorCount(IplImage *src, int *count, int extent = 0);
