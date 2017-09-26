/*******************************************************************************
	File:		CWndGrpMng.cpp

	Contains:	base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"
#include "qcErr.h"

#include "CWndGrpMng.h"

#include "CIOcurl.h"
#include "CDlgConfig.h"
#include "CDlgDownLoad.h"

#include "resource.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

CWndGrpMng::CWndGrpMng(HINSTANCE hInst)
	: m_hInst (hInst)
	, m_hMainWnd (NULL)
	, m_pLstStock(NULL)
	, m_pGrpMain(NULL)
	, m_pViewKXT(NULL)
	, m_pViewCode(NULL)
{
	SetObjectName("CWndGrpMng");

	m_pRegMng = new CRegMng("Setting");
	m_pLstStock = new CStockItemList();
	m_pLstStock->OpenFileList();
}

CWndGrpMng::~CWndGrpMng(void)
{
	QC_DEL_P(m_pGrpMain);
	QC_DEL_P(m_pViewKXT);

	QC_DEL_P(m_pViewCode);

	QC_DEL_P(m_pRegMng);
	QC_DEL_P(m_pLstStock);
}

int	CWndGrpMng::CreateWnd (HWND hWnd)
{
	m_hMainWnd = hWnd;
	SetWindowText(m_hMainWnd, "ÑôÑôÖ¤È¯");

	RECT	rcWnd;
	GetClientRect(m_hMainWnd, &rcWnd);
	if (m_pViewCode == NULL)
		m_pViewCode = new CViewCode(m_hInst);

	if (m_pGrpMain == NULL)
		m_pGrpMain = new CGroupMain(m_hInst);
	m_pGrpMain->CreateWnd(m_hMainWnd);

	if (m_pViewKXT == NULL)
		m_pViewKXT = new CViewKXT(m_hInst);
//	m_pViewKXT->CreateWnd(m_hMainWnd, rcWnd, MSC_BLACK);

	m_pViewCode->CreateWnd(m_hMainWnd, rcWnd, MSC_BLACK);

	SetWindowPos(m_pViewCode->GetWnd (), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return QC_ERR_NONE;
}

LRESULT CWndGrpMng::OnReceiveMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT	nRC = S_FALSE;
	int wmId, wmEvent;
	switch (uMsg)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_CONFIG:
		{
			CDlgConfig	dlgConfig(m_hInst, m_hMainWnd);
			if (dlgConfig.OpenDlg() == IDOK)
			{
				if (m_pViewCode != NULL)
				{
					PostMessage(m_pViewCode->GetWnd(), WM_MSG_CODE_CHANGE, (WPARAM)m_pViewCode->GetCode(), 0);
				}
			}
			break;
		}
		case ID_FILE_DOWNLOAD:
		{
			CDlgDownLoad dlgDownLoad(m_hInst, m_hMainWnd);
			dlgDownLoad.OpenDlg();
			break;
		}
		default:
			break;
		}
		break;

	case WM_MSG_CODE_REGIST:
		if (m_pViewCode != NULL)
			m_pViewCode->RegistWindow((CWndBase *)wParam);
		return S_OK;

	case WM_MSG_CODE_REMOVE:
		if (m_pViewCode != NULL)
			m_pViewCode->RemoveWindow((CWndBase *)wParam);
		return S_OK;

	case WM_MSG_CODE_CHANGE:
		if (m_pViewCode != NULL)
			SendMessage (m_pViewCode->GetWnd (), uMsg, wParam, lParam);
		return S_OK;

	case WM_MSG_CODE_REQUEST:
		if (m_pViewCode != NULL)
			strcpy((char *)wParam, m_pViewCode->GetCode());
		return S_OK;

	case WM_SIZE:
		nRC = OnResize(hWnd, uMsg, wParam, lParam);
		break;

	case WM_KEYUP:
		nRC = OnKeyUp(hWnd, uMsg, wParam, lParam);
		break;

	case WM_KEYDOWN:
		nRC = OnKeyDown(hWnd, uMsg, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		nRC = OnMouseMove(hWnd, uMsg, wParam, lParam);
		break;

	case WM_LBUTTONUP:
		nRC = OnMouseUp(hWnd, uMsg, wParam, lParam);
		break;

	case WM_LBUTTONDOWN:
		nRC = OnMouseDown(hWnd, uMsg, wParam, lParam);
		break;

	default:
		return S_FALSE;
	}
	return nRC;
}

LRESULT CWndGrpMng::OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rcWnd;
	GetClientRect(m_hMainWnd, &rcWnd);
	if (m_pViewKXT != NULL)
	{
		SetWindowPos(m_pViewKXT->GetWnd(), HWND_BOTTOM, rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, 0);
		m_pViewKXT->OnResize(hWnd, uMsg, wParam, lParam);
	}
	if (m_pGrpMain != NULL)
	{
		m_pGrpMain->OnResize(hWnd, uMsg, wParam, lParam);
	}
	if (m_pViewCode != NULL)
	{
		m_pViewCode->OnResize(hWnd, uMsg, wParam, lParam);
	}
	return S_OK;
}

LRESULT CWndGrpMng::OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pViewCode != NULL)
		m_pViewCode->OnKeyUp(hWnd, uMsg, wParam, lParam);
	if (m_pViewKXT != NULL)
		m_pViewKXT->OnKeyUp(hWnd, uMsg, wParam, lParam);

	return S_OK;
}

LRESULT CWndGrpMng::OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CWndGrpMng::OnMouseDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CWndGrpMng::OnMouseUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CWndGrpMng::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}
