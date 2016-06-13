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
#define THICK_WIDTH				15

struct TrFeatureWordFound;



// 返回点到直线的距离。x、y确定点。x1、y1，x2、y2确定直线。
// 算法取自 http://blog.csdn.net/hhhh63/article/details/25030143。
double ComPointToLineDist(int x, int y, int x1, int y1, int x2, int y2);
double ComPointToLineDist(CvPoint *p, CvPoint *p1, CvPoint *p2);
// 按cvFitLine()返回格式的点到直线距离，
double ComPointToLineDist(float *p, float *l);

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

// 按cvFitLine()返回的格式画线
void ComDrawLineForFitLine(IplImage *img, float *line, CvScalar *color = &DEFALUT_COLOR, int width = DEFAULT_WIDTH);
void ComDrawLineForFitLine(IplImage *img, CvSeq *lines, CvScalar *color = &DEFALUT_COLOR, int width = DEFAULT_WIDTH);

// 根据屏幕缩放尺寸，显示图像。
void ComShowImage(char *name, IplImage *img);

// 用于cvSeqSort()的比较函数
static int ComparePoint(void* _a, void* _b, void* userdata);
static int CompareDistance(void* _a, void* _b, void* userdata);
static int CompareClockwise(const void *_a, const void *_b, void *userdata);

// 以下函数完成第一步工作：转正图像
//
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
IplImage* TrContourDraw(IplImage *src, bool &havelarge,
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
#define DEFAULT_APPROXIMATE_VALUE	(100)
CvSeq* TrAggregationLines(CvSeq *lines, CvMemStorage *storage,
	double thresholdrate = DEFAULT_APPROXIMATE_VALUE);

// 当轮廓线大于4条时，选择其中的四条边。有很多种算法可选，目前选择最外边。
// lines：TrAggregationLines()的结果
CvSeq* TrChoiceLinesInFitLines(CvSeq *lines, CvMemStorage *storage);

// 根据四条边，求四个交点。
// fitlines：TrCreateApproximateLines的返回值
// center：图像中心点，用于判断。
CvSeq* TrGetFourCorner(CvSeq *fitlines, CvMemStorage *storage, CvPoint2D32f *center);

// 第一步最后工作，确定投影变换矩阵
//
// 调用以上函数，在指定图像中，找到矩形区域的四个顶点坐标。
// img：彩色24bit图像。
// corner：指向CvPoint2D32f[4]，当返回RESULT_OK时有效。
// loop：寻找图像轮廓时的迭代次数。
#define DEFAULT_LOOP				1
RESULT TechOcrGetFourCorner(IplImage *img, CvPoint2D32f *corner, int loop = DEFAULT_LOOP);


#define CHARTYPE_FEATURE			0
#define CHARTYPE_CONTENT			1
typedef struct CharFound {
	CvRect rect;
	int found;
	int chartype;
}CharFound, *pCharFound;

typedef enum TrEncodeMode {
	ENCODE_GBK,
	ENCODE_UTF8
}TrEncodeMode;

// 根据矩形大小，判定是否为识别字区域。
// 需小于 1/LARGE_RATE 屏幕尺寸，大于 1/SMALL_RATE 屏幕尺寸
#define LARGE_RATE				10
#define SMALL_RATE				60
bool ComIsWordBox(Box *box, Pix *pix, int largerate = LARGE_RATE, int smallrate = SMALL_RATE);

// 判定指定Box在Boxa中，是否与其它Box相交。
bool ComIsBoxIsolated(Box *box, Boxa *boxa, int space);

// 以下函数完成第二步工作：预读取
//
// 按TechOcrGetFourCorner()的结果，转正图形。
IplImage* TrWarpPerspective(IplImage *img, CvPoint2D32f *corner);

// 将OpenCV的IplImage格式，转换为Leptonica的Pix格式。
Pix* TrPixCreateFromIplImage(IplImage *img);

// 初始化Tesseract识别引擎.
tesseract::TessBaseAPI* TechOcrInitTessAPI();

// 终止识别引擎。
void TechOcrExitTessAPI(tesseract::TessBaseAPI *api);

// 在图像中，找到可能包含识别字的矩形区域。
// 返回值需要用boxaDestory()释放。
Boxa* TrChoiceBoxInBoxa(tesseract::TessBaseAPI *api, Pix *pix);

// 按指定模式、指定编码，识别指定区域中的文字。
// 返回值需要用delete[]释放。
char* TrTranslateInRect(tesseract::TessBaseAPI *api, tesseract::PageSegMode mode, TrEncodeMode encode, Box *box);

// 第二步主要工作，预识别特征字
//
// 按投影变换矩阵，生成Pix。
RESULT TechOcrCreatePix(IplImage *img, CvPoint2D32f *corner, Pix *&pix);

// 对指定图像进行预处理，获取特征字及其位置坐标。
// corner：来源于TechOcrGetFourCorner()
// api：来源于TrInitTessAPI()
// feature序列，需用cvRelease()释放
RESULT TechOcrGetFeatureChar(Pix *pix, tesseract::TessBaseAPI *api,	CvSeq *&feature);

// 第二步主要工作，定义模板
//
// 定义模板，创建模板
RESULT TechOcrCreateFormat(CvSeq *&format);
// 定义模板，添加特征字
RESULT TechOcrFormatAddFeature(CvSeq *format, int x, int y, int w, int h, char *c, TrEncodeMode encode = ENCODE_GBK);
// 定义模板，添加识别内容
RESULT TechOcrFormatAddContent(CvSeq *format, int x, int y, int w, int h, char *c, TrEncodeMode encode = ENCODE_GBK);


// 第二步最后工作，确定模板
//
// 根据特征字位置，判定模板，并返回变换矩阵
// format：当输入非0时，仅判定此模板格式；如为0，寻找最适合的模板。
RESULT TechOcrChoiceFormat(CvSeq *feature, CvSeq *&format, CvPoint2D32f *corner);


bool ComMatchPlace(TrFeatureWordFound *one, TrFeatureWordFound *two);





// following function is major for opencv
IplImage* TrCreateMaxContour(IplImage *src, CvMemStorage *storage = NULL, 
	int thresv = DEFAULT_THRESV_VALUE, double arate = MAX_CONTOUR_AREA_RATE, double lrate = MAX_CONTOUR_LENGTH_RATE);
IplImage* TrCreateLine(IplImage *img, CvSeq *lines);







// void thinImage(IplImage* src, IplImage* dst, int maxIterations);
