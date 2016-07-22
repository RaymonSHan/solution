
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "GroupBox.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView() {
}

CChildView::~CChildView() {
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() {
	CPaintDC dc(this); // 用于绘制的设备上下文
	FILE *fp;
	fopen_s(&fp, "c:\\1.bmp", "rb");
	if (fp == 0)
		return;
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	int bmpHeight = head.biHeight;
	int bmpWidth = head.biWidth;
	int biBitCount = head.biBitCount;
	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;
	RGBQUAD *pColorTable;
	if (biBitCount == 8) {
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}
	unsigned char *pBmpBuf = new unsigned char[lineByte*bmpHeight];
	fread(pBmpBuf, 1, lineByte*bmpHeight, fp);
	
	CDC *m_pMemDC;
	m_pMemDC = new CDC();
	CDC *pDC;
	pDC = GetDC();
	m_pMemDC->CreateCompatibleDC(pDC);
	CBitmap *m_pOldBmp = NULL;
	CBitmap *m_pMemBmp = new CBitmap();       //根据图片的大小创建一个兼容位图
	m_pMemBmp->CreateCompatibleBitmap(pDC, bmpWidth, bmpHeight);
	m_pOldBmp = m_pMemDC->SelectObject(m_pMemBmp);
	
// 把图像的数据绘制到兼容位图上
	SetDIBits(m_pMemDC->GetSafeHdc(), (HBITMAP)m_pMemBmp->m_hObject,
		0, bmpHeight, (LPVOID)pBmpBuf, (BITMAPINFO*)&head, DIB_RGB_COLORS);
	m_pMemDC->SetBkMode(TRANSPARENT);
	m_pMemDC->SetTextColor(RGB(255, 0, 0));
	
//设置字体
// 	CFont font;
// 	HDC hDC = ::GetDC(GetSafeHwnd());
// 	HDC hMemDC = CreateCompatibleDC(hDC);
// 	font.CreateFont(10, 10, 0, 0, 10, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, "宋体");
// 	SelectObject(hMemDC, font);
// 添加文字到指定位置
	m_pMemDC->TextOut(17, 10, "hello");
	
	unsigned char *pTemp = new unsigned char[lineByte*bmpHeight];
	GetDIBits(m_pMemDC->GetSafeHdc(), (HBITMAP)m_pMemBmp->m_hObject, 0, bmpHeight,
		(LPVOID)pTemp, (BITMAPINFO*)&head, DIB_RGB_COLORS);
	fclose(fp);
	
	char bmpwrite[] = "c:\\2.bmp";
	fopen_s(&fp, bmpwrite, "wb");
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4D42;
	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + lineByte*bmpHeight;
	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;
	fileHead.bfOffBits = 54;
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);
	
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(pTemp, lineByte*bmpHeight, 1, fp);
	fclose(fp);
	
	delete m_pMemBmp;
	delete m_pMemDC;
	delete[] pBmpBuf;
	delete[] pTemp;
}

