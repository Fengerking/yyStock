/*******************************************************************************
	File:		CViewCode.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CViewCode_H__
#define __CViewCode_H__
#include "qcStock.h"

#include "CWndBase.h"
#include "CNodeList.h"

#define		yyViewCode_Width		270
#define		yyViewCode_Height		80

#define		WM_TIMER_VC_HIDE		201

class CViewCode : public CWndBase
{
public:
	CViewCode(HINSTANCE hInst);
	virtual ~CViewCode(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT	OnResize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		RegistWindow(CWndBase * pWnd);
	virtual int		RemoveWindow(CWndBase * pWnd);

protected:
	int				UpdateView(HDC hDC);

protected:
	char				m_szKeyCode[32];
	int					m_nTimerHide;

	CObjectList<CWndBase>	m_lstWnds;

};
#endif //__CViewCode_H__