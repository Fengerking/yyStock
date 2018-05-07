/*******************************************************************************
	File:		CVideoRender.cpp

	Contains:	file info dialog implement code

	Written by:	Fenger King

	Change History (most recent first):
	2013-04-01		Fenger			Create file

*******************************************************************************/
#include "windows.h"
#include "commctrl.h"
#include "qcErr.h"
#include "qcStock.h"

#include "CDlgMyStock.h"
#include "Resource.h"

#include "CRegMng.h"

CDlgMyStock::CDlgMyStock(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
	, m_pView(NULL)
{
	SetObjectName("CDlgMyStock");
}

CDlgMyStock::~CDlgMyStock(void)
{

}

int CDlgMyStock::OpenDlg (void)
{
	CDlgBase::OpenDlg ();
	return DialogBoxParam (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_MYSTOCK), m_hParent, baseDlgProc, (LPARAM)this);
}

int CDlgMyStock::OnCloseDlg (int nCloseID)
{
	if (m_pView != NULL)
	{
		m_pView->Close();
		delete m_pView;
		m_pView = NULL;
	}
	EndDialog (m_hDlg, nCloseID);
	return QC_ERR_NONE;
}

int CDlgMyStock::OnInitDlg (void)
{
	SetWindowPos(m_hDlg, NULL, 0, 0, 1600, 900, 0);
	CenterDlg ();

	RECT rcDlg;
	GetClientRect(m_hDlg, &rcDlg);
	m_pView = new CViewMyStock(m_hInst);
	m_pView->CreateWnd(m_hDlg, rcDlg, MSC_BLACK, NULL);

	return QC_ERR_NONE;
}

INT_PTR CDlgMyStock::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		OnInitDlg ();
		break;

	case WM_COMMAND:
	{
		int wmId    = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_BUTTON_OK:
			OnCloseDlg (IDOK);
			break;

		case IDC_BUTTON_CANCEL:
			OnCloseDlg (IDCANCEL);
			break;

		default:
			break;
		}
		break;
	}
	default:
		break;
	}
	return FALSE;
}
