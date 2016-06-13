#include "PocTest.h"

#define  OUTPUTBOX(b, s)	std::cout << (b)->x << ", " << (b)->y << ", " << (b)->w << ", " << (b)->h << ", " \
	<< s << std::endl;

void pocPixCreateFromIplImage(char *src, char *dst) {
	IplImage *img = cvLoadImage(src, 1);     // 1 for is colored
	if (!img) {
		return;
	}
//	cvSaveImage(dst, img);
//	Pix* pixs = pixRead("src");
//	read as Pix, for confirm the data struct

	Pix* pixt;
	pixt = TrPixCreateFromIplImage(img);
	if (pixt) {
		pixWriteAutoFormat(dst, pixt);
		pixDestroy(&pixt);
	}
	cvReleaseImage(&img);
//	pixDestroy(&pixs);
}

void pocRotateImage(char *src, int angle, bool clockwise) {
	IplImage *img, *dst;
	img = cvLoadImage(src, 1);
	if (!img) {
		return;
	}
	dst = ComRotateImage(img, 90, false);
	cvSaveImage(src, dst);

	ComShowImage("dst", dst);
	cvWaitKey(0);
	cvReleaseImage(&dst);
	cvReleaseImage(&img);
}

void pocFindMaxRect(char *src) {
	IplImage *img, *dst;

	img = cvLoadImage(src, 0);     // 0 for is gray
	dst = TrCreateMaxContour(img);
	ComShowImage("src", img);
	ComShowImage("dst", dst);
	cvWaitKey(0);
	cvReleaseImage(&dst);
	cvReleaseImage(&img);
}

void pocFindHoughLines(char *src) {
	IplImage *img, *mimg, *dst, *dst2;
	CvMemStorage* storage;
	CvSeq *lines, *lines2;

	img = cvLoadImage(src, 0);     // 0 for gray, 1 for color
	mimg = cvCloneImage(img);

//  	cvCanny(img, mimg, 50, 150, 3);//边缘检测  
// 	cvPyrMeanShiftFiltering(img, mimg, 3, 15);

	dst = TrCreateMaxContour(mimg);
	storage = cvCreateMemStorage(0);

	int size = MAX(img->width, img->height);

	lines = cvHoughLines2(dst, storage, CV_HOUGH_PROBABILISTIC,
		size / 1000, CV_PI / 180,
		(double)size / 8, (double)size / 6, (double)size / 40);
 	ComDrawLines(img, lines, true);
	dst2 = TrCreateLine(dst/* only for size**/, lines);


	ComShowImage("src", img);
	ComShowImage("mimg", mimg);
	ComShowImage("dst", dst);
	ComShowImage("dst2", dst2);

	cvWaitKey(0);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst2);
	cvReleaseImage(&mimg);
	cvReleaseImage(&img);
	cvReleaseMemStorage(&storage);
}

void pocFindFeatureWords(char *src) {
	IplImage *img;
	Pix *pix;
	tesseract::TessBaseAPI *api;
	Pixa *pixa;
	Boxa *boxa, *boxc;
	Box **box;
	int i;
	char *str;

	img = cvLoadImage(src, 1);
	if (!img) {
		return;
	}
	pix = TrPixCreateFromIplImage(img);
	api = TechOcrInitTessAPI();
	api->SetImage(pix);
// 	boxa = api->GetConnectedComponents(&pixa);
	//boxa = api->GetWords(&pixa);
	boxc = TrChoiceBoxInBoxa(api, pix);

	box = boxc->box;
	for (i = 0; i < boxc->n; i++) {
		str = TrTranslateInRect(api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK, *box);
		OUTPUTBOX(*box, str);
		delete[] str;
		box++;
	}
	ComDrawBoxs(img, boxc);
	ComShowImage("src", img);
	cvWaitKey(0);

	boxaDestroy(&boxa);
	boxaDestroy(&boxc);

	pixaDestroy(&pixa);

	TechOcrExitTessAPI(api);
	pixDestroy(&pix);
	cvReleaseImage(&img);

}

