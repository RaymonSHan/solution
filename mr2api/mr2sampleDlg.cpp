#include "mr2sample.h"
#include "mr2sampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include <afxpriv.h>      // for WM_KICKIDLE



CMr2SampleDlg::CMr2SampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMr2SampleDlg::IDD, pParent)
  , mPacketType(_T(""))
  , mSourceUserID(_T(""))
  , mSourceAppID(_T(""))
  , mTargetUserID(_T(""))
  , mTargetAppID(_T(""))
  , mPkgID(_T(""))
  , mCurrPkgID(_T(""))
  , mUserData1(_T(""))
  , mUserData2(_T(""))
  , mMessage(_T(""))
  , mLog(_T(""))
  , mIpAddress(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  pMr = NULL;
  hThreadRecv = NULL;
  hThreadSend = NULL;
}

void CMr2SampleDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_PACKET_TYPE, mPacketType);
  DDX_Text(pDX, IDC_SOURCE_USERID, mSourceUserID);
  DDX_Text(pDX, IDC_SOURCE_APPID, mSourceAppID);
  DDX_Text(pDX, IDC_TARGET_USERID, mTargetUserID);
  DDX_Text(pDX, IDC_TARGET_APPID, mTargetAppID);
  DDX_Text(pDX, IDC_PKGID, mPkgID);
  DDX_Text(pDX, IDC_CURR_PKGID, mCurrPkgID);
  DDX_Text(pDX, IDC_USER_DATA1, mUserData1);
  DDX_Text(pDX, IDC_USER_DATA2, mUserData2);
  DDX_Text(pDX, IDC_MESSAGE, mMessage);
  DDX_Text(pDX, IDC_LOG, mLog);
  DDX_IPAddress(pDX, IDC_IPADDRESS, mIpAddress);
  DDX_Control(pDX, IDC_CONNECT, wndConnect);
  DDX_Control(pDX, IDC_RECEIVE, wndReceive);
  DDX_Control(pDX, IDC_Send, wndSend);
}






BEGIN_MESSAGE_MAP(CMr2SampleDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_CONNECT, &CMr2SampleDlg::OnBnClickedConnect)
  ON_BN_CLICKED(IDC_Send, &CMr2SampleDlg::OnBnClickedSend)
  ON_BN_CLICKED(IDC_RECEIVE, &CMr2SampleDlg::OnBnClickedReceive)
  ON_MESSAGE(WM_KICKIDLE,OnKickIdle)


END_MESSAGE_MAP()

LRESULT CMr2SampleDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
  static HANDLE lastrecv = 0;
  static ProcessMr* lastmr = 0;

  if (hThreadRecv != lastrecv)
  {
    if (hThreadRecv)
    {
      wndReceive.SetWindowText("Stop Recv");
      wndReceive.SetState(1);
    }
    else
    {
      wndReceive.SetWindowText("Receive");
      wndReceive.SetState(0);
    }
    lastrecv = hThreadRecv;
  }

  if (lastmr != pMr)
  {
    if (pMr)
    {
      wndConnect.SetWindowText("DisConnnect");
      wndConnect.SetState(1);
    }
    else
    {
      wndConnect.SetWindowText("Connnect");
      wndConnect.SetState(0);
    }
    lastmr = pMr;
  }
 // wndReceive.EnableWindow(FALSE);
  //wndReceive.SetCheck(0);
  return 0;   //一定要返回0
}

// Cmr2sampleDlg 消息处理程序

BOOL CMr2SampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMr2SampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMr2SampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

IntoClassFunction(CMr2SampleDlg, DlgSendThread);
IntoClassFunction(CMr2SampleDlg, DlgRecvThread);


void CMr2SampleDlg::OnBnClickedConnect()
{
  if (pMr)
  {
    delete pMr;
    pMr = NULL;
  }
  else
  {
    UpdateData();
    pMr = new ProcessMr();
//    pMr->InitConnect(mSourceAppID, "pass", NULL, 0, 0);   // should later
  }
}

void CMr2SampleDlg::OnBnClickedSend()
{
}

void CMr2SampleDlg::OnBnClickedReceive()
{
 // hThreadRecv = (HANDLE)_beginthreadex(NULL, 0, ::DlgRecvThread, this, 0, NULL);  

}

void CMr2SampleDlg::DlgSendThread(void)
{

}

void CMr2SampleDlg::DlgRecvThread(void)
{

}
