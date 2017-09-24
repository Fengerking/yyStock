/*******************************************************************************
	File:		CViewCode.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "qcErr.h"

#include "CViewCode.h"

#include "ULogFunc.h"
#include "USystemFunc.h"

CViewCode::CViewCode(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_nTimerHide(0)
{
	_tcscpy (m_szClassName, _T("yyStockViewCode"));
	_tcscpy (m_szWindowName, _T("yyStockViewCode"));

	memset(m_szKeyCode, 0, sizeof(m_szKeyCode));
	strcpy(m_szKeyCode, "600895");
}

CViewCode::~CViewCode(void)
{
}

int CViewCode::UpdateView (HDC hDC)
{
	if (strlen(m_szKeyCode) <= 0)
		return QC_ERR_NONE;

	DrawRect(hDC, &m_rcWnd, 5, MSC_GRAY_4);

	SetBkMode(hDC, TRANSPARENT);
	DrawStrText(hDC, m_szKeyCode, m_hFntLrg, yyViewCode_Width / 2, 0, MSC_WHITE, 2);

	return QC_ERR_NONE;
}

bool CViewCode::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	CBaseGraphics::OnCreateWnd(hParent);

	RECT	rcWnd;
	GetClientRect(hParent, &rcWnd);

	rcView.left = (rcWnd.right - yyViewCode_Width) - 72;
	rcView.top = (rcWnd.bottom - yyViewCode_Height) - 36;
	rcView.right = rcView.left + yyViewCode_Width;
	rcView.bottom = rcView.top + yyViewCode_Height;

	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;

	ShowWindow(m_hWnd, SW_HIDE);

	return true;
}

LRESULT CViewCode::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:
		if (m_nTimerHide != 0)
			KillTimer(m_hWnd, m_nTimerHide);
		m_nTimerHide = 0;
		ShowWindow(m_hWnd, SW_HIDE);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		UpdateView (hdc);
		EndPaint(hwnd, &ps);
	}
		break;

//	case WM_ERASEBKGND:
//		return S_FALSE;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

int CViewCode::RegistWindow(CWndBase * pWnd)
{
	m_lstWnds.AddTail(pWnd);
	return QC_ERR_NONE;
}

int	CViewCode::RemoveWindow(CWndBase * pWnd)
{
	m_lstWnds.Remove(pWnd);
	return QC_ERR_NONE;
}

LRESULT CViewCode::OnResize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CViewCode::OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	long	nLen = strlen(m_szKeyCode);
	if ((wParam == VK_DELETE || wParam == VK_BACK) && nLen > 0)
	{
		m_szKeyCode[nLen - 1] = 0;
		return true;
	}

	char cKey = '0';
	if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)
		cKey = wParam - VK_NUMPAD0 + '0';
	else if (wParam >= '0' && wParam <= '9')
		cKey = (char)wParam;
	else
		return false;

	if (!IsWindowVisible(m_hWnd))
		ShowWindow(m_hWnd, SW_SHOW);
	if (m_nTimerHide != 0)
		KillTimer(m_hWnd, m_nTimerHide);

	if (nLen >= 6)
	{
		m_szKeyCode[0] = cKey;
		m_szKeyCode[1] = 0;
	}
	else
	{
		m_szKeyCode[nLen] = cKey;
		m_szKeyCode[nLen + 1] = 0;
	}
	if (nLen + 1 == 6)
	{
		strcpy(m_szCode, m_szKeyCode);
		CWndBase * pWnd = NULL;
		NODEPOS pos = m_lstWnds.GetHeadPosition();
		while (pos != NULL)
		{
			pWnd = m_lstWnds.GetNext(pos);
			PostMessage(pWnd->GetWnd(), WM_MSG_CODE_CHANGE, (WPARAM)m_szCode, 0);
		}
		m_nTimerHide = SetTimer(m_hWnd, WM_TIMER_VC_HIDE, 1000, NULL);
	}
	else
	{
		m_nTimerHide = SetTimer(m_hWnd, WM_TIMER_VC_HIDE, 3000, NULL);
	}

	InvalidateRect(m_hWnd, NULL, TRUE);

	return true;
}
