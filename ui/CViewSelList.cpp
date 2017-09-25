/*******************************************************************************
	File:		CViewSelList.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "qcErr.h"

#include "CViewSelList.h"

#include "CIOFile.h"
#include "UStockParser.h"

#include "ULogFunc.h"
#include "USystemFunc.h"

CViewSelList::CViewSelList(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_nCodeNum(2)
{
	_tcscpy (m_szClassName, _T("yyStockViewSEL"));
	_tcscpy (m_szWindowName, _T("yyStockViewSEL"));

	UpdateList();
}

CViewSelList::~CViewSelList(void)
{
	for (int i = 0; i < 256; i++)
	{
		if (m_pRTInfo[i] != NULL)
			delete m_pRTInfo[i];
		delete[] m_szCodeList[i];
	}
}

int CViewSelList::UpdateView (HDC hDC)
{
	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC(hDC);
	if (m_hBmpInfo == NULL)
	{
		m_hBmpInfo = CreateBitmap(m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
		m_hBmpOld = (HBITMAP)SelectObject(m_hMemDC, m_hBmpInfo);
	}
	SetBkMode(m_hMemDC, TRANSPARENT);
	FillRect(m_hMemDC, &m_rcWnd, m_hBKBrush);
	DrawRect(m_hMemDC, &m_rcWnd, 5, MSC_GRAY_4);

	int		nX = m_rcDraw.left;
	int		nY = m_rcDraw.top;

	while (nY < m_rcDraw.bottom)
	{
		nY += m_nFntBigHeight;
		DrawLine(m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	}
	DrawLine(m_hMemDC, m_rcWnd.right / 2, 0, m_rcWnd.right / 2, nY, 4, MSC_GRAY_3);

	nY = m_rcDraw.top;
	int i = 0;
	for (i = 0; i < m_nCodeNum; i++)
	{
		m_dClosePrice = m_pRTInfo[i]->m_dClosePrice;
		DrawWtrText(m_hMemDC, m_pRTInfo[i]->m_wzName, m_hFntBig, nX + 10, nY, MSC_GRAY_3, 0);
		DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice, m_hFntBig, nX + 400, nY, "", -1, false, 1);
		if (nX + 580 < m_rcWnd.right)
			DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dDiffRate, m_hFntBig, nX + 580, nY, "", -2, true, 1);
		if (nX + 730 < m_rcWnd.right)
			DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice - m_pRTInfo[i]->m_dLastPrice, m_hFntBig, nX + 730, nY, "", -2, false, 1);
		nY += m_nFntBigHeight;
		if (nY > m_rcDraw.bottom - m_nFntBigHeight)
			break;
	}
	nY = m_rcDraw.top;
	nX = m_rcDraw.right / 2 + 8;
	for (i = i+1; i < m_nCodeNum; i++)
	{
		m_dClosePrice = m_pRTInfo[i]->m_dClosePrice;
		DrawWtrText(m_hMemDC, m_pRTInfo[i]->m_wzName, m_hFntBig, nX + 10, nY, MSC_GRAY_3, 0);
		DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice, m_hFntBig, nX + 400, nY, "", -1, false, 1);
		if (nX + 580 < m_rcWnd.right)
			DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dDiffRate, m_hFntBig, nX + 580, nY, "", -2, true, 1);
		if (nX + 730 < m_rcWnd.right)
			DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice - m_pRTInfo[i]->m_dLastPrice, m_hFntBig, nX + 730, nY, "", -2, false, 1);
		nY += m_nFntBigHeight;
		if (nY > m_rcDraw.bottom - m_nFntBigHeight)
			break;
	}

	BitBlt(hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);

	return QC_ERR_NONE;
}

int	CViewSelList::UpdateList(void)
{
	int i = 0;
	for (i = 0; i < 256; i++)
	{
		m_pRTInfo[i] = NULL;
		m_szCodeList[i] = new char[32];
		strcpy(m_szCodeList[i], "");
	}

	char	szFileName[256];
	qcGetAppPath(NULL, szFileName, sizeof(szFileName));
	strcat(szFileName, "data\\qcStockSelect.txt");

	CIOFile ioFile;
	if (ioFile.Open(szFileName, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	m_nCodeNum = 0;
	char *	pFileData = ioFile.GetData();
	int		nFileSize = (int)ioFile.GetSize();
	int		nRestSize = nFileSize;
	int		nLen = 0;
	while (nRestSize > 0)
	{
		nLen = qcReadTextLine(pFileData, nRestSize, m_szCodeList[m_nCodeNum], 32);
		if (m_szCodeList[m_nCodeNum][6] == '\r' || m_szCodeList[m_nCodeNum][6] == '\n')
			m_szCodeList[m_nCodeNum][6] = 0;
		else if (m_szCodeList[m_nCodeNum][7] == '\r' || m_szCodeList[m_nCodeNum][7] == '\n')
			m_szCodeList[m_nCodeNum][7] = 0;
		nRestSize -= nLen;
		pFileData += nLen;
		m_nCodeNum++;
	}

	for (i = 0; i < m_nCodeNum; i++)
	{
		m_pRTInfo[i] = new qcStockRealTimeItem();
		memset(m_pRTInfo[i], 0, sizeof(qcStockRealTimeItem));
	}

	return QC_ERR_NONE;
}

int CViewSelList::UpdateInfo(void)
{
	int nStart = qcGetSysTime();
	int nRC = 0;

	qcStock_ParseRTListInfo((const char **)&m_szCodeList[0], m_nCodeNum, &m_pRTInfo[0]);
/*
	for (int i = 0; i < m_nCodeNum; i++)
	{
		//memset(m_pRTInfo[i], 0, sizeof(qcStockRealTimeItem));
		m_pRTInfo[i]->m_dLastPrice = m_pRTInfo[i]->m_dNowPrice;
		nRC = qcStock_ParseRTItemInfo(m_szCodeList[i], m_pRTInfo[i]);
		if (m_pRTInfo[i]->m_dLastPrice == 0)
			m_pRTInfo[i]->m_dLastPrice = m_pRTInfo[i]->m_dNowPrice;
	}
*/
	int nUsed = qcGetSysTime() - nStart;
	return nRC;
}

bool CViewSelList::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;

	CBaseGraphics::OnCreateWnd (m_hWnd);

	SetTimer(m_hWnd, WM_TIMER_UPDATE, m_nUpdateTime * 2, NULL);

	return true;
}

LRESULT CViewSelList::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:
		if (qcIsTradeTime())
		{
			if (UpdateInfo() == QC_ERR_NONE)
				InvalidateRect(m_hWnd, NULL, FALSE);
		}
		break;

	case WM_LBUTTONUP:
	{
		int nXPos = LOWORD(lParam);
		int nYPos = HIWORD(lParam);
		int	nX = 0;
		if (nXPos > m_rcWnd.right / 2)
			nX = 1;
		int nY = nYPos / m_nFntBigHeight;
		int nYCount = m_rcWnd.bottom / m_nFntBigHeight;
		int nIndex = nX * nYCount + nY;
		if (m_szCodeList[nIndex] == NULL)
			return S_OK;
		if (strlen(m_szCodeList[nIndex]) < 6)
			return S_OK;
		PostMessage(m_hParent, WM_MSG_CODE_CHANGE, (WPARAM)m_szCodeList[nIndex], 0);
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

	case WM_ERASEBKGND:
		return S_FALSE;

	case WM_DESTROY:
		break;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hWnd, uMsg, wParam, lParam);
}

