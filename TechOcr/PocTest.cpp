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

void pocRotateImage(char* src, int angle, bool clockwise) {
	IplImage *img, *dst;
	img = cvLoadImage(src, 1);
	if (!img) {
		return;
	}
	dst = trRotateImage(img, 90, false);
	cvSaveImage(src, dst);

	trShowImage("dst", dst);
	cvWaitKey();
}

void pocShowImage(char *src) {
	IplImage *img, *img1c;//, *imgerode;
	CvMemStorage* storage;
	CvSeq *lines;

	Pix *pix;
	tesseract::TessBaseAPI *api;
	Pixa *pixa;
	Boxa *boxa;

	img = cvLoadImage(src, 1);
	if (!img) {
		return;
	}

	img1c = trCloneImg1c(img);
//	imgerode = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
// 	storage = cvCreateMemStorage(0);
// 	lines = trCreateHoughLines(img1c, storage);
	// lines will be free by cvReleaseMemStorage(&storage);


//	cvErode(img1c, img1c, NULL, 8);		// enlarge
	cvDilate(img1c, img1c, NULL, 8);

	pix = trPixCreateFromIplImage(img1c);
	api = trInitTessAPI();
	api->SetImage(pix);
	boxa = api->GetConnectedComponents(&pixa);

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

void pocImagePreprocess(IplImage *img) {

}