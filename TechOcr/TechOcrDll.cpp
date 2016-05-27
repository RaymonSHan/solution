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

// http://www.cppblog.com/kongque/archive/2011/05/24/147014.html    // pay more attention to the TYPE of CvMat
CvPoint TransformPoint(const CvPoint point, const CvMat *matrix) {
	double coordinates[3] = { (double)point.x, (double)point.y, (double)1 };
	CvMat originVector = cvMat(3, 1, CV_64F, coordinates);
	CvMat transformedVector = cvMat(3, 1, CV_64F, coordinates);
	cvMatMul(matrix, &originVector, &transformedVector);
	CvPoint outputPoint = cvPoint((int)(cvmGet(&transformedVector, 0, 0) / cvmGet(&transformedVector, 2, 0)),
		(int)(cvmGet(&transformedVector, 1, 0) / cvmGet(&transformedVector, 2, 0)));
	return outputPoint;
};

CvRect TransformRect(const CvRect rect, const CvMat *matrix, int enlarge) {
	CvPoint topleft = TransformPoint(cvPoint(rect.x - enlarge, rect.y - enlarge), matrix);
	CvPoint topright = TransformPoint(cvPoint(rect.x + rect.width + enlarge * 2, rect.y - enlarge), matrix);
	CvPoint bottomleft = TransformPoint(cvPoint(rect.x - enlarge, rect.y + rect.height + enlarge * 2), matrix);
	CvPoint bottomright = TransformPoint(cvPoint(rect.x + rect.width + enlarge * 2, rect.y + rect.height + enlarge * 2), matrix);
	int minx = MIN(topleft.x, bottomleft.x);
	int miny = MIN(topleft.y, topright.y);
	int maxx = MAX(topright.x, bottomright.x);
	int maxy = MAX(bottomleft.y, bottomright.y);
	return cvRect(minx, miny, maxx - minx, maxy - miny);
}


RESULT BusinessLicenseDatum(OnceOcrClass *ocr, Box *box, void *futureuse) {
	double xrate = ((double)box->w) / ocr->targetImage->w;
	double yrate = ((double)box->h) / ocr->targetImage->h;
	if (0.05 < xrate && xrate < 0.15 && 0.05 < yrate && yrate < 0.15) {
		return RESULT_OK;
	}
	else {
		return RESULT_ERR;
	}
}

