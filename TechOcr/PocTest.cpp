#include "PocTest.h"

#define  OUTPUTBOX(b, s)	std::cout << (b)->x << ", " << (b)->y << ", " << (b)->w << ", " << (b)->h << ", " \
	<< s << std::endl;

#define MIN_CONFIRM				6
CvSeq *format;
void pcoPreprocess(char *filename) {
	RESULT result;
	CvPoint2D32f corner[4];
	CvPoint2D32f *pcorner = corner;
	IplImage *src, *dst;
	Pix *pix;
	tesseract::TessBaseAPI* api;
	CvSeq *feature, *content, *bestformat = NULL;
// 	CvPoint2D32f csrc[4], cdst[4];
// 	int mostfea, mostfor;
	int match, rotateloop = 0;
	CvMemStorage *storage;
	int i;
	storage = GetGlobalStorage();


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

	CharFound* found, *featurematch;
	char * ch, *gb;
	long gsize;
	content = cvCreateSeq(0, sizeof(CvSeq), sizeof(CharFound), storage);
	TechOcrDetectWordsInFormat(dst, warp1, warp2, bestformat, content);

// 	for (i = 0; i < content->total; i++) {
// 		found = (CharFound*)cvGetSeqElem(content, i);
// 		ch = (char*)found->desc;
// 		ComUtf8ToGbk(ch, strlen(ch), gb, gsize);
// 		std::cout << found->rect.x << ", " << found->rect.y << ", CHAR " << ch;
// 		delete[] gb;
// 		delete[] ch;
// 		featurematch = (CharFound*)cvGetSeqElem(bestformat, found->found);
// 		ch = (char*)featurematch->desc;
// 		ComUtf8ToGbk(ch, strlen(ch), gb, gsize);
// 		std::cout << ", " << gb << std::endl;
// 		delete[] gb;
// 		delete[] ch;
// 
// 	}
	char *output;
	TechOcrOutput(content, bestformat, output);
	cvRelease((void**)&content);

// 	ComShowImage("d1", d1);
// 	ComShowImage("d2", d2);
	cvWaitKey(0);

// 	TechOcrExitTessAPI(api);

	cvReleaseMat(&warp1);
	cvReleaseMat(&warp2);

// 	cvReleaseImage(&d2);
	cvReleaseImage(&dst);
	cvReleaseImage(&src);
}

void pocCreateBusinessLicense(void) {
	TechOcrCreateFormat(format, "Ӫҵִ��", 3120, 4160);

	TechOcrFormatAddFeature(format, 622, 1638, 61, 69, "��");
	TechOcrFormatAddFeature(format, 962, 1641, 69, 66, "��");
	TechOcrFormatAddFeature(format, 624, 1758, 65, 68, "��");
	TechOcrFormatAddFeature(format, 964, 1757, 66, 65, "��");
	TechOcrFormatAddFeature(format, 623, 1872, 68, 68, "ס");
	TechOcrFormatAddFeature(format, 964, 1873, 68, 65, "��");

	TechOcrFormatAddFeature(format, 627, 2100, 64, 64, "ע");
	TechOcrFormatAddFeature(format, 738, 2098, 68, 64, "��");
	TechOcrFormatAddFeature(format, 854, 2095, 62, 70, "��");
	TechOcrFormatAddFeature(format, 965, 2095, 68, 70, "��");
	
	TechOcrFormatAddFeature(format, 625, 2212, 67, 70, "��");
	TechOcrFormatAddFeature(format, 740, 2215, 65, 63, "��");
	TechOcrFormatAddFeature(format, 853, 2212, 65, 70, "��");
	TechOcrFormatAddFeature(format, 967, 2212, 65, 68, "��");

	TechOcrFormatAddFeature(format, 625, 2332, 65, 71, "Ӫ");
	TechOcrFormatAddFeature(format, 739, 2332, 67, 65, "ҵ");
	TechOcrFormatAddFeature(format, 853, 2330, 65, 68, "��");
	TechOcrFormatAddFeature(format, 970, 2330, 63, 66, "��");

	TechOcrFormatAddFeature(format, 624, 2451, 67, 64, "��");
	TechOcrFormatAddFeature(format, 739, 2448, 66, 70, "Ӫ");
	TechOcrFormatAddFeature(format, 853, 2445, 67, 71, "��");
	TechOcrFormatAddFeature(format, 971, 2447, 59, 67, "Χ");

	TechOcrFormatAddFeature(format, 1562, 3436, 71, 67, "��");
	TechOcrFormatAddFeature(format, 1680, 3435, 71, 69, "��");
	TechOcrFormatAddFeature(format, 1798, 3435, 74, 70, "��");
	TechOcrFormatAddFeature(format, 1920, 3433, 67, 72, "��");

	TechOcrFormatAddContent(format, 1785, 1469, 800, 55, "ע���");
	TechOcrFormatAddContent(format, 1095, 1636, 1700, 66, "����");
	TechOcrFormatAddContent(format, 1095, 1758, 1700, 66, "����");
	TechOcrFormatAddContent(format, 1095, 1872, 1700, 66, "ס��");
	TechOcrFormatAddContent(format, 1095, 1984, 1700, 66, "����������");
	TechOcrFormatAddContent(format, 1095, 2095, 1700, 66, "ע���ʱ�");
	TechOcrFormatAddContent(format, 1095, 2212, 1700, 66, "��������");
	TechOcrFormatAddContent(format, 1095, 2330, 1700, 66, "Ӫҵ����");
// 	TechOcrFormatAddContent(format, 1095, 2440, 1700, 550, "��Ӫ��Χ", CHARTYPE_CONTENT_BLOCK);
	// make it faster

	return;
}
