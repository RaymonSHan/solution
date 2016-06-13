#include "PocTest.h"

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

	CvSize	resize;
	int lmax = MAX(img->width, img->height);
	double scale = lmax / 600;
	resize.width = (int)(img->width / scale);
	resize.height = (int)(img->height / scale);

	IplImage *dst;
	dst = cvCreateImage(resize, img->depth, img->nChannels);
	cvResize(img, dst, CV_INTER_LINEAR);

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

	img1c = trCloneImg1c(dst, true, 128);

	pix = trPixCreateFromIplImage(img1c);
	api = trInitTessAPI();
	api->SetImage(pix);
// 	boxa = api->GetConnectedComponents(&pixa);
	boxa = api->GetWords(&pixa);


// 	trDrawLines(img, lines, true);
	trDrawBoxs(dst, boxa);
	trShowImage("src", dst);
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