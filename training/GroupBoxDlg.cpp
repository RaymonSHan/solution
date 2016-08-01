
// GroupBoxDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GroupBox.h"
#include "GroupBoxDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGroupBoxDlg �Ի���



CGroupBoxDlg::CGroupBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GROUPBOX_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGroupBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTFONT, mCtlFont);
	DDX_Control(pDX, IDC_STATICFONT, mCtlStatic);
}

BEGIN_MESSAGE_MAP(CGroupBoxDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_LISTFONT, &CGroupBoxDlg::OnSelchangeListFont)
	ON_BN_CLICKED(IDOK, &CGroupBoxDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGroupBoxDlg ��Ϣ�������

BOOL CGroupBoxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	pDC = GetDC();
	pComDC = 0;
	pBitmap = 0;

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	CClientDC dc(this);
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfCharSet = GB2312_CHARSET;
	mCtlFont.ResetContent();
	vectorFonts.clear();
	::EnumFontFamiliesEx((HDC)dc, &lf, (FONTENUMPROC)EnumFontFamProc, (LPARAM)this, NULL);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}



void CGroupBoxDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGroupBoxDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGroupBoxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// this is Unicode version, use WCHAR
int CALLBACK EnumFontFamProc(ENUMLOGFONT* lpelf, NEWTEXTMETRIC* lpntm, int nFontType, LPARAM lParam) {
	CGroupBoxDlg* pDlg = (CGroupBoxDlg*)lParam;
	pDlg->mCtlFont.SetRedraw(FALSE);
// 	if ((pWnd->m_wndCheckBox.GetCheck() == BST_UNCHECKED)
// 		|| (nFontType & TRUETYPE_FONTTYPE))
	if (lpelf->elfLogFont.lfFaceName[0] > 255) {
		pDlg->mCtlFont.AddString(lpelf->elfLogFont.lfFaceName);
		pDlg->vectorFonts.push_back(lpelf->elfLogFont);
	}
	pDlg->mCtlFont.SetRedraw(TRUE);
	return 1;
}

void CGroupBoxDlg::OnSelchangeListFont()
{
	int nIndex = this->mCtlFont.GetCurSel();
	if (nIndex == LB_ERR)
		return;	// TODO: Add your control notification handler code here
	CFont mfont;

	if (mfont.m_hObject)
	{
		mfont.DeleteObject();
	}
	// ʹ��ѡ�������LOGFONT�����µ�����   
	mfont.CreateFontIndirect(&vectorFonts[nIndex]);
	// ��ȡ�༭��IDC_FONT_EDIT��CWndָ�룬������������   
	GetDlgItem(IDC_STATICFONT)->SetFont(&mfont);

	// ����û�ѡ��������Ի����OK��ť�����ȡ��ѡ����������Ʋ���ʾ���༭����   
	CString strFontName = vectorFonts[nIndex].lfFaceName;
	SetDlgItemText(IDC_STATICFONT, strFontName);
}

#define		WIDTH		200
#define		HEIGHT		123

