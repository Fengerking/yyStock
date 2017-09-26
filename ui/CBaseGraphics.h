/*******************************************************************************
	File:		CBaseGraphics.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CBaseGraphics_H__
#define __CBaseGraphics_H__

#include "windows.h"
#include "string.h"
#include "CBaseObject.h"

#define	MSC_BLACK	RGB (0, 0, 0)
#define	MSC_WHITE	RGB (255, 255, 255)
#define	MSC_RED		RGB (255, 0, 0)
#define	MSC_RED_1	RGB (150, 0, 0)
#define	MSC_RED_2	RGB (75, 0, 0)
#define	MSC_GREEN	RGB (0, 255, 0)
#define	MSC_GREEN_1	RGB (0, 150, 0)
#define	MSC_GREEN_2	RGB (0, 75, 0)
#define	MSC_BLUE	RGB (0, 0, 255)
#define	MSC_YELLOW	RGB (255, 255, 0)
#define MSC_PURPLE	RGB (255, 0, 255)
#define MSC_CYAN	RGB (0, 255, 255)
#define MSC_ORANGE	RGB (255, 165, 0)
#define	MSC_GRAY_1	RGB (25, 25, 25)
#define	MSC_GRAY_2	RGB (75, 75, 75)
#define	MSC_GRAY_3	RGB (125, 125, 125)
#define	MSC_GRAY_4	RGB (175, 175, 175)

#define	SAFE_DEL_OBJ(p)		\
	if (p != NULL) {		\
		DeleteObject (p);   \
		p = NULL; }			\

class CBaseGraphics : public CBaseObject
{
public:
	CBaseGraphics(void);
	virtual ~CBaseGraphics(void);

	virtual bool	DrawRect (HDC hDC, RECT * pRect, int nWidth, COLORREF nColor);
	virtual bool	DrawLine (HDC hDC, POINT ptStart, POINT ptEnd, int nWidth, COLORREF nColor);
	virtual bool	DrawLine (HDC hDC, int nX1, int nY1, int nX2, int nY2, int nWidth, COLORREF nColor);

	virtual HFONT	CreateFont (int nSize, int nWidth, char * pName = NULL);

	virtual bool	DrawDblText (HDC hDC, double dNumber, HFONT hFont, int nX, int nY, char * pName, int nColor, bool bPercent, int nPos);
	virtual bool	DrawIntText (HDC hDC, int nNumber, HFONT hFont, int nX, int nY, char * pName, int nColor, double dPrice, int nPos);
	virtual bool	DrawStrText (HDC hDC, char * pText, HFONT hFont, int nX, int nY, int nColor, int nPos);
	virtual bool	DrawWtrText (HDC hDC, wchar_t * pText, HFONT hFont, int nX, int nY, int nColor, int nPos);

protected:
	virtual bool	OnCreateWnd (HWND hWnd);
	virtual void	FormatDouble (double dNum, char * szText);

protected:
	char			m_szCode[32];
	double			m_dClosePrice;

	RECT			m_rcDraw;
	HDC				m_hMemDC;
	HBITMAP			m_hBmpInfo;
	HBITMAP			m_hBmpBack;
	HBITMAP			m_hBmpOld;
	unsigned char *	m_pBufBack;

	HFONT			m_hFntXLL;
	HFONT			m_hFntLrg;
	HFONT			m_hFntBig;
	HFONT			m_hFntMid;
	HFONT			m_hFntSml;

	int				m_nFntXLLHeight;
	int				m_nFntLrgHeight;
	int				m_nFntBigHeight;
	int				m_nFntMidHeight;
	int				m_nFntSmlHeight;

	HPEN			m_hPenWhite;
	HPEN			m_hPenRed;
	HPEN			m_hPenGreen;
	HPEN			m_hPenBlue;
	HPEN			m_hPenYellow;
	HPEN			m_hPenCyan;
	HPEN			m_hPenPurple;
	HPEN			m_hPenOrange;
	HPEN			m_hPenOld;

	HBRUSH			m_hBrushRed;
	HBRUSH			m_hBrushGreen;
	HBRUSH			m_hBrushBlue;
	HBRUSH			m_hBrushWhite;
	HBRUSH			m_hBrushYellow;
	HBRUSH			m_hBrushCyan;
	HBRUSH			m_hBrushPurple;
	HBRUSH			m_hBrushOrange;
	HBRUSH			m_hBrushGray;
};

#endif // __CBaseGraphics_H__
