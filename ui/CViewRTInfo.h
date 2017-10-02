/*******************************************************************************
	File:		CViewRTInfo.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CViewRTInfo_H__
#define __CViewRTInfo_H__
#include "qcStock.h"

#include "CWndBase.h"
#include "CNodeList.h"
#include "CIOcurl.h"

#define	 MAX_DOUBLE_VALUE	0XFFFFFFFF

struct sTradeHistory
{
	SYSTEMTIME	sTime;
	double		dPrice;
	int			nNumber;
};

class CViewRTInfo : public CWndBase
{
public:
	CViewRTInfo(HINSTANCE hInst);
	virtual ~CViewRTInfo(void);

	virtual bool	CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		GetWndWidth (void) {return m_nWndWidth;}

protected:
	virtual int		UpdateView(HDC hDC);
	virtual int		UpdateInfo(void);

	virtual bool	ReleaseHistory (void);

protected:
	int							m_nWndWidth;

	CIOcurl	*					m_pIO;
	qcStockRealTimeItem			m_stkRTInfo;
	CObjectList <sTradeHistory>	m_lstHistory;
	int							m_nLastValume;

	int							m_nTradeNum;
};
#endif //__CViewRTInfo_H__