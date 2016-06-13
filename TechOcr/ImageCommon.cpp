#include "ImageCommon.h"
double ComPointToLineDist(int x, int y, int x1, int y1, int x2, int y2) {
	double a = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
	double b = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	double s = a / b;
	return abs(s);
}
double ComPointToLineDist(CvPoint *p, CvPoint *p1, CvPoint *p2) {
	return ComPointToLineDist(p->x, p->y, p1->x, p1->y, p2->x, p2->y);
}
double ComPointToLineDist(float *p, float *l) {
	return ComPointToLineDist((int)(*(p + 2)), (int)(*(p + 3)),
		(int)(*(l + 2)), (int)(*(l + 3)), (int)(*(l + 2) + *(l + 0) * 1000), (int)(*(l + 3) + *(l + 1) * 1000));
}
bool ComIsRectIntersect(Box *b1, Box *b2, int space) {
	int minx, maxx, miny, maxy;
	minx = MAX(b1->x - space, b2->x - space);
	miny = MAX(b1->y - space, b2->y - space);
	maxx = MIN(b1->x + b1->w + space, b2->x + b2->w + space);
	maxy = MIN(b1->y + b1->h + space, b2->y + b2->h + space);

	if (minx < maxx && miny < maxy)	return true;
	else return false;
}
bool ComIsLineIntersect(CvPoint *p1, CvPoint *p2, CvPoint *p3, CvPoint *p4, CvPoint2D32f &p) {
	int d = (p2->y - p1->y)*(p4->x - p3->x) - (p4->y - p3->y)*(p2->x - p1->x);
	if (d == 0)
		return false;

	int tx = (p2->x - p1->x)*(p4->x - p3->x)*(p3->y - p1->y) +
		(p2->y - p1->y)*(p4->x - p3->x)*p1->x -
		(p4->y - p3->y)*(p2->x - p1->x)*p3->x;
	int ty = (p2->y - p1->y)*(p4->y - p3->y)*(p3->x - p1->x) +
		(p2->x - p1->x)*(p4->y - p3->y)*p1->y -
		(p4->x - p3->x)*(p2->y - p1->y)*p3->y;

	p.x = 1.0f * tx / d;
	p.y = -1.0f * ty / d;
	return true;
}
bool ComIsLineIntersect(CvPoint2D32f *p1, CvPoint2D32f *p2, CvPoint2D32f *p3, CvPoint2D32f *p4, CvPoint2D32f &p) {
	float d = (p2->y - p1->y)*(p4->x - p3->x) - (p4->y - p3->y)*(p2->x - p1->x);
	if (d == 0)
		return false;

	float tx = (p2->x - p1->x)*(p4->x - p3->x)*(p3->y - p1->y) +
		(p2->y - p1->y)*(p4->x - p3->x)*p1->x -
		(p4->y - p3->y)*(p2->x - p1->x)*p3->x;
	float ty = (p2->y - p1->y)*(p4->y - p3->y)*(p3->x - p1->x) +
		(p2->x - p1->x)*(p4->y - p3->y)*p1->y -
		(p4->x - p3->x)*(p2->y - p1->y)*p3->y;

	p.x = 1.0f * tx / d;
	p.y = -1.0f * ty / d;
	return true;
}
bool ComIsLineIntersect(float *l1, float *l2, CvPoint2D32f &p) {
	CvPoint2D32f p1 = cvPoint2D32f(*(l1 + 2), *(l1 + 3));
	CvPoint2D32f p2 = cvPoint2D32f(*(l1 + 2) + *(l1 + 0) * 1000, *(l1 + 3) + *(l1 + 1) * 1000);
	CvPoint2D32f p3 = cvPoint2D32f(*(l2 + 2), *(l2 + 3));
	CvPoint2D32f p4 = cvPoint2D32f(*(l2 + 2) + *(l2 + 0) * 1000, *(l2 + 3) + *(l2 + 1) * 1000);
	return ComIsLineIntersect(&p1, &p2, &p3, &p4, p);
}
void ComEnlargeBox(Box *box, Box &enlarge, int deltax, int deltay) {
	enlarge.x = box->x - deltax;
	enlarge.y = box->y - deltay;
	enlarge.w = box->w + deltax + deltax;
	enlarge.h = box->h + deltay + deltay;
}
void ComEnlargeRect(CvRect *box, CvRect &enlarge, int deltax, int deltay) {
	enlarge.x = box->x - deltax;
	enlarge.y = box->y - deltay;
	enlarge.width = box->width + deltax + deltax;
	enlarge.height = box->height + deltay + deltay;
}
IplImage* ComRotateImage(IplImage *src, int angle, bool clockwise) {
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
	if (anglecalc > 90) {
		anglecalc = 90 - (anglecalc % 90);
	}
	width = (int)(sin(anglecalc * CV_PI / 180.0) * src->height) +
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
RESULT ComGbkToUtf8(char* gbk, long gsize, char*& utf8, long &usize) {
	long unicodelen = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)gbk, gsize, NULL, 0);
	WCHAR *strSrc = new WCHAR[unicodelen + 16];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)gbk, gsize, strSrc, unicodelen);

	usize = WideCharToMultiByte(CP_UTF8, 0, strSrc, unicodelen, NULL, 0, NULL, NULL);
	utf8 = new char[usize + 16];
	WideCharToMultiByte(CP_UTF8, 0, strSrc, unicodelen, (LPSTR)utf8, usize, NULL, NULL);
	utf8[usize] = 0;
	delete[]strSrc;
	return RESULT_OK;
}
RESULT ComUtf8ToGbk(char* utf8, long usize, char*& gbk, long &gsize) {
	long unicodelen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, usize, NULL, 0);
	WCHAR *strSrc = new WCHAR[unicodelen + 16];
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, usize, strSrc, unicodelen);

	gsize = WideCharToMultiByte(CP_ACP, 0, strSrc, unicodelen, NULL, 0, NULL, NULL);
	gbk = new char[gsize + 16];
	WideCharToMultiByte(CP_ACP, 0, strSrc, unicodelen, (LPSTR)gbk, gsize, NULL, NULL);
	gbk[gsize] = 0;
	delete[]strSrc;
	return RESULT_OK;
}
void ComDrawBoxs(IplImage *img, Boxa *boxa, CvScalar *color, int width) {
	CvRect	cvrect;
	Box		**box;
	int		i;

	if (!img || !boxa || !boxa->box) {
		return;
	}
	box = boxa->box;
	for (i = 0; i < boxa->n; i++) {
		cvrect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
		cvRectangleR(img, cvrect, *color, width);
		box++;
	}
}
void ComDrawLines(IplImage *img, CvSeq *lines, bool drawpoint, CvScalar *color, int width, CvScalar *pointcolor) {
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
void ComDrawLineForFitLine(IplImage *img, float *line, CvScalar *color, int width) {
	CvPoint p1 = cvPoint((int)(*(line + 2) + *(line + 0) * 1000), *(line + 3) + *(line + 1) * 1000);
	CvPoint p2 = cvPoint((int)(*(line + 2) - *(line + 0) * 1000), *(line + 3) - *(line + 1) * 1000);
	cvLine(img, p1, p2, *color, width);
}
void ComDrawLineForFitLine(IplImage *img, CvSeq *lines, CvScalar *color, int width) {
	int		i;
	for (i = 0; i < lines->total; i++)
	{
		float* line = (float*)cvGetSeqElem(lines, i);
		ComDrawLineForFitLine(img, line, color, width);
	}
}
void ComShowImage(char *name, IplImage *img) {
	RECT	rect;
	HWND	hWnd;
	IplImage *dst;
	CvSize	resize;

	if (!name || !img) {
		return;
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
static int ComparePoint(void* _a, void* _b, void* userdata) {
	CvPoint *p1 = (CvPoint*)_a;
	CvPoint *p2 = (CvPoint*)_b;
	CvPoint2D32f *p = (CvPoint2D32f*)userdata;

	float d1 = p->x * p1->x + p->y * p1->y;
	float d2 = p->x * p2->x + p->y * p2->y;
	return d1 > d2 ? 1 : d1 < d2 ? -1 : 0;
}
static int CompareDistance(void* _a, void* _b, void* userdata) {
	CvPoint2D32f *p1 = (CvPoint2D32f*)_a;
	CvPoint2D32f *p2 = (CvPoint2D32f*)_b;
	CvPoint2D32f *pc = (CvPoint2D32f*)userdata;

	float d1 = (p1->x - pc->x) * (p1->x - pc->x) + (p1->y - pc->y) * (p1->y - pc->y);
	float d2 = (p2->x - pc->x) * (p2->x - pc->x) + (p2->y - pc->y) * (p2->y - pc->y);
	return d1 > d2 ? 1 : d1 < d2 ? -1 : 0;
}
static int CompareClockwise(const void *_a, const void *_b, void *userdata) {
	CvPoint2D32f *a = (CvPoint2D32f*)_a;
	CvPoint2D32f *b = (CvPoint2D32f*)_b;
	CvPoint2D32f *cog = (CvPoint2D32f *)userdata;

	float aa = cvFastArctan((a->y) - cog->y, cog->x - (a->x));
	float ba = cvFastArctan((b->y) - cog->y, cog->x - (b->x));
	return(aa < ba ? 1 : aa > ba ? -1 : 0);
}
IplImage* TrImageThreshold(IplImage *src, int gaussianx, int gaussiany, int thresvalue, int adaptivesize, int adaptivedelta) {
	IplImage *dst;

	if (!src) {
		return NULL;
	}
	dst = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	if (src->nChannels > 1) {
		cvCvtColor(src, dst, CV_BGR2GRAY);
	}
	else {
		cvCopy(src, dst);
	}
	if (gaussianx)
		cvSmooth(dst, dst, CV_GAUSSIAN, gaussianx, gaussiany);
	cvAdaptiveThreshold(dst, dst, thresvalue, CV_ADAPTIVE_THRESH_MEAN_C,
		CV_THRESH_BINARY_INV, adaptivesize, adaptivedelta);
	return dst;
}
IplImage* TrContourDraw(IplImage *src, bool &havelarge, double arate, double lrate) {
	CvSeq *conts;
	CvSeq *nowcont;
	CvSeq *hull;
	IplImage *proc, *dst;
	CvMemStorage *nowstore;
	double arearate, area;
	double lenrate, len;
	int linewidth = (int)(MIN(src->width, src->height) / 300) + 1;
	havelarge = false;
	if (!src) {
		return NULL;
	}
	nowstore = cvCreateMemStorage(0);													// release in self function
	conts = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), nowstore);	// release in self function
	proc = cvCloneImage(src);
	dst = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	cvZero(dst);

	cvFindContours(proc, nowstore, &conts, sizeof(CvContour),
		CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
	arearate = src->width * src->height * arate;
	lenrate = MIN(src->width, src->height) * lrate;
	for (nowcont = conts; nowcont; nowcont = nowcont->h_next) {
		area = fabs(cvContourArea(nowcont));
		if (area > arearate) {
			havelarge = true;
// I am NOT sure, should add hull ?
//			hull = cvConvexHull2(nowcont, nowstore, CV_CLOCKWISE, 1 /* MUST BE 1*/);
//			cvDrawContours(dst, hull, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, linewidth /*THICK_WIDTH*/);
			cvDrawContours(dst, nowcont, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, linewidth /*THICK_WIDTH*/);
		}
	}
	if (!havelarge) {
		for (nowcont = conts; nowcont; nowcont = nowcont->h_next) {
			len = fabs(cvArcLength(nowcont));
			if (len > lenrate) {
				cvDrawContours(dst, nowcont, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, linewidth /*THICK_WIDTH*/);
			}
		}
	}
	cvReleaseMemStorage(&nowstore);
	cvReleaseImage(&proc);
	return dst;
}
CvSeq* TrCreateHoughLines(IplImage *src, CvMemStorage *storage, double thresholdrate, double lengthrate, double seprationrate) {
	CvSeq* lines = NULL;
	double size = (double)MIN(src->width, src->height);
	double rho = size / 1000;
	double theta = 2 * CV_PI / 180;

	double threshold = size * thresholdrate;
	double min_length = size * lengthrate;
	double sepration_connection = size * seprationrate;

	lines = cvHoughLines2(src, storage, CV_HOUGH_PROBABILISTIC, rho, theta,
		threshold, min_length, sepration_connection);
// 	 	cvRelease((void**)lines);
	return lines;
}
CvSeq* TrAggregationLines(CvSeq *lines, CvMemStorage *storage, double thresholdrate) {
	CvSeq *cstart = NULL, *cend = NULL;
	CvSeq *cnow = NULL;
	CvSeq *output = NULL;
	int i;
	CvPoint *line;
	CvPoint *point;
	double d1, d2;
	float fitline[8];

	if (!lines || lines->total < 4) {
		return NULL;
	}
	for (i = 0; i < lines->total; i++)
	{
		line = (CvPoint*)cvGetSeqElem(lines, i);				// for every lines
		cnow = cstart;
		while (cnow) {
			point = (CvPoint*)cvGetSeqElem(cnow, 0);
			d1 = ComPointToLineDist(line, point, point + 1);
			d2 = ComPointToLineDist(line + 1, point, point + 1);
			if (d1 + d2 < thresholdrate) {						// two line is approximate
				cvSeqPush(cnow, line);
				cvSeqPush(cnow, line + 1);
				break;
			}
			cnow = cnow->h_next;
		}
		if (!cnow) {											// another kind of line
			cnow = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage);	// for cvFitLine format
			if (cstart) {
				cend->h_next = cnow;
			}
			else {
				cstart = cend = cnow;
			}
			cvSeqPush(cnow, line);
			cvSeqPush(cnow, line + 1);
			cnow->h_prev = cstart;
			cnow->h_next = NULL;
			cend = cnow;
		}
	}
// for display to confirm
// 	CvPoint *line1, *line2;
// 	cnow = cstart;
// 	while (cnow) {
// 		std::cout << cnow->total << std::endl;
// 		for (i = 0; i < cnow->total; i += 2) {
// 			line1 = (CvPoint*)cvGetSeqElem(cnow, i);
// 			line2 = (CvPoint*)cvGetSeqElem(cnow, i + 1);
// 			std::cout << line1->x << ", " << line1->y << ", TO " << line2->x << ", " << line2->y << std::endl;
// 		}
// 		std::cout << std::endl << std::endl;
// 		cnow = cnow->h_next;
// 	}

	cnow = cstart;
	output = cvCreateSeq(CV_32FC4, sizeof(CvSeq), sizeof(float) * 4, storage);
	// 四个浮点数为cvFitLines()的返回格式。
	while (cnow) {
		cvFitLine(cnow, CV_DIST_L2, 1, 0.001, 0.001, fitline);
		cvSeqPush(output, fitline);
// 		std::cout << fitline[0] << ", " << fitline[1] << ", " << fitline[2] << ", " << fitline[3] << std::endl;
		cnow = cnow->h_next;
		if (cnow) {
			cvClearSeq(cnow->h_prev);
			cnow->h_prev = NULL;
		}
	}
	return output;
}
static void pushMaxLine(CvSeq *line, CvSeq *output) {
	int i, j;
	float *l1, *l2;
	double dist, maxdist = 0;
	int maxi, maxj;

	for (i = 0; i < line->total; i++) {
		l1 = (float*)cvGetSeqElem(line, i);
		for (j = i + 1; j < line->total; j++) {
			l2 = (float*)cvGetSeqElem(line, j);
			dist = ComPointToLineDist(l1, l2);
			if (dist > maxdist) {
				maxdist = dist;
				maxi = i;
				maxj = j;
			}
		}
	}
	l1 = (float*)cvGetSeqElem(line, maxi);
	l2 = (float*)cvGetSeqElem(line, maxj);
	cvSeqPush(output, l1);
	cvSeqPush(output, l2);
}
CvSeq* TrChoiceLinesInFitLines(CvSeq *lines, CvMemStorage *storage) {
	CvSeq *hlines, *vlines;
	CvSeq *output;
	int i;
	float *line;

	hlines = cvCreateSeq(CV_32FC4, sizeof(CvSeq), sizeof(float) * 4, storage);
	vlines = cvCreateSeq(CV_32FC4, sizeof(CvSeq), sizeof(float) * 4, storage);
	output = cvCreateSeq(CV_32FC4, sizeof(CvSeq), sizeof(float) * 4, storage);
	for (i = 0; i < lines->total; i++) {
		line = (float*)cvGetSeqElem(lines, i);
		if (abs(*line) > abs(*(line + 1))) cvSeqPush(hlines, line);
		else cvSeqPush(vlines, line);
	}
	pushMaxLine(hlines, output);
	pushMaxLine(vlines, output);
	cvRelease((void**)&hlines);
	cvRelease((void**)&vlines);

	return output;
}
CvSeq* TrGetFourCorner(CvSeq *fitlines, CvMemStorage *storage, CvPoint2D32f *center) {
	int i, j, total;
	float *l0, *l1;
	CvSeq *output;
	CvPoint2D32f p;

	if (!fitlines) {
		return NULL;
	}
	total = fitlines->total;
	output = cvCreateSeq(CV_32FC2, sizeof(CvSeq), sizeof(CvPoint2D32f), storage);

	for (i = 0; i < total; i++) {
		l0 = (float*)cvGetSeqElem(fitlines, i);
		for (j = i + 1; j < total; j++) {
			l1 = (float*)cvGetSeqElem(fitlines, j);
			if (ComIsLineIntersect(l0, l1, p))
				cvSeqPush(output, &p);
		}
	}
	if (output->total < 4) {
		cvClearSeq(output);
		return NULL;
	}
	cvSeqSort(output, (CvCmpFunc)CompareDistance, center);

	cvSeqPopMulti(output, NULL, output->total - 4, 0);
	cvSeqSort(output, (CvCmpFunc)CompareClockwise, center);

// display four corner
// 	CvPoint2D32f *readp;
// 	for (i = 0; i < output->total; i++) {
// 		readp = (CvPoint2D32f*)cvGetSeqElem(output, i);
// 		std::cout << readp->x << ", " << readp->y << std::endl;
// 	}
// 	std::cout << std::endl;
	return output;
}
RESULT TechOcrGetFourCorner(IplImage *img, CvPoint2D32f *corner, int loop) {
	RESULT result = RESULT_OK;
	IplImage *thresimg, *contimg;
	IplImage *doingimg, *olddoingimg;
	CvMemStorage *storage;
	CvSeq *lines, *linesappr;
	CvSeq *conners;
	bool havelarge = false;
	int minsize = MIN(img->width, img->height);
	int i;

	storage = cvCreateMemStorage(0);

	doingimg = cvCloneImage(img);
	while (!havelarge && loop) {
		olddoingimg = doingimg;
		thresimg = TrImageThreshold(doingimg, 0, 0);
		doingimg = TrContourDraw(thresimg, havelarge);
		loop--;
		if (havelarge || !loop) {
			break;
		}
		else {
			cvReleaseImage(&olddoingimg);
			cvReleaseImage(&thresimg);
		}
	}
	if (!havelarge) {
		result = RESULT_MAYBE;
	}
	contimg = doingimg;

	lines = TrCreateHoughLines(contimg, storage);
// 	 	cvRelease((void**)lines);

	linesappr = TrAggregationLines(lines, storage, minsize / 30);
// 	cvRelease((void**)linesappr);
	if (false) {			// should add condition for draw 
		ComDrawLineForFitLine(img, linesappr);
	}
	if (!linesappr || linesappr->total == 0) {
		result = RESULT_ERR;
	}
	else
	{
		if (linesappr->total != 4) {
			linesappr = TrChoiceLinesInFitLines(linesappr, storage);
			
			result = RESULT_MAYBE;
		}
		conners = TrGetFourCorner(linesappr, storage, &cvPoint2D32f(img->width / 2, img->height / 2));
		if (!conners || conners->total != 4) {
			result = RESULT_ERR;
		}
		else {
			for (i = 0; i < 4; i++) {
				*corner = *(CvPoint2D32f*)cvGetSeqElem(conners, i);
				corner++;
			}
		}
// 		cvRelease((void**)linesappr);
// 		cvRelease((void**)conners);
// 		cvRelease((void**)lines);
	}
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&contimg);
	cvReleaseImage(&thresimg);
	cvReleaseImage(&olddoingimg);
	return result;
}


