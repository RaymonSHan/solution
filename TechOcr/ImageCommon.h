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

Pix* trPixCreateFromIplImage(IplImage *img);
void trShowImage(char *name, IplImage *img, Boxa *box = NULL);

// should read 
// http://www.open-open.com/lib/view/open1363156299203.html
// http://blog.csdn.net/lmj623565791/article/details/23960391

