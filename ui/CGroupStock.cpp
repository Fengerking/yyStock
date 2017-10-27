/*******************************************************************************
	File:		CGroupStock.cpp

	Contains:	base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"
#include "qcErr.h"

#include "CGroupStock.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

CGroupStock::CGroupStock(HINSTANCE hInst)
	: CGroupBase(hInst)
	, m_pViewFST(NULL)
	, m_pViewKXT(NULL)
	, m_pViewInfo(NULL)
	, m_nShowType(0)
{
	SetObjectName("CGroupStock");
}

CGroupStock::~CGroupStock(void)
{
	QC_DEL_P(m_pViewFST);
	QC_DEL_P(m_pViewFST);
	QC_DEL_P(m_pViewInfo);
}

int	CGroupStock::CreateWnd(HWND hWnd, RECT * pRect)
{
	CGroupBase::CreateWnd(hWnd, pRect);

	RECT	rcWnd;
	if (pRect == NULL)
		GetClientRect(m_hMainWnd, &rcWnd);
	else
		memcpy(&rcWnd, pRect, sizeof(RECT));

	if (m_pViewFST == NULL)
		m_pViewFST = new CViewFST(m_hInst);
	if (m_pViewKXT == NULL)
		m_pViewKXT = new CViewKXT(m_hInst);
	if (m_pViewInfo == NULL)
		m_pViewInfo = new CViewInfo(m_hInst);

	m_pViewFST->CreateWnd(m_hMainWnd, rcWnd, MSC_BLACK, this);
	ShowWindow(m_pViewFST->GetWnd(), SW_HIDE);
	m_pViewKXT->CreateWnd(m_hMainWnd, rcWnd, MSC_BLACK, this);
	ShowWindow(m_pViewKXT->GetWnd(), SW_HIDE);
	m_pViewInfo->CreateWnd(m_hMainWnd, rcWnd, MSC_BLACK, this);
	ShowWindow(m_pViewInfo->GetWnd(), SW_HIDE);

	ShowType();

	return QC_ERR_NONE;
}

int	CGroupStock::ShowViews(int nShow)
{
	if (m_pViewFST != NULL && m_pViewFST->GetWnd() != NULL)
		ShowWindow(m_pViewFST->GetWnd(), nShow);
	if (m_pViewKXT != NULL && m_pViewKXT->GetWnd() != NULL)
		ShowWindow(m_pViewKXT->GetWnd(), nShow);
	if (m_pViewInfo != NULL && m_pViewInfo->GetWnd() != NULL)
		ShowWindow(m_pViewInfo->GetWnd(), nShow);

	return QC_ERR_NONE;
}

LRESULT CGroupStock::OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rcView;
	if (wParam == NULL)
		GetClientRect(m_hMainWnd, &rcView);
	else
		memcpy(&rcView, (void *)wParam, sizeof(RECT));

	if (m_pViewFST != NULL && m_pViewFST->GetWnd() != NULL)
	{
		SetWindowPos(m_pViewFST->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right, rcView.bottom, 0);
	}
	if (m_pViewKXT != NULL && m_pViewKXT->GetWnd() != NULL)
	{
		SetWindowPos(m_pViewKXT->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right, rcView.bottom, 0);
	}
	if (m_pViewInfo != NULL && m_pViewInfo->GetWnd() != NULL)
	{
		SetWindowPos(m_pViewInfo->GetWnd(), HWND_BOTTOM, rcView.left, rcView.top, rcView.right, rcView.bottom, 0);
	}

	return S_OK;
}

LRESULT CGroupStock::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (qcInRect(LOWORD(lParam), HIWORD(lParam), &m_rcView) <= 0)
		return S_FALSE;

	m_nShowType = m_nShowType % 3;
	ShowType();

	m_nShowType++;
	if (m_nShowType > 3)
		m_nShowType = 0;
	return S_OK;
}

LRESULT CGroupStock::OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pViewKXT != NULL)
		m_pViewKXT->OnKeyUp(hWnd, uMsg, wParam, lParam);
	return S_FALSE;
}

int CGroupStock::ShowType(void)
{
	if (m_pViewFST != NULL)
		ShowWindow(m_pViewFST->GetWnd(), SW_HIDE);
	if (m_pViewKXT != NULL)
		ShowWindow(m_pViewKXT->GetWnd(), SW_HIDE);
	if (m_pViewInfo != NULL)
		ShowWindow(m_pViewInfo->GetWnd(), SW_HIDE);

	if (m_nShowType == 0 && m_pViewFST != NULL)
		ShowWindow(m_pViewFST->GetWnd(), SW_SHOW);
	else if (m_nShowType == 1 && m_pViewKXT != NULL)
		ShowWindow(m_pViewKXT->GetWnd(), SW_SHOW);
	else if (m_nShowType == 2 && m_pViewInfo != NULL)
		ShowWindow(m_pViewInfo->GetWnd(), SW_SHOW);

	return QC_ERR_NONE;
}
