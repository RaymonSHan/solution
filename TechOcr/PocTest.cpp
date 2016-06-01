#include "ImageCommon.h"
#include "TechOcrDll.h"

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

void pocImagePreprocess(IplImage *img) {

}