RESULT OnceOcrClass::InitOnceOcr(Pix *image) {
	api = new tesseract::TessBaseAPI;

	int rc = api->Init(NULL, DEFAULT_LAN, tesseract::OEM_DEFAULT);
	if (rc)
		return RESULT_ERR;
	targetImage = image;
	api->SetImage(image);

	return RESULT_OK;
}
RESULT OnceOcrClass::ExitOnceOcr() {
	pixDestroy(&targetImage);
	api->End();
	delete api;
	return RESULT_OK;
}
RESULT OnceOcrClass::MapRect(MYINT num, SourceItem *source) {
	Pixa *pixw, *pixt;
	Boxa *boxw = api->GetWords(&pixw);
//	Boxa *boxt = api->GetTextlines(&pixt, NULL);
	Box **box;
	SourceItem *nowitem;
	char *targetstr = NULL;

	CvPoint2D32f srcTri[4], desTri[4];
	CvMat*       warp_mat = cvCreateMat(3, 3, CV_64F);
	char nowdatum = 0;
	int i, j;
	Box *maxbox;
	MYINT maxarea = 0;

	char* gbtext;
	long gsize;

	box = boxw->box;
	for (i = 0; i < pixw->n; i++) {		// for every box
		for (j = 0; j < num; j++) {		// for every detect
			nowitem = source + j;
			if (nowitem->isDatum && nowitem->isDatumFunc && nowitem->shouldBe
				&& nowitem->isDatumFunc(this, *box, NULL) == RESULT_OK) {
				// this is datum, should detect
				if (targetstr == NULL) {
					// have not recognize
					api->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
					api->SetRectangle((*box)->x - (*box)->w / 10, (*box)->y - (*box)->h / 5,
						(*box)->w + (*box)->w / 5, (*box)->h + (*box)->h / 5);
					api->Recognize(NULL);
					targetstr = api->GetUTF8Text();
					UTF8toGB2312(targetstr, (long)strlen(targetstr), gbtext, gsize);
//					std::cout << (*box)->x << ", " << (*box)->y << ", " << (*box)->w << ", " << (*box)->h << ", ";//std::endl;
//					std::cout << gbtext;// << std::endl;
				}
				if (!strncmp(nowitem->shouldBe, targetstr, strlen(nowitem->shouldBe))) {
					// Matched
					if (nowitem->isDatum & 0x1) {
						srcTri[0].x = (float)nowitem->rectName.x;
						srcTri[0].y = (float)nowitem->rectName.y;
						desTri[0].x = (float)(*box)->x;
						desTri[0].y = (float)(*box)->y;
						nowdatum |= 0x1;
					}
					if (nowitem->isDatum & 0x2) {
						srcTri[1].x = (float)nowitem->rectName.x + nowitem->rectName.width;
						srcTri[1].y = (float)nowitem->rectName.y;
						desTri[1].x = (float)(*box)->x + (*box)->w;
						desTri[1].y = (float)(*box)->y;
						nowdatum |= 0x2;
					}
					if (nowitem->isDatum & 0x4) {
						srcTri[2].x = (float)nowitem->rectName.x + nowitem->rectName.width;;
						srcTri[2].y = (float)nowitem->rectName.y + nowitem->rectName.height;
						desTri[2].x = (float)(*box)->x + (*box)->w;
						desTri[2].y = (float)(*box)->y + (*box)->h;
						nowdatum |= 0x4;
					}
					if (nowitem->isDatum & 0x8) {
						srcTri[3].x = (float)nowitem->rectName.x;
						srcTri[3].y = (float)nowitem->rectName.y + nowitem->rectName.height;
						desTri[3].x = (float)(*box)->x;
						desTri[3].y = (float)(*box)->y + (*box)->h;
						nowdatum |= 0x8;
					}
					break;
				}
			}
		}
		box++;
		delete[] targetstr;
		targetstr = NULL;

		if (nowdatum == 0xf) {
			// have found 4 conner, to finish
			break;
		}
	}
	if (nowdatum != 0xf) {
		// error, not found conner datum
		std::cout << "NOT found datum" << std::endl;
		return RESULT_ERR;
	}
	cvGetPerspectiveTransform(srcTri, desTri, warp_mat);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (i = 0; i < num; i++) {
		nowitem = source + i;
		if (!nowitem->isOutput) continue;
		if (nowitem->rectName.width != 0 && nowitem->rectName.height != 0) {
			targetItem[i].rectName = TransformRect(nowitem->rectName, warp_mat, ENLAGRE);
			api->SetPageSegMode(nowitem->nameMode);
			api->SetRectangle(targetItem[i].rectName.x, targetItem[i].rectName.y,
				targetItem[i].rectName.width, targetItem[i].rectName.height);

//			api->SetVariable("tessedit_char_whitelist", nowitem->shouldBe);


			api->Recognize(NULL);
			targetstr = api->GetUTF8Text();
			UTF8toGB2312(targetstr, (long)strlen(targetstr), gbtext, gsize);
			std::cout << targetItem[i].rectName.x << ", " << targetItem[i].rectName.y << ", " 
				<< targetItem[i].rectName.width << ", " << targetItem[i].rectName.height << ", ";//std::endl;
			std::cout << gbtext;
			delete[] targetstr;
			targetstr = NULL;

		}
		if (nowitem->rectValue.width != 0 && nowitem->rectValue.height != 0) {

			if (nowitem->rectValue.height > 200)
			{
				int aaa = 0;
			}
			targetItem[i].rectValue = TransformRect(nowitem->rectValue, warp_mat, ENLAGRE);
			api->SetPageSegMode(nowitem->valueMode);
			api->SetRectangle(targetItem[i].rectValue.x, targetItem[i].rectValue.y,
				targetItem[i].rectValue.width, targetItem[i].rectValue.height);

//			api->SetVariable("tessedit_char_whitelist", NULL);

			api->Recognize(NULL);
			targetstr = api->GetUTF8Text();
			UTF8toGB2312(targetstr, (long)strlen(targetstr), gbtext, gsize);
			std::cout << targetItem[i].rectValue.x << ", " << targetItem[i].rectValue.y << ", "
				<< targetItem[i].rectValue.width << ", " << targetItem[i].rectValue.height << ", ";//std::endl;
			std::cout << gbtext;
			delete[] targetstr;
			targetstr = NULL;
		}
	}




	CvPoint srcpoint, despoint;
	srcpoint = cvPoint(656, 2487);
	//	srcpoint = cvPoint(1712, 997);
	despoint = TransformPoint(srcpoint, warp_mat);
	//	cvWarpPerspective(Image1c, ImageTran1c, warp_mat);
	//	cvWarpPerspective(ImageSrc, ImageTran, warp_mat);

	cvReleaseMat(&warp_mat);




	boxaDestroy(&boxw);
	pixaDestroy(&pixw);

	return RESULT_OK;
}

