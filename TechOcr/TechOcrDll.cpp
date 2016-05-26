// dllmain.cpp : 定義 DLL 應用程式的進入點。
#include "TechOcrDll.h"

// useful url
// http://www.win.tue.nl/~aeb/linux/ocr/tesseract.html
// https://github.com/tesseract-ocr/tesseract/blob/master/api/baseapi.h
// http://www.aiuxian.com/article/p-1968253.html

RESULT GB2312toUTF8(char* gb2312, long gsize, char*& utf8, long &usize) {
	long unicodelen = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)gb2312, gsize, NULL, 0);
	WCHAR *strSrc = new WCHAR[unicodelen + 16];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)gb2312, gsize, strSrc, unicodelen);

	usize = WideCharToMultiByte(CP_UTF8, 0, strSrc, unicodelen, NULL, 0, NULL, NULL);
	utf8 = new char[usize + 16];
	WideCharToMultiByte(CP_UTF8, 0, strSrc, unicodelen, (LPSTR)utf8, usize, NULL, NULL);
	utf8[usize] = 0;
	delete[]strSrc;
	return RESULT_OK;
}
RESULT UTF8toGB2312(char* utf8, long usize, char*& gb2312, long &gsize) {
	long unicodelen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, usize, NULL, 0);
	WCHAR *strSrc = new WCHAR[unicodelen + 16];
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, usize, strSrc, unicodelen);

	gsize = WideCharToMultiByte(CP_ACP, 0, strSrc, unicodelen, NULL, 0, NULL, NULL);
	gb2312 = new char[gsize + 16];
	WideCharToMultiByte(CP_ACP, 0, strSrc, unicodelen, (LPSTR)gb2312, gsize, NULL, NULL);
	gb2312[gsize] = 0;
	delete[]strSrc;
	return RESULT_OK;
}

SourceItem BusinessLicense[MAX_DETECT];

RESULT BusinessLicenseDatum(OnceOcr *ocr, CvRect &rect, void *futureuse) {
	double xrate = ((double)rect.width) / ocr->targetImage->w;
	double yrate = ((double)rect.height) / ocr->targetImage->h;
	if (0.05 < xrate && xrate < 0.15 && 0.05 < yrate && yrate < 0.15) {
	}
	return RESULT_OK;
}




char argv_prog[] = "c:\\tesseract.exe";
char argv_in[] = "Z:\\solution\\files\\c2.jpg";
char argv_out[] = "Z:\\solution\\files\\c2.txt";
char argv_flag[] = "-l";
char argv_lan[] = "chi_sim";
char *dllargv[] = { argv_prog, argv_in, argv_out, argv_flag, argv_lan };

RESULT ShowImage(IplImage* img, char* name)
{
	if (!img || !name)
		return RESULT_PARA_ERROR;
	cvNamedWindow(name, 1);

	CvSize   resize;
	IplImage *dst;
	RECT     rect;
	HWND hWnd = ::GetDesktopWindow();
	::GetWindowRect(hWnd, &rect);

	float scale = MAX((float)img->width / (rect.right - rect.left - 50),
		(float)img->height / (rect.bottom - rect.top - 100));
	if (scale < (float)1)
		scale = (float)1;
	resize.width = (int)(img->width / scale);
	resize.height = (int)(img->height / scale);

	dst = cvCreateImage(resize, img->depth, img->nChannels);
	cvResize(img, dst, CV_INTER_LINEAR);

	if (img)
		cvShowImage(name, dst);

	cvReleaseImage(&dst);
	return RESULT_OK;
};


// http://www.cppblog.com/kongque/archive/2011/05/24/147014.html    // pay more attention to the TYPE of CvMat
CvPoint transformPoint(const CvPoint pointToTransform, const CvMat* matrix)
{
	double coordinates[3] = { pointToTransform.x, pointToTransform.y, 1 };
	CvMat originVector = cvMat(3, 1, CV_64F, coordinates);
	CvMat transformedVector = cvMat(3, 1, CV_64F, coordinates);
	cvMatMul(matrix, &originVector, &transformedVector);
	CvPoint outputPoint = cvPoint((int)(cvmGet(&transformedVector, 0, 0) / cvmGet(&transformedVector, 2, 0)), 
		(int)(cvmGet(&transformedVector, 1, 0) / cvmGet(&transformedVector, 2, 0)));
	return outputPoint;
};


