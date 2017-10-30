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
	, m_pIO(NULL)
	, m_nCodeNum(0)
	, m_nMaxWidth(770)
	, m_nMinWidth(360)
{
	SetObjectName("CViewSelList");

	_tcscpy (m_szClassName, _T("yyStockViewSEL"));
	_tcscpy (m_szWindowName, _T("yyStockViewSEL"));

	UpdateList();

	m_nUpdateTime = m_nUpdateTime * 2;
}

CViewSelList::~CViewSelList(void)
{
	ThreadStop();

	for (int i = 0; i < VSL_LISTNUM_MAX; i++)
	{
		if (m_pRTInfo[i] != NULL)
			delete m_pRTInfo[i];
		delete[] m_szCodeList[i];
	}
	QC_DEL_P(m_pIO);
}

int CViewSelList::UpdateView (HDC hDC)
{
	CAutoLock lock(&m_mtLock);

	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC(hDC);
	if (m_hBmpInfo == NULL)
	{
		m_hBmpInfo = CreateBitmap(m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
		m_hBmpOld = (HBITMAP)SelectObject(m_hMemDC, m_hBmpInfo);
	}
	SetBkMode(m_hMemDC, TRANSPARENT);
	FillRect(m_hMemDC, &m_rcWnd, m_hBKBrush);
	DrawRect(m_hMemDC, &m_rcWnd, 3, MSC_GRAY_4);

	int		nX = m_rcDraw.left;
	int		nY = m_rcDraw.top;

	while (nY < m_rcDraw.bottom)
	{
		nY += m_nFntBigHeight;
		DrawLine(m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	}
	if (m_rcWnd.right > m_nMaxWidth + m_nMinWidth)
	{
		if (m_rcWnd.right > m_nMaxWidth * 2)
			DrawLine(m_hMemDC, m_rcWnd.right / 2, 0, m_rcWnd.right / 2, nY, 4, MSC_GRAY_3);
		else
			DrawLine(m_hMemDC, m_nMaxWidth, 0, m_nMaxWidth, nY, 4, MSC_GRAY_3);
	}

	int nStartItem = m_nItemNum * m_nPageIdx;
	nY = m_rcDraw.top;
	int i = 0;
	for (i = nStartItem; i < m_nCodeNum; i++)
	{
		m_dClosePrice = m_pRTInfo[i]->m_dClosePrice;
		DrawWtrText(m_hMemDC, m_pRTInfo[i]->m_wzName, m_hFntBig, nX + 10, nY, MSC_GRAY_3, 0);
		DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice, m_hFntBig, nX + 400, nY, "", -1, false, 1);
		if (nX + 580 < m_rcWnd.right)
			DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dDiffRate, m_hFntBig, nX + 580, nY, "", -2, true, 1);
		if (nX + 730 < m_rcWnd.right)
		{
			if (m_pRTInfo[i]->m_nLastIndex >= 5)
				DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice - m_pRTInfo[i]->m_dLastPrice[(m_pRTInfo[i]->m_nLastIndex-5)%5], m_hFntBig, nX + 730, nY, "", -2, false, 1);
			else
				DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice - m_pRTInfo[i]->m_dLastPrice[0], m_hFntBig, nX + 730, nY, "", -2, false, 1);
		}

		nY += m_nFntBigHeight;
		if (nY > m_rcDraw.bottom - m_nFntBigHeight)
			break;
	}

	if (m_rcWnd.right > m_nMaxWidth + m_nMinWidth)
	{
		if (m_rcWnd.right > m_nMaxWidth * 2)
			nX = m_rcDraw.right / 2 + 8;
		else
			nX = m_nMaxWidth + 8;
		nY = m_rcDraw.top;
		for (i = i + 1; i < m_nCodeNum; i++)
		{
			m_dClosePrice = m_pRTInfo[i]->m_dClosePrice;
			DrawWtrText(m_hMemDC, m_pRTInfo[i]->m_wzName, m_hFntBig, nX + 10, nY, MSC_GRAY_3, 0);
			DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice, m_hFntBig, nX + 400, nY, "", -1, false, 1);
			if (nX + 580 < m_rcWnd.right)
				DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dDiffRate, m_hFntBig, nX + 580, nY, "", -2, true, 1);
			if (nX + 730 < m_rcWnd.right)
			{
				if (m_pRTInfo[i]->m_nLastIndex >= 5)
					DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice - m_pRTInfo[i]->m_dLastPrice[(m_pRTInfo[i]->m_nLastIndex - 5) % 5], m_hFntBig, nX + 730, nY, "", -2, false, 1);
				else
					DrawDblText(m_hMemDC, m_pRTInfo[i]->m_dNowPrice - m_pRTInfo[i]->m_dLastPrice[0], m_hFntBig, nX + 730, nY, "", -2, false, 1);
			}

			nY += m_nFntBigHeight;
			if (nY > m_rcDraw.bottom - m_nFntBigHeight)
				break;
		}
	}

	DrawScrollBar(m_hMemDC);
	BitBlt(hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);

	return QC_ERR_NONE;
}

