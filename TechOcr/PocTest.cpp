#include "PocTest.h"
#include "ImageProcess.h"


void pocPixCreateFromIplImage(char *src, char *dst) {
	IplImage *img = cvLoadImage(src, 1);     // 1 for is colored
	if (!img) {
		return;
	}
//	cvSaveImage(dst, img);
//	Pix* pixs = pixRead("src");
//	read as Pix, for confirm the data struct

	Pix* pixt;
	pixt = TrCreatePixFromIplImage(img);
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
// 	dst = TrCreateMaxContour(img);
	ComShowImage("src", img);
// 	ComShowImage("dst", dst);
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

// 	dst = TrCreateMaxContour(mimg);
	storage = cvCreateMemStorage(0);

	int size = MAX(img->width, img->height);

// 	lines = cvHoughLines2(dst, storage, CV_HOUGH_PROBABILISTIC,
// 		size / 1000, CV_PI / 180,
// 		(double)size / 8, (double)size / 6, (double)size / 40);
//  	ComDrawLines(img, lines, true);
// 	dst2 = TrCreateLine(dst/* only for size**/, lines);


	ComShowImage("src", img);
	ComShowImage("mimg", mimg);
// 	ComShowImage("dst", dst);
// 	ComShowImage("dst2", dst2);

	cvWaitKey(0);
// 	cvReleaseImage(&dst);
// 	cvReleaseImage(&dst2);
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
	pix = TrCreatePixFromIplImage(img);
	api = TechOcrInitTessAPI();
	api->SetImage(pix);
// 	boxa = api->GetConnectedComponents(&pixa);
	//boxa = api->GetWords(&pixa);
	boxc = TrChoiceBoxInBoxa(api, pix);

	box = boxc->box;
	for (i = 0; i < boxc->n; i++) {
		str = TrTranslateInRect(api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK, *box);
// 		OUTPUTBOX(*box, str);
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
// void pocFindFeatureWordsInClass(char *src) {
// 	IplImage *img;
// 	Pix *pix;
// 	tesseract::TessBaseAPI *api;
// 	Pixa *pixa;
// 	Boxa *boxa, *boxc, *boxf;
// 	Box **box;
// 	int i;
// 	char *str;
// 	OcrFeatureWordsFound *found;
// 	TrFeatureWordFound *result;
// 	int count = 0;
// 
// 	ocrInitBusinessLicense();
// 
// 	img = cvLoadImage(src, 1);
// 	if (!img) {
// 		return;
// 	}
// 	pix = TrPixCreateFromIplImage(img);
// 	api = TechOcrInitTessAPI();
// 	api->SetImage(pix);
// // 	boxa = api->GetConnectedComponents(&pixa);
// 	//boxa = api->GetWords(&pixa);
// 	//boxc = TrChoiceBoxInBoxa(api, pix);
// 	found = new OcrFeatureWordsFound;
// 
// 	found->InitFound(&BusinessLicense);
// 	box = boxc->box;
// 	for (i = 0; i < boxc->n; i++) {
// 		Box enlarge;
// 		ComEnlargeBox(*box, enlarge, 1, 1);
// 		str = TrTranslateInRect(api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK, &enlarge);
// // 		str = trTranslateInRect(*box, api, tesseract::PSM_SINGLE_CHAR, ENCODE_GBK);
// 		result = found->AddFound(*box, str);
// 		if (result) {
// 			count++;
// 		}
//  		OUTPUTBOX(*box, str);
// 		delete[] str;
// 		box++;
// 	}
// 	int start = 0, match;
// 	while (start < found->GetNumber())
// 	{
// 		boxf = found->ReturnFound(start, match);
// 		if (match < MIN_MATCH) {
// 			boxaDestroy(&boxf);
// 			start++;
// 		}
// 		else {
// 			break;
// 		}
// 	}
// 	if (start < found->GetNumber()) {
// 		// have found feature
// 		ComDrawBoxs(img, boxf);
// 	}
// 	CvPoint2D32f srcTri[4], desTri[4];
// 	CvMat *warpMat;
// 
// 	found->ReturnCorner(srcTri, desTri);
// 	for (i = 0; i < 4; i++)
// 		cvCircle(img, cvPointFrom32f(desTri[i]), 20, CV_RGB(190,11,55));
// 	warpMat = cvCreateMat(3, 3, CV_64F);
// 	cvGetPerspectiveTransform(srcTri, desTri, warpMat);
// 	cvReleaseMat(&warpMat);
// 
//  //	trDrawBoxs(img, boxc, &cvScalar(232,164,74));
// 	ComShowImage("src", img);
// 	cvWaitKey(0);
// 
// 	boxaDestroy(&boxa);
// 	boxaDestroy(&boxc);
// 	boxaDestroy(&boxf);
// 	pixaDestroy(&pixa);
// 	delete found;			// found should delete after destroy boxf
// 
// 	TechOcrExitTessAPI(api);
// 	pixDestroy(&pix);
// 	cvReleaseImage(&img);
// }

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
	linesappr = TrAggregationLines(lines, storage);					// poc only
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
void pcoPreprocess(char *filename) {
	RESULT result;
	CvPoint2D32f corner[4];
	CvPoint2D32f *pcorner = corner;
	IplImage *src, *dst;
	Pix *pix;
	tesseract::TessBaseAPI *api;
	CvSeq *feature, *content, *bestformat = NULL;
// 	CvPoint2D32f csrc[4], cdst[4];
// 	int mostfea, mostfor;
	int match, rotateloop = 0;
	CvMemStorage *storage;
	CvMat *warp1, *warp2;
	int i;

	src = cvLoadImage(filename, 1);
	if (!src)
		return;
	storage = cvCreateMemStorage(0);

	warp1 = cvCreateMat(3, 3, CV_32FC1);
	warp2 = cvCreateMat(3, 3, CV_32FC1);
	dst = cvCloneImage(src);
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

		feature = cvCreateSeq(0, sizeof(CvSeq), sizeof(CharFound), storage);
		result = TechOcrGetFeatureChar(pix, api, feature);
		TechOcrFormatMostMatch(feature, bestformat, match, warp2);

		if ((bestformat && match >= MIN_CONFIRM) || rotateloop >= 4) {
			// record now bestformat
			break;
		}
		cvReleaseImage(&dst);
		pixDestroy(&pix);
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
	ComReleaseCharFound(feature);

	CharFound* found, *featurematch;
	char * ch, *gb;
	long gsize;
	content = cvCreateSeq(0, sizeof(CvSeq), sizeof(CharFound), storage);
	TechOcrDetectWordsInFormat(dst, warp1, warp2, bestformat, content);

// 	for (i = 0; i < content->total; i++) {
// 		found = (CharFound*)cvGetSeqElem(content, i);
// 		ch = (char*)found->desc;
// 		ComUtf8ToGbk(ch, strlen(ch), gb, gsize);
// 		std::cout << found->rect.x << ", " << found->rect.y << ", CHAR " << gb;
// 		delete[] gb;
// // 		delete[] ch;
// 		featurematch = (CharFound*)cvGetSeqElem(bestformat, found->found);
// 		ch = (char*)featurematch->desc;
// 		ComUtf8ToGbk(ch, strlen(ch), gb, gsize);
// 		std::cout << ", " << gb << std::endl;
// 		delete[] gb;
// // 		delete[] ch;
// 
// 	}
	std::cout << std::endl << std::endl;
	ComReleaseCharFound(content);




	cvWaitKey(0);


	cvReleaseMat(&warp1);
	cvReleaseMat(&warp2);
	cvReleaseMemStorage(&storage);

	cvReleaseImage(&dst);
	cvReleaseImage(&src);
}

void pocOnceMemoryLeak(char *filename) {
	RESULT result;
	CvPoint2D32f corner[4];
	IplImage *src, *dst, *doingimg;
	Pix *pix;
	tesseract::TessBaseAPI *api;
	// 	CvSeq *feature, *content, *bestformat = NULL;
	CvSeq *lines, *linesappr, *conners;
	CvMemStorage *storage;
	CvMat *warp1, *warp2;
	bool havelarge = false;
	int i;
	int minsize;

	src = cvLoadImage(filename, 1);
	if (!src)
		return;

// // 	minsize = MIN(src->width, src->height);
// // 	storage = cvCreateMemStorage(0);
// // 
// // // 	dst = cvCloneImage(src);
// // // 	dst = ComRotateImage(src, 90, true);
// // 	dst = TrImageThreshold(src, 5, 5);
// // 	doingimg = TrContourDraw(dst, havelarge);
// // 	lines = TrCreateHoughLines(doingimg, storage);
// // 	linesappr = TrAggregationLines(lines, storage, minsize / 30);
// // 	linesappr = TrChoiceLinesInFitLines(linesappr, storage);
// // 	conners = TrGetFourCorner(linesappr, storage, &cvPoint2D32f(src->width / 2, src->height / 2));
// // 
// // 
// // // 	api = TechOcrInitTessAPI();
// // // 	pix = TrPixCreateFromIplImage(dst);
// // // 	api->SetImage(pix);
// // // 
// // // 
// // // 	pixDestroy(&pix);
// // // 	TechOcrExitTessAPI(api);
// // 
// // 
// // 	cvReleaseMemStorage(&storage);
// // 	cvReleaseImage(&doingimg);
// // 	cvReleaseImage(&dst);
// 
// 	TechOcrGetFourCorner(src, corner, 1);			// may have memory leak 4k for 50 times




// 	// 	// 	api = TechOcrInitTessAPI();
// 	// 	// 	TechOcrExitTessAPI(api);
// result for Z:\\solution\\files\\cl.jpg
	corner[0].x = 59.41;
	corner[0].y = 127.16;
	corner[1].x = 3063.12;
	corner[1].y = 132.59;
	corner[2].x = 3164.66;
	corner[2].y = 3970.71;
	corner[3].x = 48.77;
	corner[3].y = 3969.04;

	CvSeq *feature;
	api = TechOcrInitTessAPI();
	for (int i = 0; i < 1; i++) {
		storage = cvCreateMemStorage(0);
	warp1 = cvCreateMat(3, 3, CV_32FC1);
	warp2 = cvCreateMat(3, 3, CV_32FC1);
// 		TechOcrCreatePix(src, src->width, src->height, corner, pix, warp1);

		feature = cvCreateSeq(0, sizeof(CvSeq), sizeof(CharFound), storage);
// 		result = TechOcrGetFeatureChar(pix, api, feature);

		ComReleaseCharFound(feature);
// 		pixDestroy(&pix);
	cvReleaseMat(&warp1);
	cvReleaseMat(&warp2);
		cvReleaseMemStorage(&storage);
		api->Clear();
	}

	TechOcrExitTessAPI(api);

// result for Z:\\solution\\files\\bl.jpg
// 	corner[0].x = 39.30;
// 	corner[0].y = 216.07;
// 	corner[1].x = 3049.36;
// 	corner[1].y = 218.59;
// 	corner[2].x = 3040.09;
// 	corner[2].y = 4092.81;
// 	corner[3].x = 59.38;
// 	corner[3].y = 4082.74;

	cvReleaseImage(&src);

	i = 1;
	return;
}

void pocMemoryLeak(char *filename) {
	int i;
	for (i = 0; i < 2; i++) {
		pocOnceMemoryLeak(filename);
	}
	for (i = 0; i < 2; i++) {
		pocOnceMemoryLeak(filename);
	}

	for (i = 0; i < 2; i++) {
		pocOnceMemoryLeak(filename);
	}

	for (i = 0; i < 2; i++) {
		pocOnceMemoryLeak(filename);
	}

	for (i = 0; i < 2; i++) {
		pocOnceMemoryLeak(filename);
	}
	for (i = 0; i < 3; i++) {
		pocOnceMemoryLeak(filename);
	}
	for (i = 0; i < 4; i++) {
		pocOnceMemoryLeak(filename);
	}
	pocOnceMemoryLeak(filename);

	pocOnceMemoryLeak(filename);

	pocOnceMemoryLeak(filename);

	pocOnceMemoryLeak(filename);

	int j = 0;
}

void pocCreateBusinessLicense(void) {
	CvSeq *format;
	static bool havedone = false;
	if (havedone)
		return;
	havedone = true;
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

// 	TechOcrFormatAddFeature(format, 1562, 3436, 71, 67, "登");
// 	TechOcrFormatAddFeature(format, 1680, 3435, 71, 69, "记");
// 	TechOcrFormatAddFeature(format, 1798, 3435, 74, 70, "机");
// 	TechOcrFormatAddFeature(format, 1920, 3433, 67, 72, "关");

	TechOcrFormatAddContent(format, 1785, 1469, 800, 55, "注册号");
	TechOcrFormatAddContent(format, 1095, 1636, 1700, 66, "名称");
	TechOcrFormatAddContent(format, 1095, 1758, 1700, 66, "类型");
	TechOcrFormatAddContent(format, 1095, 1822, 1700, 166, "住所");
	TechOcrFormatAddContent(format, 1095, 1984, 1700, 66, "法定代表人");
	TechOcrFormatAddContent(format, 1095, 2095, 1700, 66, "注册资本");
	TechOcrFormatAddContent(format, 1095, 2212, 1700, 66, "成立日期");
	TechOcrFormatAddContent(format, 1095, 2330, 1700, 66, "营业期限");
	TechOcrFormatAddContent(format, 1095, 2440, 1700, 660, "经营范围", CHARTYPE_CONTENT_BLOCK);

// 	TechOcrFormatAddContent(format, 1785, 1439, 800, 115, "注册号");
// 	TechOcrFormatAddContent(format, 1095, 1586, 1700, 166, "名称");
// 	TechOcrFormatAddContent(format, 1095, 1708, 1700, 166, "类型");
// 	TechOcrFormatAddContent(format, 1095, 1772, 1700, 166, "住所");
// 	TechOcrFormatAddContent(format, 1095, 1934, 1700, 166, "法定代表人");
// 	TechOcrFormatAddContent(format, 1095, 2045, 1700, 166, "注册资本");
// 	TechOcrFormatAddContent(format, 1095, 2162, 1700, 166, "成立日期");
// 	TechOcrFormatAddContent(format, 1095, 2280, 1700, 166, "营业期限");
// 	TechOcrFormatAddContent(format, 1095, 2390, 1700, 750, "经营范围", CHARTYPE_CONTENT_BLOCK);

	return;
}

void pocCreatePersonCard(void) {
	CvSeq *format;
	static bool havedone = false;
	if (havedone)
		return;
	havedone = true;
	TechOcrCreateFormat(format, "居民身份证", 4160, 3120);

	TechOcrFormatAddFeature(format, 559, 591, 108, 104, "姓");
	TechOcrFormatAddFeature(format, 753, 587, 97, 113, "名");
	TechOcrFormatAddFeature(format, 552, 873, 111, 112, "性");
	TechOcrFormatAddFeature(format, 743, 877, 114, 113, "别");
	TechOcrFormatAddFeature(format, 995, 873, 116, 129, "男");
	TechOcrFormatAddFeature(format, 546, 1468, 110, 110, "住");
	TechOcrFormatAddFeature(format, 738, 1471, 110, 108, "址");

	TechOcrFormatAddContent(format, 997, 542, 1400, 166, "姓名");
	TechOcrFormatAddContent(format, 995, 873, 300, 129, "性别");
	TechOcrFormatAddContent(format, 1736, 873, 600, 129, "民族");
	TechOcrFormatAddContent(format, 985, 1465, 1523, 536, "住址", CHARTYPE_CONTENT_BLOCK);
	TechOcrFormatAddContent(format, 1557, 2215, 2050, 137, "公民身份号码");

}


void pocTesseractInit(void) {
	char *utfstr, *gbstr;
	long gsize;
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI;
	int rc = api->Init(NULL, DEFAULT_LANGURE, tesseract::OEM_DEFAULT);
	Pix *imagec = pixRead("z:\\solution\\files\\cl.jpg");
	Pix *imageb = pixRead("z:\\solution\\files\\bs.jpg");
	Pixa *pixc, *pixb;
	Boxa *boxc, *boxb;

	api->SetImage(imagec);
	api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);			// the IMPORTANT is THIS
	boxc = api->GetWords(&pixc);
	api->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	api->SetRectangle(610, 2210, 430, 70);			// for cl.jpg
	api->Recognize(NULL);
	utfstr = api->GetUTF8Text();
	ComUtf8ToGbk(utfstr, (long)strlen(utfstr), gbstr, gsize);
	std::cout << gbstr;
	delete[] gbstr;
	delete[] utfstr;
	api->Clear();

	api->SetImage(imageb);
	api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);			// the IMPORTANT is THIS
	// 	api->SetRectangle(0, 0, imageb->w, imageb->h);			// for bs.jpg
	boxb = api->GetWords(&pixb);
	api->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	api->SetRectangle(250, 315, 555, 120);			// for bs.jpg
	api->Recognize(NULL);
	utfstr = api->GetUTF8Text();
	ComUtf8ToGbk(utfstr, (long)strlen(utfstr), gbstr, gsize);
	std::cout << gbstr;
	delete[] gbstr;
	delete[] utfstr;


	api->Clear();
	api->End();
	api->ClearPersistentCache();
	delete api;

}

