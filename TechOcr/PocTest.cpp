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
	pixt = trPixCreateFromIplImage(img);
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
	dst = trRotateImage(img, 90, false);
	cvSaveImage(src, dst);

	trShowImage("dst", dst);
	cvWaitKey();
	cvReleaseImage(&dst);
	cvReleaseImage(&img);
}

void pocFindMaxRect(char *src) {
	IplImage *img, *dst;

	img = cvLoadImage(src, 0);     // 0 for is gray
	dst = trCreateMaxContour(img);
	trShowImage("src", img);
	trShowImage("dst", dst);
	cvWaitKey();
	cvReleaseImage(&dst);
	cvReleaseImage(&img);
}

void pocFindHoughLines(char *src) {
	IplImage *img, *dst, *dst2;
	CvMemStorage* storage;
	CvSeq *lines, *lines2;

	img = cvLoadImage(src, 1);     // 0 for gray, 1 for color
	dst = trCreateMaxContour(img);
	storage = cvCreateMemStorage(0);

	int size = MAX(img->width, img->height);

	lines = cvHoughLines2(dst, storage, CV_HOUGH_PROBABILISTIC,
		size / 1000, CV_PI / 180,
		(double)size / 6, (double)size / 4, (double)size / 100);
 	trDrawLines(img, lines, true);
	dst2 = trCreateLine(dst/* only for size**/, lines);


	trShowImage("src", img);
	trShowImage("dst", dst);
	trShowImage("dst2", dst2);

	cvWaitKey();
	cvReleaseImage(&dst);
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

	pix = trPixCreateFromIplImage(img);
	api = trInitTessAPI();
	api->SetImage(pix);
	boxa = api->GetConnectedComponents(&pixa);
	//boxa = api->GetWords(&pixa);
	boxc = trChoiceBoxInBoxa(boxa, pix);

	box = boxc->box;
	for (i = 0; i < boxc->n; i++) {
		str = trTranslateInRect(*box, api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK);
		OUTPUTBOX(*box, str);
		delete[] str;
		box++;
	}


	trDrawBoxs(img, boxc);
	trShowImage("src", img);
	cvWaitKey();

	boxaDestroy(&boxa);
	boxaDestroy(&boxc);

	pixaDestroy(&pixa);

	trExitTessAPI(api);
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
	trFeatureWordFound *result;
	int count = 0;

	ocrInitBusinessLicense();

	img = cvLoadImage(src, 1);
	if (!img) {
		return;
	}
	pix = trPixCreateFromIplImage(img);
	api = trInitTessAPI();
	api->SetImage(pix);
	boxa = api->GetConnectedComponents(&pixa);
	//boxa = api->GetWords(&pixa);
	boxc = trChoiceBoxInBoxa(boxa, pix);
	found = new OcrFeatureWordsFound;

	found->InitFound(&BusinessLicense);
	box = boxc->box;
	for (i = 0; i < boxc->n; i++) {
		Box enlarge;
		comEnlargeBox(*box, enlarge, 1);
		str = trTranslateInRect(&enlarge, api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK);
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
		trDrawBoxs(img, boxf);
	}



 //	trDrawBoxs(img, boxc, &cvScalar(232,164,74));
	trShowImage("src", img);
	cvWaitKey();

	boxaDestroy(&boxa);
	boxaDestroy(&boxc);
	boxaDestroy(&boxf);
	delete found;			// found should delete after destroy boxf


	pixaDestroy(&pixa);

	trExitTessAPI(api);
	pixDestroy(&pix);
	cvReleaseImage(&img);

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

	img1c = trCloneImg1c(img, true, 160);

	pix = trPixCreateFromIplImage(img1c);
	api = trInitTessAPI();
	api->SetImage(pix);
// 	boxa = api->GetConnectedComponents(&pixa);
	boxa = api->GetWords(&pixa);


// 	trDrawLines(img, lines, true);
	trDrawBoxs(img, boxa);
	trShowImage("src", img);
	trShowImage("1c", img1c);
	cvWaitKey();

	trExitTessAPI(api);
	pixDestroy(&pix);

// 	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img1c);
//	cvReleaseImage(&imgerode);


	cvReleaseImage(&img);
}

void pocPointToLineDist() {
	double aa = comPointToLineDist(0, 0, 0, 1, 1, 0);
	return;
}
void pocImagePreprocess(IplImage *img) {

}