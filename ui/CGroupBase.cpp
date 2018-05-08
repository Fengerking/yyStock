/*******************************************************************************
	File:		CGroupBase.cpp

	Contains:	base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"
#include "qcErr.h"

#include "CGroupBase.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

CGroupBase::CGroupBase(HINSTANCE hInst)
	: CBaseObject()
	, m_hInst (hInst)
	, m_hMainWnd (NULL)
{
	SetObjectName("CGroupBase");
	memset(&m_rcView, 0, sizeof(RECT));
}

CGroupBase::~CGroupBase(void)
{
}

int	CGroupBase::CreateWnd (HWND hWnd, RECT * pRect)
{
	m_hMainWnd = hWnd;
	if (pRect != NULL)
		memcpy(&m_rcView, pRect, sizeof(RECT));
	return QC_ERR_NONE;
}

int	CGroupBase::ShowViews(int nShow)
{
	return QC_ERR_NONE;
}

LRESULT CGroupBase::OnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CGroupBase::OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CGroupBase::OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CGroupBase::OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CGroupBase::OnMouseDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CGroupBase::OnMouseUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CGroupBase::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CGroupBase::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}