void pcoImagePreprocess(char *filename) {
	IplImage *src, *dst;
	int i;

	src = cvLoadImage(filename, 1);
	dst = TrImageThreshold(src, 7, 7, 180);
	ComShowImage("dst", dst);
	cvSaveImage("z:\\solution\\files\\pb1.jpg", dst);
	cvWaitKey(0);

}
void pocMemoryDebug(void) {

}


// #define LOADFILE "z:\\solution\\files\\cl.jpg"
// #define LOADFILE "z:\\solution\\files\\p1.jpg"
// #define LOADFILE "z:\\solution\\files\\p\\p19.jpg"
// #define LOADFILE "z:\\solution\\files\\pn1.jpg"
#define LOADFILE "z:\\solution\\files\\p\\p08.jpg"

// #define LOADFILE "z:\\solution\\files\\cc\\c1.jpg"

void cvEqualizeHistColor(IplImage *src, IplImage *dst) {

	//分别均衡化每个信道  
	IplImage* redImage = cvCreateImage(cvGetSize(src), src->depth, 1);
	IplImage* greenImage = cvCreateImage(cvGetSize(src), src->depth, 1);
	IplImage* blueImage = cvCreateImage(cvGetSize(src), src->depth, 1);
	cvSplit(src, blueImage, greenImage, redImage, NULL);

	cvEqualizeHist(redImage, redImage);
	cvEqualizeHist(greenImage, greenImage);
	cvEqualizeHist(blueImage, blueImage);
	//均衡化后的图像  
	cvMerge(blueImage, greenImage, redImage, NULL, dst);
	cvReleaseImage(&redImage);
	cvReleaseImage(&greenImage);
	cvReleaseImage(&blueImage);

}



