/*******************************************************************************
	File:		CDlgDownLoad.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgDownLoad_H__
#define __CDlgDownLoad_H__

#include "CDlgBase.h"
#include "CStockItemList.h"
#include "CIOcurl.h"

class CDlgDownLoad : public CDlgBase
{
public:
	CDlgDownLoad (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgDownLoad(void);

	virtual int			OpenDlg (void);
	virtual int			CreateDlg(void);

protected:
	virtual int			OnInitDlg (void);

	virtual int			OnStart(void);
	virtual int			OnDownLoad(void);
	virtual int			OnFinish(bool bShow);

	virtual INT_PTR		OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HWND				m_hEdtResult;
	HWND				m_hProgress;

	CIOcurl  *			m_pIO;
	int					m_nDelayTime;

	int					m_nCommandID;
	qcStockInfoItem *	m_pItem;
	NODEPOS				m_hPosCode;
	int					m_nCodeNum;
	int					m_nProcNum;
	int					m_nTimer;

	char *				m_pErrorText;
	char				m_szStatus[256];
	int					m_nStartTime;

};
#endif //__CDlgDownLoad_H__

