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

void pocShowImage(char *src) {
	IplImage *img, *img1c;
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
	storage = cvCreateMemStorage(0);
	lines = trCreateHoughLines(img1c, storage);
	// lines will be free by cvReleaseMemStorage(&storage);
	trDrawLines(img, lines, true);

// 	pix = trPixCreateFromIplImage(img);
// 	api = trInitTessAPI();
// 	api->SetImage(pix);
// 	boxa = api->GetWords(&pixa);
// 	trDrawBoxs(img, boxa);

	trShowImage("src", img);
	trShowImage("1c", img1c);
	cvWaitKey();

// 	trExitTessAPI(api);
// 	pixDestroy(&pix);

	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img1c);

	cvReleaseImage(&img);
}

void pocImagePreprocess(IplImage *img) {

}