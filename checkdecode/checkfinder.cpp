// checkdecode.cpp : 
//

#include "checkfinder.h"

#define DEFAULT_NAME "ImageSrc"

#define ENLARGE          ((float)0.3)

#define RECTRATE         ((float)2.0)
#define RATELEVEL1       ((float)1.6)
#define RATELEVEL2       ((float)1.1)
#define RATELEVEL3       ((float)0.07)

int seq_cmp_by_clockwise(const void *_a, const void *_b, void *_cog)
{
  CvPoint2D32f* a = (CvPoint2D32f*)_a;
  CvPoint2D32f* b = (CvPoint2D32f*)_b;
  CvPoint2D32f *cog = (CvPoint2D32f *)_cog;

  float aa=cvFastArctan((a->y)-cog->y,cog->x-(a->x));
  float ba=cvFastArctan((b->y)-cog->y,cog->x-(b->x));

  return(aa < ba ? 1 : aa > ba ? -1 : 0);
};

RESULT SortRect(CvPoint2D32f* point, int number)
{
  CvMemStorage *store_s = cvCreateMemStorage(0);
  CvSeq *seq = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvSeq), sizeof(CvPoint2D32f), store_s);
  float x = 0, y = 0;
  CvPoint2D32f center;
  CvPoint2D32f *start = point;

  for (int i = 0; i < number; i++)
  {
    x += start->x;
    y += start->y;
    cvSeqPush(seq, start++);
  }
  center.x = x / number;
  center.y = y / number;
  cvSeqSort(seq, seq_cmp_by_clockwise, &center);

  start = point;
  for (int i = 0; i < number; i++)
  {
    CvPoint2D32f *p = (CvPoint2D32f *)cvGetSeqElem(seq, i);
    start->x = p->x;
    start->y = p->y;
    start ++;
  }
  cvRelease((void **)&seq);
  cvReleaseMemStorage(&store_s);
  return RESULT_OK;
};

void TranslatePoint(CvPoint2D32f* conner, CvPoint2D32f& result, float a, float b)
{
  float x1, y1, x2, y2;
  x1 = ( (conner+3)->x - conner->x ) * b + conner->x;
  y1 = ( (conner+3)->y - conner->y ) * b + conner->y;
  x2 = ( (conner+2)->x - (conner+1)->x ) * b + (conner+1)->x;
  y2 = ( (conner+2)->y - (conner+1)->y ) * b + (conner+1)->y;
  result.x = ( x2 - x1 ) * a + x1;
  result.y = ( y2 - y1 ) * a + y1;
  return;
};

void TranslateRect(CvPoint2D32f* conner, CvRect& result, CvRect source, CvRect paper)
{
  CvPoint2D32f topleft, topright, bottomleft, bottomright;
  float x1 = (float)source.x / paper.width;
  float x2 = (float)(source.x + source.width) / paper.width;
  float y1 = (float)source.y / paper.height;
  float y2 = (float)(source.y + source.height) / paper.height;
  TranslatePoint(conner, topleft, x1, y1); 
  TranslatePoint(conner, topright, x2, y1);
  TranslatePoint(conner, bottomleft, x1, y2);
  TranslatePoint(conner, bottomright, x2, y2);
  result.x = (int)MIN(topleft.x, bottomleft.x);
  result.y = (int)MIN(topleft.y, topright.y);
  result.width = (int)MAX(topright.x, bottomright.x) - result.x;
  result.height = (int)MAX(bottomleft.y, bottomright.y) - result.y;

  return;
};

