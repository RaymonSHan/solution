
#include "ImageCommon.h"

#define DEFAULT_ADAPTIVE_TH_SIZE  25
#define DEFAULT_ADAPTIVE_TH_DELTA 10

// http://blog.csdn.net/hhhh63/article/details/25030143
double comPointToLineDist(int x, int y, int x1, int y1, int x2, int y2) {
	double a = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
	double b = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	double s = a / b;
	return abs(s);
}
// following function is for translate data between leptonica and opencv
Pix* trPixCreateFromIplImage(IplImage *img) {
// create Pix from IplImage
	Pix	   *pix = NULL;
	int		x, y;
	char   *istart, *ix;
	l_uint32 *pstart, *px;

// only supply 8 bit gray and 24 bit color image now.
	if (!img || img->depth != IPL_DEPTH_8U || (img->nChannels != 1 && img->nChannels != 3)) {
		return NULL;
	}
	pix = pixCreateNoInit(img->width, img->height, img->nChannels == 1 ? 8 : 32);
	if (!pix) {
		return NULL;
	}

	istart = img->imageData;
	pstart = pix->data;
	if (img->nChannels == 1) {
		for (y = 0; y < img->height; y++) {
// for gray img, one for big-end in 4 byte, another for small-end. So switch the order
			ix = istart;
			px = pstart;
			for (x = 0; x < (img->width + 3) / 4; x++) {
				*px++ = (*ix << 24) + (*(ix + 1) << 16) + (*(ix + 2) << 8) + *(ix + 3);
				ix += 4;
			}
// widthStep and wpl are 4 byte align
			istart += img->widthStep;
			pstart += pix->wpl;
		}
	}
	if (img->nChannels == 3) {
// for IplImage, use 3 byte for one pixel in 24 bit image, while Pix use 4 byte.
		for (y = 0; y < img->height; y++) {
			ix = istart;
			px = pstart;
			for (x = 0; x < img->width; x++) {
				*px++ = (*(l_uint32 *)ix) << 8;
				ix += 3;
			}
			istart += img->widthStep;
			pstart += pix->wpl;
		}
	}
	return pix;
}

// following function is major for opencv
IplImage* trCloneImg1c(IplImage *src, bool threshold, int thresvalue) {
// get the one channel gray image of the source, and do threshold is need
	IplImage	*img1c;

	if (!src) {
		return NULL;
	}
	img1c = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	if (src->nChannels > 1) {
		cvCvtColor(src, img1c, CV_BGR2GRAY);
	}
	else {
		cvCopy(src, img1c);
	}

	if (threshold) {
		cvSmooth(img1c, img1c, CV_GAUSSIAN, 3, 3);
		cvAdaptiveThreshold(img1c, img1c, thresvalue, CV_ADAPTIVE_THRESH_MEAN_C,
			CV_THRESH_BINARY_INV, DEFAULT_ADAPTIVE_TH_SIZE, DEFAULT_ADAPTIVE_TH_DELTA);
	}
	return img1c;
}

IplImage* trRotateImage(IplImage *src, int angle, bool clockwise) {
	IplImage *dst = NULL, *temp;
	int anglecalc;
	int width, height;
	int templength, tempx, tempy;
	int flag = -1;

	CvMat M;
	float m[6];
	int w, h;

	angle = angle % 360;
	anglecalc = abs(angle) % 180;
	if (anglecalc > 90)	{
		anglecalc = 90 - (anglecalc % 90);
	}
	width =	(int)(sin(anglecalc * CV_PI / 180.0) * src->height) +
			(int)(cos(anglecalc * CV_PI / 180.0) * src->width) + 1;
	height = (int)(cos(anglecalc * CV_PI / 180.0) * src->height) +
			 (int)(sin(anglecalc * CV_PI / 180.0) * src->width) + 1;
	templength = (int)sqrt((double)src->width * src->width + src->height * src->height) + 10;
	tempx = (templength + 1) / 2 - src->width / 2;
	tempy = (templength + 1) / 2 - src->height / 2;

	dst = cvCreateImage(cvSize(width, height), src->depth, src->nChannels);
	cvZero(dst);
	temp = cvCreateImage(cvSize(templength, templength), src->depth, src->nChannels);
	cvZero(temp);

	cvSetImageROI(temp, cvRect(tempx, tempy, src->width, src->height));
	cvCopy(src, temp, NULL);
	cvResetImageROI(temp);

	if (clockwise)
		flag = 1;

	w = temp->width;
	h = temp->height;
	m[0] = (float)cos(flag * angle * CV_PI / 180.);
	m[1] = (float)sin(flag * angle * CV_PI / 180.);
	m[3] = -m[1];
	m[4] = m[0];
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;

	M = cvMat(2, 3, CV_32F, m);
	cvGetQuadrangleSubPix(temp, dst, &M);
	cvReleaseImage(&temp);
	return dst;
};

