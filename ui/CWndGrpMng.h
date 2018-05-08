/*******************************************************************************
	File:		CWndGrpMng.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndGrpMng_H__
#define __CWndGrpMng_H__

#include "windows.h"
#include "string.h"

#include "CBaseObject.h"
#include "CStockItemList.h"

#include "CGroupMain.h"
#include "CViewKXT.h"
#include "CViewCode.h"
#include "CDlgDownLoad.h"

#include "CRegMng.h"

class CWndGrpMng : public CBaseObject
{
public:
	CWndGrpMng(HINSTANCE hInst);
	virtual ~CWndGrpMng(void);

	virtual int			CreateWnd (HWND hWnd);
	virtual LRESULT		OnReceiveMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


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

	CStockItemList *	m_pLstStock;

	CGroupMain *		m_pGrpMain;
	CViewCode *			m_pViewCode;

	CDlgDownLoad *		m_pDlgDownLoad;

	CRegMng *			m_pRegMng;
};

#endif // __CWndGrpMng_H__