IplImage* rotateImage(IplImage* src, int angle, bool clockwise)  
{
    angle = angle % 360;
    int anglecalc = abs(angle) % 180;  
    if (anglecalc > 90)  
    {  
        anglecalc = 90 - (anglecalc % 90);  
    }  
    IplImage* dst = NULL;  
    int width =  
        (int)(sin(anglecalc * CV_PI / 180.0) * src->height) +  
        (int)(cos(anglecalc * CV_PI / 180.0 ) * src->width) + 1;  
    int height =  
        (int)(cos(anglecalc * CV_PI / 180.0) * src->height) +  
        (int)(sin(anglecalc * CV_PI / 180.0 ) * src->width) + 1;  
    int tempLength = (int)sqrt((double)src->width * src->width + src->height * src->height) + 10;  
    int tempX = (tempLength + 1) / 2 - src->width / 2;  
    int tempY = (tempLength + 1) / 2 - src->height / 2;  
    int flag = -1;  
  
    dst = cvCreateImage(cvSize(width, height), src->depth, src->nChannels);  
    cvZero(dst);  
    IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), src->depth, src->nChannels);  
    cvZero(temp);  
  
    cvSetImageROI(temp, cvRect(tempX, tempY, src->width, src->height));  
    cvCopy(src, temp, NULL);  
    cvResetImageROI(temp);  
  
    if (clockwise)  
        flag = 1;  
  
    float m[6];  
    int w = temp->width;  
    int h = temp->height;  
    m[0] = (float) cos(flag * angle * CV_PI / 180.);  
    m[1] = (float) sin(flag * angle * CV_PI / 180.);  
    m[3] = -m[1];  
    m[4] = m[0];  
    m[2] = w * 0.5f;  
    m[5] = h * 0.5f;  

    CvMat M = cvMat(2, 3, CV_32F, m);  
    cvGetQuadrangleSubPix(temp, dst, &M);  
    cvReleaseImage(&temp);  
    return dst;  
};

CheckDecode::CheckDecode()
{
  ImageSrc = 0;
  Image1c = 0;
  NowMask = 0;
  MaxContour = 0;

  QrResult[0] = 0;
};

CheckDecode::~CheckDecode()
{
  if (ImageSrc) 
    cvReleaseImage(&ImageSrc);
  if (Image1c)
    cvReleaseImage(&Image1c);

  if (MaxContour)
  //  cvClearSeq(MaxContour);
    MaxContour = NULL;

  cvDestroyAllWindows();
};

RESULT CheckDecode::LoadImage(char* filename)
{
  HWND hWnd = ::GetDesktopWindow(); 
  RECT     rect; 
  CvSize   resize;
//  IplImage *dst;
  ::GetWindowRect(hWnd,&rect); 

  if (ImageSrc) 
    cvReleaseImage(&ImageSrc);
  ImageSrc = 0;

  if (filename)
    ImageSrc = cvLoadImage(filename, 1);

  if (ImageSrc)
  {
    if (ImageSrc->height < ImageSrc->width)
    {
      ImageSrc = rotateImage(ImageSrc, 90, true);
    }

    float scale = MAX((float)ImageSrc->width / (rect.right - rect.left - 50), 
                      (float)ImageSrc->height / (rect.bottom - rect.top - 100));
    if (scale < (float)1) scale = (float)1;
    resize.width = (int)(ImageSrc->width / scale);	
    resize.height = (int)(ImageSrc->height / scale);

/*
    dst = cvCreateImage( resize, ImageSrc->depth, ImageSrc->nChannels);
    cvResize(ImageSrc, dst, CV_INTER_LINEAR);
    cvReleaseImage(&ImageSrc);
    ImageSrc = dst;
*/
  }
  return ImageSrc ? RESULT_OK : RESULT_ERR;
};

RESULT CheckDecode::LoadImage(void* image, long size)
{
  return RESULT_OK;
};

RESULT CheckDecode::SetPaper(int width, int height)
{
  PaperSize.x = 0;
  PaperSize.y = 0;
  PaperSize.width = width;
  PaperSize.height = height;
  return RESULT_OK;
}