/**********************************************************************
*  main()
*
**********************************************************************/
int main_ttt(int argc, char **argv) {
	const char * image = argv_in;
	const char * outputbase = argv_out;


	CvPoint2D32f srcTri[4], desTri[4];
	CvMat*       warp_mat = cvCreateMat(3, 3, CV_64F);

	srcTri[0].x = 1286;
	srcTri[0].y = 935;
	srcTri[1].x = 2154 + 258;
	srcTri[1].y = 937;
	srcTri[2].x = 2154 + 258;
	srcTri[2].y = 937 + 293;
	srcTri[3].x = 1286;
	srcTri[3].y = 935 + 284;

	desTri[0].x = 1059;
	desTri[0].y = 1037;
	desTri[1].x = 2252 + 391;
	desTri[1].y = 1043;
	desTri[2].x = 2252 + 391;
	desTri[2].y = 1043 + 417;
	desTri[3].x = 1059;
	desTri[3].y = 1037 + 402;

	cvGetPerspectiveTransform(srcTri, desTri, warp_mat);

	CvPoint srcpoint, despoint;
	srcpoint = cvPoint(656, 2487);
//	srcpoint = cvPoint(1712, 997);
	despoint = transformPoint(srcpoint, warp_mat);
	//	cvWarpPerspective(Image1c, ImageTran1c, warp_mat);
	//	cvWarpPerspective(ImageSrc, ImageTran, warp_mat);

	cvReleaseMat(&warp_mat);



	tesseract::TessBaseAPI api;

	api.SetOutputName(outputbase);
	int rc = api.Init(NULL, argv_lan, tesseract::OEM_DEFAULT);
	if (rc) {
		printf("Could not initialize tesseract.\n");
		exit(1);
	}
	//api.SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	Pix* pixs = pixRead(image);
	if (!pixs) {
		printf("Cannot open input file: %s\n", image);
		exit(2);
	}
	api.SetImage(pixs);
	tesseract::TessResultRenderer* renderer = new tesseract::TessTextRenderer(outputbase);

	IplImage *src = cvLoadImage(argv_in, 1);     // 1 for is colored

	Pixa* pixa;
//	Boxa* boxa = api.GetTextlines(&pixa, NULL);
//	Boxa* boxa = api.GetStrips(&pixa, NULL);
	Boxa* boxa = api.GetWords(&pixa);
	
	Box** box;

	CvRect rect;
	Box *maxrect = NULL;
	int maxrectsize = 0;
	int nowrectsize;

	box = boxa->box;




	for (int i = 0; i < pixa->n; i++) {

		rect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
		double xrate = ((double)(*box)->w) / pixs->w;
		double yrate = ((double)(*box)->h) / pixs->h;
		if (0.05 < xrate && xrate < 0.15 && 0.05 < yrate && yrate < 0.15) {
			std::cout << (*box)->x << ", " << (*box)->y << ", " << (*box)->w << ", " << (*box)->h << ", ";//std::endl;
			cvRectangleR(src, rect, CV_RGB(221, 134, 212), 3);
//1286, 935, 272, 284, 1709, 928, 282, 305, 2154, 937, 258, 293,
//1059, 1037, 382, 402, 1653, 1029, 396, 432, 2252, 1043, 391, 417,
		}

		nowrectsize = (*box)->w * (*box)->h;
		if (nowrectsize > maxrectsize) {
			maxrectsize = nowrectsize;
			maxrect = (*box);
		}
		box++;
	}

//	ShowImage(src, "src");
//	cvWaitKey(0);

//	return 0;


//	Boxa* boxa = api.GetRegions(&pixa);

	api.SetRectangle(maxrect->x - maxrect->w / 10, maxrect->y - maxrect->h / 10, 
		maxrect->w + maxrect->w / 5, maxrect->h + maxrect->h /5);
	rect = cvRect(maxrect->x - maxrect->w / 10, maxrect->y - maxrect->h / 10,
		maxrect->w + maxrect->w / 5, maxrect->h + maxrect->h / 5);
	cvRectangleR(src, rect, CV_RGB(55, 234, 112), 6);

	//PageSegMode mode;
	api.SetPageSegMode(tesseract::PSM_SINGLE_WORD);
//	api.Recognize(NULL);

	char *text;
	char *gbtext;
	long gsize;

	box = boxa->box;
	int starty = maxrect->y + maxrect->h, endy;
	starty = 9999;// 890;
	endy = 1700;// 2700;
	for (int i = 0; i < pixa->n; i++) {
		if ((*box)->y > starty && (*box)->y < endy) {
			rect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
//			cvRectangleR(src, rect, CV_RGB(221, 134, 212), 3);
			api.SetRectangle((*box)->x, (*box)->y, (*box)->w, (*box)->h);
			api.Recognize(NULL);

			std::cout << (*box)->x << ", " << (*box)->y << ", " << (*box)->w << ", " << (*box)->h << ", ";//std::endl;

			text = api.GetUTF8Text();
			UTF8toGB2312(text, strlen(text), gbtext, gsize);
			std::cout << gbtext;// << std::endl;
			delete[] text;
			delete[] gbtext;
		}
		box++;
	}


	boxaDestroy(&boxa);
	api.End();
	//if (!api.ProcessPages(image, NULL, 0, renderer)) {
	//	printf("Error during processing.\n");
	//}
	delete renderer;
	pixDestroy(&pixs);

	ShowImage(src, "src");
	cvWaitKey(0);

	return 0;
}

