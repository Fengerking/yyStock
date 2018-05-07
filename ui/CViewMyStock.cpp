/*******************************************************************************
	File:		CViewMyStock.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "qcErr.h"

#include "CViewMyStock.h"

#include "CStockItemList.h"

#include "CIOFile.h"
#include "UStockParser.h"

#include "ULogFunc.h"
#include "USystemFunc.h"

CViewMyStock::CViewMyStock(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_pIO(NULL)
{
	SetObjectName("CViewMyStock");

	_tcscpy (m_szClassName, _T("yyStockViewMyStock"));
	_tcscpy (m_szWindowName, _T("yyStockVieMyStock"));

	OpenMyStockFile();
}

CViewMyStock::~CViewMyStock(void)
{
	ThreadStop();
	qcMyStockItem * pItem = m_lstMyStock.RemoveHead();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstMyStock.RemoveHead();
	}

	for (int i = 0; i < 256; i++)
	{
		if (m_pRTInfo[i] != NULL)
			delete m_pRTInfo[i];
		delete[] m_szCodeList[i];
	}
	QC_DEL_P(m_pIO);
}

int CViewMyStock::UpdateView (HDC hDC)
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

	int		nIndex = 0;
	int		nX = m_rcDraw.left;
	int		nY = m_rcDraw.top;

	while (nY < m_rcDraw.bottom)
	{
		nY += m_nFntMidHeight + 8;
		DrawLine(m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
		nIndex++;
		if (nIndex > m_lstMyStock.GetCount() + 1)
			break;
	}
	
	nX = m_rcDraw.left;
	nY = m_rcDraw.top + 4;
	DrawStrText(m_hMemDC, "  股票       日期       数量    价格        金额     现价        盈亏   百分比       市值", m_hFntMid, nX, nY, MSC_WHITE, 0);

	double			dTotalAll = 0;
	double			dTotalOne = 0;
	int				nPreY = 0;
	char			szLineText[1024];
	qcMyStockItem * pItem = NULL;
	NODEPOS pos = m_lstMyStock.GetHeadPosition();
	while (pos != NULL)
	{
		pItem = m_lstMyStock.GetNext(pos);
		nY = nY + m_nFntMidHeight + 8;
		memset(szLineText, 0, sizeof(szLineText));
		if (strlen(pItem->m_szCode) <= 0)
		{
			if (nPreY > 0)
			{
				FormatDouble(dTotalOne, szLineText, 89);
				DrawStrText(m_hMemDC, szLineText, m_hFntMid, nX, nPreY, MSC_WHITE, 0);
			}
			DrawStrText(m_hMemDC, pItem->m_szName, m_hFntMid, nX, nY, MSC_WHITE, 0);
			nPreY = nY;
			dTotalOne = 0;
		}
		else
		{
		//	DrawStrText(m_hMemDC, "股票  日期  数量  价格  金额    现价  盈亏  百分比  市值", m_hFntMid, nX, nY, MSC_WHITE, 0);
			sprintf(szLineText, "%s %s % 8d ", pItem->m_szName, pItem->m_szDate, pItem->m_nNumber);
			FormatDouble(pItem->m_dBuyPrice, szLineText + strlen(szLineText), 4);
			FormatDouble(pItem->m_dBuyPrice * pItem->m_nNumber, szLineText + strlen(szLineText), 10);
			FormatDouble(pItem->m_dNowPrice, szLineText + strlen(szLineText), 4);
			FormatDouble((pItem->m_dNowPrice - pItem->m_dBuyPrice) * pItem->m_nNumber, szLineText + strlen(szLineText), 11);
			FormatDouble((pItem->m_dNowPrice - pItem->m_dBuyPrice) * 100 / pItem->m_dBuyPrice, szLineText + strlen(szLineText), 5);
			strcat(szLineText, "%");
			FormatDouble(pItem->m_dNowPrice * pItem->m_nNumber, szLineText + strlen(szLineText), 10);

			if (pItem->m_dNowPrice > pItem->m_dBuyPrice)
				DrawStrText(m_hMemDC, szLineText, m_hFntMid, nX, nY, MSC_RED_1, 0);
			else if (pItem->m_dNowPrice == pItem->m_dBuyPrice)
				DrawStrText(m_hMemDC, szLineText, m_hFntMid, nX, nY, MSC_WHITE, 0);
			else
				DrawStrText(m_hMemDC, szLineText, m_hFntMid, nX, nY, MSC_GREEN_1, 0);

			dTotalOne += pItem->m_dNowPrice * pItem->m_nNumber;
			dTotalAll += pItem->m_dNowPrice * pItem->m_nNumber;
		}
	}

	FormatDouble(dTotalOne, szLineText, 89);
	DrawStrText(m_hMemDC, szLineText, m_hFntMid, nX, nPreY, MSC_WHITE, 0);

	nY = nY + m_nFntMidHeight + 8;
	DrawStrText(m_hMemDC, "汇总", m_hFntMid, nX, nY, MSC_WHITE, 0);
	FormatDouble(dTotalAll, szLineText, 89);
	DrawStrText(m_hMemDC, szLineText, m_hFntMid, nX, nY, MSC_WHITE, 0);

	BitBlt(hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);

	return QC_ERR_NONE;
}

int CViewMyStock::UpdateInfo(void)
{
	CAutoLock lock(&m_mtLock);

	int nRC = 0;
	if (m_pIO == NULL)
		m_pIO = new CIOcurl();
	nRC = qcStock_ParseRTListInfo(m_pIO, (const char **)&m_szCodeList[0], m_nCodeNum, &m_pRTInfo[0]);

	int				nIndex = 0;
	qcMyStockItem * pItem = NULL;
	NODEPOS pos = m_lstMyStock.GetHeadPosition();
	while (pos != NULL)
	{
		pItem = m_lstMyStock.GetNext(pos);
		if (strlen (pItem->m_szCode) <= 0)
			continue;
		pItem->m_dNowPrice = m_pRTInfo[nIndex++]->m_dNowPrice;
	}

	if (nRC == QC_ERR_NONE)
		InvalidateRect(m_hWnd, NULL, FALSE);

	return QC_ERR_NONE;
}

bool CViewMyStock::CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup)
{
	if (!CWndBase::CreateWnd(hParent, rcView, clrBG, pGroup))
		return false;

	CBaseGraphics::OnCreateWnd (m_hWnd);

	ThreadStart();

	return true;
}

LRESULT CViewMyStock::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

int	CViewMyStock::OpenMyStockFile(void)
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
	strcat(szFileName, "data\\mystock.txt");

	CIOFile ioFile;
	if (ioFile.Open(szFileName, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	char	szLine[1024];
	char *	pFileData = ioFile.GetData();
	int		nFileSize = (int)ioFile.GetSize();
	int		nRestSize = nFileSize;
	int		nLen = 0;

	char *	pStep = NULL;
	char *	pLine = NULL;

	m_nCodeNum = 0;
	qcMyStockItem * pItem = NULL;
	while (nRestSize > 0)
	{
		nLen = qcReadTextLine(pFileData, nRestSize, szLine, sizeof (szLine));
		nRestSize -= nLen;
		pFileData += nLen;
		if (nLen <= 4)
			continue;
		if (szLine[nLen - 2] == '\r')
			szLine[nLen - 2] = 0;
		else if (szLine[nLen - 1] == '\r')
			szLine[nLen - 1] = 0;
		pItem = new qcMyStockItem();
		m_lstMyStock.AddTail(pItem);
		memset(pItem, 0, sizeof(pItem));
		if (szLine[0] == ';')
		{
			strcpy(pItem->m_szName, szLine + 1);
		}
		else
		{
			pLine = szLine;
			pStep = strchr(pLine, ','); *pStep = 0;
			strcpy(pItem->m_szCode, pLine);

			strcpy(m_szCodeList[m_nCodeNum], pLine);
			m_nCodeNum++;

			pLine = pStep + 1;
			pStep = strchr(pLine, ','); *pStep = 0;
			strcpy(pItem->m_szDate, pLine);

			pLine = pStep + 1;
			pStep = strchr(pLine, ','); *pStep = 0;
			pItem->m_dBuyPrice = atof(pLine);

			pLine = pStep + 1;
			pStep = strchr(pLine, ',');
			pItem->m_nNumber = atoi(pLine);

			qcStockInfoItem *pInfo = NULL;
			NODEPOS pos = CStockItemList::g_stkList->m_lstStock.GetHeadPosition();
			while (pos != NULL)
			{
				pInfo = CStockItemList::g_stkList->m_lstStock.GetNext(pos);
				if (!strcmp(pInfo->m_szCode, pItem->m_szCode))
				{
					strcpy(pItem->m_szName, pInfo->m_szName);
					break;
				}
			}
		}
	}

	for (i = 0; i < m_nCodeNum; i++)
	{
		m_pRTInfo[i] = new qcStockRealTimeItem();
		memset(m_pRTInfo[i], 0, sizeof(qcStockRealTimeItem));
	}
	return QC_ERR_NONE;
}

void CViewMyStock::FormatDouble(double dNum, char * szText, int nNum)
{
	char szTxtNum[32];
	char szTxtVal[32];
	sprintf(szTxtNum, "%.2f", dNum);

	int nTxtLen = strlen(szTxtNum);
	strcpy(szTxtVal, szTxtNum);
	if (nTxtLen > 7)
	{
		szTxtVal[nTxtLen - 7] = ',';
		strcpy(szTxtVal + (nTxtLen - 7 + 1), szTxtNum + (nTxtLen - 7));
	}
	int nSpaceNum = nNum - strlen(szTxtVal) + 3;
	memset(szText, ' ', nSpaceNum);
	memcpy(szText + nSpaceNum, szTxtVal, strlen(szTxtVal));

}