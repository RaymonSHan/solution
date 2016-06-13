#pragma once

#include "ImageCommon.h"

void pocPointToLineDist();
void pocPixCreateFromIplImage(char *src, char *dst);
void pocRotateImage(char* src, int angle, bool clockwise);
void pocFindMaxRect(char *src);
void pocFindHoughLines(char *src);



void pocShowImage(char *src);
void pocImagePreprocess(IplImage *img);
