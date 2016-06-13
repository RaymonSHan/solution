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
// #include "ProcessFormat.h"

#define DEFAULT_LANGURE			"chi_sim"
#define DEFALUT_COLOR			CV_RGB(221, 134, 212)
#define DEFALUT_POINT_COLOR		CV_RGB(112, 222, 66)

#define MAX_CONTOUR_AREA_RATE	0.1
#define MAX_CONTOUR_LENGTH_RATE 2

#define THIN_WIDTH				1
#define DEFAULT_WIDTH			3
#define THICK_WIDTH				15

#define TECHOCR_VERSION       "TechOcr V0.2"			// finish in Jun 11 '16

// ���ص㵽ֱ�ߵľ��롣x��yȷ���㡣x1��y1��x2��y2ȷ��ֱ�ߡ�
// �㷨ȡ�� http://blog.csdn.net/hhhh63/article/details/25030143��
double ComPointToLineDist(int x, int y, int x1, int y1, int x2, int y2);
double ComPointToLineDist(CvPoint *p, CvPoint *p1, CvPoint *p2);
// ��cvFitLine()���ظ�ʽ�ĵ㵽ֱ�߾��룬
double ComPointToLineDist(float *p, float *l);

// �ж������μ���Ƿ�С��space����space����0ʱ���൱���ж��������Ƿ��ཻ�������εı�ƽ���������ᡣ
bool ComIsRectIntersect(Box *b1, Box *b2, int space);

// �ж���ֱ���Ƿ��ཻ���緵��Ϊ�棬��p��ʾ���㡣
// �㷨ȡ�� http://blog.csdn.net/zhanghefu/article/details/6076542
bool ComIsLineIntersect(CvPoint *p1, CvPoint *p2, CvPoint *p3, CvPoint *p4, CvPoint2D32f &p);
bool ComIsLineIntersect(CvPoint2D32f *p1, CvPoint2D32f *p2, CvPoint2D32f *p3, CvPoint2D32f *p4, CvPoint2D32f &p);
// ��cvFitLine()���ظ�ʽ������ֱ�߲���
bool ComIsLineIntersect(float *l1, float *l2, CvPoint2D32f &p);

// ��box��Rect��������ĸ��ߣ�����delta���ء�
void ComEnlargeBox(Box *box, Box &enlarge, int deltax, int deltay);
void ComEnlargeRect(CvRect *box, CvRect &enlarge, int deltax, int deltay);

// ��ͼ�񰴶�����ת��clockwiseΪ��ʱΪ˳ʱ�롣
IplImage* ComRotateImage(IplImage* src, int angle, bool clockwise);

// �ַ���ת�������ص�ָ������delete[]�ͷš�
RESULT ComGbkToUtf8(char *gbk, long gsize, char *&utf8, long &usize);
RESULT ComUtf8ToGbk(char *utf8, long usize, char *&gbk, long &gsize);

// ��boxa�б��еľ��Σ���ָ����ɫ���ȣ�����ͼ���ϡ�
void ComDrawBoxs(IplImage *img, Boxa *boxa, CvScalar *color = &DEFALUT_COLOR, int width = DEFAULT_WIDTH);

// ��lines�б��е��߶Σ���ָ����ɫ���ȣ�����ͼ���ϣ���ָ���Ƿ����߶ζ˵㻭Բ��
void ComDrawLines(IplImage *img, CvSeq *lines, bool drawpoint, 
	CvScalar *color = &DEFALUT_POINT_COLOR, int width = DEFAULT_WIDTH, CvScalar *pointcolor = &DEFALUT_POINT_COLOR);

// ��cvFitLine()���صĸ�ʽ����
void ComDrawLineForFitLine(IplImage *img, float *line, CvScalar *color = &DEFALUT_COLOR, int width = DEFAULT_WIDTH);
void ComDrawLineForFitLine(IplImage *img, CvSeq *lines, CvScalar *color = &DEFALUT_COLOR, int width = DEFAULT_WIDTH);

// ������Ļ���ųߴ磬��ʾͼ��
void ComShowImage(char *name, IplImage *img);

// ����cvSeqSort()�ıȽϺ���
static int ComparePoint(void* _a, void* _b, void* userdata);
static int CompareDistance(void* _a, void* _b, void* userdata);
static int CompareClockwise(const void *_a, const void *_b, void *userdata);

// ���º�����ɵ�һ��������ת��ͼ��
//
// ȡ�ö�ֵ��ͼ�񣬷���ͼ����ʹ��cvReleaseImage�ͷš�
// gaussianx, gaussiany����˹�˲�����
// thresvalue����ֵ����ֵ
// adaptivesize, adaptivedelta��cvAdaptiveThreshold�е�block_size��param1������
#define DEFAULT_GAUSSIAN			3
#define DEFAULT_THRESV_VALUE		160
#define DEFAULT_ADAPTIVE_TH_SIZE	25
#define DEFAULT_ADAPTIVE_TH_DELTA	10
IplImage* TrImageThreshold(IplImage *src, 
	int gaussianx = DEFAULT_GAUSSIAN, int gaussiany = DEFAULT_GAUSSIAN, 
	int thresvalue = DEFAULT_THRESV_VALUE,
	int adaptivesize = DEFAULT_ADAPTIVE_TH_SIZE, int adaptivedelta = DEFAULT_ADAPTIVE_TH_DELTA);