static int BITMASK32[4] = { 0xff, 0xffff, 0xffffff, 0xffffffff };
CvMemStorage* GetGlobalStorage(void) {
	static CvMemStorage *GlobalStorage = NULL;
	if (!GlobalStorage) {
		GlobalStorage = cvCreateMemStorage(0);
	}
	return GlobalStorage;
}
void ComReleaseCharFound(CvSeq *foundlist) {
	int i;
	CharFound *found;
	for (i = 0; i < foundlist->total; i++) {
		found = (CharFound*)cvGetSeqElem(foundlist, i);
		if (found->desc) {
			delete[] found->desc;
		}
	}
	cvRelease((void**)&foundlist);
}

static CvSeq* GetFormatStart(void) {
	static CvSeq FormatStart;
	static bool first = true;
	if (!first) {
		cvZero(&FormatStart);
		first = false;
	}
	return &FormatStart;
}
static void Assign(CvRect &rect, Box box) {
	rect.x = box.x;
	rect.y = box.y;
	rect.width = box.w;
	rect.height = box.h;
}
static void Assign(CvRect &rect, int x, int y, int w, int h) {
	rect.x = x;
	rect.y = y;
	rect.width = w;
	rect.height = h;
}
static void Assign(char *&d, char *c, TrEncodeMode encode) {
	char *utf8;
	long size;
	if (encode == ENCODE_GBK) {
		ComGbkToUtf8(c, strlen(c), utf8, size);
		d = utf8;
	}
	else {
		size = strlen(c);
		d = new char[strlen(c) + 16];
		strncpy(d, c, size);
	}
}
static void AssignRemoveCR(int &i, char *c, TrEncodeMode encode) {
	char *utf8;
	long usize;
	unsigned char *uc;
	int l;
	if (encode == ENCODE_GBK) {
		ComGbkToUtf8(c, strlen(c), utf8, usize);
		c = utf8;
	}
	l = strlen(c);
	uc = (unsigned char*)c;
	while (l && uc[l - 1] < 0x20) {
		l--;
	}
	if (l > 4 || l <= 0) {
		i = 0;
	}
	else {
		i = *((int*)c) & BITMASK32[l - 1];
	}
	if (encode == ENCODE_GBK) {
		delete[] utf8;
	}
}
bool ComIsWordBox(Box *box, Pix *pix, int largerate, int smallrate) {
	int l, s, bl, bs;
	s = MIN(pix->w, pix->h);
	l = MAX(pix->w, pix->h);
	bs = MIN(box->w, box->h);
	bl = MAX(box->w, box->h);
	if (box->w > s / smallrate && box->w < l / largerate &&
		box->h > s / smallrate && box->h < l / largerate &&
		bl / bs < 2) {
		return true;
	}
	return false;
}
bool ComIsBoxIsolated(Box *box, Boxa *boxa, int space) {
	int i, count = 0;
	Box **now;
	now = boxa->box;
	for (i = 0; i < boxa->n; i++) {
		if (ComIsRectIntersect(box, *now, space)) {
			count++;
		}
		now++;
	}
	if (count == 1) return true;
	else return false;
}
void ComCenterPoint(CvPoint2D32f *corner, CvPoint2D32f *dstcornet) {
	dstcornet[0].x = dstcornet[3].x = ((corner + 0)->x + (corner + 3)->x) / 2;
	dstcornet[1].x = dstcornet[2].x = ((corner + 1)->x + (corner + 2)->x) / 2;
	dstcornet[0].y = dstcornet[1].y = ((corner + 0)->y + (corner + 1)->y) / 2;
	dstcornet[2].y = dstcornet[3].y = ((corner + 2)->y + (corner + 3)->y) / 2;
}
IplImage* TrWarpPerspective(IplImage *img, int w, int h, CvMat *warp, CvMat *warp2) {
	IplImage *dst = cvCreateImage(cvSize(w, h), img->depth, img->nChannels);
	cvZero(dst);
	CvMat *totalwarp;

	if (warp2 != NULL) {
		CvMat *totalwarp = cvCreateMat(3, 3, CV_32FC1);
		cvMatMul(warp2, warp, totalwarp);			// the order is important
		cvWarpPerspective(img, dst, totalwarp);
		cvReleaseMat(&totalwarp);
	}
	else {
		cvWarpPerspective(img, dst, warp);
	}

	return dst;
}
Pix* TrPixCreateFromIplImage(IplImage *img) {
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
tesseract::TessBaseAPI* TechOcrInitTessAPI() {
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI;
	int rc = api->Init(NULL, DEFAULT_LANGURE, tesseract::OEM_DEFAULT);
	if (rc)
		return NULL;
	return api;
}
void TechOcrExitTessAPI(tesseract::TessBaseAPI *api) {
	api->End();
	delete api;
}
Boxa* TrChoiceBoxInBoxa(tesseract::TessBaseAPI *api, Pix *pix) {
	Pixa *pixa;
	Boxa *boxa, *boxc, *boxr;
	Box  **box;
	int i;
	int size = MAX(pix->w, pix->h);

// 	api->SetImage(pix);
// 	boxa = api->GetConnectedComponents(&pixa);
	boxa = api->GetWords(&pixa);
	if (!boxa || !boxa->box) {
		return NULL;
	}

	boxc = boxaCreate(boxa->n);
	box = boxa->box;
	for (i = 0; i < boxa->n; i++) {
		// detect size
		if (ComIsWordBox(*box, pix)) {
			boxaAddBox(boxc, *box, L_CLONE);
		}
		box++;
	}
	boxaDestroy(&boxa);
// 	return boxc;

	boxr = boxaCreate(boxc->n);
	box = boxc->box;
	for (i = 0; i < boxc->n; i++) {
		if (ComIsBoxIsolated(*box, boxc, size / 300)) {
			boxaAddBox(boxr, *box, L_CLONE);
		}
		box++;
	}
	boxaDestroy(&boxc);
	return boxr;
}
char* TrTranslateInRect(tesseract::TessBaseAPI *api, tesseract::PageSegMode mode, TrEncodeMode encode, Box *box) {
	// encode for ENCODE_GB2312 or ENCODE_UTF8
	char *utfstr, *str;
	long gsize;

	api->SetPageSegMode(mode);
	api->SetRectangle(box->x, box->y, box->w, box->h);

	api->Recognize(NULL);
	if (encode == ENCODE_GBK) {
		utfstr = api->GetUTF8Text();
		ComUtf8ToGbk(utfstr, (long)strlen(utfstr), str, gsize);
		delete[] utfstr;
	}
	else if (encode == ENCODE_UTF8) {
		str = api->GetUTF8Text();
	}
	return str;
}
char* TrTranslateInRect(tesseract::TessBaseAPI *api, tesseract::PageSegMode mode, TrEncodeMode encode, CvRect *rect) {
	// the first 4 * 32 bit is same for CvRect and Box
	return TrTranslateInRect(api, mode, encode, (Box*)rect);
}
RESULT TechOcrCreatePix(IplImage *img, int w, int h, CvPoint2D32f *corner, Pix *&pix, CvMat *warp) {
	IplImage *dst;
	CvPoint2D32f dstcornet[4];
// 	CvMat *warp;

	if (corner) {
// 		warp = cvCreateMat(3, 3, CV_32FC1);

		ComCenterPoint(corner, dstcornet);
		cvGetPerspectiveTransform(corner, dstcornet, warp);
		dst = TrWarpPerspective(img, w, h, warp);
// 		cvReleaseMat(&warp);
	}
	else {
		dst = img;
	}
	pix = TrPixCreateFromIplImage(dst);
	if (corner) {
		cvReleaseImage(&dst);
	}
	return RESULT_OK;
}
RESULT TechOcrGetFeatureChar(Pix *pix, tesseract::TessBaseAPI *api, CvSeq *feature) {
	RESULT result;
	Boxa *boxa;
	Box **box;
	char *str;
	int i;
	CharFound found;
// 	CvMemStorage *storage;

	api->SetImage(pix);

	boxa = TrChoiceBoxInBoxa(api, pix);

	box = boxa->box;
	for (i = 0; i < boxa->n; i++) {
		str = TrTranslateInRect(api, tesseract::PSM_SINGLE_CHAR, ENCODE_UTF8, *box);
		Assign(found.rect, **box);
		AssignRemoveCR(found.found, str, ENCODE_UTF8);
		found.desc = NULL;
		cvSeqPush(feature, &found);
		delete[] str;
		box++;
	}
	boxaDestroy(&boxa);
	return RESULT_OK;
}
RESULT TechOcrCreateFormat(CvSeq *&format, char *name, int w, int h, TrEncodeMode encode) {
	static volatile MYINT nowid = 0;
	CvMemStorage *storage;
	CvSeq *last;
	CharFound found;

	last = GetFormatStart();
	storage = GetGlobalStorage();
	while (last->h_next) {
		last = last->h_next;
	}
	format = cvCreateSeq(0, sizeof(CvSeq), sizeof(CharFound), storage);			// this is global, not free
	last->h_next = format;
	format->h_next = NULL;

	Assign(found.rect, 0, 0, w, h);
	found.found = (int)InterInc(&nowid) - 1;
	Assign(found.desc, name, encode);
	found.chartype = CHARTYPE_FORMAT_ID;
	cvSeqPush(format, &found);
	return RESULT_OK;
}
RESULT TechOcrFormatAddFeature(CvSeq *format, int x, int y, int w, int h, char *c, TrEncodeMode encode) {
	CharFound found;

	Assign(found.rect, x, y, w, h);
	AssignRemoveCR(found.found, c, encode);
	found.chartype = CHARTYPE_FEATURE;
	found.desc = NULL;
	cvSeqPush(format, &found);
	return RESULT_OK;
}
RESULT TechOcrFormatAddContent(CvSeq *format, int x, int y, int w, int h, char *c, int mode, TrEncodeMode encode) {
	CharFound found;

	Assign(found.rect, x, y, w, h);
	Assign(found.desc, c, encode);
	found.chartype = mode;
	cvSeqPush(format, &found);
	return RESULT_OK;
}
void TrGetFormatScreenRect(CvSeq *format, int &w, int &h) {
	int i;
	CharFound *found;
	for (i = 0; i < format->total; i++) {
		found = (CharFound*)cvGetSeqElem(format, i);
		if (found->chartype == CHARTYPE_FORMAT_ID) {
			w = found->rect.width;
			h = found->rect.height;
			return;
		}
	}
}

#define DELTA_RATE 0.1
#define DELTA_DISTANCE 10
int TrFeatureMatch(CvSeq *feature, CvSeq *format, int feaorder, int fororder) {
	int fealen, forlen;
	CharFound *feachar, *forchar;
	float xrate, yrate;
	int forx, foxy, feax, feay;
	float shouldx, shouldy, shouldw, shouldh;
	int i, j;
	int match = 0;

	// 	int maxdeltax = 0, maxdeltay = 0;

	fealen = feature->total;
	forlen = format->total;
	forchar = (CharFound*)cvGetSeqElem(format, fororder);
	feachar = (CharFound*)cvGetSeqElem(feature, feaorder);
	xrate = (float)forchar->rect.width / feachar->rect.width;
	yrate = (float)forchar->rect.height / feachar->rect.height;
	forx = forchar->rect.x;
	foxy = forchar->rect.y;
	feax = feachar->rect.x;
	feay = feachar->rect.y;

	for (j = 0; j < fealen; j++) {
		feachar = (CharFound*)cvGetSeqElem(feature, j);
		feachar->chartype = CHARTYPE_NOT_MATCH;
	}
	for (i = 0; i < forlen; i++) {
		forchar = (CharFound*)cvGetSeqElem(format, i);
		if (forchar->chartype != CHARTYPE_FEATURE) {
			continue;
		}
		shouldx = (forchar->rect.x - forx) / xrate + feax;
		shouldy = (forchar->rect.y - foxy) / yrate + feay;
		shouldw = (float)forchar->rect.width / xrate;
		shouldh = (float)forchar->rect.height / yrate;

		for (j = 0; j < fealen; j++) {
			feachar = (CharFound*)cvGetSeqElem(feature, j);
			if (forchar->found != feachar->found) {
				continue;
			}
			if ((feachar->rect.x > shouldx * (1 - DELTA_RATE) - DELTA_DISTANCE) &&
				(feachar->rect.x < shouldx * (1 + DELTA_RATE) + DELTA_DISTANCE) &&
				(feachar->rect.y > shouldy * (1 - DELTA_RATE) - DELTA_DISTANCE) &&
				(feachar->rect.y < shouldy * (1 + DELTA_RATE) + DELTA_DISTANCE)) {
				feachar->chartype = CHARTYPE_MATCH | i;
				match++;
				// 				if (abs(feachar->rect.x - shouldx) > maxdeltax) maxdeltax = abs(feachar->rect.x - shouldx);
				// 				if (abs(feachar->rect.x - shouldx) > maxdeltay) maxdeltay = abs(feachar->rect.x - shouldx);
				break;
			}
		}
	}
	return match;
}
int TrFeatureMostMatch(CvSeq *feature, CvSeq *format, int &feaorder, int &fororder) {
	int fealen, forlen;
	int i, j;
	CharFound *feachar, *forchar;
	int matched, maxmatch = 0;

	if (!feature || !format) {
		return 0;
	}
	fealen = feature->total;
	forlen = format->total;
	if (!fealen || !forlen) {
		return 0;
	}

	for (i = 0; i < forlen; i++) {
		forchar = (CharFound*)cvGetSeqElem(format, i);
		if (forchar->chartype != CHARTYPE_FEATURE) {
			continue;
		}
		for (j = 0; j < fealen; j++) {
			feachar = (CharFound*)cvGetSeqElem(feature, j);
			if (forchar->found != feachar->found) {
				continue;
			}
			matched = TrFeatureMatch(feature, format, j, i);
			if (matched > maxmatch) {
				maxmatch = matched;
				fororder = i;
				feaorder = j;
			}
		}
	}
	return maxmatch;
}
void TrGetCornerInMatch(CvSeq *feature, CvSeq *format, int maxfea, int maxfor, CvPoint2D32f *csrc, CvPoint2D32f *cdst) {
	int tl = MAXINT, tr = -1 * MAXINT, bl = MAXINT, br = -1 * MAXINT;
	int j, fealen;
	CharFound *feachar, *forchar;
	CvRect *rect;
	int cornum[4];

	if (!format) {
		csrc = cdst = NULL;
		return;
	}
	// for set CHARTYPE_MATCH 
	TrFeatureMatch(feature, format, maxfea, maxfor);

	fealen = feature->total;
	for (j = 0; j < fealen; j++) {
		feachar = (CharFound*)cvGetSeqElem(feature, j);
		if (feachar->chartype & CHARTYPE_MATCH) {
			rect = &feachar->rect;
			if (rect->x + rect->y < tl) {
				tl = rect->x + rect->y;
				cornum[0] = j;
			}
			if (rect->x + rect->width - rect->y > tr) {
				tr = rect->x + rect->width - rect->y;
				cornum[1] = j;
			}
			if (rect->x - rect->y - rect->height < bl) {
				bl = rect->x - rect->y - rect->height;
				cornum[3] = j;
			}
			if (rect->x + rect->y + rect->width + rect->height > br) {
				br = rect->x + rect->y + rect->width + rect->height;
				cornum[2] = j;
			}
		}
	}

	for (j = 0; j < 4; j++) {
		feachar = (CharFound*)cvGetSeqElem(feature, cornum[j]);
		forchar = (CharFound*)cvGetSeqElem(format, feachar->chartype & (~CHARTYPE_MATCH));
		(csrc + j)->x = (float)feachar->rect.x;
		(csrc + j)->y = (float)feachar->rect.y;
		(cdst + j)->x = (float)forchar->rect.x;
		(cdst + j)->y = (float)forchar->rect.y;
		if (j == 1 || j == 2) {
			(csrc + j)->x += (float)feachar->rect.width;
			(cdst + j)->x += (float)forchar->rect.width;
		}
		if (j == 2 || j == 3) {
			(csrc + j)->y += (float)feachar->rect.height;
			(cdst + j)->y += (float)forchar->rect.height;
		}
	}
}
#define MIN_CONFIRM				6
RESULT TechOcrFormatMostMatch(CvSeq *feature, CvSeq *&bestformat, int &maxmatch, CvMat *wrap) {		// *& only returen pointer
	CvSeq *now;
	RESULT result = RESULT_ERR;
	int matched;
	int feaorder, fororder;
	int maxfea, maxfor;
	CvPoint2D32f csrc[4], cdst[4];

	if (bestformat) {
		maxmatch = TrFeatureMostMatch(feature, bestformat, maxfea, maxfor);
		if (maxmatch > MIN_CONFIRM)
			result = RESULT_OK;
	}
	else {
		maxmatch = MIN_CONFIRM;
		now = GetFormatStart()->h_next;
		while (now) {
			matched = TrFeatureMostMatch(feature, now, feaorder, fororder);
			if (matched > maxmatch) {
				bestformat = now;
				maxmatch = matched;
				maxfea = feaorder;
				maxfor = fororder;
				result = RESULT_OK;
			}
			now = now->h_next;
		}
	}
	if (result == RESULT_OK) {
		TrGetCornerInMatch(feature, bestformat, maxfea, maxfor, csrc, cdst);
		cvGetPerspectiveTransform(csrc, cdst, wrap);
	}
	return result;
}



#define ENLAGRE_X     16
#define ENLARGE_Y     24
#define MIN_WORD_SIZE 30
bool ComBoxInRect(Box *box, CvRect *rect) {
	if (box->x + box->w > rect->x + MIN_WORD_SIZE &&
		box->x < rect->x + rect->width - MIN_WORD_SIZE &&			// attention the condition
		box->y > rect->y &&
		box->y + box->h < rect->y + rect->height) {
		return true;
	}
	else {
		return false;
	}
}
CvRect ComDetectWord(Pixa *pixa, CvRect *rect) {
	int i;
	int minx = rect->x + rect->width;
	int miny = rect->y + rect->height;
	int maxx = rect->x;
	int maxy = rect->y;
	Box **box = pixa->boxa->box;
	for (i = 0; i < pixa->n; i++) {
		if ((*box)->w > MIN_WORD_SIZE && (*box)->h > MIN_WORD_SIZE &&
			ComBoxInRect(*box, rect)) {
			minx = MIN(minx, (*box)->x);
			miny = MIN(miny, (*box)->y);
			maxx = MAX(maxx, ((*box)->x + (*box)->w));
			maxy = MAX(maxy, ((*box)->y + (*box)->h));
		}
		box++;
	}
	minx = MAX(rect->x, minx);
	miny = MAX(rect->y, miny);
	maxx = MIN((rect->x + rect->width), maxx);
	maxy = MIN((rect->y + rect->height), maxy);
	if (maxx < minx || maxy < miny) {
		return *rect;
	}
	else {
		return cvRect(minx, miny, MAX(maxx - minx, 0), MAX(maxy - miny, 0));
	}
}
void ComAddToJson(std::string &str, char *chars) {
	int i;
	char now;

	for (i = 0; i < strlen(chars); i++) {
		now = *(chars + i);
		if (now > 0 && now < 0x20)
			continue;
		if (now == '\'' && now == '\\') {
			str += '\\';
		}
		str += now;
	}
}
RESULT TechOcrDetectWordsInFormat(IplImage *img, CvMat *warp1, CvMat *warp2, CvSeq *bestformat, CvSeq *content) {
	int i;
	int w, h;
	IplImage *rotated;
	Pix *pix;
	tesseract::TessBaseAPI* api;
	Boxa *boxa;
	Pixa *pixa;
	Box **box;
	CvRect rect, rectenlarge;
	CharFound* found, foundcontent;


	TrGetFormatScreenRect(bestformat, w, h);
	rotated = TrWarpPerspective(img, w, h, warp1, warp2);
	pix = TrPixCreateFromIplImage(rotated);
	api = TechOcrInitTessAPI();
	api->SetImage(pix);
	boxa = api->GetWords(&pixa);
	tesseract::PageSegMode mode;
	for (i = 0; i < bestformat->total; i++) {
		found = (CharFound*)cvGetSeqElem(bestformat, i);
		if (found->chartype != CHARTYPE_CONTENT_BLOCK && found->chartype != CHARTYPE_CONTENT_WORD) {
			continue;
		}
		rect = ComDetectWord(pixa, &(found->rect));
		ComEnlargeRect(&rect, rect, ENLAGRE_X, ENLARGE_Y);
		if (found->chartype == CHARTYPE_CONTENT_BLOCK) {
			mode = tesseract::PSM_SINGLE_BLOCK;
		}
		else {
			mode = tesseract::PSM_SINGLE_WORD;
		}
		char* str = TrTranslateInRect(api, mode, ENCODE_UTF8, &rect);
		foundcontent.desc = str;
		foundcontent.found = i;
		Assign(foundcontent.rect, rect.x, rect.y, rect.width, rect.height);
		cvSeqPush(content, &foundcontent);
		//	found->desc = str;
		//	std::cout << str << std::endl;
	}
	boxaDestroy(&boxa);
	pixaDestroy(&pixa);
	cvReleaseImage(&rotated);
	pixDestroy(&pix);
	TechOcrExitTessAPI(api);
	return RESULT_OK;
}
RESULT TechOcrOutput(CvSeq *content, CvSeq *bestformat, char *&output) {
	CharFound* found, *featurematch;
	std::string outstring;
	bool first = true;
	char * ch, *gb;
	int i, strlength;

	outstring = "{\"version\":\"";
	outstring += TECHOCR_VERSION;
	outstring += "\", \"result\":[";
	for (i = 0; i < content->total; i++) {
		found = (CharFound*)cvGetSeqElem(content, i);
		featurematch = (CharFound*)cvGetSeqElem(bestformat, found->found);
		if (first) {
			first = false;
		}
		else {
			outstring += ",";
		}
		outstring += "{\"name\":\"";
		ComAddToJson(outstring, (char*)featurematch->desc);
		outstring += "\",\"value\":\"";
		ComAddToJson(outstring, (char*)found->desc);
		outstring += "\"}";
		delete[] featurematch->desc;
		delete[] found->desc;
	}
	outstring += "]}";
	strlength = outstring.length();
	output = new char[strlength + 10];
	strncpy(output, outstring.c_str(), strlength);
	output[strlength] = 0;
	return RESULT_OK;
}
