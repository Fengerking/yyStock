/*******************************************************************************
	File:		CDlgConfig.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgConfig_H__
#define __CDlgConfig_H__

#include "CDlgBase.h"

class CDlgConfig : public CDlgBase
{
public:
	CDlgConfig (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgConfig(void);

	virtual int			OpenDlg (void);

protected:
	virtual int			OnInitDlg (void);
	virtual int			OnCloseDlg (int nCloseID);

	virtual INT_PTR		OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:

};
#endif //__CDlgConfig_H__

