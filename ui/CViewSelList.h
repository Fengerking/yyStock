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

class CViewSelList : public CWndBase
{
public:
	CViewSelList(HINSTANCE hInst);
	virtual ~CViewSelList(void);


	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual int		UpdateList (void);
	virtual int		UpdateInfo (void);

	virtual int		UpdateView (HDC hDC);

protected:
	char *					m_szCodeList[256];
	int						m_nCodeNum;

	qcStockRealTimeItem *	m_pRTInfo[256];

};
#endif //__CViewSelList_H__