// ȡ������ֱ�ߣ�����ͼ����ʹ��cvReleaseImage�ͷš�����ͼ����Ϊ�Ҷ�ͼ����TrImageThreshold()�ķ���ֵ��
// arate��ѡ�����ͼ���������arate��������
// lrate��ѡ�����ͼ��̱߳���lrate��������
#define DEFAULT_CONT_AREA_RATE		0.1
#define DEFAULT_CONT_LENGTH_RATE	1.5
IplImage* TrContourDraw(IplImage *src, bool &havelarge,
	double arate = DEFAULT_CONT_AREA_RATE, double lrate = DEFAULT_CONT_LENGTH_RATE);

// ȡ���ʻ�����ֱ�ߣ��贴��storage������ʹ�ú��ͷš�����ͼ����Ϊ�Ҷ�ͼ����TrContourDraw()�ķ���ֵ��
// thresholdrate, lengthrate, seprationrate�ֱ����ͼ��̱ߺ󣬶�Ӧ��cvHoughLines2()�е�threshold��param1��param2��
#define DEFAULT_HOUGH_RATE			0.25
#define DEFAULT_LENGTH_RATE			0.3
#define DEFAULT_SEPRATION_RATE		0.015
CvSeq* TrCreateHoughLines(IplImage *src, CvMemStorage *storage,
	double thresholdrate = DEFAULT_HOUGH_RATE, 
	double lengthrate = DEFAULT_LENGTH_RATE, 
	double seprationrate = DEFAULT_SEPRATION_RATE);

// �ϲ����Ƶ�ֱ�ߣ��贴��storage������ʹ�ú��ͷš���������ΪTrCreateHoughLines()�ķ���ֵ��
// thresholdrate���ж���ֵ
// ����ֵ�ǰ�cvFitLine()���ظ�ʽ�����У�CV_32FC4��
#define DEFAULT_APPROXIMATE_VALUE	(100)
CvSeq* TrAggregationLines(CvSeq *lines, CvMemStorage *storage,
	double thresholdrate = DEFAULT_APPROXIMATE_VALUE);

// �������ߴ���4��ʱ��ѡ�����е������ߡ��кܶ����㷨��ѡ��Ŀǰѡ������ߡ�
// lines��TrAggregationLines()�Ľ��
CvSeq* TrChoiceLinesInFitLines(CvSeq *lines, CvMemStorage *storage);

// ���������ߣ����ĸ����㡣
// fitlines��TrCreateApproximateLines�ķ���ֵ
// center��ͼ�����ĵ㣬�����жϡ�
CvSeq* TrGetFourCorner(CvSeq *fitlines, CvMemStorage *storage, CvPoint2D32f *center);

// ��һ���������ȷ��ͶӰ�任����
//
// �������Ϻ�������ָ��ͼ���У��ҵ�����������ĸ��������ꡣ
// img����ɫ24bitͼ��
// corner��ָ��CvPoint2D32f[4]��������RESULT_OKʱ��Ч��
// loop��Ѱ��ͼ������ʱ�ĵ���������
#define DEFAULT_LOOP				1
RESULT TechOcrGetFourCorner(IplImage *img, CvPoint2D32f *corner, int loop = DEFAULT_LOOP);



// following for format use
#define CHARTYPE_FEATURE			0
#define CHARTYPE_CONTENT_WORD		1
#define CHARTYPE_CONTENT_BLOCK		2
#define CHARTYPE_FORMAT_ID			3
// following for candidate feature use
#define CHARTYPE_NOT_MATCH			0x10000
#define CHARTYPE_MATCH				0x100000
typedef struct CharFound {
	CvRect rect;
	int found;
	int chartype;
	char *desc;
}CharFound, *pCharFound;

// �ͷ�CharFound���У��Լ��ڲ��ַ�����
void ComReleaseCharFound(CvSeq *found);

typedef enum TrEncodeMode {
	ENCODE_GBK,
	ENCODE_UTF8
}TrEncodeMode;

CvMemStorage* GetGlobalStorage(void);

// ���ݾ��δ�С���ж��Ƿ�Ϊʶ��������
// ��С�� 1/LARGE_RATE ��Ļ�ߴ磬���� 1/SMALL_RATE ��Ļ�ߴ�
#define LARGE_RATE				10
#define SMALL_RATE				60
bool ComIsWordBox(Box *box, Pix *pix, int largerate = LARGE_RATE, int smallrate = SMALL_RATE);

// �ж�ָ��Box��Boxa�У��Ƿ�������Box�ཻ��
bool ComIsBoxIsolated(Box *box, Boxa *boxa, int space);

