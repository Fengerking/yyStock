/*******************************************************************************
	File:		CGroupMain.cpp

	Contains:	base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"
#include "qcErr.h"

#include "CGroupMain.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

CGroupMain::CGroupMain(HINSTANCE hInst)
	: CGroupBase(hInst)
	, m_pGroupStock(NULL)
	, m_pViewSEL(NULL)
	, m_pViewRTI(NULL)
	, m_pViewList(NULL)
	, m_dSplt(0.8)
	, m_hCursorSize(NULL)
	, m_hCursorArrow(NULL)
	, m_bResize(false)
{
	SetObjectName("CGroupMain");
}

CGroupMain::~CGroupMain(void)
{
	QC_DEL_P(m_pGroupStock);
	QC_DEL_P(m_pViewSEL);
	QC_DEL_P(m_pViewRTI);
	QC_DEL_P(m_pViewList);
}

int	CGroupMain::CreateWnd(HWND hWnd, RECT * pRect)
{
	CGroupBase::CreateWnd(hWnd, pRect);

	m_hCursorSize = LoadCursor(NULL, IDC_SIZENS);
	m_hCursorArrow = LoadCursor(NULL, IDC_ARROW);

	if (m_pGroupStock == NULL)
		m_pGroupStock = new CGroupStock(m_hInst);
	if (m_pViewSEL == NULL)
		m_pViewSEL = new CViewSelList(m_hInst);
	if (m_pViewRTI == NULL)
		m_pViewRTI = new CViewRTInfo(m_hInst);
	if (m_pViewList == NULL)
		m_pViewList = new CViewCodeList(m_hInst);

	OnResize(m_hMainWnd, WM_SIZE, 0, 0);

	return QC_ERR_NONE;
}

int	CGroupMain::ShowViews(int nShow)
{
	if (m_pGroupStock != NULL)
		m_pGroupStock->ShowViews(nShow);
	if (m_pViewRTI != NULL && m_pViewRTI->GetWnd() != NULL)
		ShowWindow(m_pViewRTI->GetWnd(), nShow);
	if (m_pViewSEL != NULL && m_pViewSEL->GetWnd() != NULL)
		ShowWindow(m_pViewSEL->GetWnd(), nShow);

	return QC_ERR_NONE;
}

LRESULT CGroupMain::OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT	rcWnd;
	RECT	rcView;
	int		nListWidth = 240;
	GetClientRect(m_hMainWnd, &rcWnd);
	if (m_pGroupStock != NULL)
	{
		SetRect(&rcView, 0, 0, rcWnd.right - m_pViewRTI->GetWndWidth() - nListWidth, (int)(rcWnd.bottom * m_dSplt));
		if (m_pGroupStock->GetWnd () == NULL)
			m_pGroupStock->CreateWnd(m_hMainWnd, &rcView);
		else
			m_pGroupStock->OnResize(hWnd, uMsg, (WPARAM)&rcView, lParam);
	}
	if (m_pViewList != NULL)
	{
		SetRect(&rcView, rcWnd.right - m_pViewRTI->GetWndWidth() - nListWidth, 0, rcWnd.right - m_pViewRTI->GetWndWidth(), (int)(rcWnd.bottom * m_dSplt));
		if (m_pViewList->GetWnd() == NULL)
			m_pViewList->CreateWnd(m_hMainWnd, rcView, MSC_BLACK, this);
		else
			SetWindowPos(m_pViewList->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	}
	if (m_pViewRTI != NULL)
	{
		SetRect(&rcView, rcWnd.right - m_pViewRTI->GetWndWidth(), 0, rcWnd.right, rcWnd.bottom);
		if (m_pViewRTI->GetWnd () == NULL)
			m_pViewRTI->CreateWnd(m_hMainWnd, rcView, MSC_BLACK, this);
		else
			SetWindowPos(m_pViewRTI->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	}
	if (m_pViewSEL != NULL)
	{
		SetRect(&rcView, 0, (int)(rcWnd.bottom * m_dSplt), rcWnd.right - m_pViewRTI->GetWndWidth(), rcWnd.bottom);
		if (m_pViewSEL->GetWnd() == NULL)
			m_pViewSEL->CreateWnd(m_hMainWnd, rcView, MSC_BLACK, this);
		else
			SetWindowPos(m_pViewSEL->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	}

	return S_OK;
}

LRESULT CGroupMain::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRC = S_FALSE;
	if (m_pGroupStock != NULL)
		lRC = m_pGroupStock->OnMouseWheel(hWnd, uMsg, wParam, lParam);
	if (lRC == S_OK)
		return lRC;
	if (m_pViewList != NULL)
		lRC = m_pViewList->OnMouseWheel(hWnd, uMsg, wParam, lParam);
	if (lRC == S_OK)
		return lRC;
	if (m_pViewSEL != NULL)
		lRC = m_pViewSEL->OnMouseWheel(hWnd, uMsg, wParam, lParam);
	if (lRC == S_OK)
		return lRC;
	if (m_pViewRTI != NULL)
		lRC = m_pViewRTI->OnMouseWheel(hWnd, uMsg, wParam, lParam);

	return S_OK;
}

LRESULT CGroupMain::OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pGroupStock != NULL)
		m_pGroupStock->OnKeyUp(hWnd, uMsg, wParam, lParam);
	return S_FALSE;
}

LRESULT CGroupMain::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);
	if (m_bResize && wParam == MK_LBUTTON)
	{
		SetCursor(m_hCursorSize);
		RECT rcView;
		GetClientRect(hWnd, &rcView);
		InvalidateRect(hWnd, NULL, TRUE);
		HDC hDC = GetDC(hWnd);
		if (m_pViewSEL != NULL)
			m_pViewSEL->DrawLine(hDC, rcView.left, yPos - 2, rcView.right, yPos + 2, 4, MSC_CYAN);
		ReleaseDC(hWnd, hDC);
		return S_OK;
	}

	if (m_pViewSEL == NULL || m_pViewSEL->GetWnd() == NULL || m_hCursorSize == NULL)
		return S_FALSE;
	m_bResize = false;
	if (hWnd != m_pViewSEL->GetWnd())
	{
		SetCursor(m_hCursorArrow);
		return S_FALSE;
	}

	if (yPos < 16)
	{
		m_bResize = true;
		SetCursor(m_hCursorSize);
	}
	else
	{
		SetCursor(m_hCursorArrow);
	}

	return S_OK;
}

LRESULT CGroupMain::OnMouseDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bResize == true)
	{
		SetCursor(m_hCursorSize);
		return S_OK;
	}

	return S_FALSE;
}

LRESULT CGroupMain::OnMouseUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bResize == true)
	{
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		RECT rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		yPos = rcWnd.top + yPos;
		RECT rcMain;
		GetWindowRect(m_hMainWnd, &rcMain);

		m_dSplt = (double)yPos / (rcMain.bottom - rcMain.top);

		OnResize(NULL, 0, 0, 0);

		m_bResize = false;
		SetCursor(m_hCursorArrow);
		return S_OK;
	}
		
	return S_FALSE;
}
