/*******************************************************************************
	File:		CViewMyStock.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CViewMyStock_H__
#define __CViewMyStock_H__
#include "qcStock.h"

#include "CWndBase.h"
#include "CNodeList.h"
#include "CIOcurl.h"

struct qcMyStockItem
{
	char		m_szCode[16];
	char		m_szName[32];
	char		m_szDate[32];
	int			m_nNumber;
	double		m_dBuyPrice;
	double		m_dNowPrice;
};

class CViewMyStock : public CWndBase
{
public:
	CViewMyStock(HINSTANCE hInst);
	virtual ~CViewMyStock(void);

	virtual bool	CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual int		UpdateView (HDC hDC);
	virtual int		UpdateInfo(void);
	virtual int		OpenMyStockFile (void);

protected:
	CObjectList<qcMyStockItem>	m_lstMyStock;

	char *						m_szCodeList[256];
	int							m_nCodeNum;

	CIOcurl	*					m_pIO;
	qcStockRealTimeItem *		m_pRTInfo[256];
};
#endif //__CViewMyStock_H__