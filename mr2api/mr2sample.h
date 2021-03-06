// mr2sample.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#include "targetver.h"

#include "resource.h"		// 主符号

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <process.h>
#include <afxsock.h>            // MFC 套接字扩展

#include "jni.h"
#include "rcommon.h"

#undef DLLOUT_EXPORTS
#include "mr2api.h"
#include "mr2main.h"

#pragma comment(lib, "mr2dll.lib")

class CMr2SampleApp : public CWinApp
{
public:
	CMr2SampleApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMr2SampleApp theApp;