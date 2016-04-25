#include "checkfinder.h"
#include "checkmore.h"

#define PI                (3.1415926)
#define IS_LINE_RATE      ((double)155)
#define SAME_POINT_RATE   50
#define EDGE_RATE         50
#define ROI_ENLARGE       20

void NotUse_OnlyForRead(void)
{
  CvMemStorage *store = cvCreateMemStorage(0);
  cvReleaseMemStorage(&store);

  CvPoint2D32f *start = NULL;
  CvSeq *seq = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvSeq), sizeof(CvPoint2D32f), store);
  cvSeqPush(seq, start++);
  for (int i = 0; i < MAX_PARA; i++)
    CvPoint2D32f *p = (CvPoint2D32f *)cvGetSeqElem(seq, i);
  cvRelease((void **)&seq);
  
  //cvSeqSort(seq, seq_cmp_by_clockwise, &center);

  IplImage *src = cvLoadImage("filename", 1);     // 1 for is colored
  IplImage *dst = cvCreateImage(cvSize(0, 0), src->depth, src->nChannels);  
  cvZero(dst); 
  IplImage *dst1c = cvCloneImage(src);
  cvSetImageROI(dst, cvRect(0, 0, src->width, src->height));  
  cvCopy(src, dst, NULL);  
  cvResetImageROI(dst);  
  cvReleaseImage(&dst); 
  cvReleaseImage(&dst1c);  

  HWND hWnd = ::GetDesktopWindow(); 
  RECT rect; 
  ::GetWindowRect(hWnd,&rect); 
};

double ACOS(CvPoint *p0, CvPoint *p1, CvPoint *p2)
{
  double cm2 = (p2->x - p0->x) * (p2->x - p0->x) + (p2->y - p0->y) * (p2->y - p0->y);
  double am2 = (p1->x - p0->x) * (p1->x - p0->x) + (p1->y - p0->y) * (p1->y - p0->y);
  double bm2 = (p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y);
  return acos( (am2 + bm2 - cm2) / 2 / sqrt( am2 * bm2) );
}

CheckMore::CheckMore()
{
  ImageSrc = NULL;
  Image1c = NULL;
  ImageTran = NULL;
  ImageTran1c = NULL;
};

CheckMore::~CheckMore()
{
  ClearState();
}

RESULT CheckMore::SetPaper(int width, int height)
{
  PaperSize.x = EDGE_RATE;
  PaperSize.y = EDGE_RATE;
  PaperSize.width = width;
  PaperSize.height = height;
  // total image size is (width + 2 * EDGE_RATE) * (height + 2 * EDGE_RATE)
  return RESULT_OK;
}

RESULT CheckMore::LoadMask(int* xplace, long xsize, int* yplace, int ysize)
{
  int x, y;
  int nowx, nextx;
  int nowy, nexty, nowheight;
  pShareInfo info;

  NowCheck = 0;
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
      info = &CheckInfo[NowCheck];
      info->MaskRect.x = nowx + EDGE_RATE;
      info->MaskRect.y = nowy + EDGE_RATE;
      info->MaskRect.width = nextx - nowx;
      info->MaskRect.height = nowheight;

      NowCheck ++;
      if (NowCheck >= MAX_RECT) 
      {
        NowCheck = 0;
        return RESULT_ERR;
      }
    }
  }
  return RESULT_OK;
}


RESULT CheckMore::PrepareImage(char* filename)
{
  ClearState();

  if (filename)
    ImageSrc = cvLoadImage(filename, 1);
  if (!ImageSrc)
    return RESULT_FILE_ERROR;

  if (ImageSrc)
  {
    if (ImageSrc->height < ImageSrc->width)
    {
      ImageSrc = rotateImage(ImageSrc, 90, true);
    }
  }

  Image1c = cvCreateImage(cvSize(ImageSrc->width, ImageSrc->height), IPL_DEPTH_8U, 1);

  if(ImageSrc->nChannels > 1)
    cvCvtColor(ImageSrc, Image1c, CV_BGR2GRAY);
  else
    cvCopy(ImageSrc, Image1c);

  cvSmooth(Image1c, Image1c, CV_GAUSSIAN, 3, 3);
  cvAdaptiveThreshold(Image1c, Image1c, 128, CV_ADAPTIVE_THRESH_MEAN_C,
    CV_THRESH_BINARY_INV, DEFAULT_ADAPTIVE_TH_SIZE, DEFAULT_ADAPTIVE_TH_DELTA);

  return RESULT_OK;
};

