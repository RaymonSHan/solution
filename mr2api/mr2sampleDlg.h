// mr2sampleDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

unsigned int __stdcall DlgRecvThread(PVOID pM);
unsigned int __stdcall DlgSendThread(PVOID pM);

// Cmr2sampleDlg 对话框
class CMr2SampleDlg : public CDialog
{
// 构造
public:
	CMr2SampleDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MR2SAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
  ProcessMr* pMr;
  HANDLE hThreadRecv;
  HANDLE hThreadSend;

  CString mPacketType;
  CString mSourceUserID;
  CString mSourceAppID;
  CString mTargetUserID;
  CString mTargetAppID;
  CString mPkgID;
  CString mCurrPkgID;
  CString mUserData1;
  CString mUserData2;
  CString mMessage;
  CString mLog;
  DWORD mIpAddress;

  afx_msg void OnBnClickedConnect();
  afx_msg void OnBnClickedSend();
  afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);

  RESULT GenSTU(STUMsgProperty2& stu);

public:
  void DlgSendThread(void);
  void DlgRecvThread(void);

  CButton wndConnect;
  CButton wndSend;
  DWORD mPort;
};
