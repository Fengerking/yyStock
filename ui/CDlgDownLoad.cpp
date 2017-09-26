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

#include "CDlgDownLoad.h"

#include "CRegMng.h"
#include "Resource.h"

#include "UStockTools.h"
#include "USystemFunc.h"

CDlgDownLoad::CDlgDownLoad(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
	, m_hEdtResult(NULL)
	, m_hProgress(NULL)
	, m_pItem(NULL)
	, m_nCommandID(0)
	, m_hPosCode(NULL)
	, m_nCodeNum(0)
	, m_nProcNum(0)
	, m_nTimer(0)
	, m_nStartTime(0)
{
	SetObjectName("CDlgDownLoad");
	m_pErrorText = new char[1024 * 1024];
}

CDlgDownLoad::~CDlgDownLoad(void)
{
	QC_DEL_A(m_pErrorText);
}

int CDlgDownLoad::OpenDlg (void)
{
	CDlgBase::OpenDlg ();
	return DialogBoxParam (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_DOWNLOAD), m_hParent, baseDlgProc, (LPARAM)this);
}

int CDlgDownLoad::OnInitDlg (void)
{
	m_hEdtResult = GetDlgItem(m_hDlg, IDC_EDIT_RESULT);
	m_hProgress = GetDlgItem(m_hDlg, IDC_PROGRESS_DOWNLOAD);

	CenterDlg ();
	return QC_ERR_NONE;
}

int	CDlgDownLoad::OnStart(void)
{
	if (CStockItemList::g_stkList == NULL)
		return QC_ERR_FAILED;
	m_nCodeNum = CStockItemList::g_stkList->m_lstStock.GetCount();
	if (m_nCodeNum <= 0)
		return QC_ERR_FAILED;

	SendMessage(m_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, m_nCodeNum));
	SendMessage(m_hProgress, PBM_SETSTEP, (WPARAM)1, 0);
	SendMessage(m_hProgress, PBM_SETPOS, (WPARAM)0, 0);
	SetWindowText(m_hEdtResult, "");

	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_HISTORY), FALSE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_CODELIST), FALSE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_CQFQ), FALSE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_FINANCE), FALSE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_COMPINFO), FALSE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_HYGN), FALSE);

	m_nTimer = SetTimer(m_hDlg, 1001, 1, NULL);
	m_nProcNum = 0;
	strcpy(m_pErrorText, "");
	m_nStartTime = qcGetSysTime();
	m_hPosCode = CStockItemList::g_stkList->m_lstStock.GetHeadPosition();

	return 0;
}

int CDlgDownLoad::OnDownLoad(void)
{
	if (m_hPosCode == NULL)
	{
		OnFinish(true);
		return QC_ERR_NONE;
	}
	m_pItem = CStockItemList::g_stkList->m_lstStock.GetNext(m_hPosCode);
	int nStart = qcGetSysTime();
	int nRC = 0;
	if (m_nCommandID == IDC_BUTTON_HISTORY)
		nRC = qcStock_DownLoadHistoryData(m_pItem->m_szCode);
	int nEnd = qcGetSysTime();
	int nTotal = (nEnd - m_nStartTime) / 1000;
	sprintf(m_szStatus, "%s %s    Used:  % 8d.   Total:   %02d:%02d:%02d   % 8d / %d", 
						m_pItem->m_szCode, m_pItem->m_szName, nEnd - nStart, 
						nTotal / 3600, (nTotal % 3600) / 60, nTotal % 60, m_nProcNum, m_nCodeNum);
	SetDlgItemText(m_hDlg, IDC_STATIC_STATUS, m_szStatus);

	m_nProcNum++;
	SendMessage(m_hProgress, PBM_SETPOS, (WPARAM)m_nProcNum, 0);
	if (nRC != QC_ERR_NONE)
	{
		strcat(m_pErrorText, m_pItem->m_szCode);
		strcat(m_pErrorText, "    Download Failed! \r\n");
		SetWindowText(m_hEdtResult, m_pErrorText);
	}
	m_nTimer = SetTimer(m_hDlg, 1001, 1, NULL);

	return 0;
}

int CDlgDownLoad::OnFinish(bool bShow)
{
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_HISTORY), TRUE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_CODELIST), TRUE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_CQFQ), TRUE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_FINANCE), TRUE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_COMPINFO), TRUE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_HYGN), TRUE);

	if (bShow)
		MessageBox(m_hDlg, "Download finished!", "Information...", MB_OK);

	return 0;
}

INT_PTR CDlgDownLoad::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		case IDC_BUTTON_HISTORY:
		case IDC_BUTTON_CODELIST:
		case IDC_BUTTON_CQFQ:
		case IDC_BUTTON_FINANCE:
		case IDC_BUTTON_COMPINFO:
		case IDC_BUTTON_HYGN:
			m_nCommandID = wmId;
			OnStart();
			break;

		case IDC_BUTTON_OK:
		case IDC_BUTTON_CANCEL:
			EndDialog(m_hDlg, wmId);
			break;

		default:
			break;
		}
		break;
	}

	case WM_TIMER:
		if (m_nTimer != 0)
			KillTimer(m_hDlg, m_nTimer);
		m_nTimer = 0;
		OnDownLoad();
		break;

	default:
		break;
	}
	return FALSE;
}