RESULT CheckMore::ClearState(void)
{
  if (ImageSrc) 
    cvReleaseImage(&ImageSrc);
  ImageSrc = NULL;

  if (Image1c)
    cvReleaseImage(&Image1c);
  Image1c = NULL;

  if (ImageTran)
    cvReleaseImage(&ImageTran);
  ImageTran = NULL;

  if (ImageTran1c)
    cvReleaseImage(&ImageTran1c);
  ImageTran1c = NULL;

  return RESULT_OK;
};

RESULT CheckMore::FindMaxRect(void)
{
  int i, j;

  CvMemStorage *store = cvCreateMemStorage(0);
  CvSeq *contours = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),sizeof(CvPoint), store);
  CvSeq *nowcontour, *largecontour;
  CvSeq *points = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),sizeof(CvPoint), store);
  IplImage *tmp1c = cvCloneImage(Image1c);

  cvFindContours(tmp1c, store, &contours, sizeof(CvContour),
    CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0,0));

  int largerect = 0;
  for(nowcontour = contours; nowcontour; nowcontour = nowcontour->h_next)
  {
    CvRect feret = cvBoundingRect(nowcontour);
    int nowrect = feret.width * feret.height;
    if (nowrect > largerect)
    {
      largerect = nowrect;
      largecontour = nowcontour;
    }
  }
  if (largerect < ImageSrc->height * ImageSrc->width / 10)
    return RESULT_PARA_ERROR;

  CvSeq *hull = cvConvexHull2(largecontour, 0, CV_CLOCKWISE, 1 /* MUST BE 1*/);
  int totalhull = hull->total;
  double angleacos;
  double isline = IS_LINE_RATE / 180 * PI;
  int samepoint = MIN(Image1c->width, Image1c->height) / SAME_POINT_RATE;
  CvPoint *p0, *p1, *p2;

  int nowrp = 0;
  memset(edgerp, 0, sizeof(edgerp));

  for (i = 0; i < totalhull; i++)
  {
    p0 = (CvPoint*) cvGetSeqElem(hull, (i-1) % totalhull);
    p1 = (CvPoint*) cvGetSeqElem(hull, i);  
    p2 = (CvPoint*) cvGetSeqElem(hull, (i+1) % totalhull);
    angleacos = ACOS(p0, p1, p2);

    if (angleacos < isline)
    {
      for (j = 0; j < nowrp; j++)
      {
        if (fabs(edgerp[j].val[0] - p1->x) < samepoint && fabs(edgerp[j].val[1] - p1->y) < samepoint)
          break;
      }
      if (j == nowrp)
        nowrp ++;
      if (nowrp > 4)
        return RESULT_PARA_ERROR;
      edgerp[j].val[0] = (edgerp[j].val[0] * edgerp[j].val[3] + p1->x) / (edgerp[j].val[3] + 1);
      edgerp[j].val[1] = (edgerp[j].val[1] * edgerp[j].val[3] + p1->y) / (edgerp[j].val[3] + 1);
      edgerp[j].val[3] += 1; 
    }
  }

  for(nowcontour = contours; nowcontour; nowcontour = nowcontour->h_next)
  {
    cvClearSeq(nowcontour);
  }
  cvReleaseMemStorage(&store);

#ifdef _DEBUG
  for (i = 0; i < nowrp; i++)
  {
    cvCircle(ImageSrc, cvPoint((int)edgerp[i].val[0], (int)edgerp[i].val[1]), 20, CV_RGB(255,65,123), 10);
  }
#endif _DEBUG
  return RESULT_OK;
};

RESULT CheckMore::WarpImage(void)
{
  int i;
  CvPoint2D32f srcTri[4];
  CvMat*       warp_mat = cvCreateMat (3, 3, CV_32FC1);  

  ImageTran = cvCreateImage(cvSize(PaperSize.width + 2 * EDGE_RATE, PaperSize.height + 2 * EDGE_RATE), 
    ImageSrc->depth, ImageSrc->nChannels);
  ImageTran1c = cvCreateImage(cvSize(PaperSize.width + 2 * EDGE_RATE, PaperSize.height + 2 * EDGE_RATE), 
    Image1c->depth, Image1c->nChannels);  

  cvZero(ImageTran); 
  cvZero(ImageTran1c); 

  for (i = 0; i < 4; i++)
  {
    srcTri[i].x = (float)edgerp[i].val[0];
    srcTri[i].y = (float)edgerp[i].val[1];
  }
  SortRect(srcTri, 4);

  MaxRect[0].x = EDGE_RATE;  
  MaxRect[0].y = EDGE_RATE;
  MaxRect[1].x = (float)PaperSize.width + EDGE_RATE;
  MaxRect[1].y = EDGE_RATE;
  MaxRect[2].x = (float)PaperSize.width + EDGE_RATE;
  MaxRect[2].y = (float)PaperSize.height + EDGE_RATE;
  MaxRect[3].x = EDGE_RATE;
  MaxRect[3].y = (float)PaperSize.height + EDGE_RATE;

  cvGetPerspectiveTransform (srcTri, MaxRect, warp_mat);
  cvWarpPerspective (Image1c, ImageTran1c, warp_mat);
  cvWarpPerspective (ImageSrc, ImageTran, warp_mat);

  cvReleaseMat (&warp_mat);  

  return RESULT_OK;
}

