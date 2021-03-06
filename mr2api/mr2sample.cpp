#include "mr2sample.h"
#include "mr2sampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Cmr2sampleApp

BEGIN_MESSAGE_MAP(CMr2SampleApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CMr2SampleApp::CMr2SampleApp()
{
}

CMr2SampleApp theApp;

BOOL CMr2SampleApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
  Mr2Init2(0, 0, 0, 0, 0, 0, 0, 0, 0);

	SetRegistryKey(_T("Local App"));

	CMr2SampleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	return FALSE;
}
