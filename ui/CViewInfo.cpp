/*******************************************************************************
	File:		CViewInfo.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "qcErr.h"

#include "CViewInfo.h"

#include "ULogFunc.h"
#include "USystemFunc.h"

CViewInfo::CViewInfo(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_pIOFile(NULL)
{
	SetObjectName("CViewInfo");

	_tcscpy (m_szClassName, _T("yyStockViewFST"));
	_tcscpy (m_szWindowName, _T("yyStockViewFST"));
}

CViewInfo::~CViewInfo(void)
{
	SendMessage(m_hParent, WM_MSG_CODE_REMOVE, (WPARAM)this, 0);

	QC_DEL_P(m_pIOFile);
}

int CViewInfo::UpdateView (HDC hDC)
{
	if (m_pIOFile == NULL)
		m_pIOFile = new CIOFile();
	char szURL[1024];
	qcGetAppPath(NULL, szURL, sizeof(szURL));
	sprintf(szURL, "%sdata\\info\\%s.txt", szURL, m_szCode);
	if (m_pIOFile->Open(szURL, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	SetBkMode(hDC, TRANSPARENT);
	GetClientRect(m_hWnd, &m_rcWnd);
	int nX = 4;
	int nY = 4;

	char *	pFileData = m_pIOFile->GetData();
	int		nFileSize = (int)m_pIOFile->GetSize();
	char	szLine[2048];
	int		nLine = 0;
	int		nLineNum = 0;
	char *	pSplt = NULL;

	for (int i = 0; i < 24; i++)
	{
		nLine = qcReadTextLine(pFileData, nFileSize, szLine, 2048);
		pFileData += nLine; nFileSize -= nLine;
		pSplt = strstr(szLine, "|");
		if (pSplt != NULL)
			*pSplt = ' ';

		nLineNum = DrawLineText(hDC, nX, nY, szLine);
		nY += (m_nFntMidHeight + 8) * nLineNum;
		if (nFileSize <= 3)
			break;
	}

	m_pIOFile->Close();

	return QC_ERR_NONE;
}

int CViewInfo::UpdateInfo(void)
{
	InvalidateRect(m_hWnd, NULL, TRUE);

	return QC_ERR_NONE;
}

bool CViewInfo::CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup)
{
	if (!CWndBase::CreateWnd(hParent, rcView, clrBG, pGroup))
		return false;

	CBaseGraphics::OnCreateWnd(m_hParent);

	SendMessage(m_hParent, WM_MSG_CODE_REGIST, (WPARAM)this, 0);
	SendMessage(m_hParent, WM_MSG_CODE_REQUEST, (WPARAM)m_szCode, 0);

	UpdateInfo();

	return true;
}

LRESULT CViewInfo::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MSG_CODE_CHANGE:
		strcpy(m_szCode, (char *)wParam);
		UpdateInfo();
		return S_OK;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		UpdateView (hdc);
		EndPaint(hWnd, &ps);
	}
		break;

	case WM_DESTROY:
		break;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hWnd, uMsg, wParam, lParam);
}

int CViewInfo::DrawLineText(HDC hDC, int nXPos, int nYPos, char * pLine){

	SIZE	szSize;
	char *	pTxt = pLine;
	char	szTxt[256];
	int		nTxtNum = 0;
	int		i = 0;
	int		nCharNum = 0;
	int		nLineNum = 0;
	GetTextExtentPoint(hDC, "A", 1, &szSize);
	while ((unsigned int)(pTxt - pLine) < strlen(pLine))
	{
		nTxtNum = (m_rcWnd.right / szSize.cx) / 2 * 2;

		nCharNum = 0;
		for (i = 0; i < nTxtNum; i++)
		{
			if (pTxt[i] < 0)
				nCharNum++;
		}
		if (nCharNum % 2 != 0)
			nTxtNum += 1;

		strncpy(szTxt, pTxt, nTxtNum);
		szTxt[nTxtNum] = 0;

		pTxt += nTxtNum;
		DrawStrText(hDC, szTxt, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0);
		nYPos += m_nFntMidHeight + 8;
		nLineNum++;
	}

	return nLineNum;
}