RESULT CheckMore::GetLevelInfo(CvSeq* contour, pShareInfo info, bool doself, bool doloop)
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
      if (length * 10 > info->length && (doloop == true || rectrate > 2))
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
};


RESULT CheckMore::DetectOneCheck(int order)
{

  pShareInfo info = &CheckInfo[order];
  if (info->MaskRect.height == 0 || info->MaskRect.width == 0)
  {
    return RESULT_ERR;
  }

  CvMemStorage *store;
  store=cvCreateMemStorage(0);

  CvSeq *contours = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),sizeof(CvPoint), store);
  CvSeq *nowcontour;
  IplImage *tmp1c = cvCloneImage(ImageTran1c);

  CvRect roirect;
  roirect.x = info->MaskRect.x - ROI_ENLARGE;
  roirect.y = info->MaskRect.y - ROI_ENLARGE;
  roirect.width = info->MaskRect.width + 2 * ROI_ENLARGE;
  roirect.height = info->MaskRect.height + 2 * ROI_ENLARGE;
  cvSetImageROI(tmp1c, roirect);

  //   ShowImage(tmp1c);
  cvFindContours(tmp1c, store, &contours, sizeof(CvContour),
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
  float roiarea = (float)( roirect.width - 2 * ROI_ENLARGE) * (roirect.height - 2 * ROI_ENLARGE);
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

//  result = DetectCheck(order);

  for(nowcontour = contours; nowcontour; nowcontour = nowcontour->h_next)
  {
    cvClearSeq(nowcontour);
  }

  cvReleaseImage(&tmp1c);
  cvRelease((void **)&contours);
  cvReleaseMemStorage(&store);


  return RESULT_OK;
}

#define RATE_LEVEL1       ((float)1.6)
#define RATE_LEVEL2       ((float)1.1)
#define RATE_LEVEL3       ((float)0.07)

RESULT CheckMore::DetectCheck(int order)
{
  pShareInfo info = &CheckInfo[order];
  float framelength = (float)(info->MaskRect.width + info->MaskRect.height) * 2 + ROI_ENLARGE * 16;
//  float rectlength = framelength / (1 + 2 * ENLARGE);
  float rectlength = (float)(info->MaskRect.width + info->MaskRect.height) * 2;
  double ratelevel1, ratelevel2, ratelevel3;
  ratelevel1 = info->length / framelength;
  ratelevel2 = info->l2length / rectlength;
  ratelevel3 = info->l3length / rectlength ;

  printf("i:%2d, l1a:%5.0f, l1l:%4.0f, l2c:%1d, l2a:%5.0f, l2l:%4.0f, l3c:%1d, l3a:%3.0f, l3l:%2.0f\r\n",
    order, info->area, info->length, info->l2count, info->l2area, info->l2length, info->l3count, info->l3area, info->l3length);
  printf("rl:%6.0f, r1:%6.4f, r2:%6.4f, r3:%6.4f,   rt:%6.4f\r\n", rectlength, ratelevel1, ratelevel2, ratelevel3, ratelevel2/ratelevel1);

  if ((ratelevel1 < RATE_LEVEL1 && ratelevel2 > RATE_LEVEL2) || ratelevel3 > RATE_LEVEL3 )
  {
    CheckInfo[order].CheckResult = 'Y';
    printf ("    is mark !!! for V0.08\r\n");
    //     printf("i:%2d, l1a:%5.0f, l1l:%4.0f, l2c:%1d, l2a:%5.0f, l2l:%4.0f, l3c:%1d, l3a:%3.0f, l3l:%2.0f\r\n",
    //       order, info->area, info->length, info->l2count, info->l2area, info->l2length, info->l3count, info->l3area, info->l3length);
  }
  else
  {
    CheckInfo[order].CheckResult = ' ';
  }
  return RESULT_OK;
};

