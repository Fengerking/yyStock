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
	: m_hInst (hInst)
	, m_hMainWnd (NULL)
	, m_pViewFST(NULL)
	, m_pViewKXT(NULL)
	, m_pViewSEL(NULL)
	, m_pViewRTI(NULL)
{
}

CGroupMain::~CGroupMain(void)
{
	QC_DEL_P(m_pViewFST);
	QC_DEL_P(m_pViewFST);
	QC_DEL_P(m_pViewSEL);
	QC_DEL_P(m_pViewRTI);
}

int	CGroupMain::CreateWnd (HWND hWnd)
{
	m_hMainWnd = hWnd;

	RECT	rcWnd;
	GetClientRect(m_hMainWnd, &rcWnd);

	if (m_pViewFST == NULL)
		m_pViewFST = new CViewFST(m_hInst);
	if (m_pViewKXT == NULL)
		m_pViewKXT = new CViewKXT(m_hInst);
	if (m_pViewSEL == NULL)
		m_pViewSEL = new CViewSelList(m_hInst);
	if (m_pViewRTI == NULL)
		m_pViewRTI = new CViewRTInfo(m_hInst);

	double	dSplt = 0.8;
	RECT rcView;
	SetRect(&rcView, 0, 0, rcWnd.right - m_pViewRTI->GetWndWidth(), (int)(rcWnd.bottom * dSplt));
	m_pViewFST->CreateWnd(m_hMainWnd, rcView, MSC_BLACK);
	ShowWindow(m_pViewFST->GetWnd(), SW_HIDE);
	m_pViewKXT->CreateWnd(m_hMainWnd, rcView, MSC_BLACK);

	SetRect(&rcView, rcWnd.right - m_pViewRTI->GetWndWidth(), 0, rcWnd.right, rcWnd.bottom);
	m_pViewRTI->CreateWnd(m_hMainWnd, rcView, MSC_BLACK);

	SetRect(&rcView, 0, (int)(rcWnd.bottom * dSplt), rcWnd.right - m_pViewRTI->GetWndWidth(), rcWnd.bottom);
	m_pViewSEL->CreateWnd(m_hMainWnd, rcView, MSC_BLACK);

	return QC_ERR_NONE;
}

int	CGroupMain::ShowViews(int nShow)
{
	if (m_pViewFST != NULL && m_pViewFST->GetWnd() != NULL)
		ShowWindow(m_pViewFST->GetWnd(), nShow);
	if (m_pViewKXT != NULL && m_pViewKXT->GetWnd() != NULL)
		ShowWindow(m_pViewKXT->GetWnd(), nShow);
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
	double	dSplt = 0.8;
	RECT rcView;
	if (m_pViewFST != NULL && m_pViewFST->GetWnd() != NULL)
	{
		SetRect(&rcView, 0, 0, rcWnd.right - m_pViewRTI->GetWndWidth(), (int)(rcWnd.bottom * dSplt));
		SetWindowPos(m_pViewFST->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right, rcView.bottom, 0);
	}
	if (m_pViewKXT != NULL && m_pViewKXT->GetWnd() != NULL)
	{
		SetRect(&rcView, 0, 0, rcWnd.right - m_pViewRTI->GetWndWidth(), (int)(rcWnd.bottom * dSplt));
		SetWindowPos(m_pViewKXT->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right, rcView.bottom, 0);
	}
	if (m_pViewRTI != NULL && m_pViewRTI->GetWnd() != NULL)
	{
		SetRect(&rcView, rcWnd.right - m_pViewRTI->GetWndWidth(), 0, rcWnd.right, rcWnd.bottom);
		SetWindowPos(m_pViewRTI->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	}
	if (m_pViewSEL != NULL && m_pViewSEL->GetWnd() != NULL)
	{
		SetRect(&rcView, 0, (int)(rcWnd.bottom * dSplt), rcWnd.right - m_pViewRTI->GetWndWidth(), rcWnd.bottom);
		SetWindowPos(m_pViewSEL->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	}

	return S_OK;
}
