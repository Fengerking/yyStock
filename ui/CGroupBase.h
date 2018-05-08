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

#include "CBaseObject.h"
#include "CRegMng.h"
#include "CViewRTInfo.h"

class CGroupBase : public CBaseObject
{
public:
	CGroupBase(HINSTANCE hInst);
	virtual ~CGroupBase(void);

	virtual int			CreateWnd (HWND hWnd, RECT * pRect);
	virtual int			ShowViews(int nShow);
	virtual HWND		GetWnd(void) { return m_hMainWnd; }
	virtual HWND		GetMainWnd(void) { return m_hMainWnd; }

	virtual LRESULT		OnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HINSTANCE			m_hInst;
	HWND				m_hMainWnd;
	RECT				m_rcView;
};

#endif // __CGroupBase_H__
