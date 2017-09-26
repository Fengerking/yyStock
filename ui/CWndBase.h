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
#include "CThreadWork.h"
#include "CMutexLock.h"

#define		WM_TIMER_FIRST			101
#define		WM_TIMER_UPDATE			102
#define		WM_TIMER_LBUTTONCLICK	103

#define		WM_MSG_CODE_REGIST	WM_USER + 101   // CWndBase
#define		WM_MSG_CODE_REMOVE	WM_USER + 102   // CWndBase
#define		WM_MSG_CODE_CHANGE	WM_USER + 103	// char *
#define		WM_MSG_CODE_REQUEST	WM_USER + 104	// char *

#define		QC_THREAD_EVENT_UPDATE	1001

class CWndBase : public CBaseGraphics, public CThreadFunc
{
public:
	static LRESULT CALLBACK ViewWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
	virtual char *	GetCode (void) { return m_szCode; }

	virtual LRESULT	OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnMouseDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnMouseUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual int		UpdateView(HDC hDC);
	virtual int		UpdateInfo(void);

	virtual int		ThreadStart(void);
	virtual int		ThreadStop(void);

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

protected:
	virtual int			OnHandleEvent(CThreadEvent * pEvent);
	CThreadWork	*		m_pThreadWork;
	CMutexLock			m_mtLock;
};

#endif //__CWndBase_H__