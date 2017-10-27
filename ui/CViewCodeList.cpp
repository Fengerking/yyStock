/*******************************************************************************
	File:		CViewCodeList.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "qcErr.h"

#include "CViewCodeList.h"

#include "CRegMng.h"
#include "ULogFunc.h"
#include "USystemFunc.h"

CViewCodeList::CViewCodeList(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_pCodeList(NULL)
	, m_nSolidNum(1)
{
	SetObjectName("CViewCodeList");

	_tcscpy (m_szClassName, _T("yyStockViewCodeList"));
	_tcscpy (m_szWindowName, _T("yyStockViewCodeList"));
}

CViewCodeList::~CViewCodeList(void)
{
}

int CViewCodeList::UpdateView (HDC hDC)
{
	GetClientRect(m_hWnd, &m_rcWnd);
	GetClientRect(m_hWnd, &m_rcDraw);
	DrawRect(hDC, &m_rcWnd, 2, MSC_GRAY_4);

	SetBkMode(hDC, TRANSPARENT);

	m_nSolidNum = 1;
	int nY = 6;
	DrawStrText(hDC, "000001", m_hFntMid, 8, nY, MSC_GRAY_3, 0);
	DrawStrText(hDC, "上证指数", m_hFntMid, m_rcDraw.right / 2 - 12, nY, MSC_GRAY_4, 0);
	nY += m_nFntMidHeight;

	int nIndex = 0;
	qcStockInfoItem * pItem = NULL;
	NODEPOS pos = m_pCodeList->GetHeadPosition();
	while (pos != NULL)
	{
		pItem = m_pCodeList->GetNext(pos);
		nIndex++;
		if (nIndex < m_nItemNum * m_nPageIdx + m_nSolidNum)
			continue;

		if (!strcmp(m_szCode, pItem->m_szCode))
		{
			RECT rcItem;
			SetRect(&rcItem, 2, nY, m_rcDraw.right - 10, nY + m_nFntMidHeight);
			FillRect(hDC, &rcItem, m_hBrushBlue);
		}
		DrawStrText(hDC, pItem->m_szCode, m_hFntMid, 8, nY, MSC_GRAY_3, 0);
		DrawStrText(hDC, pItem->m_szName, m_hFntMid, m_rcDraw.right / 2 - 12, nY, MSC_GRAY_4, 0);
		nY += m_nFntMidHeight;
		if (nY + m_nFntMidHeight  > m_rcWnd.bottom)
			break;
	}

	DrawScrollBar(hDC);

	return QC_ERR_NONE;
}

int CViewCodeList::UpdatePageNum(void)
{
	GetClientRect(m_hWnd, &m_rcWnd);
	m_nItemNum = (m_rcWnd.bottom - m_nSolidNum * m_nFntMidHeight) / m_nFntMidHeight;
	m_nPageNum = (m_pCodeList->GetCount() + m_nItemNum - 1) / m_nItemNum;

	InvalidateRect(m_hWnd, NULL, TRUE);

	return QC_ERR_NONE;
}

bool CViewCodeList::CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup)
{
	CBaseGraphics::OnCreateWnd(m_hWnd);

	if (!CWndBase::CreateWnd(hParent, rcView, clrBG, pGroup))
		return false;

	m_pCodeList = &CStockItemList::g_stkList->m_lstStock;

	UpdatePageNum();

	return true;
}

LRESULT CViewCodeList::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		UpdatePageNum();
		break;

	case WM_LBUTTONUP:
	{
		int nIndex = 0;
		int nYPos = HIWORD(lParam);
		int nItem = nYPos / m_nFntMidHeight;
		int nStartItem = m_nItemNum * m_nPageIdx;
		nItem = nItem + nStartItem - m_nSolidNum;

		qcStockInfoItem * pItem = NULL;
		NODEPOS pos = m_pCodeList->GetHeadPosition();
		while (pos != NULL)
		{
			pItem = m_pCodeList->GetNext(pos);
			nIndex++;
			if (nIndex > nItem)
				break;
		}
		if (pos != NULL && pItem != NULL)
		{
			strcpy(m_szCode, pItem->m_szCode);
			PostMessage(m_hParent, WM_MSG_CODE_CHANGE, (WPARAM)m_szCode, 0);
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
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

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hWnd, uMsg, wParam, lParam);
}

LRESULT	CViewCodeList::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
