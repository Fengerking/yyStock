/*******************************************************************************
	File:		CBaseGraphics.cpp

	Contains:	base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"
#include "string.h"

#include "qcDef.h"
#include "CBaseGraphics.h"

CBaseGraphics::CBaseGraphics(void)
	: CBaseObject()
	, m_dClosePrice (0)
	, m_hMemDC (NULL)
	, m_hBmpInfo (NULL)
	, m_hBmpBack (NULL)
	, m_pBufBack (NULL)
	, m_hFntXLL (NULL)
	, m_hFntLrg (NULL)
	, m_hFntBig (NULL)
	, m_hFntMid (NULL)
	, m_hFntSml (NULL)
	, m_nFntXLLHeight (168)
	, m_nFntLrgHeight (72)
	, m_nFntBigHeight (48)
	, m_nFntMidHeight (28)
	, m_nFntSmlHeight (20)
	, m_hPenRed (NULL)
	, m_hPenGreen (NULL)
	, m_hPenBlue (NULL)
	, m_hPenWhite (NULL)
	, m_hPenYellow (NULL)
	, m_hPenCyan (NULL)
	, m_hPenPurple (NULL)
	, m_hPenOrange (NULL)
	, m_hPenOld (NULL)
	, m_hBrushRed (NULL)
	, m_hBrushGreen (NULL)
	, m_hBrushBlue (NULL)
	, m_hBrushWhite (NULL)
	, m_hBrushCyan (NULL)
	, m_hBrushPurple (NULL)
	, m_hBrushYellow (NULL)
	, m_hBrushOrange (NULL)
	, m_hBrushGray (NULL)
{
	SetObjectName("CBaseGraphics");
	strcpy (m_szCode, "600895");
}

CBaseGraphics::~CBaseGraphics(void)
{
	if (m_hMemDC != NULL)
	{
		SelectObject (m_hMemDC, m_hBmpOld);
		DeleteDC (m_hMemDC);
	}
	SAFE_DEL_OBJ (m_hFntXLL);
	SAFE_DEL_OBJ (m_hFntLrg);
	SAFE_DEL_OBJ (m_hFntBig);
	SAFE_DEL_OBJ (m_hFntMid);
	SAFE_DEL_OBJ (m_hFntSml);

	SAFE_DEL_OBJ (m_hBmpInfo);
	SAFE_DEL_OBJ (m_hBmpBack);
	QC_DEL_A (m_pBufBack);

	SAFE_DEL_OBJ (m_hPenRed);
	SAFE_DEL_OBJ (m_hPenGreen);
	SAFE_DEL_OBJ (m_hPenBlue);
	SAFE_DEL_OBJ (m_hPenWhite);
	SAFE_DEL_OBJ (m_hPenYellow);
	SAFE_DEL_OBJ (m_hPenCyan);
	SAFE_DEL_OBJ (m_hPenPurple);
	SAFE_DEL_OBJ (m_hPenOrange);

	SAFE_DEL_OBJ (m_hBrushRed);
	SAFE_DEL_OBJ (m_hBrushGreen);
	SAFE_DEL_OBJ (m_hBrushBlue);
	SAFE_DEL_OBJ (m_hBrushWhite);
	SAFE_DEL_OBJ (m_hBrushYellow);
	SAFE_DEL_OBJ (m_hBrushCyan);
	SAFE_DEL_OBJ (m_hBrushPurple);
	SAFE_DEL_OBJ (m_hBrushOrange);
	SAFE_DEL_OBJ (m_hBrushGray);
}

bool CBaseGraphics::DrawLine (HDC hDC, POINT ptStart, POINT ptEnd, int nWidth, COLORREF nColor)
{
	if (hDC == NULL)
		return false;

	HPEN	hPen = CreatePen (PS_SOLID, nWidth, nColor);
	HGDIOBJ	hOld = SelectObject (hDC, hPen);

	MoveToEx (hDC, ptStart.x, ptStart.y, NULL);
	LineTo (hDC, ptEnd.x, ptEnd.y);

	SelectObject (hDC, hOld);
	DeleteObject (hPen); 

	return true;
}

bool CBaseGraphics::DrawLine (HDC hDC, int nX1, int nY1, int nX2, int nY2, int nWidth, COLORREF nColor)
{
	if (hDC == NULL)
		return false;

	HPEN	hPen = CreatePen (PS_SOLID, nWidth, nColor);
	HGDIOBJ	hOld = SelectObject (hDC, hPen);

	MoveToEx (hDC, nX1, nY1, NULL);
	LineTo (hDC, nX2, nY2);

	SelectObject (hDC, hOld);
	DeleteObject (hPen); 

	return true;
}

bool CBaseGraphics::DrawRect (HDC hDC, RECT * pRect, int nWidth, COLORREF nColor)
{
	if (hDC == NULL || pRect == NULL)
		return false;

	HPEN	hPen = CreatePen (PS_SOLID, nWidth, nColor);
	HGDIOBJ	hOld = SelectObject (hDC, hPen);

	RECT rcDraw;
	SetRect(&rcDraw, pRect->left + nWidth / 2, pRect->top + nWidth / 2, pRect->right - nWidth / 2, pRect->bottom - nWidth / 2);
	MoveToEx(hDC, rcDraw.left, rcDraw.top, NULL);
	LineTo (hDC, rcDraw.right, rcDraw.top);
	LineTo (hDC, rcDraw.right, rcDraw.bottom);
	LineTo (hDC, rcDraw.left, rcDraw.bottom);
	LineTo (hDC, rcDraw.left, rcDraw.top);

	SelectObject (hDC, hOld);
	DeleteObject (hPen); 

	return true;
}

HFONT CBaseGraphics::CreateFont (int nSize, int nWidth, char * pName)
{
	HFONT hFont = NULL;

    LOGFONT lf; 
	lf.lfHeight = -nSize;
	lf.lfWidth = 0;
	lf.lfEscapement	= 0;
	lf.lfOrientation = 0;
	lf.lfWeight = nWidth;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = GB2312_CHARSET;
	lf.lfOutPrecision = 1;
	lf.lfClipPrecision = 2;
	lf.lfQuality = 1;
	lf.lfPitchAndFamily	= 49;
	if (pName != NULL)
		strcpy (lf.lfFaceName, pName);
	else
		strcpy (lf.lfFaceName, "·ÂËÎÌå");

    hFont = CreateFontIndirect(&lf); 

	return hFont;
}

bool CBaseGraphics::DrawDblText (HDC hDC, double dNumber, HFONT hFont, int nX, int nY, char * pName, int nColor, bool bPercent, int nPos)
{
	RECT	rcDraw;
	char	szDraw[64];
	if (nColor >= 0)
	{
		SetTextColor (hDC, nColor);
	}
	else
	{
		if (nColor == -1)
		{
			if (dNumber > m_dClosePrice)
				SetTextColor (hDC, MSC_RED);
			else if (dNumber == m_dClosePrice || dNumber == 0)
				SetTextColor (hDC, MSC_WHITE);
			else
				SetTextColor (hDC, MSC_GREEN);
		}
		else if (nColor == -2)
		{
			if (dNumber > 0)
				SetTextColor (hDC, MSC_RED);
			else if (dNumber == 0)
				SetTextColor (hDC, MSC_WHITE);
			else
				SetTextColor (hDC, MSC_GREEN);
		}
	}
	if (bPercent)
		sprintf (szDraw, "%s%3.2f%%", pName, dNumber);
	else
		sprintf (szDraw, "%s%3.2f", pName, dNumber);
	SelectObject (hDC, hFont);

	if (nPos == 1)
	{
		SIZE szSize;
		GetTextExtentPoint (hDC, szDraw, strlen (szDraw), &szSize);
		SetRect (&rcDraw, nX - szSize.cx, nY, m_rcDraw.right, nY + 300);
	}
	else if (nPos == 2)
	{
		SIZE szSize;
		GetTextExtentPoint (hDC, szDraw, strlen (szDraw), &szSize);
		SetRect (&rcDraw, nX - (szSize.cx / 2), nY, m_rcDraw.right, nY + 300);
	}
	else
	{
		SetRect (&rcDraw, nX, nY, m_rcDraw.right, nY + 300);
	}
	DrawText (hDC, szDraw, strlen (szDraw), &rcDraw, DT_LEFT);

	return true;
}

bool CBaseGraphics::DrawIntText (HDC hDC, int nNumber, HFONT hFont, int nX, int nY, char * pName, int nColor, double dPrice, int nPos)
{
	RECT	rcDraw;
	char	szDraw[64];
	if (nColor >= 0)
	{
		SetTextColor (hDC, nColor);
	}
	else
	{
		if (nColor == -1)
		{
			if (dPrice > m_dClosePrice)
				SetTextColor (hDC, MSC_RED);
			else if (dPrice == m_dClosePrice || dPrice == 0)
				SetTextColor (hDC, MSC_WHITE);
			else
				SetTextColor (hDC, MSC_GREEN);
		}
		else if (nColor == -2)
		{
			if (nNumber > 0)
				SetTextColor (hDC, MSC_RED);
			else if (nNumber == 0)
				SetTextColor (hDC, MSC_WHITE);
			else
				SetTextColor (hDC, MSC_GREEN);
		}
	}
	sprintf (szDraw, "%s%d", pName, nNumber);
	SelectObject (hDC, hFont);
	if (nPos == 1)
	{
		SIZE szSize;
		GetTextExtentPoint (hDC, szDraw, strlen (szDraw), &szSize);
		SetRect (&rcDraw, nX - szSize.cx, nY, m_rcDraw.right, nY + 300);
	}
	else if (nPos == 2)
	{
		SIZE szSize;
		GetTextExtentPoint (hDC, szDraw, strlen (szDraw), &szSize);
		SetRect (&rcDraw, nX - (szSize.cx / 2), nY, m_rcDraw.right, nY + 300);
	}
	else
	{
		SetRect (&rcDraw, nX, nY, m_rcDraw.right, nY + 300);
	}
	DrawText (hDC, szDraw, strlen (szDraw), &rcDraw, DT_LEFT);

	return true;
}

bool CBaseGraphics::DrawStrText (HDC hDC, char * pText, HFONT hFont, int nX, int nY, int nColor, int nPos)
{
	RECT	rcDraw;
	SetTextColor (hDC, nColor);
	SelectObject (hDC, hFont);
	if (nPos == 1)
	{
		SIZE szSize;
		GetTextExtentPoint (hDC, pText, strlen (pText), &szSize);
		SetRect (&rcDraw, nX - szSize.cx, nY, m_rcDraw.right, nY + 300);
	}
	else if (nPos == 2)
	{
		SIZE szSize;
		GetTextExtentPoint (hDC, pText, strlen (pText), &szSize);
		SetRect (&rcDraw, nX - (szSize.cx / 2), nY, m_rcDraw.right, nY + 300);
	}
	else
	{
		SetRect (&rcDraw, nX, nY, m_rcDraw.right, nY + 300);
	}
	DrawText (hDC, pText, strlen (pText), &rcDraw, DT_LEFT);
	return true;
}

bool CBaseGraphics::DrawWtrText (HDC hDC, wchar_t * pText, HFONT hFont, int nX, int nY, int nColor, int nPos)
{
	RECT	rcDraw;
	SetTextColor (hDC, nColor);
	SelectObject (hDC, hFont);
	if (nPos == 1)
	{
		SIZE szSize;
		GetTextExtentPointW (hDC, pText, wcslen (pText), &szSize);
		SetRect (&rcDraw, nX - szSize.cx, nY, m_rcDraw.right, nY + 300);
	}
	else if (nPos == 2)
	{
		SIZE szSize;
		GetTextExtentPointW (hDC, pText, wcslen (pText), &szSize);
		SetRect (&rcDraw, nX - (szSize.cx / 2), nY, m_rcDraw.right, nY + 300);
	}
	else
	{
		SetRect (&rcDraw, nX, nY, m_rcDraw.right, nY + 300);
	}
	DrawTextW (hDC, pText, wcslen (pText), &rcDraw, DT_LEFT);
	return true;
}


bool CBaseGraphics::OnCreateWnd (HWND hWnd)
{
	GetClientRect (hWnd, &m_rcDraw);
	m_rcDraw.left += 8;
	m_rcDraw.top += 8;
	m_rcDraw.bottom -= 8;
	m_rcDraw.right -= 8;

	if (m_hFntXLL != NULL)
		return true;

	m_hFntXLL = CreateFont (m_nFntXLLHeight, 900);
	m_hFntLrg = CreateFont (m_nFntLrgHeight, 900);
	m_hFntBig = CreateFont (m_nFntBigHeight, 900);
	m_hFntMid = CreateFont (m_nFntMidHeight, 600);
	m_hFntSml = CreateFont (m_nFntSmlHeight, 300);

	m_hPenRed	= CreatePen (PS_SOLID, 1, MSC_RED);
	m_hPenGreen = CreatePen (PS_SOLID, 1, MSC_GREEN);
	m_hPenBlue = CreatePen (PS_SOLID, 1, MSC_BLUE);
	m_hPenWhite = CreatePen (PS_SOLID, 1, MSC_WHITE);
	m_hPenYellow = CreatePen (PS_SOLID, 1, MSC_YELLOW);
	m_hPenPurple = CreatePen (PS_SOLID, 1, MSC_PURPLE);
	m_hPenCyan = CreatePen (PS_SOLID, 1, MSC_CYAN);
	m_hPenOrange = CreatePen (PS_SOLID, 1, MSC_ORANGE);

	m_hBrushRed = CreateSolidBrush (MSC_RED);
	m_hBrushGreen = CreateSolidBrush (MSC_GREEN);
	m_hBrushBlue = CreateSolidBrush (MSC_BLUE);
	m_hBrushWhite = CreateSolidBrush (MSC_WHITE);
	m_hBrushCyan = CreateSolidBrush (MSC_CYAN);
	m_hBrushPurple = CreateSolidBrush (MSC_PURPLE);
	m_hBrushYellow = CreateSolidBrush (MSC_YELLOW);
	m_hBrushOrange = CreateSolidBrush (MSC_ORANGE);
	m_hBrushGray = CreateSolidBrush (MSC_GRAY_1);

	return true;
}

void CBaseGraphics::FormatDouble (double dNum, char * szText)
{
	if (dNum < -10)
		sprintf (szText, " %.2f", dNum);
	else if (dNum < 0)
		sprintf (szText, "  %.2f", dNum);
	else if (dNum < 10)
		sprintf (szText, "   %.2f", dNum);
	else if (dNum < 100)
		sprintf (szText, "  %.2f", dNum);
	else
		sprintf (szText, " %.2f", dNum);
}
