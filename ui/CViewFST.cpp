/*******************************************************************************
	File:		CViewFST.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "qcErr.h"

#include "CViewFST.h"

#include "ULogFunc.h"
#include "USystemFunc.h"

CViewFST::CViewFST(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_pPngDec(NULL)
{
	SetObjectName("CViewFST");

	_tcscpy (m_szClassName, _T("yyStockViewFST"));
	_tcscpy (m_szWindowName, _T("yyStockViewFST"));
}

CViewFST::~CViewFST(void)
{
	ThreadStop();

	SendMessage(m_hParent, WM_MSG_CODE_REMOVE, (WPARAM)this, 0);

	QC_DEL_P(m_pPngDec);
}

int CViewFST::UpdateView (HDC hDC)
{
	if (m_pPngDec == NULL)
		return QC_ERR_FAILED;

	RECT rcView;
	GetClientRect(m_hWnd, &rcView);
	HBITMAP hBmp = m_pPngDec->GetBmp();
	if (hBmp != NULL)
	{
		if (m_hMemDC == NULL)
			m_hMemDC = CreateCompatibleDC(hDC);
		HBITMAP hOld = (HBITMAP)SelectObject(m_hMemDC, hBmp);
		StretchBlt(hDC, 0, 0, rcView.right, rcView.bottom, m_hMemDC, 0, 0, m_pPngDec->GetWidth(), m_pPngDec->GetHeight(), SRCCOPY);
		SelectObject(m_hMemDC, hOld);
	}
	DrawRect(hDC, &rcView, 4, MSC_WHITE);
	return QC_ERR_NONE;
}

int CViewFST::UpdateInfo(void)
{
	if (!qcIsTradeTime())
	{
		if (m_pPngDec != NULL && m_pPngDec->GetBmp() != NULL)
			return QC_ERR_FAILED;
	}

	char szURL[256];
	if (strlen(m_szCode) == 7)
		sprintf(szURL, "http://img1.money.126.net/chart/hs/time/540x360/%s.png", m_szCode);
	else if (m_szCode[0] == '6')
		sprintf(szURL, "http://img1.money.126.net/chart/hs/time/540x360/0%s.png", m_szCode);
	else
		sprintf(szURL, "http://img1.money.126.net/chart/hs/time/540x360/1%s.png", m_szCode);

	if (m_pPngDec == NULL)
		m_pPngDec = new CPngDec();
	if (m_pPngDec->OpenSource(szURL) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	InvalidateRect(m_hWnd, NULL, FALSE);
	return QC_ERR_NONE;
}

bool CViewFST::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;

	SendMessage(m_hParent, WM_MSG_CODE_REGIST, (WPARAM)this, 0);

	if (1)
	{
		ThreadStart();
	}
	else
	{
		UpdateInfo();
		SetTimer(m_hWnd, WM_TIMER_UPDATE, m_nUpdateTime, NULL);
	}

	return true;
}

LRESULT CViewFST::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MSG_CODE_CHANGE:
		strcpy(m_szCode, (char *)wParam);
		if (m_pPngDec != NULL)
			m_pPngDec->Close();
		UpdateInfo();
		return S_OK;

	case WM_TIMER:
		UpdateInfo();
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		UpdateView (hdc);
		EndPaint(hWnd, &ps);
	}
		break;

	case WM_ERASEBKGND:
		return S_FALSE;

	case WM_DESTROY:
		break;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hWnd, uMsg, wParam, lParam);
}