RESULT OnceOcrClass::OutputOcr(char* &output) {		// should delete[] outside the function
	output = new char[100];
	return RESULT_OK;
}



RESULT ModelOcrClass::AddOcrItem(CvRect &name, CvRect &value, char* shouldbe, bool isoutput,
	tesseract::PageSegMode nmode, tesseract::PageSegMode vmode) {
	MYINT num = InterInc(&sourceNumber) - 1;
	SourceItem *now = &sourceItem[num];
	now->rectName = name;
	now->rectValue = value;
	now->nameMode = nmode;
	now->valueMode = vmode;
	char* utf8;
	long usize;
	GB2312toUTF8(shouldbe, strlen(shouldbe), utf8, usize);
	strncpy_s(now->shouldBe, utf8, sizeof(SourceItem::shouldBe));
	delete []utf8;
	now->isDatumFunc = NULL;
	now->isDatum = 0;
	now->isOutput = isoutput;
	return RESULT_OK;
}

RESULT ModelOcrClass::AddOcrItem(CvRect &name, DetectDatum isdatum, char datum, char* shouldbe, bool isoutput,
	tesseract::PageSegMode nmode) {
	MYINT num = InterInc(&sourceNumber) - 1;
	SourceItem *now = &sourceItem[num];
	now->rectName = name;
	now->rectValue = RECT_ZERO;
	now->nameMode = nmode;
	now->valueMode = nmode;
	char* utf8;
	long usize;
	GB2312toUTF8(shouldbe, strlen(shouldbe), utf8, usize);
	strncpy_s(now->shouldBe, utf8, sizeof(SourceItem::shouldBe));
	delete[]utf8;
	now->isDatumFunc = isdatum;
	now->isDatum = datum;
	now->isOutput = isoutput;
	return RESULT_OK;
}

RESULT ModelOcrClass::GenerateTarget(Pix *image, OnceOcrClass* &once) {
	RESULT result;
	result = once->InitOnceOcr(image);
	if (result != RESULT_OK)
		return RESULT_ERR;

	result = once->MapRect(sourceNumber, sourceItem);
	if (result != RESULT_OK)
		return RESULT_ERR;

	result = once->ExitOnceOcr();
	if (result != RESULT_OK)
		return RESULT_ERR;

	return RESULT_OK;
}

RESULT TechOcrClass::InitBusinessLicense(void) {
	BusinessLicenseOcr.AddOcrItem(cvRect(1286, 935, 272, 284), BusinessLicenseDatum, 0x9, "业", false);
	BusinessLicenseOcr.AddOcrItem(cvRect(2154, 937, 258, 293), BusinessLicenseDatum, 0x6, "照", false);
	BusinessLicenseOcr.AddOcrItem(cvRect(1533, 1466, 230, 67), cvRect(1785, 1469, 519, 55), "注册号");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 1638, 409, 69), cvRect(1106, 1640, 691, 67), "名称");
	BusinessLicenseOcr.AddOcrItem(cvRect(624, 1758, 406, 68), cvRect(1103, 1753, 832, 71), "类型");
	BusinessLicenseOcr.AddOcrItem(cvRect(623, 1872, 409, 68), cvRect(1104, 1866, 1091, 70), "住所");
	BusinessLicenseOcr.AddOcrItem(cvRect(624, 1984, 408, 66), cvRect(1107, 1985, 200, 62), "法定代表人");
	BusinessLicenseOcr.AddOcrItem(cvRect(627, 2095, 406, 70), cvRect(1103, 2098, 282, 62), "注册资本");
	BusinessLicenseOcr.AddOcrItem(cvRect(625, 2212, 407, 70), cvRect(1105, 2210, 483, 67), "成立日期");
	BusinessLicenseOcr.AddOcrItem(cvRect(625, 2330, 408, 72), cvRect(1105, 2325, 741, 68), "营业期限");
	BusinessLicenseOcr.AddOcrItem(cvRect(624, 2448, 406, 70), cvRect(1106, 2443, 1644, 300), "经营范围",
		true, tesseract::PSM_SINGLE_WORD, tesseract::PSM_SINGLE_BLOCK);
	

	return RESULT_OK;
}

