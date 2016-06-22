#pragma once

// �ǳ���Ҫ
// �����ֺ�������֧�� CV_8UC3 ��ʽ�Ķ�άMat

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

// for opencv
#include "opencv2\\opencv.hpp"

using namespace cv;


void ComMatInit(Mat &src, uchar val, int channel = -1);


// ���ָ��ͼ��,����X��Y����Ĳ�������ָ��ͨ���������е�����ֵ��
// channelΪ����ʱ��������ͨ����
void TrGetMaxInMat(Mat &src, Mat &dst, int channel = -1);

void pocRetinex(char *filename);