RESULT CheckDecode::LoadMask(int* xplace, long xsize, int* yplace, int ysize)
{
  int x, y;
  int nowx, nextx;
  int nowy, nexty, nowheight;
  pShareInfo info;

  NowMask = 0;
  for (y = 0; y < ysize; y++)
  {
    nowy = *(yplace + y);
    nexty = *(yplace + y + 1);
    if (nowy == 0 || nexty == 0)
      continue;
    nowheight = nexty - nowy;
    for (x = 0; x < xsize; x++)
    {
      nowx = *(xplace + x);
      nextx = *(xplace + x + 1);
      if ( nowx == 0 || nextx == 0)
        continue;
      info = &Info[NowMask];
      info->MaskRect.x = nowx;
      info->MaskRect.y = nowy;
      info->MaskRect.width = nextx - nowx;
      info->MaskRect.height = nowheight;

      CvRect checkroi;
      TranslateRect(MaxRect, checkroi, info->MaskRect, PaperSize);
      float enlarge = ENLARGE * MAX(checkroi.width, checkroi.height);
      info->RoiRect.x = checkroi.x - (int)enlarge;
      info->RoiRect.y = checkroi.y - (int)enlarge;
      info->RoiRect.width = checkroi.width + (int)enlarge * 2;
      info->RoiRect.height = checkroi.height + (int)enlarge * 2;

      info->DisplayPoint.x = info->RoiRect.x + info->RoiRect.width / 2 - 20;
      info->DisplayPoint.y = info->RoiRect.y + info->RoiRect.height / 2 + 10;

      NowMask ++;
      if (NowMask >= MAX_RECT) 
      {
        NowMask = 0;
        return RESULT_ERR;
      }
    }
  }
  return RESULT_OK;
}

RESULT CheckDecode::LoadMask(CvRect* clist, long size)
{

  return RESULT_OK;
};

// pattern is better for not gaussion, while check is better for with gaussion
RESULT CheckDecode::PrepareImage(bool gaussian)
{
  if (Image1c)
    cvReleaseImage(&Image1c);
  Image1c = cvCreateImage(cvSize(ImageSrc->width, ImageSrc->height), IPL_DEPTH_8U, 1);

  // to gray
  if(ImageSrc->nChannels > 1)
    cvCvtColor(ImageSrc, Image1c, CV_BGR2GRAY);
  else
    cvCopy(ImageSrc, Image1c);

  if (gaussian)
    cvSmooth(Image1c, Image1c, CV_GAUSSIAN, 3, 3);
  cvAdaptiveThreshold(Image1c, Image1c, 128, CV_ADAPTIVE_THRESH_MEAN_C,
    CV_THRESH_BINARY_INV, DEFAULT_ADAPTIVE_TH_SIZE, DEFAULT_ADAPTIVE_TH_DELTA);

  return RESULT_OK;
};

RESULT CheckDecode::FindPattern(void)
{
  return RESULT_OK;
};


RESULT CheckDecode::FIndMaxRect(void)
{
  CvMemStorage *store_c;
  CvMemStorage *store_r;

  store_c=cvCreateMemStorage(0);
  store_r=cvCreateMemStorage(0);

  CvSeq *contours = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),sizeof(CvPoint), store_c);
  CvSeq *nowcontour;
  CvSeq *points = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),sizeof(CvPoint), store_r);

  CvBox2D maxbox;
  IplImage *tmp1c = cvCloneImage(Image1c);


  cvFindContours(tmp1c, store_c, &contours, sizeof(CvContour),
    CV_RETR_TREE,CV_CHAIN_APPROX_NONE,cvPoint(0,0));

 for(nowcontour = contours; nowcontour; nowcontour = nowcontour->h_next)
  {
    CvRect feret = cvBoundingRect(nowcontour);
    CvPoint topleft, topright, bottomleft, bottomright;

    if (feret.width > (tmp1c->width / 3) && feret.height > (tmp1c->height / 3))
    {
      if (MaxContour) return RESULT_ERR;
      topleft.x = feret.x;
      topleft.y = feret.y;
      topright.x = feret.x;
      topright.y = feret.y + feret.width;
      bottomleft.x = feret.x + feret.width;
      bottomleft.y = feret.y;
      bottomright.x = feret.x + feret.width;
      bottomright.y = feret.y + feret.height;
      cvSeqPush(points,&topleft);
      cvSeqPush(points,&topright);
      cvSeqPush(points,&bottomleft);
      cvSeqPush(points,&bottomright);
      MaxContour = nowcontour;


      IplImage * dst = cvCreateImage(cvSize(ImageSrc->width, ImageSrc->height), ImageSrc->depth, ImageSrc->nChannels);  


  CvMemStorage* storage = cvCreateMemStorage(0);  
        CvSeq* lines = 0;   
   double rho=1;  
   double theta=CV_PI/180;  
   double threshold=30;  
   double min_length=200;//CV_HOUGH_PROBABILISTIC  
   double sepration_connection=4;//CV_HOUGH_PROBABILISTIC  
  
   //binary image is needed.  
    lines = cvHoughLines2(  
      Image1c,   
      storage,   
      CV_HOUGH_PROBABILISTIC,   
      rho,  
      theta,   
      threshold,  
      min_length,   
      sepration_connection);  
  
   //draw lines found by cvHoughLines2  
    for( int i = 0; i < lines->total; i++ )  
    {  
       CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);  
       cvLine(dst, line[0], line[1], CV_RGB(255,0,0),3, 8 );//cannyImgColor  
       cvCircle(dst, line[0], 3* (i%5) + 4, CV_RGB(0,255,255), 2);
       cvCircle(dst, line[1], 3* (i%5) + 4, CV_RGB(255,0,255), 2);

    } 
    ShowImage(dst, "dst");

      cvReleaseMemStorage(&storage);




          CvSeq*     hull = cvConvexHull2(nowcontour,0,CV_CLOCKWISE,1);  
       for(int i = 0; i < hull -> total; i++) 
        {  
            CvPoint *pt = (CvPoint*) cvGetSeqElem(hull, i);  
            cvCircle(ImageSrc, *pt, 20, CV_RGB(255,255,255), 10);

       }
  //    cvDrawContours(ImageSrc, nowcontour, CV_RGB(0,0,255), CV_RGB(0,255,0), 0, 2, 8);
      cvRectangle(ImageSrc, topleft, bottomright, CV_RGB(221,134,212));
    }
