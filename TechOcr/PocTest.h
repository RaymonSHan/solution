#pragma once

#include "ImageCommon.h"
#include "ProcessFormat.h"

void pocPointToLineDist();
void pocPixCreateFromIplImage(char *src, char *dst);
void pocRotateImage(char* src, int angle, bool clockwise);
void pocFindMaxRect(char *src);
void pocFindHoughLines(char *src);
void pocFindFeatureWords(char *src);
void pocFindFeatureWordsInClass(char *src);
void pcoPreprocess(char *filename);

void pocCreateBusinessLicense(void);
void pocCreatePersonCard(void);
// void pocSystemExit(void);

void pocMemoryLeak(char *filename);
void pocTesseractInit(void);		// OK !! SetPageSegMode !!
void pocMemoryDebug(void);
void pcoImagePreprocess(char *filename);


void pocShowImage(char *src);

void pocPointToLineDist(void);
void pocIsIntersect(void);

void pocApproximateLine(char *filename);


IplImage* pocImageProcess(char *filename);
IplImage* pocContours(IplImage *src);
