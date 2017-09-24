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

#include "USystemFunc.h"
#include "ULogFunc.h"

CWndGrpMng::CWndGrpMng(HINSTANCE hInst)
	: m_hInst (hInst)
	, m_hMainWnd (NULL)
	, m_pGrpMain(NULL)
	, m_pViewCode(NULL)
{
}

CWndGrpMng::~CWndGrpMng(void)
{
	QC_DEL_P(m_pViewCode);

	QC_DEL_P(m_pGrpMain);
}

int	CWndGrpMng::CreateWnd (HWND hWnd)
{
	m_hMainWnd = hWnd;

	RECT	rcWnd;
	GetClientRect(m_hMainWnd, &rcWnd);

	if (m_pViewCode == NULL)
		m_pViewCode = new CViewCode(m_hInst);
	m_pViewCode->CreateWnd(m_hMainWnd, rcWnd, MSC_BLACK);

	if (m_pGrpMain == NULL)
		m_pGrpMain = new CGroupMain(m_hInst);
	m_pGrpMain->CreateWnd(m_hMainWnd);

	SetWindowPos(m_pViewCode->GetWnd (), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return QC_ERR_NONE;
}

LRESULT CWndGrpMng::OnResize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CWndGrpMng::OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pViewCode != NULL)
		m_pViewCode->OnKeyUp(hwnd, uMsg, wParam, lParam);

	return S_OK;
}

LRESULT CWndGrpMng::OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CWndGrpMng::OnMouseDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CWndGrpMng::OnMouseUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CWndGrpMng::OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}