//    else
//    {
      cvClearSeq(nowcontour);
//    }
  }

  maxbox = cvMinAreaRect2(points);
  cvBoxPoints(maxbox, MaxRect);
  DisplaySize.x = DisplaySize.y = (int)maxbox.size.height / 40;   // for display size

  SortRect(MaxRect, 4);

  cvReleaseImage(&tmp1c);
  cvRelease((void **)&contours);
  cvRelease((void **)&points);

  cvReleaseMemStorage(&store_c);
  cvReleaseMemStorage(&store_r);

  return RESULT_OK;
};

RESULT CheckDecode::GetLevelInfo(CvSeq* contour, pShareInfo info, bool doself, bool doloop)
{
  if (doself) 
  {
    info->area = fabs( cvContourArea( contour ));
    info->length = fabs( cvArcLength(contour));
  }

  ShareInfo vinfo;
  CvSeq* now = contour;
  info->l2count = info->l3count = 0;
  info->l2area = info->l2length = info->l3area = info->l3length = 0;
  double area = 0, length = 0, rectrate;

  if (now->v_next != NULL)
  {
    now = now->v_next;
    do
    {
      length = fabs( cvArcLength( now ));
      area = fabs( cvContourArea( now ));

      rectrate = length * length / 16 / area;
      if (length * 10 * (1 + ENLARGE) > info->length && (doloop == true || rectrate > RECTRATE))
      {
        info->l2count ++;
        info->l2area += area;
        info->l2length += length;
        if (doloop)
        {
          vinfo.length = length;
          vinfo.area = area;
          GetLevelInfo(now, &vinfo, false, false);
          info->l3count += vinfo.l2count;
          info->l3area += vinfo.l2area;
          info->l3length += vinfo.l2length;
        }
      }
      now = now->h_next;
    }
    while (now);
  }

  return RESULT_OK;
}

RESULT CheckDecode::FindCheck(int order)
{
  pShareInfo info = &Info[order];
  if (info->MaskRect.height == 0 || info->MaskRect.width == 0)
  {
    return RESULT_ERR;
  }

  CvMemStorage *store_c;
  store_c=cvCreateMemStorage(0);

  CvSeq *contours = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),sizeof(CvPoint), store_c);
  CvSeq *nowcontour;
  IplImage *tmp1c = cvCloneImage(Image1c);
  RESULT result;


  cvSetImageROI(tmp1c, info->RoiRect);

