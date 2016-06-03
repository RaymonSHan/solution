#pragma once

#include "ImageCommon.h"

void pocPixCreateFromIplImage(char *src, char *dst);
void pocShowImage(char *src);
void pocRotateImage(char* src, int angle, bool clockwise);
void pocImagePreprocess(IplImage *img);