IplImage* trCreateMaxContour(IplImage *src, CvMemStorage *storage, int thresv, double arate, double lrate) {
// src can be 8 or 24 bit, return image is 8 bit
	CvSeq *conts;
	CvSeq *nowcont;
	IplImage *img1c, *dst;
	CvMemStorage *nowstore;
	double arearate, area;
	double lenrate, len;

	if (!src) {
		return NULL;
	}
	if (!storage) {
		nowstore = cvCreateMemStorage(0);
	}
	else {
		nowstore = storage;
	}
	conts = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), nowstore);

	img1c = trCloneImg1c(src, true, thresv);	dst = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	cvZero(dst);

	cvFindContours(img1c, nowstore, &conts, sizeof(CvContour),
		CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

	arearate = src->width * src->height * arate;
	lenrate = MIN(src->width, src->height) * lrate;

	for (nowcont = conts; nowcont; nowcont = nowcont->h_next) {
		len = fabs(cvArcLength(nowcont));
		area = fabs(cvContourArea(nowcont));
		if (area > arearate || len > lenrate) {
			cvDrawContours(dst, nowcont, CV_RGB(255,255,255), CV_RGB(255, 255, 255), 0, DEFAULT_WIDTH);
		}
		cvClearSeq(nowcont);
	}
	cvReleaseImage(&img1c);
	if (!storage) {
		cvReleaseMemStorage(&nowstore);
	}
	return dst;
}

IplImage* trCreateLine(IplImage *img, CvSeq *lines) {
	IplImage *dst;
	dst = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);
// 	cvSet(dst, CV_RGB(255, 255, 255));
// 	trDrawLines(dst, lines, false, &CV_RGB(0, 0, 0), DEFAULT_WIDTH, &CV_RGB(0, 0, 0));
	cvSet(dst, CV_RGB(0, 0, 0));
	trDrawLines(dst, lines, false, &CV_RGB(255, 255, 255), DEFAULT_WIDTH, &CV_RGB(255, 255, 255));

	return dst;
}

CvSeq* trCreateHoughLines(IplImage *src, CvMemStorage *storage) {
// here src MUST be one channel
//	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	double rho = 3;
// 	double theta = CV_PI / 180;
// 	double threshold = 30;
// 	double min_length = 200;//CV_HOUGH_PROBABILISTIC  
// 	double sepration_connection = 4;//CV_HOUGH_PROBABILISTIC  
	double theta = 2*CV_PI  / 180;
// 	double threshold = 100;
// 	double min_length = 150;//CV_HOUGH_PROBABILISTIC  
// 	double sepration_connection = 40;//CV_HOUGH_PROBABILISTIC  

	int size = MAX(src->width, src->height);
	double threshold = (double)size / 4;
	double min_length = (double)size / 3;
	double sepration_connection = (double)size / 40;




									//binary image is needed.  
	lines = cvHoughLines2(
		src,
		storage,
		CV_HOUGH_PROBABILISTIC,
		rho,
		theta,
		threshold,
		min_length,
		sepration_connection);

//	cvReleaseMemStorage(&storage);
	return lines;
}

// following function is major for tesseract
tesseract::TessBaseAPI* trInitTessAPI(void) {
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI;
	int rc = api->Init(NULL, DEFAULT_LANGURE, tesseract::OEM_DEFAULT);
	if (rc)
		return NULL;
	return api;
}

void trExitTessAPI(tesseract::TessBaseAPI *api) {
	api->End();
	delete api;
}


// above function is inter use
// following function is declare outside
void trDrawBoxs(IplImage *img, Boxa *boxa, CvScalar *color, int width) {
	CvRect	cvrect;
	Box		**box;
	int		i;

	if (!img || !boxa || !boxa->box) {
		return;
	}
	if (boxa && boxa->box) {
		box = boxa->box;
		for (i = 0; i < boxa->n; i++) {
			cvrect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
			cvRectangleR(img, cvrect, *color, width);
			box++;
		}
	}
}

// here lines is created by cvHoughLines2
void trDrawLines(IplImage *img, CvSeq *lines, bool drawpoint, CvScalar *color, int width, CvScalar *pointcolor) {
	int		i;
	for (i = 0; i < lines->total; i++)
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
		cvLine(img, line[0], line[1], *color, width);
		if (drawpoint) {
			cvCircle(img, line[0], 3 * (i % 5) + 4, *pointcolor, 2);
			cvCircle(img, line[1], 3 * (i % 5) + 4, *pointcolor, 4);
		}
	}
}


void trShowImage(char *name, IplImage *img, Boxa *boxa, CvScalar *color, int width) {
	RECT	rect;
	HWND	hWnd;
	IplImage *dst;
	CvSize	resize;

	if (!name || !img) {
		return;
	}
	if (boxa && boxa->box) {
		trDrawBoxs(img, boxa, color, width);
	}

	hWnd = ::GetDesktopWindow();
	::GetWindowRect(hWnd, &rect);
	cvNamedWindow(name, 1);

	float scale = MAX((float)img->width / (rect.right - rect.left - 50),
		(float)img->height / (rect.bottom - rect.top - 100));
	if (scale > (float)1) {
		resize.width = (int)(img->width / scale);
		resize.height = (int)(img->height / scale);

		dst = cvCreateImage(resize, img->depth, img->nChannels);
		cvResize(img, dst, CV_INTER_LINEAR);
		cvShowImage(name, dst);
		cvReleaseImage(&dst);
	}
	else {
		cvShowImage(name, img);
	}
}

