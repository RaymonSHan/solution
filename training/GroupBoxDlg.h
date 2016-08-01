
// GroupBoxDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include "afxwin.h"

// CGroupBoxDlg �Ի���
class CGroupBoxDlg : public CDialogEx
{
// ����
public:
	CGroupBoxDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GROUPBOX_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CDC *pDC;					// by GetDC();
	CDC *pComDC;				// by CreateCompatibleDC(pDC);
	CBitmap *pBitmap;

	// ���ɵ���Ϣӳ�亯��
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