int CViewSelList::UpdatePageNum(void)
{
	CAutoLock lock(&m_mtLock);

	m_nItemNum = m_rcWnd.bottom / m_nFntBigHeight;
	if (m_rcWnd.right > m_nMaxWidth + 400)
		m_nItemNum = m_nItemNum * 2;
	m_nPageNum = (m_nCodeNum + m_nItemNum - 1) / m_nItemNum;
	m_nPageIdx = 0;

	return QC_ERR_NONE;
}

int	CViewSelList::UpdateList(void)
{
	int i = 0;
	for (i = 0; i < VSL_LISTNUM_MAX; i++)
	{
		m_pRTInfo[i] = NULL;
		m_szCodeList[i] = new char[32];
		strcpy(m_szCodeList[i], "");
	}

	char	szFileName[VSL_LISTNUM_MAX];
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
		if (m_nCodeNum >= VSL_LISTNUM_MAX)
			break;
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
	CAutoLock lock(&m_mtLock);
	int nStartItem = m_nItemNum * m_nPageIdx;
	if (!qcIsTradeTime())
	{
		if (m_pRTInfo[nStartItem] != NULL && m_pRTInfo[nStartItem]->m_dNowPrice != 0)
			return QC_ERR_FAILED;
	}
	int nRC = 0;
	if (m_pIO == NULL)
		m_pIO = new CIOcurl();
	int nItemNum = m_nItemNum;
	if (nItemNum + nStartItem > m_nCodeNum)
		nItemNum = m_nCodeNum - nStartItem;
	nRC = qcStock_ParseRTListInfo(m_pIO, (const char **)&m_szCodeList[nStartItem], nItemNum, &m_pRTInfo[nStartItem]);
	if (nRC == QC_ERR_NONE)
		InvalidateRect(m_hWnd, NULL, FALSE);

	return nRC;
}

bool CViewSelList::CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup)
{
	if (!CWndBase::CreateWnd(hParent, rcView, clrBG, pGroup))
		return false;

	CBaseGraphics::OnCreateWnd (m_hWnd);

	UpdatePageNum();

	if (1)
		ThreadStart();
	else
		SetTimer(m_hWnd, WM_TIMER_UPDATE, m_nUpdateTime, NULL);

	return true;
}

LRESULT	CViewSelList::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	if (qcInRect(LOWORD(lParam), HIWORD(lParam), &rcWnd) <= 0)
		return S_FALSE;

	CAutoLock lock(&m_mtLock);
	short zDelta = HIWORD(wParam);
	if (zDelta > 0)
		m_nPageIdx--;
	else
		m_nPageIdx++;
	if (m_nPageIdx >= m_nPageNum)
		m_nPageIdx = 0;
	if (m_nPageIdx < 0)
		m_nPageIdx = m_nPageNum - 1;

	UpdateInfo();

	InvalidateRect(m_hWnd, NULL, TRUE);

	return S_OK;
}

LRESULT CViewSelList::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_msgWnd.m_hWnd = hWnd;
	m_msgWnd.m_uMsg = uMsg;
	m_msgWnd.m_wParam = wParam;
	m_msgWnd.m_lParam = lParam;

	switch (uMsg)
	{
	case WM_TIMER:
		UpdateInfo();
		break;

	case WM_SIZE:
		GetClientRect(m_hWnd, &m_rcWnd);
		GetClientRect(m_hWnd, &m_rcDraw);
		m_rcDraw.left += 8;
		m_rcDraw.top += 8;
		m_rcDraw.bottom -= 8;
		m_rcDraw.right -= 8;
		if (m_hBmpInfo != NULL)
		{
			SelectObject(m_hMemDC, m_hBmpOld);
			SAFE_DEL_OBJ(m_hBmpInfo);
			SAFE_DEL_OBJ(m_hBmpBack);
		}
		UpdatePageNum();
		break;

	case WM_LBUTTONUP:
	{
		if (SendMessage(m_hParent, WM_MSG_CHILDWND_MSG, (WPARAM)&m_msgWnd, NULL) == S_OK)
			return S_OK;

		int nXPos = LOWORD(lParam);
		int nYPos = HIWORD(lParam);
		int	nX = 0;
		if (nXPos > m_rcWnd.right / 2)
			nX = 1;
		int nY = nYPos / m_nFntBigHeight;
		int nYCount = m_rcWnd.bottom / m_nFntBigHeight;
		int nIndex = nX * nYCount + nY;
		int nStartItem = m_nItemNum * m_nPageIdx;
		nIndex = nIndex + nStartItem;
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

