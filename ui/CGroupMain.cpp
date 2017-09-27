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
	, m_dSplt(0.8)
{
	SetObjectName("CGroupMain");
}

CGroupMain::~CGroupMain(void)
{
	QC_DEL_P(m_pGroupStock);
	QC_DEL_P(m_pViewSEL);
	QC_DEL_P(m_pViewRTI);
}

int	CGroupMain::CreateWnd(HWND hWnd, RECT * pRect)
{
	m_hMainWnd = hWnd;

	RECT	rcWnd;
	GetClientRect(m_hMainWnd, &rcWnd);

	if (m_pGroupStock == NULL)
		m_pGroupStock = new CGroupStock(m_hInst);
	if (m_pViewSEL == NULL)
		m_pViewSEL = new CViewSelList(m_hInst);
	if (m_pViewRTI == NULL)
		m_pViewRTI = new CViewRTInfo(m_hInst);

	RECT rcView;
	SetRect(&rcView, 0, 0, rcWnd.right - m_pViewRTI->GetWndWidth(), (int)(rcWnd.bottom * m_dSplt));
	m_pGroupStock->CreateWnd(m_hMainWnd, &rcView);

	SetRect(&rcView, rcWnd.right - m_pViewRTI->GetWndWidth(), 0, rcWnd.right, rcWnd.bottom);
	m_pViewRTI->CreateWnd(m_hMainWnd, rcView, MSC_BLACK, this);

	SetRect(&rcView, 0, (int)(rcWnd.bottom * m_dSplt), rcWnd.right - m_pViewRTI->GetWndWidth(), rcWnd.bottom);
	m_pViewSEL->CreateWnd(m_hMainWnd, rcView, MSC_BLACK, this);

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
	GetClientRect(m_hMainWnd, &rcWnd);
	RECT rcView;
	SetRect(&rcView, 0, 0, rcWnd.right - m_pViewRTI->GetWndWidth(), (int)(rcWnd.bottom * m_dSplt));
	if (m_pGroupStock != NULL)
		m_pGroupStock->OnResize(hWnd, uMsg, (WPARAM)&rcView, lParam);

	if (m_pViewRTI != NULL && m_pViewRTI->GetWnd() != NULL)
	{
		SetRect(&rcView, rcWnd.right - m_pViewRTI->GetWndWidth(), 0, rcWnd.right, rcWnd.bottom);
		SetWindowPos(m_pViewRTI->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	}
	if (m_pViewSEL != NULL && m_pViewSEL->GetWnd() != NULL)
	{
		SetRect(&rcView, 0, (int)(rcWnd.bottom * m_dSplt), rcWnd.right - m_pViewRTI->GetWndWidth(), rcWnd.bottom);
		SetWindowPos(m_pViewSEL->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	}

	return S_OK;
}

LRESULT CGroupMain::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pGroupStock != NULL)
		m_pGroupStock->OnMouseWheel(hWnd, uMsg, wParam, lParam);
	return S_OK;
}

LRESULT CGroupMain::OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pGroupStock != NULL)
		m_pGroupStock->OnKeyUp(hWnd, uMsg, wParam, lParam);
	return S_FALSE;
}
