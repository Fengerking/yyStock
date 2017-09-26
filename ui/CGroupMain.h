/*******************************************************************************
	File:		CGroupMain.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CGroupMain_H__
#define __CGroupMain_H__

#include "windows.h"
#include "string.h"

#include "CRegMng.h"

#include "CViewFST.h"
#include "CViewKXT.h"
#include "CViewSelList.h"
#include "CViewRTInfo.h"

class CGroupMain
{
public:
	CGroupMain(HINSTANCE hInst);
	virtual ~CGroupMain(void);

	virtual int			CreateWnd (HWND hWnd);
	virtual int			ShowViews(int nShow);

	virtual LRESULT		OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HINSTANCE			m_hInst;
	HWND				m_hMainWnd;

	CViewFST *			m_pViewFST;
	CViewKXT *			m_pViewKXT;
	CViewSelList *		m_pViewSEL;
	CViewRTInfo *		m_pViewRTI;

};

#endif // __CGroupMain_H__
