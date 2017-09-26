/*******************************************************************************
	File:		CViewKXT.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CViewKXT_H__
#define __CViewKXT_H__
#include "qcStock.h"

#include "CWndBase.h"
#include "CNodeList.h"

#define	KXT_ITEM_MAX_WIDTH	96
#define	KXT_ITEM_STD_WIDTH	12

class CViewKXT : public CWndBase
{
public:
	CViewKXT(HINSTANCE hInst);
	virtual ~CViewKXT(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT	OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		SetStartEndDate(long long llStartDate, long long llEndDate);
	virtual void	SetBuyTime(SYSTEMTIME tmBuy) { m_tmBuyTime = tmBuy; }
	virtual void	SetSellTime (SYSTEMTIME tmSell) {m_tmSellTime = tmSell;}

protected:
	virtual int		UpdateInfo(void);
	virtual int		UpdateView(HDC hDC);

	virtual int		DrawKXImage (HDC hDC);
	virtual int		DrawDayLine (HDC hDC);
	virtual int		DrawDayLine (HDC hDC, int nDays);
	virtual int		DrawMACDPic (HDC hDC);
	virtual int		DrawDayInfoPic (HDC hDC);

	virtual int		DrawInfoText (HDC hDC);
	virtual int		DrawBackImage (HDC hDC);

	virtual int		MoveToStartItem (void);
	virtual int		AdjustItemSize (void);

	virtual int		ReleaseData(void);

protected:
	CObjectList<qcStockKXTInfoItem> m_lstData;

	double			m_dMaxPrice;
	double			m_dMinPrice;
	long long		m_llMaxVolume;
	double			m_dMaxExchange;
	double			m_dMaxMacdBar;
	double			m_dMinMacdBar;

	int				m_nItemStart;
	int				m_nItemCount;
	NODEPOS			m_hItemPos;
	int				m_nItemWidth;
	int				m_nItemGap;
	double			m_dItemWidth;
	double			m_dScale;

	RECT			m_rcKXT;
	RECT			m_rcNUM;
	RECT			m_rcJST;

	bool			m_bMouseDown;
	int				m_nXPos;
	int				m_nYPos;

	SYSTEMTIME		m_tmBuyTime;
	SYSTEMTIME		m_tmSellTime;
};
#endif //__CViewKXT_H__