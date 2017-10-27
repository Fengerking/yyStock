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
