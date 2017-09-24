/*******************************************************************************
	File:		CWndBase.h

	Contains:	the base window header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndBase_H__
#define __CWndBase_H__

#include "CBaseGraphics.h"

#define		WM_TIMER_FIRST			101
#define		WM_TIMER_UPDATE			102
#define		WM_TIMER_LBUTTONCLICK	103

#define		WM_MSG_CODE_CHANGE	WM_USER + 101
#define		WM_MSG_NEW_PRICE	WM_USER + 102


class CWndBase : public CBaseGraphics
{
public:
	static LRESULT CALLBACK ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CWndBase(HINSTANCE hInst);
	virtual ~CWndBase(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual void	Close (void);

	virtual HWND	GetWnd (void) {return m_hWnd;}

	virtual void	SetFontColor (COLORREF nColor) {m_nClrFont = nColor;}
	virtual void	SetBGColor (COLORREF nColor);

	virtual void	SetText (TCHAR * pText);
	virtual int		SetCode (char * pCode) {strcpy (m_szCode, pCode); return 0;}

	virtual LRESULT	OnResize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnMouseDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnMouseUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HINSTANCE		m_hInst;
	HWND			m_hParent;
	HWND			m_hWnd;
	RECT			m_rcWnd;

	TCHAR			m_szClassName[64];
	TCHAR			m_szWindowName[64];

	HBRUSH 			m_hBKBrush;

	TCHAR			m_szText[1024];
	COLORREF		m_nClrFont;

	int				m_nTimerUpdate;
	int				m_nTimerFirst;

	bool			m_bLButtonClick;
	int				m_nTimerLBClick;

	int				m_nUpdateTime;
};

#endif //__CWndBase_H__