//   ShowImage(tmp1c);
  cvFindContours(tmp1c, store_c, &contours, sizeof(CvContour),
    CV_RETR_TREE,CV_CHAIN_APPROX_NONE,cvPoint(0,0));
  cvResetImageROI(tmp1c);

  double largearea = 0, area;
  CvSeq* largecoutour = NULL;
  for(nowcontour = contours; nowcontour; nowcontour = nowcontour->h_next)
  {
    area = fabs( cvContourArea( nowcontour ));
    if ( area > largearea )
    {
      largearea = area;
      largecoutour = nowcontour;
    }
  }
  float roiarea = info->RoiRect.width * info->RoiRect.height / (1 + ENLARGE) / (1 + ENLARGE);
  if (largearea < roiarea / 2 ) 
  {
    return RESULT_ERR + 100; // NOT FOUND TOP
  }
  if (largecoutour->v_next == NULL) 
  {
    return RESULT_ERR + 101;                    // NOT FOUND SECOND
  }

  info->area = largearea;
  info->length = fabs(cvArcLength(largecoutour));
  GetLevelInfo(largecoutour, info, false, true);

  result = DetectCheck(order);

  for(nowcontour = contours; nowcontour; nowcontour = nowcontour->h_next)
  {
    cvClearSeq(nowcontour);
  }

  cvReleaseImage(&tmp1c);
  cvRelease((void **)&contours);
  cvReleaseMemStorage(&store_c);

  return result;
};

RESULT CheckDecode::DetectCheck(int order)
{
  pShareInfo info = &Info[order];
  float framelength = (float)(info->RoiRect.width + info->RoiRect.height) * 2;
  float rectlength = framelength / (1 + 2 * ENLARGE);
  double ratelevel1, ratelevel2, ratelevel3;
  ratelevel1 = info->length / framelength;
  ratelevel2 = info->l2length / rectlength;
  ratelevel3 = info->l3length / rectlength ;

  printf("i:%2d, l1a:%5.0f, l1l:%4.0f, l2c:%1d, l2a:%5.0f, l2l:%4.0f, l3c:%1d, l3a:%3.0f, l3l:%2.0f\r\n",
    order, info->area, info->length, info->l2count, info->l2area, info->l2length, info->l3count, info->l3area, info->l3length);
  printf("rl:%6.0f, r1:%6.4f, r2:%6.4f, r3:%6.4f,   rt:%6.4f\r\n", rectlength, ratelevel1, ratelevel2, ratelevel3, ratelevel2/ratelevel1);

  if ((ratelevel1 < RATELEVEL1 && ratelevel2 > RATELEVEL2) || ratelevel3 > RATELEVEL3 )
  {
    Info[order].CheckResult = 'Y';
     printf ("    is mark !!! for V1.0\r\n");
//     printf("i:%2d, l1a:%5.0f, l1l:%4.0f, l2c:%1d, l2a:%5.0f, l2l:%4.0f, l3c:%1d, l3a:%3.0f, l3l:%2.0f\r\n",
//       order, info->area, info->length, info->l2count, info->l2area, info->l2length, info->l3count, info->l3area, info->l3length);
  }
  else
  {
    Info[order].CheckResult = ' ';
  }
  return RESULT_OK;
};

RESULT CheckDecode::DisplayResult(void)
{
  CvFont font, font2;  
  cvInitFont(&font, CV_FONT_HERSHEY_SCRIPT_COMPLEX , 1.0, 1.0, 0, 2, 2);
  cvInitFont(&font2, CV_FONT_HERSHEY_DUPLEX , 0.5, 0.5, 0, 1, 1);

  for (int i = 0; i < NowMask; i++)
  {
    if (Info[i].CheckResult == 'Y')
    cvPutText(ImageSrc, "Yes", Info[i].DisplayPoint, &font, CV_RGB(211,34,0));  

    if (i % 2 == 0)
    {
      CvPoint lt = cvPoint(Info[i].RoiRect.x, Info[i].RoiRect.y);
      CvPoint rb = cvPoint(Info[i].RoiRect.x + Info[i].RoiRect.width, Info[i].RoiRect.y + Info[i].RoiRect.height);
      cvRectangle(ImageSrc, lt, rb, CV_RGB(21,34,212));
    }
  }
  if (QrResult[0])
  {
    cvPutText(ImageSrc, (const char*)&QrResult[0], cvPoint(10,100), &font2, CV_RGB(150,34,232));  
  }

  ShowImage(ImageSrc);
  return RESULT_OK;
}


