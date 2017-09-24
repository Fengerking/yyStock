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
	: m_hInst (hInst)
	, m_hMainWnd (NULL)
{
}

CGroupBase::~CGroupBase(void)
{

}

int	CGroupBase::CreateWnd (HWND hWnd)
{
	m_hMainWnd = hWnd;

	return QC_ERR_NONE;
}


LRESULT CGroupBase::OnResize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CGroupBase::OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CGroupBase::OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CGroupBase::OnMouseDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CGroupBase::OnMouseUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CGroupBase::OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}
