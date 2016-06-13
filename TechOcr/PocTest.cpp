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
	cvWaitKey();
	cvReleaseImage(&dst);
	cvReleaseImage(&img);
}

void pocFindMaxRect(char *src) {
	IplImage *img, *dst;

	img = cvLoadImage(src, 0);     // 0 for is gray
	dst = TrCreateMaxContour(img);
	ComShowImage("src", img);
	ComShowImage("dst", dst);
	cvWaitKey();
	cvReleaseImage(&dst);
	cvReleaseImage(&img);
}

void pocFindHoughLines(char *src) {
	IplImage *img, *mimg, *dst, *dst2;
	CvMemStorage* storage;
	CvSeq *lines, *lines2;

	img = cvLoadImage(src, 0);     // 0 for gray, 1 for color
	mimg = cvCloneImage(img);

//  	cvCanny(img, mimg, 50, 150, 3);//±ßÔµ¼ì²â  
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

	cvWaitKey();
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
	api = TrInitTessAPI();
	api->SetImage(pix);
	boxa = api->GetConnectedComponents(&pixa);
	//boxa = api->GetWords(&pixa);
	boxc = TrChoiceBoxInBoxa(boxa, pix);

	box = boxc->box;
	for (i = 0; i < boxc->n; i++) {
		str = TrTranslateInRect(*box, api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK);
		OUTPUTBOX(*box, str);
		delete[] str;
		box++;
	}
	ComDrawBoxs(img, boxc);
	ComShowImage("src", img);
	cvWaitKey();

	boxaDestroy(&boxa);
	boxaDestroy(&boxc);

	pixaDestroy(&pixa);

	TrExitTessAPI(api);
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
	api = TrInitTessAPI();
	api->SetImage(pix);
	boxa = api->GetConnectedComponents(&pixa);
	//boxa = api->GetWords(&pixa);
	boxc = TrChoiceBoxInBoxa(boxa, pix);
	found = new OcrFeatureWordsFound;

	found->InitFound(&BusinessLicense);
	box = boxc->box;
	for (i = 0; i < boxc->n; i++) {
		Box enlarge;
		ComEnlargeBox(*box, enlarge, 1);
		str = TrTranslateInRect(&enlarge, api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK);
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
	cvWaitKey();

	boxaDestroy(&boxa);
	boxaDestroy(&boxc);
	boxaDestroy(&boxf);
	pixaDestroy(&pixa);
	delete found;			// found should delete after destroy boxf

	TrExitTessAPI(api);
	pixDestroy(&pix);
	cvReleaseImage(&img);

}

void pocApproximateLine(char *filename) {
	IplImage *srcimg, *thresimg, *contimg, *cloneimg;
	CvMemStorage *storage;
	CvSeq *lines, *linesappr;
	CvSeq *conners;

	storage = cvCreateMemStorage(0);
	srcimg = cvLoadImage(filename, 1);
	thresimg = TrImageThreshold(srcimg);
	contimg = TrContourDraw(thresimg);
	lines = TrCreateHoughLines(contimg, storage);
	linesappr = TrCreateApproximateLines(lines, storage);

	if (linesappr->total != 4) {
		// match four lines
	}
	conners = TrGetIntersection(linesappr, storage, &cvPoint2D32f(thresimg->width/2, thresimg->height/2));
	cloneimg = cvCloneImage(srcimg);
	ComDrawLines(cloneimg, lines, true);
	ComShowImage("img", cloneimg);
	ComShowImage("cont", contimg);
	cvWaitKey();
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&cloneimg);
	cvReleaseImage(&contimg);
	cvReleaseImage(&thresimg);
	cvReleaseImage(&srcimg);
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
	api = TrInitTessAPI();
	api->SetImage(pix);
// 	boxa = api->GetConnectedComponents(&pixa);
	boxa = api->GetWords(&pixa);


// 	trDrawLines(img, lines, true);
	ComDrawBoxs(img, boxa);
	ComShowImage("src", img);
	ComShowImage("1c", img1c);
	cvWaitKey();

	TrExitTessAPI(api);
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
	cvWaitKey();
}