RESULT TechOcrClass::DoBusinessLicense(Pix* image, char* &output) {
	OnceOcrClass* once = new OnceOcrClass();

	BusinessLicenseOcr.GenerateTarget(image, once);
	once->OutputOcr(output);
	delete once;
	return RESULT_OK;
}


char argv_prog[] = "c:\\tesseract.exe";
char argv_in[] = "Z:\\solution\\files\\cl.jpg";
char argv_out[] = "Z:\\solution\\files\\cl.txt";
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




/**********************************************************************
*  main()
*
**********************************************************************/
int main_ttt(int argc, char **argv) {
	const char * image = argv_in;
	const char * outputbase = argv_out;

	tesseract::TessBaseAPI api;

//	api.SetOutputName(outputbase);
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
//	tesseract::TessResultRenderer* renderer = new tesseract::TessTextRenderer(outputbase);

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
	char *text;
	char *gbtext;
	long gsize;

	box = boxa->box;
	for (int i = 0; i < pixa->n; i++) {

		rect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
		double xrate = ((double)(*box)->w) / pixs->w;
		double yrate = ((double)(*box)->h) / pixs->h;
		if (0.05 < xrate && xrate < 0.15 && 0.05 < yrate && yrate < 0.15) {
			//1286, 935, 272, 284, 1709, 928, 282, 305, 2154, 937, 258, 293,
			//1059, 1037, 382, 402, 1653, 1029, 396, 432, 2252, 1043, 391, 417,
		}
		cvRectangleR(src, rect, CV_RGB(221, 134, 212), 3);

		if ((*box)->x > 529 && (*box)->y > 1437 && (*box)->x + (*box)->w < 2817 && (*box)->y + (*box)->h < 2785)
		{
			std::cout << (*box)->x << ", " << (*box)->y << ", " << (*box)->w << ", " << (*box)->h << ", "; // std::endl;
			api.SetRectangle((*box)->x - 5, (*box)->y - 5, (*box)->w + 10, (*box)->h + 10);
			api.Recognize(NULL);
			text = api.GetUTF8Text();
			UTF8toGB2312(text, (long)strlen(text), gbtext, gsize);
			std::cout << gbtext;// << std::endl;
			delete[] text;
			delete[] gbtext;
		}

		nowrectsize = (*box)->w * (*box)->h;
		if (nowrectsize > maxrectsize) {
			maxrectsize = nowrectsize;
			maxrect = (*box);
		}
		box++;
	}

	ShowImage(src, "src");
	cvWaitKey(0);
	return 0;


//	Boxa* boxa = api.GetRegions(&pixa);

	api.SetRectangle(maxrect->x - maxrect->w / 10, maxrect->y - maxrect->h / 10, 
		maxrect->w + maxrect->w / 5, maxrect->h + maxrect->h /5);
	rect = cvRect(maxrect->x - maxrect->w / 10, maxrect->y - maxrect->h / 10,
		maxrect->w + maxrect->w / 5, maxrect->h + maxrect->h / 5);
//	cvRectangleR(src, rect, CV_RGB(55, 234, 112), 6);

	//PageSegMode mode;
	api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
//	api.Recognize(NULL);



	box = boxa->box;
	int starty = maxrect->y + maxrect->h, endy;
	starty =  890;
	endy =  1700;
	for (int i = 0; i < pixa->n; i++) {
		if ((*box)->y > starty && (*box)->y < endy) {
			rect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
			cvRectangleR(src, rect, CV_RGB(221, 134, 212), 3);
			api.SetRectangle((*box)->x, (*box)->y, (*box)->w, (*box)->h);
			api.Recognize(NULL);

			std::cout << (*box)->x << ", " << (*box)->y << ", " << (*box)->w << ", " << (*box)->h << ", ";//std::endl;

			text = api.GetUTF8Text();
			UTF8toGB2312(text, (long)strlen(text), gbtext, gsize);
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
//	delete renderer;
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

TechOcrClass MainClass;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MainClass.InitBusinessLicense();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
};

char* WINAPI TechOcr(char* imgtype, char* filename)
{
// 	main_ttt(5, NULL);
// 	return NULL;

	char* output;
	Pix* pixs = pixRead(filename);
	if (!pixs) {
		output = new char[MAX_CHAR];
		output[0] = 0;
	}
	else {
		MainClass.DoBusinessLicense(pixs, output);
		pixDestroy(&pixs);
	}

	return output;
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
   delete[] checked;

   jstring returnstr = env->NewStringUTF(checked);
   return returnstr;
};

