
#include "ImageCommon.h"

#define DEFAULT_ADAPTIVE_TH_SIZE  25
#define DEFAULT_ADAPTIVE_TH_DELTA 10

// following function is for translate data between leptonica and opencv
Pix* trPixCreateFromIplImage(IplImage *img) {
// create Pix from IplImage
	Pix	   *pix = NULL;
	int		x, y;
	char   *istart, *ix;
	l_uint32 *pstart, *px;

// only supply 8 bit gray and 24 bit color image now.
	if (!img || img->depth != 8 || (img->nChannels != 1 && img->nChannels != 3)) {
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
IplImage* trCloneImg1c(IplImage *src) {
// get the one channel gray image of the source
	IplImage	*img1c;
	img1c = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	if (src->nChannels > 1) {
		cvCvtColor(src, img1c, CV_BGR2GRAY);
	}
	else {
		cvCopy(src, img1c);
	}

//	if (gaussian)
		cvSmooth(img1c, img1c, CV_GAUSSIAN, 3, 3);
	cvAdaptiveThreshold(img1c, img1c, 128, CV_ADAPTIVE_THRESH_MEAN_C,
		CV_THRESH_BINARY_INV, DEFAULT_ADAPTIVE_TH_SIZE, DEFAULT_ADAPTIVE_TH_DELTA);

	return img1c;
}

CvSeq* trCreateHoughLines(IplImage *src, CvMemStorage* storage) {
// here src MUST be one channel
//	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	double rho = 1;
// 	double theta = CV_PI / 180;
// 	double threshold = 30;
// 	double min_length = 200;//CV_HOUGH_PROBABILISTIC  
// 	double sepration_connection = 4;//CV_HOUGH_PROBABILISTIC  
	double theta = CV_PI  / 180;
// 	double threshold = 100;
// 	double min_length = 150;//CV_HOUGH_PROBABILISTIC  
// 	double sepration_connection = 40;//CV_HOUGH_PROBABILISTIC  

	int size = MAX(src->width, src->height);
	double threshold = (double)size / 8;
	double min_length = (double)size / 5;
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

