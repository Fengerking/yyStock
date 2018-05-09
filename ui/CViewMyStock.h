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
	virtual void	FormatDouble(double dNum, char * szText, int nNum);
	virtual void	DrawDouble(HDC hDC, HFONT hFont, double dNum, int nX, int nY, int nPos, bool bPercent = false);

protected:
	CObjectList<qcMyStockItem>	m_lstMyStock;

	char *						m_szCodeList[256];
	int							m_nCodeNum;

	CIOcurl	*					m_pIO;
	qcStockRealTimeItem *		m_pRTInfo[256];

	char *						m_szBuyCode[256];
	int							m_nBuyCodeNum;
};
#endif //__CViewMyStock_H__