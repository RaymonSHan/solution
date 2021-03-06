#include "mr2sample.h"
#include "mr2sampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include <afxpriv.h>      // for WM_KICKIDLE



CMr2SampleDlg::CMr2SampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMr2SampleDlg::IDD, pParent)
  , mPacketType(_T("2"))
  , mSourceUserID(_T(""))
  , mSourceAppID(_T("OTC APP"))
  , mTargetUserID(_T("ZZBJ"))
  , mTargetAppID(_T("OTC_APP"))
  , mPkgID(_T(""))
  , mCurrPkgID(_T(""))
  , mUserData1(_T(""))
  , mUserData2(_T(""))
  , mMessage(_T("First Packet"))
  , mLog(_T(""))
  , mIpAddress(0xc0a8ce73)
  , mPort(9898)
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
  DDX_Control(pDX, IDC_SEND, wndSend);
  DDX_Text(pDX, IDC_PORT, mPort);
}






BEGIN_MESSAGE_MAP(CMr2SampleDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_CONNECT, &CMr2SampleDlg::OnBnClickedConnect)
  ON_BN_CLICKED(IDC_SEND, &CMr2SampleDlg::OnBnClickedSend)
  ON_MESSAGE(WM_KICKIDLE,OnKickIdle)


END_MESSAGE_MAP()

LRESULT CMr2SampleDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
  static HANDLE lastrecv = 0;
  static ProcessMr* lastmr = 0;

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



RESULT CMr2SampleDlg::GenSTU(STUMsgProperty2& stu)
{
  UpdateData();
  strncpy_s(stu.m_szSourceUserID, (LPCTSTR)mSourceUserID, MR2_MAXLEN_ADDR);
  strncpy_s(stu.m_szSourceAppID, (LPCTSTR)mSourceAppID, MR2_MAXLEN_ADDR);
  strncpy_s(stu.m_szDestUserID, (LPCTSTR)mTargetUserID, MR2_MAXLEN_ADDR);
  strncpy_s(stu.m_szDestAppID, (LPCTSTR)mTargetUserID, MR2_MAXLEN_ADDR);
  strncpy_s(stu.m_szPkgID, (LPCTSTR)mPkgID, MR2_MAXLEN_PKGID);
  strncpy_s(stu.m_szCorrPkgID, (LPCTSTR)mCurrPkgID, MR2_MAXLEN_PKGID);
  strncpy_s(stu.m_szUserData1, (LPCTSTR)mUserData1, MR2_MAXLEN_USERDATA);
  strncpy_s(stu.m_szUserData2, (LPCTSTR)mUserData2, MR2_MAXLEN_USERDATA);

  return RESULT_OK;
}


void CMr2SampleDlg::OnBnClickedConnect()
{
  STUConnInfo2 conn;
  in_addr addr;
  UpdateData();
  addr.S_un.S_addr = htonl(mIpAddress);
  strncpy_s(conn.m_szMRIP, inet_ntoa(addr), MR2_MAXLEN_IP);

  if (pMr)
  {
    delete pMr;
    pMr = NULL;
  }
  else
  {
    pMr = new ProcessMr();
 //   pMr->SetSendAfterConnect(false);
    pMr->InitConnect(mSourceAppID, "pass", &conn, 1, 1);
  }
}

void CMr2SampleDlg::OnBnClickedSend()
{
}

void CMr2SampleDlg::DlgSendThread(void)
{

}

void CMr2SampleDlg::DlgRecvThread(void)
{

}
