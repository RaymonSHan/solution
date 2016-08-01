
// GroupBoxDlg.h : 头文件
//

#pragma once
#include <vector>
#include "afxwin.h"

// CGroupBoxDlg 对话框
class CGroupBoxDlg : public CDialogEx
{
// 构造
public:
	CGroupBoxDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GROUPBOX_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CDC *pDC;					// by GetDC();
	CDC *pComDC;				// by CreateCompatibleDC(pDC);
	CBitmap *pBitmap;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	std::vector<LOGFONT> vectorFonts;
	CListBox mCtlFont;
	CStatic mCtlStatic;
	CDC* CreateBitmapDC(int width, int height);
	int OutputPicture(WORD start, WORD end, int xoff, int yoff, int linenum, LOGFONT &font);

	afx_msg void OnSelchangeListFont();
	afx_msg void OnBnClickedOk();
};

int CALLBACK EnumFontFamProc(ENUMLOGFONT* lpelf, NEWTEXTMETRIC* lpntm, int nFontType, LPARAM lParam);

bool GetValidUnicode(WORD gbk, WCHAR *unicode);
