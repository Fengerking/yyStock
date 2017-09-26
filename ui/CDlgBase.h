/*******************************************************************************
	File:		CDlgBase.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgBase_H__
#define __CDlgBase_H__

#include "CBaseObject.h"

#define		WM_TIMER_ANALYSE	601

class CDlgBase : public CBaseObject
{
public:
	CDlgBase (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgBase(void);

	virtual int				OpenDlg (void);
	virtual int				CreateDlg (void);

	virtual HWND			GetDlg (void) {return m_hDlg;}

protected:
	virtual INT_PTR			OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual bool			CenterDlg (void);

	virtual void			FormatDouble (double dNum, char * szText);

protected:
	HINSTANCE				m_hInst;
	HWND					m_hParent;
	HWND					m_hDlg;
	HBRUSH					m_hBrushBG;

	bool					m_bModeless;

public:
	static INT_PTR CALLBACK baseDlgProc (HWND, UINT, WPARAM, LPARAM);

};
#endif //__CDlgBase_H__