extern OcrFormat BusinessLicense;

#define MIN_MATCH		6
void pocFindFeatureWordsInClass(char *src) {
	IplImage *img;
	Pix *pix;
	tesseract::TessBaseAPI *api;
	Pixa *pixa;
	Boxa *boxa, *boxc, *boxf;
	Box **box;
	int i;
	char *str;
	OcrFeatureWordsFound *found;
	TrFeatureWordFound *result;
	int count = 0;

	ocrInitBusinessLicense();

	img = cvLoadImage(src, 1);
	if (!img) {
		return;
	}
	pix = TrPixCreateFromIplImage(img);
	api = TechOcrInitTessAPI();
	api->SetImage(pix);
// 	boxa = api->GetConnectedComponents(&pixa);
	//boxa = api->GetWords(&pixa);
	//boxc = TrChoiceBoxInBoxa(api, pix);
	found = new OcrFeatureWordsFound;

	found->InitFound(&BusinessLicense);
	box = boxc->box;
	for (i = 0; i < boxc->n; i++) {
		Box enlarge;
		ComEnlargeBox(*box, enlarge, 1);
		str = TrTranslateInRect(api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK, &enlarge);
// 		str = trTranslateInRect(*box, api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK);
		result = found->AddFound(*box, str);
		if (result) {
			count++;
		}
 		OUTPUTBOX(*box, str);
		delete[] str;
		box++;
	}
	int start = 0, match;
	while (start < found->GetNumber())
	{
		boxf = found->ReturnFound(start, match);
		if (match < MIN_MATCH) {
			boxaDestroy(&boxf);
			start++;
		}
		else {
			break;
		}
	}
	if (start < found->GetNumber()) {
		// have found feature
		ComDrawBoxs(img, boxf);
	}
	CvPoint2D32f srcTri[4], desTri[4];
	CvMat *warpMat;

	found->ReturnCorner(srcTri, desTri);
	for (i = 0; i < 4; i++)
		cvCircle(img, cvPointFrom32f(desTri[i]), 20, CV_RGB(190,11,55));
	warpMat = cvCreateMat(3, 3, CV_64F);
	cvGetPerspectiveTransform(srcTri, desTri, warpMat);
	cvReleaseMat(&warpMat);

 //	trDrawBoxs(img, boxc, &cvScalar(232,164,74));
	ComShowImage("src", img);
	cvWaitKey(0);

	boxaDestroy(&boxa);
	boxaDestroy(&boxc);
	boxaDestroy(&boxf);
	pixaDestroy(&pixa);
	delete found;			// found should delete after destroy boxf

	TechOcrExitTessAPI(api);
	pixDestroy(&pix);
	cvReleaseImage(&img);
}

