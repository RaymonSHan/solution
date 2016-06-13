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

void pocShowImage(char *src);

void pocPointToLineDist(void);
void pocIsIntersect(void);

void pocApproximateLine(char *filename);
