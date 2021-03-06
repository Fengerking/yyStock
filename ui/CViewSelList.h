/*******************************************************************************
	File:		CViewSelList.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CViewSelList_H__
#define __CViewSelList_H__
#include "qcStock.h"

#include "CWndBase.h"
#include "CNodeList.h"
#include "CIOcurl.h"

#define	VSL_LISTNUM_MAX	256

class CViewSelList : public CWndBase
{
public:
	CViewSelList(HINSTANCE hInst);
	virtual ~CViewSelList(void);


	virtual bool	CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT	OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual int		UpdateList (void);
	virtual int		UpdateInfo (void);

	virtual int		UpdatePageNum(void);
	virtual int		UpdateView (HDC hDC);

protected:
	char *					m_szCodeList[VSL_LISTNUM_MAX];
	int						m_nCodeNum;

	CIOcurl	*				m_pIO;
	qcStockRealTimeItem *	m_pRTInfo[VSL_LISTNUM_MAX];

	int						m_nMaxWidth;
	int						m_nMinWidth;
};
#endif //__CViewSelList_H__