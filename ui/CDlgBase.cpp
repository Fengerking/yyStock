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

#include "CDlgBase.h"
#include "CBaseGraphics.h"

#include "CRegMng.h"

CDlgBase::CDlgBase(HINSTANCE hInst, HWND hParent)
	: CBaseObject()
	, m_hInst (hInst)
	, m_hParent (hParent)
	, m_hDlg (NULL)
	, m_hBrushBG (NULL)
	, m_bModeless (false)
{
	SetObjectName("CDlgBase");
}

CDlgBase::~CDlgBase(void)
{
	if (m_hDlg != NULL)
		DestroyWindow (m_hDlg);
	SAFE_DEL_OBJ (m_hBrushBG);
}

int CDlgBase::OpenDlg (void)
{
	m_bModeless = false;
	return QC_ERR_NONE;
}

int CDlgBase::CreateDlg (void)
{
	m_bModeless = true;
	return QC_ERR_NONE;
}

INT_PTR CDlgBase::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

bool CDlgBase::CenterDlg (void)
{
	if (m_hDlg == NULL)
		return false;
	int		nScreenX = GetSystemMetrics (SM_CXSCREEN);
	int		nScreenY = GetSystemMetrics (SM_CYSCREEN);
	RECT	rcDlg;
	GetClientRect (m_hDlg, &rcDlg);
	SetWindowPos (m_hDlg, NULL, (nScreenX - rcDlg.right) / 2, (nScreenY - rcDlg.bottom) / 2 - 30, rcDlg.right, rcDlg.bottom, SWP_NOSIZE);
	return true;
}

INT_PTR CALLBACK CDlgBase::baseDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int				wmId, wmEvent;
	CDlgBase *		pDlgBase = NULL;

	if (hDlg != NULL)
		pDlgBase = (CDlgBase *)GetWindowLong (hDlg, GWL_USERDATA);

	switch (message)
	{
	case WM_INITDIALOG:
		if (lParam != NULL)
		{
			SetWindowLong (hDlg, GWL_USERDATA, lParam);
			pDlgBase = (CDlgBase *)lParam;
			pDlgBase->m_hDlg = hDlg;
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;

		default:
			break;
		}
		break;

	case WM_ERASEBKGND:
		if (pDlgBase != NULL && pDlgBase->m_hBrushBG != NULL)
		{
			RECT	rcDlg;
			GetClientRect (hDlg, &rcDlg);
			FillRect ((HDC)wParam, &rcDlg, pDlgBase->m_hBrushBG);
			return S_FALSE;
		}
		return S_OK;

	default:
		break;
	}

	if (pDlgBase != NULL)
		return pDlgBase->OnReceiveMsg (hDlg, message, wParam, lParam);
	else
		return (INT_PTR)FALSE;
}

void CDlgBase::FormatDouble (double dNum, char * szText)
{
	if (dNum < -10)
		sprintf (szText, " %.2f", dNum);
	else if (dNum < 0)
		sprintf (szText, "  %.2f", dNum);
	else if (dNum < 10)
		sprintf (szText, "   %.2f", dNum);
	else if (dNum < 100)
		sprintf (szText, "  %.2f", dNum);
	else
		sprintf (szText, " %.2f", dNum);
}