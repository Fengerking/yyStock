/*******************************************************************************
	File:		CViewInfo.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CViewInfo_H__
#define __CViewInfo_H__
#include "qcStock.h"

#include "CWndBase.h"
#include "CIOFile.h"

class CViewInfo : public CWndBase
{
public:
	CViewInfo(HINSTANCE hInst);
	virtual ~CViewInfo(void);

	virtual bool	CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual int		UpdateView(HDC hDC);
	virtual int		UpdateInfo(void);
	virtual int		DrawLineText(HDC hDC, int nXPos, int nYPos, char * pLine);

protected:
	CIOFile *		m_pIOFile;

};
#endif //__CViewInfo_H__