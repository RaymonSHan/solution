#pragma once

// 非常重要
// 本部分函数，仅支持 CV_8UC3 格式的二维Mat

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

// for opencv
#include "opencv2\\opencv.hpp"

using namespace cv;


void ComMatInit(Mat &src, uchar val, int channel = -1);


// 针对指定图像,按照X，Y方向的步长，对指定通道求区域中的最亮值。
// channel为负数时，求所有通道。
void TrGetMaxInMat(Mat &src, Mat &dst, int channel = -1);

void pocRetinex(char *filename);