// ȡת������ֵ��
void ComCenterPoint(CvPoint2D32f *corner, CvPoint2D32f *dstcornet);

// ���º�����ɵڶ���������Ԥ��ȡ
//
// ��TechOcrGetFourCorner()�Ľ����ת��ͼ�Ρ�
IplImage* TrWarpPerspective(IplImage *img, int w, int h, CvMat *warp, CvMat *warp2 = NULL);
// IplImage* TrWarpPerspective(IplImage *img, int w, int h, CvPoint2D32f *corner);

// ��OpenCV��IplImage��ʽ��ת��ΪLeptonica��Pix��ʽ��
Pix* TrPixCreateFromIplImage(IplImage *img);

// ��ʼ��Tesseractʶ������.
tesseract::TessBaseAPI* TechOcrInitTessAPI();

// ��ֹʶ�����档
void TechOcrExitTessAPI(tesseract::TessBaseAPI *api);

// ��ͼ���У��ҵ����ܰ���ʶ���ֵľ�������
// ����ֵ��Ҫ��boxaDestory()�ͷš�
Boxa* TrChoiceBoxInBoxa(tesseract::TessBaseAPI *api, Pix *pix);

// ��ָ��ģʽ��ָ�����룬ʶ��ָ�������е����֡�
// ����ֵ��Ҫ��delete[]�ͷš�
char* TrTranslateInRect(tesseract::TessBaseAPI *api, tesseract::PageSegMode mode, TrEncodeMode encode, Box *box);
char* TrTranslateInRect(tesseract::TessBaseAPI *api, tesseract::PageSegMode mode, TrEncodeMode encode, CvRect *rect);

// �ڶ�����Ҫ������Ԥʶ��������
//
// ���ݱ궨��Ե�㣬����ͶӰ�任���󣬲�����Pix��
RESULT TechOcrCreatePix(IplImage *img, int w, int h, CvPoint2D32f *corner, Pix *&pix, CvMat *warp);

// ��ָ��ͼ�����Ԥ������ȡ�����ּ���λ�����ꡣ
// corner����Դ��TechOcrGetFourCorner()
// api����Դ��TrInitTessAPI()
// feature���У�����cvRelease()�ͷ�
RESULT TechOcrGetFeatureChar(Pix *pix, tesseract::TessBaseAPI *api,	CvSeq *feature);

// �ڶ�����Ҫ����������ģ��
//
// ����ģ�壬����ģ��
RESULT TechOcrCreateFormat(CvSeq *&format, char *name, int w, int h, TrEncodeMode encode = ENCODE_GBK);
// ����ģ�壬���������
RESULT TechOcrFormatAddFeature(CvSeq *format, int x, int y, int w, int h, char *c, TrEncodeMode encode = ENCODE_GBK);
// ����ģ�壬���ʶ������
RESULT TechOcrFormatAddContent(CvSeq *format, int x, int y, int w, int h, char *c, 
	int mode = CHARTYPE_CONTENT_WORD, TrEncodeMode encode = ENCODE_GBK);

// ȡ��ģ������Ӧ����Ļ�ߴ硣��TechOcrCreateFormat()�е�w��h������
void TrGetFormatScreenRect(CvSeq *format, int &w, int &h);

// �����ض�λ���µ�ƥ��̶�
int TrFeatureMatch(CvSeq *feature, CvSeq *format, int feaorder, int fororder);

// ����������λ�ã�Ѱ���ض�ģ���е�ƥ���������Լ����ƥ���������е�λ��
int TrFeatureMostMatch(CvSeq *feature, CvSeq *format, int &feaorder, int &fororder);

// ��ȷ��ƥ����������У��ҵ��ĸ���λ�õ���š�
void TrGetCornerInMatch(CvSeq *feature, CvSeq *format, int maxfea, int maxfor, CvPoint2D32f *csrc, CvPoint2D32f *cdst);

// �ڶ����������ȷ��ģ��
//
// ������ƥ���ģ�壬�Լ���Ӧ��λ�ú�ƥ��ȡ�������ͶӰ�任����
// ���bestformat���벻Ϊ�գ���������Ӧģ�塣
RESULT TechOcrFormatMostMatch(CvSeq *feature, CvSeq *&bestformat, int &maxmatch, CvMat *wrap);



// ���ݺ��ֺ��ŵĹ��ɣ��������Ҫ����ƥ�䣬������Ҫȫ��ƥ�䡣
bool ComBoxInRect(Box *box, CvRect *rect);

// ���ݱ궨�Ĵ�ʶ�������ҵ�ʵ�ʵ�����λ�á�
CvRect ComDetectWord(Pixa *pixa, CvRect *rect);

// ��������Ҫ���������ݴ�ʶ������ʶ�����֡�
RESULT TechOcrDetectWordsInFormat(IplImage *img, CvMat *warp1, CvMat *warp2, CvSeq *bestformat, CvSeq *content);

// �����������������ַ���
RESULT TechOcrOutput(CvSeq *content, CvSeq *bestformat, char *&output);







