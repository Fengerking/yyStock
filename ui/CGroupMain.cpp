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
	, m_pViewSEL(NULL)
	, m_pViewRTI(NULL)
{
}

CGroupMain::~CGroupMain(void)
{
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
	if (m_pViewSEL == NULL)
		m_pViewSEL = new CViewSelList(m_hInst);
	if (m_pViewRTI == NULL)
		m_pViewRTI = new CViewRTInfo(m_hInst);

	double	dSplt = 0.8;
	RECT rcView;
	SetRect(&rcView, 0, 0, rcWnd.right - m_pViewRTI->GetWndWidth(), (int)(rcWnd.bottom * dSplt));
	m_pViewFST->CreateWnd(m_hMainWnd, rcView, MSC_BLACK);

	SetRect(&rcView, rcWnd.right - m_pViewRTI->GetWndWidth(), 0, rcWnd.right, rcWnd.bottom);
	m_pViewRTI->CreateWnd(m_hMainWnd, rcView, MSC_BLACK);

	SetRect(&rcView, 0, (int)(rcWnd.bottom * dSplt), rcWnd.right - m_pViewRTI->GetWndWidth(), rcWnd.bottom);
	m_pViewSEL->CreateWnd(m_hMainWnd, rcView, MSC_BLACK);

	return QC_ERR_NONE;
}

