
// GroupBox.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGroupBoxApp: 
// �йش����ʵ�֣������ GroupBox.cpp
//

class CGroupBoxApp : public CWinApp
{
public:
	CGroupBoxApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGroupBoxApp theApp;