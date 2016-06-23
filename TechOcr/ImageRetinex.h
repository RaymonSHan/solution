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
void TrGetLightInMat(Mat &src, Mat &dst);

void TrMatDiv(Mat &src, Mat &dst, int channel = -1);
void TrMatDivOneChannel(Mat &src, Mat &dst, int channel = -1);


void pocRetinex(char *filename);


// from http://blog.csdn.net/onezeros/article/details/6342661
/*
* Copyright (c) 2006, Douglas Gray (dgray@soe.ucsc.edu, dr.de3ug@gmail.com)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Douglas Gray ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// #pragma once  

#include "cv.h"  

extern double* CreateKernel(double sigma);
extern int* CreateFastKernel(double sigma);

extern void FilterGaussian(IplImage* img, double sigma);
extern void FastFilter(IplImage *img, double sigma);

extern void Retinex
(IplImage *img, double sigma, int gain = 128, int offset = 128);

extern void MultiScaleRetinex
(IplImage *img, int scales, double *weights, double *sigmas, int gain = 128, int offset = 128);

extern void MultiScaleRetinexCR
(IplImage *img, int scales, double *weights, double *sigmas, int gain = 128, int offset = 128,
	double restoration_factor = 6, double color_gain = 2);