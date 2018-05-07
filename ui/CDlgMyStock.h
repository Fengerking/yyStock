/*******************************************************************************
	File:		CDlgMyStock.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgMyStock_H__
#define __CDlgMyStock_H__

#include "CDlgBase.h"
#include "CViewMyStock.h"

class CDlgMyStock : public CDlgBase
{
public:
	CDlgMyStock (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgMyStock(void);

	virtual int			OpenDlg (void);

protected:
	virtual int			OnInitDlg (void);
	virtual int			OnCloseDlg (int nCloseID);

	virtual INT_PTR		OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CViewMyStock *		m_pView;

};
#endif //__CDlgMyStock_H__