int ColorCount[COLOR_RANGE * COLOR_RANGE * COLOR_RANGE];


IplImage* pocImageProcess(char *filename) {
// 	if (false) {
// 		IplImage *srcmin = cvLoadImage(LOADFILE);
// 		TrIplImageChannelChoiceMin(srcmin);
// 		ComShowImage("min", srcmin);
// 		IplImage *srcmax = cvLoadImage(LOADFILE);
// 		TrIplImageChannelChoiceMin(srcmax);
// 		ComShowImage("max", srcmax);
// 		cvWaitKey(0);
// 	}
// 
// 	if (false) {
// 		IplImage *srcr = cvLoadImage(LOADFILE);
// 		IplImage *srcg = cvLoadImage(LOADFILE);
// 		IplImage *srcb = cvLoadImage(LOADFILE);
// 		TrIplImageChannelChoice(srcr, true, false, false);
// 		TrIplImageChannelChoice(srcg, false, true, false);
// 		TrIplImageChannelChoice(srcb, false, false, true);
// 		ComShowImage("maxr", srcr);
// 		ComShowImage("maxg", srcg);
// 		ComShowImage("maxb", srcb);
// 		cvWaitKey(0);
// 	}

#ifdef _DEBUG
	filename = LOADFILE;
#endif _DEBUG
	if (false) {					// for get same color
		IplImage *src = cvLoadImage(filename);
// 		cvSetImageROI(src, cvRect(319, 155, 36, 32));			// for p07
		cvSetImageROI(src, cvRect(779, 1072, 118, 104));			// for pn1

//  		cvSetImageROI(src, cvRect(300, 300, 100, 100));

		TrIplImageColorCount(src, ColorCount, 3);
// 		cvResetImageROI(src);

		ComShowImage("src", src);
		cvWaitKey(0);
		exit(0);

	}
	if (false) {
		IplImage *src = cvLoadImage(filename);
		TrIplImageRgbToYuv(src);


		IplImage *srcy = cvCreateImage(cvGetSize(src), src->depth, 1);
		IplImage *srcu = cvCreateImage(cvGetSize(src), src->depth, 1);
		IplImage *srcv = cvCreateImage(cvGetSize(src), src->depth, 1);

		cvSplit(src, srcv, srcu, srcy, NULL);

		ComShowImage("srcy", srcy);
		ComShowImage("srcu", srcu);
		ComShowImage("srcv", srcv);
		cvWaitKey(0);
		exit(0);
	}
	if (false) {
		IplImage *src = cvLoadImage(filename);
		ComShowImage("src", src);

		IplImage *dst = cvCreateImage(cvGetSize(src), 8, 3);
		IplImage *src1c = cvCreateImage(cvGetSize(src), 8, 1);

		cvSmooth(src, dst, CV_BLUR, src->width/10, src->height/10, 0, 0);
		TrIplImageRgbToYuv(src);
		TrIplImageRgbToYuv(dst);
		TrIplImageDiv(dst, src);
		TrIplImageYuvToRgb(src);

		cvCvtColor(src, src1c, CV_BGR2GRAY);
		cvEqualizeHist(src1c, src1c);

// 		ComShowImage("src1", dst);
		ComShowImage("src1c", src1c);
// 		TrIplImageRgbToHsl(src);
// 		TrIplImageChannelChoice(src, false, false, true, 0, 0, 128);
// 		TrIplImageHslToRgb(src);

// 		TrIplImageRgbToYuv(src);
// 		TrIplImageDeltaColor(src, 79, 123, 148);
// 		TrIplImageDeltaColor(src, 78, 173, 191);

// 		dst = TrImageThreshold(src);

// 		ComShowImage("dst", src);
// 		ComShowImage("dst1", dst);
		cvWaitKey(0);
	}
	if (false) {
		IplImage *src = cvLoadImage(filename);
		ComShowImage("src", src);

		TrIplImageRateOneColor(src, 14, 95, 122, 40);
// 		TrIplImageToWhite(src, 100);

		ComShowImage("dst", src);
		cvWaitKey(0);
	}
	if (false) {			// OK for white rect around
		IplImage *src = cvLoadImage(filename);

		ComShowImage("src", src);


		IplImage *src1 = cvCloneImage(src);
		IplImage *src2 = cvCloneImage(src);

		IplImage *dst1 = cvCreateImage(cvGetSize(src), 8, 3);
		IplImage *dst2 = cvCreateImage(cvGetSize(src), 8, 3);

		IplImage *src1c1 = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage *src1c2 = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage *src1c3 = cvCreateImage(cvGetSize(src), 8, 1);

		cvSmooth(src, dst1, CV_BLUR, src->width / 40, src->height / 40, 0, 0);
		cvSmooth(src, dst2, CV_BLUR, src->width / 130, src->height / 130, 0, 0);

		TrIplImageRgbToYuv(src1);
		TrIplImageRgbToYuv(src2);

		TrIplImageRgbToYuv(dst1);
		TrIplImageRgbToYuv(dst2);

		TrIplImageDiv(dst1, src1);
		TrIplImageDiv(dst2, src2);

		TrIplImageYuvToRgb(src1);
		TrIplImageYuvToRgb(src2);

// 		IplImage *src3 = cvCloneImage(src2);

// 		TrIplImageOr(src1, src3);

		cvCvtColor(src1, src1c1, CV_BGR2GRAY);
		cvCvtColor(src2, src1c2, CV_BGR2GRAY);
// 		cvCvtColor(src3, src1c3, CV_BGR2GRAY);
		cvEqualizeHist(src1c1, src1c1);
		cvEqualizeHist(src1c2, src1c2);
// 		cvEqualizeHist(src1c3, src1c3);

		ComShowImage("src1c1", src1c1);
		ComShowImage("src1c2", src1c2);


		// 		ComShowImage("src1", dst);
// 		ComShowImage("src1c3", src1c3);
		// 		TrIplImageRgbToHsl(src);
		// 		TrIplImageChannelChoice(src, false, false, true, 0, 0, 128);
		// 		TrIplImageHslToRgb(src);

		// 		TrIplImageRgbToYuv(src);
		// 		TrIplImageDeltaColor(src, 79, 123, 148);
		// 		TrIplImageDeltaColor(src, 78, 173, 191);

		// 		dst = TrImageThreshold(src);

		// 		ComShowImage("dst", src);
		// 		ComShowImage("dst1", dst);
		cvWaitKey(0);

	}
	if (false) {		// make blank words
		IplImage *src = cvLoadImage(filename);

		ComShowImage("src", src);

		IplImage *src2 = cvCloneImage(src);

		IplImage *dst2 = cvCreateImage(cvGetSize(src), 8, 3);

		IplImage *src1c2 = cvCreateImage(cvGetSize(src), 8, 1);

		cvSmooth(src, dst2, CV_BLUR, src->width / 130, src->height / 130, 0, 0);

		TrIplImageRgbToYuv(src2);

		TrIplImageRgbToYuv(dst2);

		TrIplImageDiv(dst2, src2);
		// 		TrIplImageOneColor(src2, 55, 0, 0, 55);
		// 		TrIplImageNot(src2, true, false, false);

		TrIplImageYuvToRgb(src2);
// 		ComShowImage("src2", src2);
// 		TrIplImageOneColor(src2, 200, 200, 200, 55);

		// 		IplImage *src3 = cvCloneImage(src2);

		// 		TrIplImageOr(src1, src3);

		cvCvtColor(src2, src1c2, CV_BGR2GRAY);
		// 		cvCvtColor(src3, src1c3, CV_BGR2GRAY);
		cvEqualizeHist(src1c2, src1c2);
		// 		cvEqualizeHist(src1c3, src1c3);
		ComShowImage("src1c1", src1c2);
		TrIplImageOneColor(src1c2, 15, 15);
		int gassize = (MIN(src->height, src->width) / 250) * 2 + 5;
		cvSmooth(src1c2, src1c2, CV_GAUSSIAN, gassize, gassize, 0, 0);
		ComShowImage("src1c2", src1c2);
		TrIplImageOneColor(src1c2, 15, 15);

		ComShowImage("src1c3", src1c2);


		// 		ComShowImage("src1", dst);
		// 		ComShowImage("src1c3", src1c3);
		// 		TrIplImageRgbToHsl(src);
		// 		TrIplImageChannelChoice(src, false, false, true, 0, 0, 128);
		// 		TrIplImageHslToRgb(src);

		// 		TrIplImageRgbToYuv(src);
		// 		TrIplImageDeltaColor(src, 79, 123, 148);
		// 		TrIplImageDeltaColor(src, 78, 173, 191);

		// 		dst = TrImageThreshold(src);

		// 		ComShowImage("dst", src);
		// 		ComShowImage("dst1", dst);
		cvWaitKey(0);

	}
	if (false) {				// sub is better than div
		IplImage *src = cvLoadImage(filename);

// 		cvEqualizeHistColor(src, src);
		ComShowImage("src", src);

// 		IplImage *src1 = cvCloneImage(src);
// 		IplImage *dst = cvCreateImage(cvGetSize(src), 8, 3);
// 		IplImage *src1c1 = cvCreateImage(cvGetSize(src), 8, 1);
// 		IplImage *src1c2 = cvCreateImage(cvGetSize(src), 8, 1);
// 		IplImage *src1c3 = cvCreateImage(cvGetSize(src), 8, 1);
// 
// 		for (int i = 0; i < 10; i++) {
// 			cvSmooth(src1, dst, CV_BLUR, src->width / 130, src->height / 130, 0, 0);
// 			TrIplImageRgbToYuv(src1);
// 			TrIplImageRgbToYuv(dst);
// 			TrIplImageSub(dst, src1);
// 			TrIplImageYuvToRgb(src1);
// 			cvCvtColor(src1, src1c1, CV_BGR2GRAY);
// 			cvEqualizeHist(src1c1, src1c1);
// 
// 			ComShowImage("src1", src1);
// 			ComShowImage("src1c1", src1c1);
// 			cvMerge(src1c1, NULL, NULL, NULL, src1);
// 
// 			cvWaitKey(0);
// 
// 		}

		IplImage *src2 = cvCloneImage(src);

		IplImage *dst2 = cvCreateImage(cvGetSize(src), 8, 3);

		IplImage *src1c2 = cvCreateImage(cvGetSize(src), 8, 1);

		cvSmooth(src, dst2, CV_BLUR, src->width / 130, src->height / 130, 0, 0);

		TrIplImageRgbToYuv(src2);

		TrIplImageRgbToYuv(dst2);

		TrIplImageSub(dst2, src2);
		// 		TrIplImageOneColor(src2, 55, 0, 0, 55);
		// 		TrIplImageNot(src2, true, false, false);

		TrIplImageYuvToRgb(src2);
		// 		ComShowImage("src2", src2);
		// 		TrIplImageOneColor(src2, 200, 200, 200, 55);

		// 		IplImage *src3 = cvCloneImage(src2);

		// 		TrIplImageOr(src1, src3);

		cvCvtColor(src2, src1c2, CV_BGR2GRAY);
		// 		cvCvtColor(src3, src1c3, CV_BGR2GRAY);
		cvEqualizeHist(src1c2, src1c2);
		// 		cvEqualizeHist(src1c3, src1c3);
		ComShowImage("src1c1", src1c2);
		TrIplImageOneColor(src1c2, 225, 31);

// 		cvAdaptiveThreshold(src1c2, src1c2, 130, CV_ADAPTIVE_THRESH_MEAN_C,
// 			CV_THRESH_BINARY_INV, 65, 20);

		ComShowImage("src1c2", src1c2);
// 		int gassize = (MIN(src->height, src->width) / 250) * 2 + 5;
// 		cvSmooth(src1c2, src1c2, CV_GAUSSIAN, gassize, gassize, 0, 0);
		TrIplImageOneColor(src1c2, 15, 15);

		ComShowImage("src1c3", src1c2);


		// 		ComShowImage("src1", dst);
		// 		ComShowImage("src1c3", src1c3);
		// 		TrIplImageRgbToHsl(src);
		// 		TrIplImageChannelChoice(src, false, false, true, 0, 0, 128);
		// 		TrIplImageHslToRgb(src);

		// 		TrIplImageRgbToYuv(src);
		// 		TrIplImageDeltaColor(src, 79, 123, 148);
		// 		TrIplImageDeltaColor(src, 78, 173, 191);

		// 		dst = TrImageThreshold(src);

		// 		ComShowImage("dst", src);
		// 		ComShowImage("dst1", dst);
		cvWaitKey(0);

	}
	if (true) {			// for cmp function, 

		IplImage *src = cvLoadImage(filename);


		IplImage *src1 = cvCloneImage(src);
		IplImage *dst1 = cvCreateImage(cvGetSize(src), 8, 3);				// for 1/10 gassize
		IplImage *dst2 = cvCreateImage(cvGetSize(src), 8, 3);				// for 1/60 gassize
		IplImage *src1c1 = cvCreateImage(cvGetSize(src), 8, 1);
		int gassize = (MIN(src->height, src->width) / 20) * 2 + 3;
		cvSmooth(src, dst1, CV_BLUR, gassize, gassize, 0, 0);
		TrIplImageRgbToYuv(dst1);
		TrIplImageRgbToYuv(src1);
		TrIplImageBeOne(dst1, src1);
		TrIplImageYuvToRgb(src1);
		ComShowImage("src1", src1);

		gassize = (MIN(src->height, src->width) / 120) * 2 + 3;
		cvSmooth(src1, dst2, CV_GAUSSIAN, gassize, gassize, 0, 0);
		TrIplImageRgbToYuv(src1);
		TrIplImageRgbToYuv(dst2);
		TrIplImageCmp(dst2, src1);
		TrIplImageYuvToRgb(src1);

		// 		TrIplImageOneColor(src2, 55, 0, 0, 55);
		// 		TrIplImageNot(src2, true, false, false);


		cvCvtColor(src1, src1c1, CV_BGR2GRAY);
		gassize = (MIN(src->height, src->width) / 400) * 2 + 3;
		cvSmooth(src1c1, src1c1, CV_GAUSSIAN, gassize, gassize, 0, 0);

// 		cvEqualizeHist(src1c2, src1c2);
		ComShowImage("src1c1", src1c1);
		TrIplImageOneColor(src1c1, 200, 56);


		ComShowImage("src1c2", src1c1);
// 		int gassize = (MIN(src->height, src->width) / 250) * 2 + 5;
// 		cvSmooth(src1c2, src1c2, CV_GAUSSIAN, gassize, gassize, 0, 0);
		TrIplImageOneColor(src1c1, 15, 15);

		ComShowImage("src1c3", src1c1);

		cvWaitKey(0);


	}
	if (false) {
		IplImage *src = cvLoadImage(filename);
		int gassize = (MIN(src->height, src->width) / 400) * 2 + 3;
		cvSmooth(src, src, CV_GAUSSIAN, gassize, gassize, 0, 0);

// 		TrIplImageChannelChoiceMin(src);
		IplImage *src1 = cvCloneImage(src);
		IplImage *src2 = cvCloneImage(src);
		IplImage *dst1 = cvCreateImage(cvGetSize(src), 8, 3);
		IplImage *src1c1 = cvCreateImage(cvGetSize(src), 8, 1);
		
		cvSmooth(src, dst1, CV_BLUR, src->width / 145, src->height / 145, 0, 0);
		TrIplImageRgbToYuv(src1);
		TrIplImageRgbToYuv(src2);
		TrIplImageRgbToYuv(dst1);
		TrIplImageDiv(dst1, src1);


// 		TrIplImageSub(src2, src1);

// 		TrIplImageOneColor(src1, 10, 10, 10, 15);

		TrIplImageYuvToRgb(src1);
		TrIplImageYuvToRgb(src2);
// 		cvDilate(src1, src1, NULL, 1);
// 		cvErode(src1, src1, NULL, 1);
// 		cvSmooth(src1, src1, CV_GAUSSIAN, 35, 35, 0, 0);

// 		cvCvtColor(src1, src1c1, CV_BGR2GRAY);
// 		cvEqualizeHist(src1c1, src1c1);// 		ComShowImage("src1c", src1c1);


// 		cvAdaptiveThreshold(src1c1, src1c1, 200, CV_ADAPTIVE_THRESH_MEAN_C,
// 			CV_THRESH_BINARY_INV, 25, 10);

		ComShowImage("src1", src1);
		ComShowImage("src2", src2);
		cvWaitKey(0);
		return src1;
	}
	if (false) {
		IplImage *src = cvLoadImage(filename);
		ComShowImage("src", src);

		IplImage *dst = cvCreateImage(cvGetSize(src), 8, 3);
		IplImage *src1c = cvCreateImage(cvGetSize(src), 8, 1);

		cvSmooth(src, dst, CV_BLUR, src->width / 100, src->height / 100, 0, 0);
		TrIplImageRgbToYuv(src);
		TrIplImageRgbToYuv(dst);
		TrIplImageDiv(dst, src);
		TrIplImageYuvToRgb(src);

		cvSmooth(src, dst, CV_BLUR, src->width / 100, src->height / 100, 0, 0);
		TrIplImageRgbToYuv(src);
		TrIplImageRgbToYuv(dst);
		TrIplImageDiv(dst, src);
		TrIplImageYuvToRgb(src);




		cvCvtColor(src, src1c, CV_BGR2GRAY);
		cvEqualizeHist(src1c, src1c);

		// 		cvAdaptiveThreshold(src1c1, src1c1, 200, CV_ADAPTIVE_THRESH_MEAN_C,
		// 			CV_THRESH_BINARY_INV, 25, 10);

		ComShowImage("src1c", src1c);
		cvWaitKey(0);
	}
	if (false) {
		IplImage *src = cvLoadImage(filename);
		ComShowImage("src", src);
		TrIplImageRgbToYuv(src);

		IplImage *imgy = cvCloneImage(src);
		IplImage *imgu = cvCloneImage(src);
		IplImage *imgv = cvCloneImage(src);

		TrIplImageChannelChoice(imgy, false, true, true, 0, 0, 0);
		TrIplImageChannelChoice(imgu, true, false, true, 0, 0, 0);
		TrIplImageChannelChoice(imgv, true, true, false, 0, 0, 0);



		// 		cvAdaptiveThreshold(imgu, imgu, 10, CV_ADAPTIVE_THRESH_MEAN_C,
		// 			CV_THRESH_BINARY_INV, 25, 10);
		ComShowImage("imgy", imgy);
		ComShowImage("imgu", imgu);
		ComShowImage("imgv", imgv);
		cvWaitKey(0);
	}
	if (false) {
		IplImage *src = cvLoadImage(filename);

		IplImage *imgy = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage *imgu = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage *imgv = cvCreateImage(cvGetSize(src), 8, 1);

		ComShowImage("src", src);
		TrIplImageRgbToYuv(src);
		cvSplit(src, imgv, 0, 0, 0);//分离三个通道
		cvSplit(src, 0, imgu, 0, 0);
		cvSplit(src, 0, 0, imgy, 0);


// 		cvAdaptiveThreshold(imgu, imgu, 10, CV_ADAPTIVE_THRESH_MEAN_C,
// 			CV_THRESH_BINARY_INV, 25, 10);
		ComShowImage("imgy", imgy);
		ComShowImage("imgu", imgu);
		ComShowImage("imgv", imgv);
		cvWaitKey(0);
	}
	if (false) {
		IplImage *src = cvLoadImage(filename);
		ComShowImage("src", src);
		TrIplImageRgbToHsl(src);
		TrIplImageChannelChoice(src, false, true, true, 128, 128, 128);
		TrIplImageHslToRgb(src);

		ComShowImage("dst", src);
		cvWaitKey(0);
	}
	if (false) {
		IplImage *image, *hsv, *hue, *saturation, *value;//图像空间
		image = cvLoadImage(filename);//打开图像源图像

		hsv = cvCreateImage(cvGetSize(image), 8, 3);//给hsv色系的图像申请空间
		hue = cvCreateImage(cvGetSize(image), 8, 3);  //色调
		saturation = cvCreateImage(cvGetSize(image), 8, 1);//饱和度
		value = cvCreateImage(cvGetSize(image), 8, 1);//亮度

	// 	cvNamedWindow("image", CV_WINDOW_AUTOSIZE);//用于显示图像的窗口
	// 	cvNamedWindow("hsv", CV_WINDOW_AUTOSIZE);
	// 	cvNamedWindow("hue", CV_WINDOW_AUTOSIZE);
	// 	cvNamedWindow("saturation", CV_WINDOW_AUTOSIZE);
	// 	cvNamedWindow("value", CV_WINDOW_AUTOSIZE);

		cvCvtColor(image, hsv, CV_BGR2YUV);//将RGB色系转为HSV色系
	// 	TrIplImageSetOne(hsv);
		cvCvtColor(hsv, hue, CV_YUV2BGR);


		ComShowImage("image", image);
	// 	ComShowImage("hsv", hsv);

	// 	cvSplit(hsv, hue, 0, 0, 0);//分离三个通道
	// 	cvSplit(hsv, 0, saturation, 0, 0);
	// 	cvSplit(hsv, 0, 0, value, 0);

		ComShowImage("hue", hue);
	// 	ComShowImage("saturation", saturation);
	// 	ComShowImage("value", value);
		cvWaitKey(0);
		cvDestroyWindow("image");
		cvDestroyWindow("hsv");
		cvDestroyWindow("hue");
		cvDestroyWindow("saturation");
		cvDestroyWindow("value");
	}
	if (false) {
		IplImage *src = cvLoadImage(filename, 1);//原图
		IplImage *dst_gray = cvCreateImage(cvGetSize(src), src->depth, 1);//灰度图
		IplImage *dst_image = cvCreateImage(cvGetSize(src), 32, src->nChannels);
		IplImage *src_image_32 = cvCreateImage(cvGetSize(src), 32, src->nChannels);
		//这两个图需要是32浮点位的，因为对原图进行归一化后得到的是浮点数
		cvCvtColor(src, dst_gray, CV_BGR2GRAY);//得到灰度图
		cvConvertScale(src, src_image_32, 1.0 / 255.0, 0);//将原图RGB归一化到0-1之间
		cvCvtColor(src_image_32, dst_image, CV_BGR2HSV);//得到HSV图
		ComShowImage("src", src);

		ComShowImage("src_image_32", src_image_32);
		cvWaitKey(0);

	}
}

