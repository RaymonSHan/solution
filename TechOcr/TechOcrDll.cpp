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

CvRect TransformRect(const CvRect rect, const CvMat *matrix, int enx, int eny) {
	CvPoint topleft = TransformPoint(cvPoint(rect.x - enx, rect.y - eny), matrix);
	CvPoint topright = TransformPoint(cvPoint(rect.x + rect.width + enx * 2, rect.y - eny), matrix);
	CvPoint bottomleft = TransformPoint(cvPoint(rect.x - enx, rect.y + rect.height + eny * 2), matrix);
	CvPoint bottomright = TransformPoint(cvPoint(rect.x + rect.width + enx * 2, rect.y + rect.height + eny * 2), matrix);
	int minx = MIN(topleft.x, bottomleft.x);
	int miny = MIN(topleft.y, topright.y);
	int maxx = MAX(topright.x, bottomright.x);
	int maxy = MAX(bottomleft.y, bottomright.y);
	return cvRect(minx, miny, maxx - minx, maxy - miny);
}

void AddToJson(std::string &str, char *chars) {
	int i;
	char now;

	for (i = 0; i < strlen(chars); i++) {
		now = *(chars + i);
		if (now > 0 && now < 0x20)
			continue;
		if (now == '\'' && now == '\\') {
			str += '\\';
		}
		str += now;
	}
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

RESULT OnceOcrClass::InitOnceOcr(Pix *image, char *filename) {
	api = new tesseract::TessBaseAPI;

	int rc = api->Init(NULL, DEFAULT_LAN, tesseract::OEM_DEFAULT);
	if (rc)
		return RESULT_ERR;
	targetImage = image;
	fileName = filename;
	api->SetImage(image);
	boxW = api->GetWords(&pixW);
	warpMat = cvCreateMat(3, 3, CV_64F);

	return RESULT_OK;
}
RESULT OnceOcrClass::ExitOnceOcr() {
	cvReleaseMat(&warpMat);
	boxaDestroy(&boxW);
	pixaDestroy(&pixW);
	api->End();
	delete api;
	return RESULT_OK;
}

#define  OUTPUTRECT(r, s)	std::cout << r->x << ", " << r->y << ", " << r->width << ", " << r->height << ", " \
	<< s << std::endl;

RESULT BoxInRect(Box *box, CvRect *rect) {
	if (box->x + box->w > rect->x  + MIN_WORD_SIZE &&
		box->x < rect->x + rect->width - MIN_WORD_SIZE &&			// attention the condition
		box->y > rect->y &&
		box->y + box->h < rect->y + rect->height) {
		return RESULT_OK;
	}
	else {
		return RESULT_ERR;
	}
}

CvRect DetectWord(Pixa *pixa, CvRect *rect) {
	int i;
	int minx = rect->x + rect->width;
	int miny = rect->y + rect->height;
	int maxx = rect->x;
	int maxy = rect->y;
	Box **box = pixa->boxa->box;
	for (i = 0; i < pixa->n; i++) {
		if ((*box)->w > MIN_WORD_SIZE && (*box)->h > MIN_WORD_SIZE &&
			BoxInRect(*box, rect) == RESULT_OK) {
			minx = MIN(minx, (*box)->x);
			miny = MIN(miny, (*box)->y);
			maxx = MAX(maxx, ((*box)->x + (*box)->w));
			maxy = MAX(maxy, ((*box)->y + (*box)->h));
		}
		box++;
	}
	minx = MAX(rect->x, minx);
	miny = MAX(rect->y, miny);
	maxx = MIN((rect->x + rect->width), maxx);
	maxy = MIN((rect->y + rect->height), maxy);
	if (maxx < minx || maxy < miny) {
		return *rect;
	}
	else {
		return cvRect(minx, miny, MAX(maxx - minx, 0), MAX(maxy - miny, 0));
	}
}

// RESULT OnceOcrClass::TranWordInRect(Pixa *pixa, CvRect *rect, char *&str) {
// 	int i;
// 	char *wordstr;
// 	CvRect wordrect;
// 	Box **box = pixa->boxa->box;
// 	for (i = 0; i < pixa->n; i++) {
// 		if (BoxInRect(*box, rect) == RESULT_OK) {
// 			wordrect = cvRect((*box)->x - 2, (*box)->y - 2, (*box)->w + 4, (*box)->h + 4);
// 			TranAllInRect(&wordrect, tesseract::PSM_SINGLE_WORD, wordstr);
// 			delete[] wordstr;
// 			// should assign str at last
// 		}
// 		box++;
// 	}
// 	return RESULT_OK;
// }

RESULT OnceOcrClass::TranAllInRect(CvRect *rect, tesseract::PageSegMode mode, char *&str) {
	char *utfstr;
	long gsize;

	api->SetPageSegMode(mode);
	api->SetRectangle(rect->x, rect->y, rect->width, rect->height);

	api->Recognize(NULL);
#ifdef _DEBUG
	utfstr = api->GetUTF8Text();
	UTF8toGB2312(utfstr, (long)strlen(utfstr), str, gsize);
	delete[] utfstr;
	OUTPUTRECT(rect, str);
#else DEBUG
	str = api->GetUTF8Text();
#endif DEBUG
	return RESULT_OK;
}

RESULT OnceOcrClass::MapRect(MYINT num, SourceItem *source) {
	Box **box;
	SourceItem *nowitem;
	char *targetstr = NULL;

	CvPoint2D32f srcTri[4], desTri[4];
	char nowdatum = 0;
	int i, j;
	Box *maxbox;
	MYINT maxarea = 0;

// 	char* gbtext;
// 	long gsize;
	
	box = boxW->box;
	for (i = 0; i < pixW->n; i++) {		// for every box
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
//					UTF8toGB2312(targetstr, (long)strlen(targetstr), gbtext, gsize);
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
		if (targetstr) {
			delete[] targetstr;
			targetstr = NULL;
		}

		if (nowdatum == 0xf) {
			// have found 4 conner, to finish
			break;
		}
	}
	if (nowdatum != 0xf) {
		// error, not found conner datum
		std::cout << "NOT found datum" << VERSION << std::endl;
		return RESULT_ERR;
	}
	cvGetPerspectiveTransform(srcTri, desTri, warpMat);
	return RESULT_OK;
}

RESULT OnceOcrClass::DoOcr(MYINT num, SourceItem *source) {
	int i;
	SourceItem *nowitem;
	CvRect rect;

	for (i = 0; i < num; i++) {
		nowitem = source + i;
		if (!nowitem->isOutput) continue;
		if (nowitem->rectName.width != 0 && nowitem->rectName.height != 0) {
			targetItem[i].rectName = TransformRect(nowitem->rectName, warpMat, ENLAGRE_X, ENLARGE_Y);
			rect = DetectWord(pixW, &targetItem[i].rectName);
//			TranWordInRect(pixw, &targetItem[i].rectName, targetItem[i].strName);
			TranAllInRect(&rect, nowitem->modeName, targetItem[i].strName);
		//	targetItem[i].rectName = rect;

		}
		if (nowitem->rectValue.width != 0 && nowitem->rectValue.height != 0) {
			targetItem[i].rectValue = TransformRect(nowitem->rectValue, warpMat, ENLAGRE_X, ENLARGE_Y);
			rect = DetectWord(pixW, &targetItem[i].rectValue);
//			TranWordInRect(pixw, &targetItem[i].rectValue, targetItem[i].strValue);
			TranAllInRect(&rect, nowitem->modeValue, targetItem[i].strValue);
		//	targetItem[i].rectValue = rect;
		}
	}
	return RESULT_OK;
}

RESULT OnceOcrClass::ShowImage(MYINT num, char *name) {
	int i;
	Box **box;
	CvRect cvrect;
	CvSize   resize;
	IplImage *dst;
	RECT     rect;
	HWND hWnd = ::GetDesktopWindow();
	::GetWindowRect(hWnd, &rect);
	IplImage *img = cvLoadImage(fileName, 1);     // 1 for is colored

	cvNamedWindow(name, 1);

	box = boxW->box;
	for (i = 0; i < pixW->n; i++) {
		cvrect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
		cvRectangleR(img, cvrect, CV_RGB(221, 134, 212), 3);
		box++;
	}
	for (i = 0; i < num; i++) {
		cvRectangleR(img, (targetItem + i)->rectName, CV_RGB(33, 112, 111), 5);
		cvRectangleR(img, (targetItem + i)->rectValue, CV_RGB(33, 112, 111), 5);
	}

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
	cvReleaseImage(&img);
	return RESULT_OK;
}

RESULT OnceOcrClass::OutputOcr(MYINT num, SourceItem *source, char* &output) {		// should delete[] outside the function
	
	std::string outstring;
	int i, strlength;
	SourceItem *sourceitem;
	TargetItem *targetitem;
	bool first = true;

	outstring = "{\"version\":\"";
	outstring += VERSION;
	outstring += "\", \"result\":[";
	for (i = 0; i < num; i++) {
		sourceitem = source + i;
		targetitem = targetItem + i;
		if (sourceitem->isOutput) {
			if (targetitem->strName && targetitem->strValue) {
				if (first) {
					first = false;
				} 
				else {
					outstring += ",";
				}
				outstring += "{\"name\":\"";
#ifdef _DEBUG
				AddToJson(outstring, targetitem->strName);
#else _DEBUG
				AddToJson(outstring, sourceitem->shouldBe);
#endif _DEBUG
				outstring += "\",\"value\":\"";
				AddToJson(outstring, targetitem->strValue);
				outstring += "\"}";
			}
		}
		if (targetitem->strName)
			delete[] targetitem->strName;
		if (targetitem->strValue)
			delete[] targetitem->strValue;
	}
	outstring += "]}";

	strlength = outstring.length();
	output = new char [strlength + 10];
	strncpy(output, outstring.c_str(), strlength);
	output[strlength] = 0;
	return RESULT_OK;
}

RESULT ModelOcrClass::AddOcrItem(CvRect &name, CvRect &value, char* shouldbe, bool isoutput,
	tesseract::PageSegMode nmode, tesseract::PageSegMode vmode) {
	char* utf8;
	long usize;

	MYINT num = InterInc(&sourceNumber) - 1;
	SourceItem *now = &sourceItem[num];
	now->rectName = name;
	now->rectValue = value;
	now->modeName = nmode;
	now->modeValue = vmode;
	GB2312toUTF8(shouldbe, strlen(shouldbe), utf8, usize);
	strncpy_s(now->shouldBe, utf8, sizeof(SourceItem::shouldBe));
	now->isDatumFunc = NULL;
	now->isDatum = 0;
	now->isOutput = isoutput;
	delete []utf8;
	return RESULT_OK;
}

RESULT ModelOcrClass::AddOcrItem(CvRect &name, DetectDatum isdatum, char datum, char* shouldbe, bool isoutput,
	tesseract::PageSegMode nmode) {
	char* utf8;
	long usize;

	MYINT num = InterInc(&sourceNumber) - 1;
	SourceItem *now = &sourceItem[num];
	now->rectName = name;
	now->rectValue = RECT_ZERO;
	now->modeName = nmode;
	now->modeValue = nmode;
	GB2312toUTF8(shouldbe, strlen(shouldbe), utf8, usize);
	strncpy_s(now->shouldBe, utf8, sizeof(SourceItem::shouldBe));
	now->isDatumFunc = isdatum;
	now->isDatum = datum;
	now->isOutput = isoutput;
	delete[]utf8;
	return RESULT_OK;
}

int SHOULDDISPLAY = 0;

RESULT ModelOcrClass::GenerateTarget(Pix *image, OnceOcrClass *&once, char *filename, char *&output) {
	RESULT result;

	result = once->InitOnceOcr(image, filename);
	if (result != RESULT_OK)
		return RESULT_ERR;

	if (SHOULDDISPLAY) {
		result = once->ShowImage(sourceNumber, "base");
		if (result != RESULT_OK)
			return RESULT_ERR;
	}

	result = once->MapRect(sourceNumber, sourceItem);
	if (result != RESULT_OK) {
		if (SHOULDDISPLAY) {
			cvWaitKey();
		}
		return RESULT_ERR;
	}

	result = once->DoOcr(sourceNumber, sourceItem);
	if (result != RESULT_OK)
		return RESULT_ERR;

	if (SHOULDDISPLAY) {
		result = once->ShowImage(sourceNumber, "all");
		if (result != RESULT_OK)
			return RESULT_ERR;
	}

	result = once->OutputOcr(sourceNumber, sourceItem, output);
	if (result != RESULT_OK)
		return RESULT_ERR;

	result = once->ExitOnceOcr();
	if (result != RESULT_OK)
		return RESULT_ERR;

	if (SHOULDDISPLAY) {
		cvWaitKey();
	}

	return RESULT_OK;
}

RESULT TechOcrClass::InitBusinessLicense(void) {
	BusinessLicenseOcr.AddOcrItem(cvRect(1286, 935, 272, 284), BusinessLicenseDatum, 0xf, "业", false);

//	BusinessLicenseOcr.AddOcrItem(cvRect(1708-5, 924-5, 290+10, 307+10), BusinessLicenseDatum, 0x6, "执", false);
//	BusinessLicenseOcr.AddOcrItem(cvRect(1707, 924, 1993-1707, 1231-924), BusinessLicenseDatum, 0x6, "执", false);
//	BusinessLicenseOcr.AddOcrItem(cvRect(2154, 937, 258, 293), BusinessLicenseDatum, 0x6, "照", false);


	BusinessLicenseOcr.AddOcrItem(cvRect(1533, 1466, 230, 60), cvRect(1785, 1469, 800, 55), "注册号");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 1636-1, 420, 66), cvRect(1095, 1636-1, 1700, 66), "名称");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 1758-3, 420, 66), cvRect(1095, 1758-3, 1700, 66), "类型");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 1872-5, 420, 66), cvRect(1095, 1872-5, 1700, 66), "住所");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 1984-7, 420, 66), cvRect(1095, 1984-7, 1700, 66), "法定代表人");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 2095-9, 420, 66), cvRect(1095, 2095-9, 1700, 66), "注册资本");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 2212-11, 420, 66), cvRect(1095, 2212-11, 1700, 66), "成立日期");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 2330-13, 420, 66), cvRect(1095, 2330-13, 1700, 66), "营业期限");
	BusinessLicenseOcr.AddOcrItem(cvRect(622, 2448-15, 420, 66), cvRect(1095, 2440-15, 1700, 550), "经营范围",
		true, tesseract::PSM_SINGLE_WORD, tesseract::PSM_SINGLE_BLOCK);
	

	return RESULT_OK;
}

