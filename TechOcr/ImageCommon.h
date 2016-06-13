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
#include "ProcessFormat.h"

#define DEFAULT_LANGURE			"chi_sim"
#define DEFALUT_COLOR			CV_RGB(221, 134, 212)
#define DEFALUT_POINT_COLOR		CV_RGB(112, 222, 66)

#define MAX_CONTOUR_AREA_RATE	0.1
#define MAX_CONTOUR_LENGTH_RATE 2

#define THIN_WIDTH				1
#define DEFAULT_WIDTH			3

struct TrFeatureWordFound;

typedef enum TrEncodeMode {
	ENCODE_GBK,
	ENCODE_UTF8
}TrEncodeMode;

// 返回点到直线的距离。x、y确定点。x1、y1，x2、y2确定直线。
// 算法取自 http://blog.csdn.net/hhhh63/article/details/25030143。
double ComPointToLineDist(int x, int y, int x1, int y1, int x2, int y2);
double ComPointToLineDist(CvPoint *p, CvPoint *p1, CvPoint *p2);

// 判定两矩形间距是否小于space。当space等于0时，相当于判定两矩形是否相交。两矩形的边平行于坐标轴。
bool ComIsRectIntersect(Box *b1, Box *b2, int space);

// 判定两直线是否相交，如返回为真，则p表示交点。
// 算法取自 http://blog.csdn.net/zhanghefu/article/details/6076542
bool ComIsLineIntersect(CvPoint *p1, CvPoint *p2, CvPoint *p3, CvPoint *p4, CvPoint2D32f &p);
bool ComIsLineIntersect(CvPoint2D32f *p1, CvPoint2D32f *p2, CvPoint2D32f *p3, CvPoint2D32f *p4, CvPoint2D32f &p);
// 按cvFitLine()返回格式的两条直线参数
bool ComIsLineIntersect(float *l1, float *l2, CvPoint2D32f &p);

// 将box矩形区域的各边，扩大delta像素。
void ComEnlargeBox(Box *box, Box &enlarge, int delta);

// 将图像按度数旋转，clockwise为真时为顺时针。
IplImage* ComRotateImage(IplImage* src, int angle, bool clockwise);

// 字符集转换，返回的指针需用delete[]释放。
RESULT ComGbkToUtf8(char *gbk, long gsize, char *&utf8, long &usize);
RESULT ComUtf8ToGbk(char *utf8, long usize, char *&gbk, long &gsize);

// 将boxa列表中的矩形，按指定颜色与宽度，画在图像上。
void ComDrawBoxs(IplImage *img, Boxa *boxa, CvScalar *color = &DEFALUT_COLOR, int width = DEFAULT_WIDTH);

// 将lines列表中的线段，按指定颜色与宽度，画在图像上；并指定是否在线段端点画圆。
void ComDrawLines(IplImage *img, CvSeq *lines, bool drawpoint, 
	CvScalar *color = &DEFALUT_POINT_COLOR, int width = DEFAULT_WIDTH, CvScalar *pointcolor = &DEFALUT_POINT_COLOR);

// 根据屏幕缩放尺寸，显示图像。
void ComShowImage(char *name, IplImage *img);

// 取得二值化图像，返回图像需使用cvReleaseImage释放。
// gaussianx, gaussiany：高斯滤波参数
// thresvalue：二值化阈值
// adaptivesize, adaptivedelta：cvAdaptiveThreshold中的block_size，param1参数。
#define DEFAULT_GAUSSIAN			3
#define DEFAULT_THRESV_VALUE		160
#define DEFAULT_ADAPTIVE_TH_SIZE	25
#define DEFAULT_ADAPTIVE_TH_DELTA	10
IplImage* TrImageThreshold(IplImage *src, 
	int gaussianx = DEFAULT_GAUSSIAN, int gaussiany = DEFAULT_GAUSSIAN, 
	int thresvalue = DEFAULT_THRESV_VALUE,
	int adaptivesize = DEFAULT_ADAPTIVE_TH_SIZE, int adaptivedelta = DEFAULT_ADAPTIVE_TH_DELTA);

// 取得轮廓直线，返回图像需使用cvReleaseImage释放。输入图像需为灰度图像，如TrImageThreshold()的返回值。
// arate：选择大于图像面积乘以arate的轮廓，
// lrate：选择大于图像短边乘以lrate的轮廓。
#define DEFAULT_CONT_AREA_RATE		0.1
#define DEFAULT_CONT_LENGTH_RATE	1.5
IplImage* TrContourDraw(IplImage *src, 
	double arate = DEFAULT_CONT_AREA_RATE, double lrate = DEFAULT_CONT_LENGTH_RATE);

// 取概率霍夫曼直线，需创建storage，并在使用后释放。输入图像需为灰度图像，如TrContourDraw()的返回值。
// thresholdrate, lengthrate, seprationrate分别乘以图像短边后，对应于cvHoughLines2()中的threshold，param1，param2。
#define DEFAULT_HOUGH_RATE			0.25
#define DEFAULT_LENGTH_RATE			0.3
#define DEFAULT_SEPRATION_RATE		0.015
CvSeq* TrCreateHoughLines(IplImage *src, CvMemStorage *storage,
	double thresholdrate = DEFAULT_HOUGH_RATE, 
	double lengthrate = DEFAULT_LENGTH_RATE, 
	double seprationrate = DEFAULT_SEPRATION_RATE);

// 合并近似的直线，需创建storage，并在使用后释放。输入序列为TrCreateHoughLines()的返回值。
// thresholdrate：判定阈值
// 返回值是按cvFitLine()返回格式的序列：CV_32FC4。
#define DEFAULT_APPROXIMATE_VALUE	(35*35)
CvSeq* TrCreateApproximateLines(CvSeq *lines, CvMemStorage *storage,
	double thresholdrate = DEFAULT_APPROXIMATE_VALUE);

// 根据四条边，求四个交点。
// fitlines：TrCreateApproximateLines的返回值
// center：图像中心点，用于判断。
CvSeq* TrGetIntersection(CvSeq *fitlines, CvMemStorage *storage, CvPoint2D32f *center);

int ComIsRectIsolated(Box *box, Boxa *boxa, int space);
bool ComMatchPlace(TrFeatureWordFound *one, TrFeatureWordFound *two);


int ComIsWord(Box *box, Pix *pix);



// following function is major for opencv
IplImage* TrCreateMaxContour(IplImage *src, CvMemStorage *storage = NULL, 
	int thresv = DEFAULT_THRESV_VALUE, double arate = MAX_CONTOUR_AREA_RATE, double lrate = MAX_CONTOUR_LENGTH_RATE);
IplImage* TrCreateLine(IplImage *img, CvSeq *lines);

// following function is for translate data between leptonica and opencv
Pix* TrPixCreateFromIplImage(IplImage *img);
Boxa* TrChoiceBoxInBoxa(Boxa *boxa, Pix *pix);

// following function is major for tesseract
tesseract::TessBaseAPI* TrInitTessAPI(void);
void TrExitTessAPI(tesseract::TessBaseAPI *api);
char* TrTranslateInRect(Box *box, tesseract::TessBaseAPI *api, tesseract::PageSegMode mode, TrEncodeMode encode);





// void thinImage(IplImage* src, IplImage* dst, int maxIterations);
