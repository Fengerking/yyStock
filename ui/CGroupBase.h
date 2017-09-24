/*******************************************************************************
	File:		CGroupBase.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CGroupBase_H__
#define __CGroupBase_H__

#include "windows.h"
#include "string.h"

#include "CRegMng.h"
#include "CViewRTInfo.h"

class CGroupBase
{
public:
	CGroupBase(HINSTANCE hInst);
	virtual ~CGroupBase(void);

	virtual int			CreateWnd (HWND hWnd);
	virtual LRESULT		OnResize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HINSTANCE			m_hInst;
	HWND				m_hMainWnd;


};

#endif // __CGroupBase_H__