void CGroupBoxDlg::OnBnClickedOk()
{
// 	WCHAR uni[10];
// 	bool aa = GetValidUnicode(0xaaab, uni);
// 	uni[1] = 0;
// 	SetDlgItemText(IDC_STATICFONT, uni);
// 	return;

 	cv::Mat		mat;
	cv::Size	size;
	BITMAPINFOHEADER head;
	memset(&head, 0, sizeof(BITMAPINFOHEADER));
	head.biSize = sizeof(BITMAPINFOHEADER);
	head.biWidth = WIDTH;
	head.biHeight = HEIGHT;
	head.biPlanes = 1;
	head.biBitCount = 24;
	head.biSizeImage = WIDTH * HEIGHT * 3;


	int bmpHeight = head.biHeight;
	int bmpWidth = head.biWidth;
	int biBitCount = head.biBitCount;
	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;
	
	size.width = bmpWidth;
	size.height = bmpHeight;
	mat.create(size, CV_8UC3);
	cv::Vec3b *dstnow = &mat.at<cv::Vec3b>(0, 0);
	unsigned char *pTemp = (unsigned char *)dstnow;

	CDC *m_pMemDC;
	m_pMemDC = new CDC();
	CDC *pDC;
	pDC = GetDC();
	m_pMemDC->CreateCompatibleDC(pDC);
	CBitmap *m_pMemBmp = new CBitmap();       //����ͼƬ�Ĵ�С����һ������λͼ
	m_pMemBmp->CreateCompatibleBitmap(pDC, bmpWidth, bmpHeight);
	/*m_pOldBmp =*/ m_pMemDC->SelectObject(m_pMemBmp);
	
	m_pMemDC->SetBkMode(TRANSPARENT);
	m_pMemDC->SetTextColor(RGB(0, 0, 0));
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = bmpWidth;
	rect.bottom = bmpHeight;

	m_pMemDC->FillRect(&rect, new CBrush(RGB(255, 255, 255)));
// 
// //��������
	CFont font;
	font.CreateFont(20, 10, 0, 0, 10, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, _T("����"));
	m_pMemDC->SelectObject(font);
// // ������ֵ�ָ��λ��
	m_pMemDC->TextOut(17, 10, _T("hello"));

	GetDIBits(m_pMemDC->GetSafeHdc(), (HBITMAP)m_pMemBmp->m_hObject, 0, bmpHeight,
	    (LPVOID)pTemp, (BITMAPINFO*)&head, DIB_RGB_COLORS);
	
	delete m_pMemBmp;
	delete m_pMemDC;
	cv::flip(mat, mat, 0);		// flip up and down
	cv::imwrite("c:\\1.tif", mat);

	CDialogEx::OnOK();
}

bool GetValidUnicode(WORD gbk, WCHAR *unicode) {
	WORD gbkagain = 0;
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)&gbk, sizeof(WORD), unicode, 1);
	WideCharToMultiByte(CP_ACP, 0, unicode, 1, (LPSTR)&gbkagain, 2, NULL, NULL);
	return (gbk == gbkagain);
}

// create bitmap and selected by dc, should fill and set font outside
CDC* CGroupBoxDlg::CreateBitmapDC(int width, int height) {
	int bmpHeight = height;
	int bmpWidth = width;
	int biBitCount = 24;
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	if (pComDC) {
		delete pComDC;
	}
	pComDC = new CDC();
	pComDC->CreateCompatibleDC(pDC);
	if (pBitmap) {
		delete pBitmap;
	}
	pBitmap = new CBitmap();       //����ͼƬ�Ĵ�С����һ������λͼ
	pBitmap->CreateCompatibleBitmap(pDC, bmpWidth, bmpHeight);
	pComDC->SelectObject(pBitmap);
	pComDC->SetBkMode(TRANSPARENT);
	pComDC->SetTextColor(RGB(0, 0, 0));
	return pComDC;
}

#define XSTART		20
#define YSTART		20
int CGroupBoxDlg::OutputPicture(WORD start, WORD end, int xoff, int yoff, int linenum, LOGFONT &font) {
	uchar segstart, segend, offstart, offend, nowstart, nowend;
	int nowx = XSTART, nowy = YSTART, nownum = 0;
	segstart = HIBYTE(start);
	segend = HIBYTE(end);
	offstart = LOBYTE(start);
	offend = LOBYTE(end);
	uchar i, j;
	WCHAR uni[2];

	for (i = segstart; i <= segend; i++) {
		nowstart = i == segstart ? offstart : 0x40;
		nowend = i == segend ? offend : 0xfe;
		for (j = nowstart; j <= nowend; j++) {
			if (j == 0x7f) {
				continue;
			}
			if (nownum == linenum) {
				nowx = XSTART;
				nowy += yoff;
				nownum = 0;
			}
			else {
				nowx += xoff;
				nownum++;
			}
			GetValidUnicode(j << 8 + i, uni);
			uni[1] = 0;
			pComDC->TextOut(nowx, nowy, uni);
			if (/*have real write*/true) {
				// write box item
			}
		}
	}
	return 0;
}