RESULT CheckDecode::ShowImage(IplImage* img, char* name)
{
  if (!img)
    img = ImageSrc;
  if (!name)
    name = DEFAULT_NAME;
  cvNamedWindow(name,1);


  HWND hWnd = ::GetDesktopWindow(); 
  RECT     rect; 
  CvSize   resize;
  IplImage *dst;
  ::GetWindowRect(hWnd,&rect); 
  float scale = MAX((float)img->width / (rect.right - rect.left - 50), (float)img->height / (rect.bottom - rect.top - 100));
  if (scale < (float)1) scale = (float)1;
  resize.width = (int)(img->width / scale);	
  resize.height = (int)(img->height / scale);

  dst = cvCreateImage( resize, img->depth, img->nChannels);
  cvResize(img, dst, CV_INTER_LINEAR);

  if (img)
    cvShowImage(name, dst);

  cvReleaseImage(&dst);
/*
  if (img)
    cvShowImage(name, img);
*/
  return RESULT_OK;
};


#ifdef READQR
RESULT CheckDecode::GetQrCode(void)
{
  QrDecoderHandle decoder=qr_decoder_open();
  short stat = qr_decoder_decode_image(decoder,ImageSrc);
  QrCodeHeader header;
  if(qr_decoder_get_header(decoder,&header))
  {
    qr_decoder_get_body(decoder, QrResult, header. byte_size+1);
  }
  qr_decoder_close(decoder);
  return RESULT_OK;
}
#endif READQR

// for word
/*int Xplace[] = {466, 544, 625, 705, 0};
int Yplace[] = {61, 172, 283, 392, 454, 516, 0, 575, 637, 698, 759, 817, 0};
int PaperWidth = 705;
int PaperHeight = 817;*/

int Xplace[] = {620-43, 723-43, 827-43, 932-43, 0};
int Yplace[] = {344-267, 422-267, 500-267, 642-267, 722-267, 799-267, 0, 880-267, 956-267, 1037-267, 1114-267, 1263-267, 0};
int PaperWidth = 932-43;
int PaperHeight = 1263-267;


RESULT CheckDecode::Process(char* filename, char* checkresult, bool rotate)
{
  RESULT result;
  char check[MAX_RECT];
  int nook = 0;

  result = SetPaper(PaperWidth, PaperHeight);
  result = LoadImage(filename);
  if (result != RESULT_OK) return result;

  if (rotate)
    ImageSrc = rotateImage(ImageSrc, 180, false);

  result = PrepareImage(true);
  result = FIndMaxRect();
  result = LoadMask(Xplace, sizeof(Xplace)/sizeof(int) - 1, Yplace, sizeof(Yplace) /sizeof(int)- 1);
  for (int i = 0; i < NowMask; i++)
  {
    result = FindCheck(i);
    if (result != RESULT_OK)
      nook ++;
  }
  GetResult(check);

  if (nook == 0)
  {
    strncpy(checkresult, check, MAX_RECT);
    return RESULT_OK;
  }
  else
  {
    *checkresult = 0;
    return RESULT_ERR;
  }

#ifdef READQR
  result = GetQrCode();
#endif READQR

  //ShowImage(ImageSrc);
};

RESULT CheckDecode::ProcessAndDisplay(char* filename, char* checkresult, bool rotate)
{
  RESULT result;
  result = Process(filename, checkresult, rotate);

//  if (result == RESULT_OK)
  {
    result = DisplayResult();
  }
  cvWaitKey(0);
  return result;
}

RESULT CheckDecode::GetResult(char* checked)
{
  for (int i = 0; i < NowMask; i++ )
  {
    if (Info[i].CheckResult == 'Y') *checked ++ = 'Y';
    else *checked ++ = ' ';
    if (i % (sizeof(Xplace)/sizeof(int) - 2) == 2)
    {
      *checked ++ = ',';
    }
  }
  *checked ++ = 0;
  return RESULT_OK;
}

RESULT CheckDecode::TestProcess(char* filename)
{
  RESULT result;

  result = SetPaper(PaperWidth, PaperHeight);
  result = LoadImage(filename);
  result = PrepareImage(true);
//  ShowImage(Image1c);

  if (result != RESULT_OK) return result;

//  ImageSrc = rotateImage(ImageSrc, 180, false);

  ShowImage(ImageSrc);
  return 0;
}

