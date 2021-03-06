#include "ImageCommon.h"
#include "TechOcrDll.h"
#include "PocTest.h"
#include "ImageRetinex.h"

// useful url
// http://www.win.tue.nl/~aeb/linux/ocr/tesseract.html
// https://github.com/tesseract-ocr/tesseract/blob/master/api/baseapi.h
// http://www.aiuxian.com/article/p-1968253.html



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 ) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetGlobalStorage();
		pocCreateBusinessLicense();
		pocCreatePersonCard();
		break;
// 		MainClass.InitBusinessLicense();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
// 		pocSystemExit();
		break;
	}
	return TRUE;
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
   jstring returnstr = env->NewStringUTF(checked);
   delete[] checked;

   return returnstr;
};

char* WINAPI OcrTest(char *filename) {
	RESULT result;

	pocRetinex(filename);
	return NULL;

// 	IplImage *src = cvLoadImage("z:\\solution\\files\\b\\b8.jpg");
// 	pocFilter(src);

// 	IplImage *img = pocImageProcess(filename);
// 	pocContours(img);

// 	pcoImagePreprocess("Z:\\solution\\files\\pp.jpg");
// 	pcoImagePreprocess("Z:\\solution\\files\\test6.jpg");


// 	pocTesseractInit();

//	pocPointToLineDist();

//	pocPixCreateFromIplImage("c:\\a.jpg", "c:\\b.jpg");
// 	pocRotateImage("Z:\\solution\\files\\a.jpg", 30, true);
//	pocFindHoughLines("Z:\\solution\\files\\c3.jpg");			//ok

//	pocFindHoughLines("Z:\\solution\\files\\c2.jpg");			//ok without contour NOT OK
// 	pocFindHoughLines("Z:\\solution\\files\\test6.jpg");
//  pocFindHoughLines("Z:\\solution\\files\\cl.jpg");			//ok
//  	pocFindHoughLines("Z:\\solution\\files\\cc\\c3.jpg");			//ok

// 	pocApproximateLine("Z:\\solution\\files\\bs1.jpg");	// too more lines, test later
// 	pocApproximateLine("Z:\\solution\\files\\cs.jpg");	// too more lines, test later

// 	pocApproximateLine("Z:\\solution\\files\\cl.jpg");	// too more lines, test later
// 	pocApproximateLine("Z:\\solution\\files\\cc\\c3.jpg");
// 	pocApproximateLine("Z:\\solution\\files\\test6.jpg");
//  	pocApproximateLine("Z:\\solution\\files\\bl.jpg");	// too more lines, test later
// 	pocApproximateLine("Z:\\solution\\files\\c3.jpg");

// 	pcoPreprocess("Z:\\solution\\files\\cc\\c3.jpg");
// 	// 	//  	pcoPreprocess("Z:\\solution\\files\\c3.jpg");
// 	// 	// 	pcoPreprocess("Z:\\solution\\files\\cl.jpg");	// too more lines, test later
// 	// 	// 	pcoPreprocess("Z:\\solution\\files\\test6.jpg");
// 	// 	// 	pcoPreprocess("Z:\\solution\\files\\clr.jpg");	// for rotate
// 	// 	// 	pcoPreprocess("Z:\\solution\\files\\bl.jpg");	// too more lines, test later

// 	pocMemoryLeak("Z:\\solution\\files\\cl.jpg");
// 	pocMemoryLeak("Z:\\solution\\files\\small.jpg");


// 		pocApproximateLine("Z:\\solution\\files\\c2.jpg");			//ok without contour NOT OK


// 	pocFindHoughCircles("Z:\\solution\\files\\cl.jpg");		// circle is NOT OK

// 	pocFindMaxRect("Z:\\solution\\files\\cc\\p1.jpg");
// 	pocShowImage("Z:\\solution\\files\\cl.jpg");
//	pocShowImage("c:\\a.jpg");
// 	pocShowImage("Z:\\solution\\files\\c2.jpg");
// 	pocShowImage("Z:\\solution\\files\\b10d.jpg");
//	pocShowImage("Z:\\solution\\files\\p1.jpg");
// 	pocShowImage("Z:\\solution\\files\\bl.jpg");

//	pocFindFeatureWords("Z:\\solution\\files\\cl.jpg");

// 	pocFindFeatureWordsInClass("Z:\\solution\\files\\cl.jpg");	// OK
//  pocFindFeatureWordsInClass("Z:\\solution\\files\\c2.jpg");
//	pocFindFeatureWordsInClass("Z:\\solution\\files\\test6.jpg");

// 	pocIsIntersect();		OK

	return 0;

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