void ComBalanceImage(IplImage *src) {

// 	TrIplImageChannelChoice(src, true, false, false, 255, 0, 0);				// remove red

	IplImage *dst1 = cvCreateImage(cvGetSize(src), 8, 3);				// for 1/10 gassize
	int gassize = (MIN(src->height, src->width) / 100) * 2 + 3;
	cvSmooth(src, dst1, CV_BLUR, gassize, gassize, 0, 0);

	IplImage *dst2 = cvCreateImage(cvGetSize(src), 8, 3);				// for 1/60 gassize
	gassize = (MIN(src->height, src->width) / 360) * 2 + 3;
	cvSmooth(src, dst2, CV_GAUSSIAN, gassize, gassize, 0, 0);

	TrIplImageRgbToYuv(src);
	TrIplImageChannelChoice(src, false, true, true, 0, 0, 0);				// remove color

	TrIplImageRgbToYuv(dst1);
	TrIplImageBeOne(dst1, src);

// 	TrIplImageRgbToYuv(dst2);
	TrIplImageYuvToRgb(src);
	cvEqualizeHistColor(src, src);


	ComShowImage("src", src);
	cvWaitKey(0);

	cvReleaseImage(&dst2);
	cvReleaseImage(&dst1);
}

void DrawBox(IplImage* img, CvBox2D box, CvScalar color) {
	CvPoint2D32f point[4];
	int i;
	for (i = 0; i < 4; i++) {
		point[i].x = 0;
		point[i].y = 0;
	}
	cvBoxPoints(box, point); //计算二维盒子顶点 
	CvPoint pt[4];
	for (i = 0; i < 4; i++) {
		pt[i].x = (int)point[i].x;
		pt[i].y = (int)point[i].y;
	}
	cvLine(img, pt[0], pt[1], color, 6, 8, 0);
	cvLine(img, pt[1], pt[2], color, 6, 8, 0);
	cvLine(img, pt[2], pt[3], color, 6, 8, 0);
	cvLine(img, pt[3], pt[0], color, 6, 8, 0);
}

