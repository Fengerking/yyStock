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

#include "resource.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

CWndGrpMng::CWndGrpMng(HINSTANCE hInst)
	: m_hInst (hInst)
	, m_hMainWnd (NULL)
	, m_pLstStock(NULL)
	, m_pGrpMain(NULL)
	, m_pViewCode(NULL)
	, m_pDlgDownLoad(NULL)
{
	SetObjectName("CWndGrpMng");

	m_pRegMng = new CRegMng("Setting");
	m_pLstStock = new CStockItemList();
	m_pLstStock->OpenFileList();
//	m_pLstStock->OpenHttpList();
}

CWndGrpMng::~CWndGrpMng(void)
{
	QC_DEL_P(m_pGrpMain);
	QC_DEL_P(m_pViewCode);

	QC_DEL_P(m_pRegMng);
	QC_DEL_P(m_pLstStock);

	QC_DEL_P(m_pDlgDownLoad);
}

int	CWndGrpMng::CreateWnd (HWND hWnd)
{
	m_hMainWnd = hWnd;
	SetWindowText(m_hMainWnd, "ÑôÑôÖ¤È¯");

	RECT	rcWnd;
	GetClientRect(m_hMainWnd, &rcWnd);
	if (m_pViewCode == NULL)
		m_pViewCode = new CViewCode(m_hInst);
	m_pViewCode->CreateWnd(m_hMainWnd, rcWnd, MSC_BLACK, NULL);

	if (m_pGrpMain == NULL)
		m_pGrpMain = new CGroupMain(m_hInst);
	m_pGrpMain->CreateWnd(m_hMainWnd, NULL);

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
			if (m_pDlgDownLoad == NULL)
				m_pDlgDownLoad = new CDlgDownLoad(m_hInst, m_hMainWnd);
			m_pDlgDownLoad->CreateDlg();
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

	case WM_MSG_CHILDWND_MSG:
	{
		nRC = S_FALSE;
		qcChildWndMsg * pMsg = (qcChildWndMsg *)wParam;
		if (pMsg->m_uMsg == WM_MOUSEMOVE)
			nRC = OnMouseMove(pMsg->m_hWnd, pMsg->m_uMsg, pMsg->m_wParam, pMsg->m_lParam);
		else if (pMsg->m_uMsg == WM_LBUTTONDOWN)
			nRC = OnMouseDown (pMsg->m_hWnd, pMsg->m_uMsg, pMsg->m_wParam, pMsg->m_lParam);
		else if (pMsg->m_uMsg == WM_LBUTTONUP)
			nRC = OnMouseUp (pMsg->m_hWnd, pMsg->m_uMsg, pMsg->m_wParam, pMsg->m_lParam);
		return nRC;
	}

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

	case WM_MOUSEWHEEL:
		nRC = OnMouseWheel(hWnd, uMsg, wParam, lParam);
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
	if (m_pGrpMain != NULL)
		m_pGrpMain->OnResize(hWnd, uMsg, wParam, lParam);
	if (m_pViewCode != NULL)
		m_pViewCode->OnResize(hWnd, uMsg, wParam, lParam);
	return S_OK;
}

LRESULT CWndGrpMng::OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRC = S_FALSE;
	if (m_pViewCode != NULL)
		lRC = m_pViewCode->OnKeyUp(hWnd, uMsg, wParam, lParam);
	if (m_pGrpMain != NULL)
		lRC = m_pGrpMain->OnKeyUp(hWnd, uMsg, wParam, lParam);

	return lRC;
}

LRESULT CWndGrpMng::OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CWndGrpMng::OnMouseDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRC = S_FALSE;
	if (m_pGrpMain != NULL)
		lRC = m_pGrpMain->OnMouseDown(hWnd, uMsg, wParam, lParam);
	return lRC;
}

LRESULT CWndGrpMng::OnMouseUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRC = S_FALSE;
	if (m_pGrpMain != NULL)
		lRC = m_pGrpMain->OnMouseUp(hWnd, uMsg, wParam, lParam);
	return lRC;
}

LRESULT CWndGrpMng::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRC = S_FALSE;
	if (m_pGrpMain != NULL)
		lRC = m_pGrpMain->OnMouseMove(hWnd, uMsg, wParam, lParam);
	return lRC;
}

LRESULT CWndGrpMng::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRC = S_FALSE;
	if (m_pGrpMain != NULL)
		lRC = m_pGrpMain->OnMouseWheel(hWnd, uMsg, wParam, lParam);
	return lRC;
}