int main_page(int argc, char **argv) {
	const char * image = argv_in;
	const char * outputbase = argv_out;
	tesseract::TessBaseAPI api;
	api.SetOutputName(outputbase);
	int rc = api.Init(NULL, argv_lan, tesseract::OEM_DEFAULT);
	if (rc) {
		printf("Could not initialize tesseract.\n");
		exit(1);
	}
	//api.SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	Pix* pixs = pixRead(image);
	if (!pixs) {
		printf("Cannot open input file: %s\n", image);
		exit(2);
	}
	api.SetImage(pixs);
	tesseract::TessResultRenderer* renderer = new tesseract::TessTextRenderer(outputbase);
	if (!api.ProcessPages(image, NULL, 0, renderer)) {
		printf("Error during processing.\n");
	}

	IplImage *src = cvLoadImage(argv_in, 1);     // 1 for is colored

	Pixa* pixa;
	Boxa* boxa = api.GetTextlines(&pixa, NULL);
	Box** box = boxa->box;

	CvRect rect;
	for (int i = 0; i < pixa->n; i++) {
		rect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
		cvRectangleR(src, rect, CV_RGB(221, 134, 212), 3);
		box++;
	}
	ShowImage(src, "src");
	cvWaitKey(0);


	delete renderer;
	pixDestroy(&pixs);
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
};


char* WINAPI TechOcr(char* imgtype, char* filename)
{
//  RESULT result;
//  result = decode.Process(filename, checked, false);
//   if (result != RESULT_OK)
//   {
//     result = decoderot.Process(filename, checked, true);
//   }
  main_ttt(5, dllargv);
  //main_page(0, 0);
  return "try first";
};

JNIEXPORT jstring JNICALL Java_net_gbicc_xbrl_otc_util_ImageScanForC_TechOcr
  (JNIEnv * env, jclass jclassname, jstring imgtype, jstring filename)
{
   const char* strimgtype;
   const char* strfilename;

   strimgtype = env->GetStringUTFChars(imgtype, false);
   strfilename = env->GetStringUTFChars(filename, false);
   if(strimgtype == NULL || strfilename == NULL)
   {  
       return NULL; // OutOfMemoryError already thrown 
   }  
   char* checked = TechOcr((char*)imgtype, (char*)strfilename);

   env->ReleaseStringUTFChars(imgtype, strimgtype);
   env->ReleaseStringUTFChars(filename, strfilename);  

   jstring returnstr = env->NewStringUTF(checked);
   return returnstr;
};

