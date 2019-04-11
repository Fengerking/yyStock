/*******************************************************************************
	File:		CViewRTInfo.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "qcErr.h"

#include "CViewRTInfo.h"
#include "CIOFile.h"

#include "UStockParser.h"

#include "ULogFunc.h"
#include "USystemFunc.h"

CViewRTInfo::CViewRTInfo(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_nWndWidth (398)
	, m_pIO(NULL)
	, m_nLastValume (0)
	, m_llTradeNum(0)
{
	SetObjectName("CViewRTInfo");

	_tcscpy (m_szClassName, _T("yyStockViewRTInfo"));
	_tcscpy (m_szWindowName, _T("yyStockViewRTInfo"));

	memset(&m_stkRTInfo, 0, sizeof(qcStockRealTimeItem));
}

CViewRTInfo::~CViewRTInfo(void)
{
	ThreadStop();

	SendMessage(m_hParent, WM_MSG_CODE_REMOVE, (WPARAM)this, 0);

	ReleaseHistory ();

	QC_DEL_P(m_pIO);
}

int CViewRTInfo::UpdateView (HDC hDC)
{
	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC (hDC);
	if (m_hBmpInfo == NULL)
	{
		m_hBmpInfo = CreateBitmap (m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
		m_hBmpOld = (HBITMAP)SelectObject (m_hMemDC, m_hBmpInfo);
	}
	SetBkMode (m_hMemDC, TRANSPARENT);
	FillRect (m_hMemDC, &m_rcWnd, m_hBKBrush);
	DrawRect (m_hMemDC, &m_rcWnd, 3, MSC_GRAY_4);

	int		nX = m_rcDraw.left;
	int		nY = m_rcDraw.top;

	SetBkMode(m_hMemDC, TRANSPARENT);
	DrawWtrText(m_hMemDC, m_stkRTInfo.m_wzName, m_hFntBig, nX + 10, nY, MSC_GRAY_4, 0);
	DrawStrText(m_hMemDC, m_szCode, m_hFntBig, m_rcDraw.right - 8, nY, MSC_GRAY_4, 1);

	nY = m_rcDraw.top + (m_nFntBigHeight + 8);
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------

	nY += 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dDiffRate, m_hFntMid, m_rcDraw.left, nY, "涨幅: ", -2, true, 0);
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dDiffNum, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "数量: ", -2, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dTurnOver, m_hFntMid, m_rcDraw.left, nY, "换手: ", MSC_WHITE, true, 0);
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dSwing, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "振幅: ", MSC_WHITE, true, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dMaxPrice, m_hFntMid, m_rcDraw.left, nY, "最高: ", -1, false, 0);
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dMinPrice, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "最低: ", -1, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dOpenPrice, m_hFntMid, m_rcDraw.left, nY, "开盘: ", -1, false, 0);
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dClosePrice, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "收盘: ", -1, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nTradeNum / 100, m_hFntMid, m_rcDraw.left, nY, "成交量 ", MSC_WHITE, 0, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nTradeMoney / 10000, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "成交额 ", MSC_WHITE, 0, 0);

	nY = m_rcDraw.top + (m_nFntBigHeight + 8) + (m_nFntMidHeight + 8) * 5;
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------

	int nMaxNum = GetMaxNum();
	int nPosX = m_rcDraw.right / 2 - 30;
	int nLineX = nPosX + 120;
	int nLineY = 10;
	int nLineW = 10;

	nY += 8;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dSellPrice5, m_hFntSml, m_rcDraw.left, nY, "卖5: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nSellNum5, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dSellPrice5, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nSellNum5, m_stkRTInfo.m_dSellPrice5, nLineX, nY + nLineY, nLineW);

	nY += m_nFntSmlHeight + 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dSellPrice4, m_hFntSml, m_rcDraw.left, nY, "卖4: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nSellNum4, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dSellPrice4, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nSellNum4, m_stkRTInfo.m_dSellPrice4, nLineX, nY + nLineY, nLineW);

	nY += m_nFntSmlHeight + 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dSellPrice3, m_hFntSml, m_rcDraw.left, nY, "卖3: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nSellNum3, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dSellPrice3, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nSellNum3, m_stkRTInfo.m_dSellPrice3, nLineX, nY + nLineY, nLineW);

	nY += m_nFntSmlHeight + 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dSellPrice2, m_hFntSml, m_rcDraw.left, nY, "卖2: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nSellNum2, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dSellPrice2, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nSellNum2, m_stkRTInfo.m_dSellPrice2, nLineX, nY + nLineY, nLineW);

	nY += m_nFntSmlHeight + 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dSellPrice1, m_hFntSml, m_rcDraw.left, nY, "卖1: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nSellNum1, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dSellPrice1, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nSellNum1, m_stkRTInfo.m_dSellPrice1, nLineX, nY + nLineY, nLineW);

	nY = m_rcDraw.top + (m_nFntBigHeight + 8) + (m_nFntMidHeight + 8) * 5 + (m_nFntSmlHeight + 4) * 5 + 8;;
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------
	nY += 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dNowPrice, m_hFntLrg, m_rcDraw.left, nY, "现价", -1, false, 0);

	nY = m_rcDraw.top + (m_nFntBigHeight + 8) + (m_nFntMidHeight + 8) * 5 + (m_nFntSmlHeight + 4) * 5 + 8 + m_nFntLrgHeight + 8;;
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------


	nY += 8;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dBuyPrice1, m_hFntSml, m_rcDraw.left, nY, "买1: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nBuyNum1, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dBuyPrice1, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nBuyNum1, m_stkRTInfo.m_dBuyPrice1, nLineX, nY + nLineY, nLineW);

	nY += m_nFntSmlHeight + 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dBuyPrice2, m_hFntSml, m_rcDraw.left, nY, "买2: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nBuyNum2, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dBuyPrice2, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nBuyNum2, m_stkRTInfo.m_dBuyPrice2, nLineX, nY + nLineY, nLineW);

	nY += m_nFntSmlHeight + 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dBuyPrice3, m_hFntSml, m_rcDraw.left, nY, "买3: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nBuyNum3, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dBuyPrice3, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nBuyNum3, m_stkRTInfo.m_dBuyPrice3, nLineX, nY + nLineY, nLineW);

	nY += m_nFntSmlHeight + 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dBuyPrice4, m_hFntSml, m_rcDraw.left, nY, "买4: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nBuyNum4, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dBuyPrice4, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nBuyNum4, m_stkRTInfo.m_dBuyPrice4, nLineX, nY + nLineY, nLineW);

	nY += m_nFntSmlHeight + 4;
	DrawDblText (m_hMemDC, m_stkRTInfo.m_dBuyPrice5, m_hFntSml, m_rcDraw.left, nY, "买5: ", -1, false, 0);
	DrawIntText (m_hMemDC, m_stkRTInfo.m_nBuyNum5, m_hFntSml, nPosX, nY, " ", -1, m_stkRTInfo.m_dBuyPrice5, 0);
	DrawNumLine(m_hMemDC, nMaxNum, m_stkRTInfo.m_nBuyNum5, m_stkRTInfo.m_dBuyPrice5, nLineX, nY + nLineY, nLineW);

	nY = m_rcDraw.top + (m_nFntBigHeight + 8) + (m_nFntMidHeight + 8) * 5 + (m_nFntSmlHeight + 4) * 10 + 16 + m_nFntLrgHeight + 8;;
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------

	char			szTime[64];
	double			dLastPrice = m_dClosePrice;
	COLORREF		clrText = MSC_RED;
	sTradeHistory * pItem = NULL;
	sTradeHistory * pPrev = NULL;
	nY += 4; 
	NODEPOS pos = m_lstHistory.GetHeadPosition ();
	if (pos != NULL)
		pItem = m_lstHistory.GetNext (pos);
	while (pItem != NULL)
	{
		if (pos != NULL)
		{
			pPrev = m_lstHistory.GetNext (pos);
			dLastPrice = pPrev->dPrice;
		}
		else
		{
			pPrev = NULL;
			dLastPrice = m_dClosePrice;
		}
		sprintf (szTime, "时间: %02d:%02d:%02d  ",pItem->sTime.wHour, pItem->sTime.wMinute, pItem->sTime.wSecond);
		if (pItem->dPrice > dLastPrice)
			clrText = MSC_RED;
		else if (pItem->dPrice == dLastPrice)
			clrText = MSC_WHITE;
		else
			clrText = MSC_GREEN;
		DrawDblText (m_hMemDC, pItem->dPrice, m_hFntSml, m_rcDraw.left, nY, szTime, clrText, false, 0);
		DrawIntText (m_hMemDC, pItem->nNumber, m_hFntSml, m_rcDraw.right / 2 + 48, nY, " ", clrText, pItem->dPrice, 0);
		pItem = pPrev;

		nY += m_nFntSmlHeight + 4;
		if (nY >= m_rcDraw.bottom)
			break;
	}

	BitBlt (hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);

	return QC_ERR_NONE;
}

int CViewRTInfo::UpdateInfo(void)
{
	if (!qcIsTradeTime())
	{
		if (m_stkRTInfo.m_dNowPrice != 0)
			return QC_ERR_FAILED;
	}
//	memset(&m_stkRTInfo, 0, sizeof(qcStockRealTimeItem));
	int nRC = qcStock_ParseRTItemInfo(m_pIO, m_szCode, &m_stkRTInfo);
	if (nRC == QC_ERR_NONE)
	{
		if (m_llTradeNum == 0)
		{
			char szURL[1024];
			qcGetAppPath(NULL, szURL, sizeof(szURL));
			sprintf(szURL, "%sdata\\info\\%s.txt", szURL, m_szCode);
			CIOFile ioFile;
			if (ioFile.Open(szURL, 0, QCIO_FLAG_READ) == QC_ERR_NONE)
			{
				char *	pFileData = ioFile.GetData();
				int		nFileSize = (int)ioFile.GetSize();
				char	szLine[2048];
				int nLine = qcReadTextLine(pFileData, nFileSize, szLine, 2048);
				nLine = qcReadTextLine(pFileData + nLine, nFileSize, szLine, 2048);
				char * pValue = strstr(szLine, ":");
				if (pValue != NULL)
				{
					pValue++;
					m_llTradeNum = atoll(pValue);
				}
				ioFile.Close();
			}
		}
		if (m_llTradeNum > 0)
			m_stkRTInfo.m_dTurnOver = m_stkRTInfo.m_nTradeNum * 100.0 / m_llTradeNum;

		m_dClosePrice = m_stkRTInfo.m_dClosePrice;
		sTradeHistory * pItem = new sTradeHistory();
		GetLocalTime(&pItem->sTime);
		pItem->dPrice = m_stkRTInfo.m_dNowPrice;
		pItem->nNumber = m_stkRTInfo.m_nTradeNum - m_nLastValume;
		m_nLastValume = m_stkRTInfo.m_nTradeNum;
		if (pItem->nNumber > 0)
			m_lstHistory.AddHead(pItem);
		else
			delete pItem;
		InvalidateRect(m_hWnd, NULL, FALSE);
	}
	return nRC;
}

int	CViewRTInfo::GetMaxNum(void)
{
	int nMaxNum = m_stkRTInfo.m_nBuyNum1;
	if (nMaxNum < m_stkRTInfo.m_nBuyNum2)
		nMaxNum = m_stkRTInfo.m_nBuyNum2;
	if (nMaxNum < m_stkRTInfo.m_nBuyNum3)
		nMaxNum = m_stkRTInfo.m_nBuyNum3;
	if (nMaxNum < m_stkRTInfo.m_nBuyNum4)
		nMaxNum = m_stkRTInfo.m_nBuyNum4;
	if (nMaxNum < m_stkRTInfo.m_nBuyNum5)
		nMaxNum = m_stkRTInfo.m_nBuyNum5;
	if (nMaxNum < m_stkRTInfo.m_nSellNum1)
		nMaxNum = m_stkRTInfo.m_nSellNum1;
	if (nMaxNum < m_stkRTInfo.m_nSellNum2)
		nMaxNum = m_stkRTInfo.m_nSellNum2;
	if (nMaxNum < m_stkRTInfo.m_nSellNum3)
		nMaxNum = m_stkRTInfo.m_nSellNum3;
	if (nMaxNum < m_stkRTInfo.m_nSellNum4)
		nMaxNum = m_stkRTInfo.m_nSellNum4;
	if (nMaxNum < m_stkRTInfo.m_nSellNum5)
		nMaxNum = m_stkRTInfo.m_nSellNum5;
	return nMaxNum;
}

int	CViewRTInfo::DrawNumLine(HDC hDC, int nMax, int nNum, double dPrice, int nX, int nY, int nW)
{
	HBRUSH	hBrushDraw = m_hBrushWhite;
	int		nColor = MSC_WHITE;
	if (dPrice < m_stkRTInfo.m_dClosePrice)
	{
		nColor = MSC_GREEN_1;
		hBrushDraw = m_hBrushGreen;
	}
	else if (dPrice > m_stkRTInfo.m_dClosePrice)
	{
		nColor = MSC_RED_1;
		hBrushDraw = m_hBrushRed;
	}
	if (dPrice == 0)
	{
		nColor = MSC_WHITE;
		hBrushDraw = m_hBrushWhite;
	}

	long long llLen = m_rcDraw.right - nX - 4;
	llLen = llLen * nNum / nMax;
	if (llLen <= 0)
		llLen = 1;
	//DrawLine(hDC, nX, nY, nX + (int)llLen, nY, nW, nColor);
	RECT	rcItem;
	SetRect(&rcItem, nX, nY, nX + (int)llLen, nY + nW);
	FillRect(hDC, &rcItem, hBrushDraw);

	return 0;
}
bool CViewRTInfo::ReleaseHistory (void)
{
	sTradeHistory * pItem = m_lstHistory.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstHistory.RemoveHead ();
	}
	m_nLastValume = 0;
	return true;
}

bool CViewRTInfo::CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup)
{
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG, pGroup))
		return false;

	CBaseGraphics::OnCreateWnd (m_hWnd);

	if (m_pIO == NULL)
		m_pIO = new CIOcurl();

	SendMessage(m_hParent, WM_MSG_CODE_REQUEST, (WPARAM)m_szCode, 0);
	SendMessage(m_hParent, WM_MSG_CODE_REGIST, (WPARAM)this, 0);

	if (1)
	{
		ThreadStart();
	}
	else
	{
		UpdateInfo();
		SetTimer(m_hWnd, WM_TIMER_UPDATE, m_nUpdateTime, NULL);
	}

	return true;
}

LRESULT CViewRTInfo::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MSG_CODE_CHANGE:
		ReleaseHistory();
		m_llTradeNum = 0;
		memset(&m_stkRTInfo, 0, sizeof(qcStockRealTimeItem));
		strcpy(m_szCode, (char *)wParam);
		UpdateInfo();
		return S_OK;

	case WM_TIMER:
		UpdateInfo();
		break;

	case WM_SIZE:
		GetClientRect (hWnd, &m_rcWnd);
		GetClientRect (hWnd, &m_rcDraw);
		m_rcDraw.left += 8;
		m_rcDraw.top += 8;
		m_rcDraw.bottom -= 8;
		m_rcDraw.right -= 8;
		if (m_hBmpInfo != NULL)
		{
			SelectObject (m_hMemDC, m_hBmpOld);
			SAFE_DEL_OBJ (m_hBmpInfo);
		}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		UpdateView (hdc);
		EndPaint(hWnd, &ps);
	}
		break;

//	case WM_ERASEBKGND:
//		return S_FALSE;

	case WM_DESTROY:
		break;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hWnd, uMsg, wParam, lParam);
}

