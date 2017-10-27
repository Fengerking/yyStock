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

#include "CGroupBase.h"
#include "CRegMng.h"

#include "CViewFST.h"
#include "CViewKXT.h"
#include "CViewSelList.h"
#include "CViewRTInfo.h"
#include "CViewCodeList.h"

#include "CGroupStock.h"

class CGroupMain : public CGroupBase
{
public:
	CGroupMain(HINSTANCE hInst);
	virtual ~CGroupMain(void);

	virtual int			CreateWnd(HWND hWnd, RECT * pRect);
	virtual int			ShowViews(int nShow);

	virtual LRESULT		OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CGroupStock *		m_pGroupStock;
	CViewSelList *		m_pViewSEL;
	CViewRTInfo *		m_pViewRTI;
	CViewCodeList *		m_pViewList;

	double				m_dSplt;;

};

#endif // __CGroupMain_H__