RESULT CheckMore::DisplayResult(void)
{
  CvFont font, font2;  
  cvInitFont(&font, CV_FONT_HERSHEY_SCRIPT_COMPLEX , 1.0, 1.0, 0, 2, 2);
  cvInitFont(&font2, CV_FONT_HERSHEY_DUPLEX , 0.5, 0.5, 0, 1, 1);

  for (int i = 0; i < NowCheck; i++)
  {
    if (CheckInfo[i].CheckResult == 'Y')
    {
      CvRect disprect = cvRect(CheckInfo[i].MaskRect.x - ROI_ENLARGE, CheckInfo[i].MaskRect.y - ROI_ENLARGE,
        CheckInfo[i].MaskRect.width + ROI_ENLARGE * 2, CheckInfo[i].MaskRect.height + ROI_ENLARGE * 2);
      cvRectangleR(ImageTran, disprect, CV_RGB(221,134,212), 3);
      cvRectangleR(ImageTran, CheckInfo[i].MaskRect, CV_RGB(22,253,211), 3);

      CvPoint disppoint = cvPoint(CheckInfo[i].MaskRect.x + CheckInfo[i].MaskRect.width / 3,
        CheckInfo[i].MaskRect.y + CheckInfo[i].MaskRect.height / 3 * 2);
      cvPutText(ImageTran, "Yes", disppoint, &font, CV_RGB(211,34,0));
    }
  }


  return RESULT_OK;
}


RESULT CheckMore::ShowImage(IplImage* img, char* name)
{
  if (!img || !name)
    return RESULT_PARA_ERROR;
  cvNamedWindow(name,1);

  CvSize   resize;
  IplImage *dst;
  RECT     rect; 
  HWND hWnd = ::GetDesktopWindow(); 
  ::GetWindowRect(hWnd,&rect); 

  float scale = MAX((float)img->width / (rect.right - rect.left - 50), 
    (float)img->height / (rect.bottom - rect.top - 100));
  if (scale < (float)1) 
    scale = (float)1;
  resize.width = (int)(img->width / scale);	
  resize.height = (int)(img->height / scale);

  dst = cvCreateImage( resize, img->depth, img->nChannels);
  cvResize(img, dst, CV_INTER_LINEAR);

  if (img)
    cvShowImage(name, dst);

  cvReleaseImage(&dst);
  return RESULT_OK;
};


int static Xplace[] = {620-43, 723-43, 827-43, 932-43, 0};
int static Yplace[] = {344-267, 422-267, 500-267, 642-267, 722-267, 799-267, 0,
  880-267, 956-267, 1037-267, 1114-267, 1263-267, 0};
int static PaperWidth = 932-43;
int static PaperHeight = 1263-267;

RESULT CheckMore::GetResult(char* checked)
{
  for (int i = 0; i < NowCheck; i++ )
  {
    if (CheckInfo[i].CheckResult == 'Y') *checked ++ = 'Y';
    else *checked ++ = ' ';
    if (i % (sizeof(Xplace)/sizeof(int) - 2) == 2)
    {
      *checked ++ = ',';
    }
  }
  *checked ++ = 0;
  return RESULT_OK;
}

RESULT CheckMore::Process(char* filename, char* checkresult, bool display)
{
  int nook = 0;
  char check[MAX_RECT];


  if (PrepareImage(filename) != RESULT_OK) 
    return RESULT_FILE_ERROR;
  
  if (SetPaper(PaperWidth, PaperHeight) != RESULT_OK)
    return RESULT_PARA_ERROR;

  if (LoadMask(Xplace, sizeof(Xplace)/sizeof(int) - 1, Yplace, sizeof(Yplace) /sizeof(int)- 1) != RESULT_OK)
    return RESULT_PARA_ERROR;

  if (FindMaxRect() != RESULT_OK) 
    return RESULT_PARA_ERROR;

  if (WarpImage() != RESULT_OK) 
    return RESULT_PARA_ERROR;
  
  for (int i = 0; i < NowCheck; i++)
  {
    if (DetectOneCheck(i) != RESULT_OK)
      nook ++;
  }
  for (int i = 0; i < NowCheck; i++)
  {
    DetectCheck(i);
  }
  
  GetResult(check);

  if (display)
  {
    DisplayResult();
    ShowImage(ImageTran, "ImageTran");
    ShowImage(ImageSrc, "ImageSrc");
  }


  return RESULT_OK;
}
