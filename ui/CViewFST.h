/*******************************************************************************
	File:		CViewFST.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CViewFST_H__
#define __CViewFST_H__
#include "qcStock.h"

#include "CWndBase.h"
#include "CPngDec.h"

class CViewFST : public CWndBase
{
public:
	CViewFST(HINSTANCE hInst);
	virtual ~CViewFST(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	int				UpdateView(HDC hDC);
	int				UpdateInfo(void);

protected:
	CPngDec *		m_pPngDec;
};
#endif //__CViewFST_H__