RESULT TechOcrClass::DoBusinessLicense(Pix* image, char* &output, char* filename) {
	OnceOcrClass* once = new OnceOcrClass();

	BusinessLicenseOcr.GenerateTarget(image, once, filename, output);
	delete once;
	return RESULT_OK;
}

TechOcrClass MainClass;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 ) {
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

char* WINAPI TechOcr(char* imgtype, char* filename) {
	char* output = NULL;
	Pix* pixs = pixRead(filename);
	if (!pixs) {
		output = new char[MAX_CHAR];
		output[0] = 0;
	}
	else {
		if (!strncmp(imgtype, "SHOULDDISPLAY", sizeof(SHOULDDISPLAY) - 1))
			SHOULDDISPLAY = 1;
		MainClass.DoBusinessLicense(pixs, output, filename);
		pixDestroy(&pixs);
	}
	if (output) {
		std::cout << output << std::endl;
	}
	return output;
};

JNIEXPORT jstring JNICALL Java_net_gbicc_xbrl_otc_util_ImageScanForC_TechOcr
  (JNIEnv * env, jclass jclassname, jstring imgtype, jstring filename) {
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

RESULT ShowImage(Pixa *pixa, char* name, char *fileName) {
	int i;
	Box **box;
	CvRect cvrect;
	CvSize   resize;
	IplImage *dst;
	RECT     rect;
	HWND hWnd = ::GetDesktopWindow();
	::GetWindowRect(hWnd, &rect);
	IplImage *img = cvLoadImage(fileName, 1);     // 1 for is colored

	cvNamedWindow(name, 1);

	if (pixa && pixa->boxa) {
		box = pixa->boxa->box;
		for (i = 0; i < pixa->n; i++) {
			cvrect = cvRect((*box)->x, (*box)->y, (*box)->w, (*box)->h);
			cvRectangleR(img, cvrect, CV_RGB(221, 134, 212), 3);
			box++;
		}
	}

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
	cvReleaseImage(&img);

	return RESULT_OK;
}

 RESULT CreatePixFromIplImage(Pix *&pix, IplImage *img) {
	 int x, y;
	 char *istart, *ix;
	 l_uint32 *pstart, *px;

	 if (!img || img->depth != 8 || (img->nChannels != 1 && img->nChannels != 3)) {
		 pix = NULL;
		 return RESULT_PARA_ERROR;
	 }
	 pix = pixCreate(img->width, img->height, img->nChannels == 1 ? 8 : 32);
	 if (!pix) {
		 return RESULT_ERR;
	 }

	 istart = img->imageData;
	 pstart = pix->data;
	 if (img->nChannels == 1) {
		 for (y = 0; y < img->height; y++) {
			 ix = istart;
			 px = pstart;
			 for (x = 0; x < (img->width + 3)/4; x++) {
				 *px++ = (*ix << 24) + (*(ix + 1) << 16) + (*(ix + 2) << 8) + *(ix + 3);
				 ix += 4;
			 }
			 istart += img->widthStep;
			 pstart += pix->wpl;
		 }
	 }
	 if (img->nChannels == 3) {
		 for (y = 0; y < img->height; y++) {
			 ix = istart;
			 px = pstart;
			 for (x = 0; x < img->width; x++) {
				 *px++ = (*(l_uint32 *)ix) << 8;
				 ix += 3;
			 }
			 istart += img->widthStep;
			 pstart += pix->wpl;
		 }
	 }
 }

char* WINAPI OcrTest(char *filename) {
	IplImage *img = cvLoadImage("c:\\a.jpg", 1);     // 1 for is colored
	Pix* pixs = pixRead("c:\\a.jpg");
	Pix* pixt;

	CreatePixFromIplImage(pixt, img);
	pixWriteAutoFormat("c:\\c.jpg", pixt);
	cvReleaseImage(&img);
	pixDestroy(&pixs);
	pixDestroy(&pixt);

	return NULL;

// 	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI;
// 	Pix* image = pixRead(filename);
// 	Pixa *pixw;
// 	Boxa *boxw;
// 	int rc = api->Init(NULL, DEFAULT_LAN, tesseract::OEM_DEFAULT);
// 	if (rc)
// 		return NULL;
// 	api->SetImage(image);
// 	boxw = api->GetWords(&pixw);
// 
// 	ShowImage(pixw, "test", filename);
// 	cvWaitKey();
// 
// 	boxaDestroy(&boxw);
// 	pixaDestroy(&pixw);
// 	api->End();
// 	delete api;
// 	return 0;
}