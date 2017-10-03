/*******************************************************************************
	File:		CGroupStock.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CGroupStock_H__
#define __CGroupStock_H__

#include "windows.h"
#include "string.h"

#include "CGroupBase.h"
#include "CRegMng.h"

#include "CViewFST.h"
#include "CViewKXT.h"
#include "CViewInfo.h"

class CGroupStock : public CGroupBase
{
public:
	CGroupStock(HINSTANCE hInst);
	virtual ~CGroupStock(void);

	virtual int			CreateWnd(HWND hWnd, RECT * pRect);
	virtual int			ShowViews(int nShow);

	virtual LRESULT		OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual int			ShowType(void);

protected:
	CViewFST *			m_pViewFST;
	CViewKXT *			m_pViewKXT;
	CViewInfo *			m_pViewInfo;

	int					m_nShowType;
};

#endif // __CGroupStock_H__
