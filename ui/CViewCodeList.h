/*******************************************************************************
	File:		CViewCodeList.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CViewCodeList_H__
#define __CViewCodeList_H__
#include "qcStock.h"

#include "CWndBase.h"
#include "CNodeList.h"

#include "CStockItemList.h"

class CViewCodeList : public CWndBase
{
public:
	CViewCodeList(HINSTANCE hInst);
	virtual ~CViewCodeList(void);

	virtual bool	CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	int				UpdateView(HDC hDC);
	int				UpdatePageNum(void);

protected:
	CObjectList<qcStockInfoItem> *	m_pCodeList;
	int								m_nSolidNum;
};
#endif //__CViewCodeList_H__