IplImage* pocContours(IplImage *src) {
	CvMemStorage *store;
	CvSeq *conts, *now;
	IplImage *src1c;

	return NULL;



	store = cvCreateMemStorage(0);
// 	cvErode(src, src, NULL, 3);
	src1c = cvCreateImage(cvGetSize(src), 8, 1);
	cvCvtColor(src, src1c, CV_BGR2GRAY);
	cvFindContours(src1c, store, &conts, sizeof(CvContour),
		CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
	CvBox2D box2d;

	for (now = conts; now; now = now->h_next) {
		box2d = cvMinAreaRect2(now);
		if (box2d.size.width > src->width / 100 && box2d.size.height > src->height / 100) {
			DrawBox(src, box2d, CV_RGB(32, 131, 222));

			// 		area = fabs(cvContourArea(nowcont));
			// 		if (area > arearate) {
			// 			havelarge = true;
						// I am NOT sure, should add hull ?
						//			hull = cvConvexHull2(nowcont, nowstore, CV_CLOCKWISE, 1 /* MUST BE 1*/);
						//			cvDrawContours(dst, hull, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, linewidth /*THICK_WIDTH*/);
// 			cvDrawContours(src, now, CV_RGB(32, 231, 22), CV_RGB(221, 111, 55), 0, 7 /*THICK_WIDTH*/);
		}
// 		}
// 	}
// 	if (!havelarge) {
// 		for (nowcont = conts; nowcont; nowcont = nowcont->h_next) {

// 			if (len > lenrate) {
// 				cvDrawContours(dst, nowcont, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, linewidth /*THICK_WIDTH*/);
// 			}
 		}


	ComShowImage("src", src);
	cvWaitKey(0);
	return NULL;
}




void pocFilter(IplImage *src) {

	IplImage *dst;

	float low[9] = { 1.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 4.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 1.0 / 16 };//低通滤波核
	float high[9] = { -1,-1,-1,-1,9,-1,-1,-1,-1 };//高通滤波核
	CvMat km = cvMat(3, 3, CV_32FC1, high);  //构造单通道浮点矩阵，将图像IplImage结构转换为图像数组 
	dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	cvFilter2D(src, src, &km, cvPoint(-1, -1));  //设参考点为核的中心
// 	return dst;
// 	ComShowImage("src", src);
// 
// 	ComShowImage("dst", dst);
// 
// 	cvWaitKey(0);
// 	cvReleaseImage(&src);
// 	cvReleaseImage(&dst);
}