void pocApproximateLine(char *filename) {
	IplImage *srcimg, *thresimg, *contimg, *cloneimg = NULL;
	CvMemStorage *storage;
	CvSeq *lines, *linesappr;
	CvSeq *conners;
	bool havelarge = false;
	int loop = 1;
	IplImage *doingimg, *olddoingimg;

	storage = cvCreateMemStorage(0);
	srcimg = cvLoadImage(filename, 1);

	doingimg = cvCloneImage(srcimg);
	while (!havelarge && loop) {
		olddoingimg = doingimg;
		thresimg = TrImageThreshold(doingimg);
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
	contimg = doingimg;

	lines = TrCreateHoughLines(contimg, storage);
	linesappr = TrAggregationLines(lines, storage);
	conners = TrGetFourCorner(linesappr, storage, &cvPoint2D32f(srcimg->width / 2, srcimg->height / 2));

	cloneimg = cvCloneImage(srcimg);
	// 	if (linesappr->total != 4) {
// 	for (int i = 0; i < linesappr->total; i++) {
// 		float *p = (float*)cvGetSeqElem(linesappr, i);
// 		CvPoint p1 = cvPoint((int)(*(p + 4)), (int)(*(p + 5)));
// 		CvPoint p2 = cvPoint((int)(*(p + 6)), (int)(*(p + 7)));
// 		cvLine(cloneimg, p1, p2, CV_RGB(23, 132, 231), 3, 8);
// 		ComDrawLineForFitLine(cloneimg, p, &CV_RGB(23, 132, 231));

// 	}
	// match four lines
	// 	}

	ComDrawLines(cloneimg, lines, true);
	ComShowImage("img", cloneimg);
	ComShowImage("cont", contimg);
	cvWaitKey(0);
	cvReleaseMemStorage(&storage);
	if (cloneimg)
		cvReleaseImage(&cloneimg);
	cvReleaseImage(&contimg);
	cvReleaseImage(&thresimg);
	cvReleaseImage(&olddoingimg);
	cvReleaseImage(&srcimg);
}


#define MIN_CONFIRM				6
CvSeq *format;
void pcoPreprocess(char *filename) {
	RESULT result;
	CvPoint2D32f corner[4];
	CvPoint2D32f *pcorner = corner;
	IplImage *src, *dst;
	Pix *pix;
	tesseract::TessBaseAPI* api;
	CvSeq *feature, *bestformat = NULL;
// 	CvPoint2D32f csrc[4], cdst[4];
// 	int mostfea, mostfor;
	int match, rotateloop = 0;

	CvMat *warp1 = cvCreateMat(3, 3, CV_32FC1);
	CvMat *warp2 = cvCreateMat(3, 3, CV_32FC1);

	src = cvLoadImage(filename, 1);
	dst = cvCloneImage(src);
	pocCreateBusinessLicense();
	for (;;) {
		api = TechOcrInitTessAPI();
		result = TechOcrGetFourCorner(dst, corner, 1);
		if (result == RESULT_ERR) {
			pcorner = NULL;
		}
		else {
			pcorner = corner;
		}
		TechOcrCreatePix(dst, dst->width, dst->height, pcorner, pix, warp1);
		result = TechOcrGetFeatureChar(pix, api, feature);
		TechOcrFormatMostMatch(feature, bestformat, match, warp2);

		if ((bestformat && match > MIN_CONFIRM) || rotateloop >= 4) {
			// record now bestformat
			break;
		}
		cvReleaseImage(&dst);
		cvRelease((void**)&feature);
		TechOcrExitTessAPI(api);
		rotateloop++;
		dst = ComRotateImage(src, 90 * rotateloop, false);
	}
	pixDestroy(&pix);
	TechOcrExitTessAPI(api);
// 	CvSeq *cnow = feature;
// 	int i;
// 	CharFound *found;
// 	char *ch;
// 	char *gb;
// 	long gsize;
// 	for (i = 0; i < cnow->total; i++) {
// 		found = (CharFound*)cvGetSeqElem(cnow, i);
// 		ch = (char*)&found->found;
// 		ComUtf8ToGbk(ch, strlen(ch), gb, gsize);
// 		std::cout << found->rect.x << ", " << found->rect.y << ", CHAR " << gb << std::endl;
// 		delete[] gb;
// 	}
// 	std::cout << std::endl << std::endl;
	cvRelease((void**)&feature);

	int w, h, i;
	TrGetFormatScreenRect(bestformat, w, h);

	IplImage *d1, *d2;
	CharFound* found;
// 	d1 = TrWarpPerspective(dst, dst->width, dst->height, warp1);
	d2 = TrWarpPerspective(dst, w, h, warp1, warp2);
	pix = TrPixCreateFromIplImage(d2);
	api = TechOcrInitTessAPI();
	api->SetImage(pix);
	Boxa *boxa;
	Pixa *pixa;
	Box **box;
	boxa = api->GetWords(&pixa);
	CvRect rect;
	tesseract::PageSegMode mode;

	for (i = 0; i < bestformat->total; i++) {
		found = (CharFound*)cvGetSeqElem(bestformat, i);
		if (found->chartype != CHARTYPE_CONTENT_BLOCK && found->chartype != CHARTYPE_CONTENT_WORD) {
			continue;
		}
		rect = comDetectWord(pixa, &(found->rect));
		rect.x -= 16;
		rect.y -= 16;
		rect.width += 32;
		rect.height += 32;

		cvRectangleR(d2, rect, CV_RGB(100, 200, 33), 3);
		if (found->chartype == CHARTYPE_CONTENT_BLOCK) {
			mode = tesseract::PSM_SINGLE_BLOCK;
		}
		else {
			mode = tesseract::PSM_SINGLE_WORD;
		}
		char* str = TrTranslateInRect(api, mode, ENCODE_GBK, &rect);
		std::cout << str << std::endl;
	}
	ComDrawBoxs(d2, boxa, &CV_RGB(200, 1, 22), 3);


// 	ComShowImage("d1", d1);
	ComShowImage("d2", d2);
	cvWaitKey(0);

	TechOcrExitTessAPI(api);

	cvReleaseMat(&warp1);
	cvReleaseMat(&warp2);

	cvReleaseImage(&d2);
	cvReleaseImage(&dst);
	cvReleaseImage(&src);
}

void pocCreateBusinessLicense(void) {
	TechOcrCreateFormat(format, "营业执照", 3120, 4160);

	TechOcrFormatAddFeature(format, 622, 1638, 61, 69, "名");
	TechOcrFormatAddFeature(format, 962, 1641, 69, 66, "称");
	TechOcrFormatAddFeature(format, 624, 1758, 65, 68, "类");
	TechOcrFormatAddFeature(format, 964, 1757, 66, 65, "型");
	TechOcrFormatAddFeature(format, 623, 1872, 68, 68, "住");
	TechOcrFormatAddFeature(format, 964, 1873, 68, 65, "所");

	TechOcrFormatAddFeature(format, 627, 2100, 64, 64, "注");
	TechOcrFormatAddFeature(format, 738, 2098, 68, 64, "册");
	TechOcrFormatAddFeature(format, 854, 2095, 62, 70, "资");
	TechOcrFormatAddFeature(format, 965, 2095, 68, 70, "本");
	
	TechOcrFormatAddFeature(format, 625, 2212, 67, 70, "成");
	TechOcrFormatAddFeature(format, 740, 2215, 65, 63, "立");
	TechOcrFormatAddFeature(format, 853, 2212, 65, 70, "日");
	TechOcrFormatAddFeature(format, 967, 2212, 65, 68, "期");

	TechOcrFormatAddFeature(format, 625, 2332, 65, 71, "营");
	TechOcrFormatAddFeature(format, 739, 2332, 67, 65, "业");
	TechOcrFormatAddFeature(format, 853, 2330, 65, 68, "期");
	TechOcrFormatAddFeature(format, 970, 2330, 63, 66, "限");

	TechOcrFormatAddFeature(format, 624, 2451, 67, 64, "经");
	TechOcrFormatAddFeature(format, 739, 2448, 66, 70, "营");
	TechOcrFormatAddFeature(format, 853, 2445, 67, 71, "范");
	TechOcrFormatAddFeature(format, 971, 2447, 59, 67, "围");

	TechOcrFormatAddFeature(format, 1562, 3436, 71, 67, "登");
	TechOcrFormatAddFeature(format, 1680, 3435, 71, 69, "记");
	TechOcrFormatAddFeature(format, 1798, 3435, 74, 70, "机");
	TechOcrFormatAddFeature(format, 1920, 3433, 67, 72, "关");

	TechOcrFormatAddContent(format, 1785, 1469, 800, 55, "注册号");
	TechOcrFormatAddContent(format, 1095, 1636, 1700, 66, "名称");
	TechOcrFormatAddContent(format, 1095, 1758, 1700, 66, "类型");
	TechOcrFormatAddContent(format, 1095, 1872, 1700, 66, "住所");
	TechOcrFormatAddContent(format, 1095, 1984, 1700, 66, "法定代表人");
	TechOcrFormatAddContent(format, 1095, 2095, 1700, 66, "注册资本");
	TechOcrFormatAddContent(format, 1095, 2212, 1700, 66, "成立日期");
	TechOcrFormatAddContent(format, 1095, 2330, 1700, 66, "营业期限");
	TechOcrFormatAddContent(format, 1095, 2440, 1700, 550, "经营范围", CHARTYPE_CONTENT_BLOCK);

	return;
}

void pocShowImage(char *src) {
	IplImage *img, *img1c;//, *imgerode;
//	CvMemStorage* storage;
	CvSeq *lines;

	Pix *pix;
	tesseract::TessBaseAPI *api;
	Pixa *pixa;
	Boxa *boxa;

	img = cvLoadImage(src, 1);
	if (!img) {
		return;
	}

// 	CvSize	resize;
// 	int lmax = MAX(img->width, img->height);
// 	double scale = lmax / 600;
// 	resize.width = (int)(img->width / scale);
// 	resize.height = (int)(img->height / scale);
// 
// 	IplImage *dst;
// 	dst = cvCreateImage(resize, img->depth, img->nChannels);
// 	cvResize(img, dst, CV_INTER_LINEAR);

//	imgerode = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
// 	storage = cvCreateMemStorage(0);
// 	lines = trCreateHoughLines(img1c, storage);
	// lines will be free by cvReleaseMemStorage(&storage);


// 	IplConvKernel *ck = cvCreateStructuringElementEx(13, 1, 0, 0, CV_SHAPE_CROSS);
// 	cvErode(img, img, ck, 1);		// enlarge
// 	cvDilate(img, img, ck, 1);
// 	cvReleaseStructuringElement(&ck);

//  	cvDilate(img, img, NULL, 5);
// 	cvErode(img, img, NULL, 5);

	img1c = TrImageThreshold(img);

	pix = TrPixCreateFromIplImage(img1c);
	api = TechOcrInitTessAPI();
	api->SetImage(pix);
// 	boxa = api->GetConnectedComponents(&pixa);
	boxa = api->GetWords(&pixa);


// 	trDrawLines(img, lines, true);
	ComDrawBoxs(img, boxa);
	ComShowImage("src", img);
	ComShowImage("1c", img1c);
	cvWaitKey(0);

	TechOcrExitTessAPI(api);
	pixDestroy(&pix);

// 	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img1c);
//	cvReleaseImage(&imgerode);


	cvReleaseImage(&img);
}

void pocPointToLineDist(void) {
	double aa = ComPointToLineDist(0, 0, 0, 1, 1, 0);
	return;
}

void pocIsIntersect(void) {
	CvPoint a1, a2, b1, b2;
	CvPoint2D32f c1;
	a1 = cvPoint(1, 1);
	a2 = cvPoint(2, 2);
	b1 = cvPoint(0, 5);
	b2 = cvPoint(4, 1);
	bool result = ComIsLineIntersect(&a1, &a2, &b1, &b2, c1);
	return;
}

// http://www.linuxidc.com/Linux/2015-01/111962.htm
// this is what I need
// should read http://blog.csdn.net/qq61394323/article/details/10018967
// http://blog.csdn.net/wangyaninglm/article/details/41863867
void pocNewContour(char *src) {
	IplImage *img, *mimg;

	img = cvLoadImage(src, 1);
	if (!img) {
		return;
	}
	mimg = cvCloneImage(img);

	cvPyrMeanShiftFiltering(img, mimg, 3, 15);
	ComShowImage("src", img);
	ComShowImage("mean", mimg);
	cvWaitKey(0);
}
