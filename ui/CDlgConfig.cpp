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

#include "CDlgConfig.h"
#include "Resource.h"

#include "CRegMng.h"

CDlgConfig::CDlgConfig(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
{
	SetObjectName("CDlgConfig");
}

CDlgConfig::~CDlgConfig(void)
{

}

int CDlgConfig::OpenDlg (void)
{
	CDlgBase::OpenDlg ();
	return DialogBoxParam (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_CONFIG), m_hParent, baseDlgProc, (LPARAM)this);
}

int CDlgConfig::OnCloseDlg (int nCloseID)
{
	if (nCloseID == IDOK)
	{
		char szDays[8];
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE5), szDays, sizeof (szDays));
		CRegMng::g_pRegMng->SetTextValue ("Line5Days", szDays);
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE10), szDays, sizeof (szDays));
		CRegMng::g_pRegMng->SetTextValue ("Line10Days", szDays);
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE20), szDays, sizeof (szDays));
		CRegMng::g_pRegMng->SetTextValue ("Line20Days", szDays);
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE30), szDays, sizeof (szDays));
		CRegMng::g_pRegMng->SetTextValue ("Line30Days", szDays);
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE60), szDays, sizeof (szDays));
		CRegMng::g_pRegMng->SetTextValue ("Line60Days", szDays);
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE120), szDays, sizeof (szDays));
		CRegMng::g_pRegMng->SetTextValue ("Line120Days", szDays);

		CRegMng::g_pRegMng->SetIntValue ("Line5Show", SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE5), BM_GETCHECK, 0, 0));
		CRegMng::g_pRegMng->SetIntValue ("Line10Show", SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE10), BM_GETCHECK, 0, 0));
		CRegMng::g_pRegMng->SetIntValue ("Line20Show", SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE20), BM_GETCHECK, 0, 0));
		CRegMng::g_pRegMng->SetIntValue ("Line30Show", SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE30), BM_GETCHECK, 0, 0));
		CRegMng::g_pRegMng->SetIntValue ("Line60Show", SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE60), BM_GETCHECK, 0, 0));
		CRegMng::g_pRegMng->SetIntValue ("Line120Show", SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE120), BM_GETCHECK, 0, 0));
	
		int nChuFuQuan = 1;
		if (SendMessage (GetDlgItem (m_hDlg, IDC_RADIO_NO), BM_GETCHECK, 0, 0))
			nChuFuQuan = 0;
		else if (SendMessage (GetDlgItem (m_hDlg, IDC_RADIO_PREV), BM_GETCHECK, 0, 0))
			nChuFuQuan = 1;
		else
			nChuFuQuan = 2;
		CRegMng::g_pRegMng->SetIntValue ("ChuFuQuan", nChuFuQuan);
	}
	EndDialog (m_hDlg, nCloseID);
	return QC_ERR_NONE;
}

int CDlgConfig::OnInitDlg (void)
{
	char * szDays[] = {"5", "10", "20", "30", "60", "120", "180", "240", "360"};
	for (int i = 0; i < 9; i++)
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE5), CB_ADDSTRING, NULL, (LPARAM)szDays[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE10), CB_ADDSTRING, NULL, (LPARAM)szDays[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE20), CB_ADDSTRING, NULL, (LPARAM)szDays[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE30), CB_ADDSTRING, NULL, (LPARAM)szDays[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE60), CB_ADDSTRING, NULL, (LPARAM)szDays[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE120), CB_ADDSTRING, NULL, (LPARAM)szDays[i]);
	}

	char *pDays = CRegMng::g_pRegMng->GetTextValue ("Line5Days");
	if (strlen (pDays) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE5), pDays);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE5), CB_SETCURSEL, 0, NULL);
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line10Days");
	if (strlen (pDays) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE10), pDays);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE10), CB_SETCURSEL, 1, NULL);
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line20Days");
	if (strlen (pDays) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE20), pDays);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE20), CB_SETCURSEL, 2, NULL);
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line30Days");
	if (strlen (pDays) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE30), pDays);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE30), CB_SETCURSEL, 3, NULL);
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line60Days");
	if (strlen (pDays) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE60), pDays);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE60), CB_SETCURSEL, 4, NULL);
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line120Days");
	if (strlen (pDays) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LINE120), pDays);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LINE120), CB_SETCURSEL, 5, NULL);

	int nCheck = CRegMng::g_pRegMng->GetIntValue ("Line5Show", 1);
	if (nCheck > 0)
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE5), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE5), BM_SETCHECK, BST_UNCHECKED, 0);
	nCheck = CRegMng::g_pRegMng->GetIntValue ("Line10Show", 1);
	if (nCheck > 0)
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE10), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE10), BM_SETCHECK, BST_UNCHECKED, 0);
	nCheck = CRegMng::g_pRegMng->GetIntValue ("Line20Show", 1);
	if (nCheck > 0)
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE20), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE20), BM_SETCHECK, BST_UNCHECKED, 0);
	nCheck = CRegMng::g_pRegMng->GetIntValue ("Line30Show", 1);
	if (nCheck > 0)
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE30), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE30), BM_SETCHECK, BST_UNCHECKED, 0);
	nCheck = CRegMng::g_pRegMng->GetIntValue ("Line60Show", 1);
	if (nCheck > 0)
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE60), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE60), BM_SETCHECK, BST_UNCHECKED, 0);
	nCheck = CRegMng::g_pRegMng->GetIntValue ("Line120Show", 1);
	if (nCheck > 0)
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE120), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LINE120), BM_SETCHECK, BST_UNCHECKED, 0);

	int nChuFuQuan = CRegMng::g_pRegMng->GetIntValue ("ChuFuQuan", 1);
	SendMessage (GetDlgItem (m_hDlg, IDC_RADIO_NO), BM_SETCHECK, BST_UNCHECKED, 0);
	SendMessage (GetDlgItem (m_hDlg, IDC_RADIO_PREV), BM_SETCHECK, BST_UNCHECKED, 0);
	SendMessage (GetDlgItem (m_hDlg, IDC_RADIO_NEXT), BM_SETCHECK, BST_UNCHECKED, 0);
	if (nChuFuQuan == 0)
		SendMessage (GetDlgItem (m_hDlg, IDC_RADIO_NO), BM_SETCHECK, BST_CHECKED, 0);
	else if (nChuFuQuan == 1)
		SendMessage (GetDlgItem (m_hDlg, IDC_RADIO_PREV), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage (GetDlgItem (m_hDlg, IDC_RADIO_NEXT), BM_SETCHECK, BST_CHECKED, 0);
	CenterDlg ();
	return QC_ERR_NONE;
}

INT_PTR CDlgConfig::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
