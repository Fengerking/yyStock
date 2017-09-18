/*******************************************************************************
	File:		CPngDec.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CPngDec_H__
#define __CPngDec_H__
#include <string>  
#include <iostream>  

#include "CBaseObject.h"
#include "png.h"

class CPngDec : public CBaseObject
{
public:
    CPngDec(void);
    virtual ~CPngDec(void);

	virtual int		OpenSource(const char * pURL);
	virtual int		Close(void);

	virtual int		GetWidth(void) { return m_nWidth; }
	virtual int		GetHeight(void) { return m_nHeight; }
	virtual HBITMAP GetBmp(void) { return m_hBmpImage; }

protected:
	png_structp		m_hHandle;
	png_infop		m_hInfo;
	png_bytep *		m_pData;

	int				m_nColorType;
	int				m_nWidth;
	int				m_nHeight;

	unsigned char *	m_pBuffData;
	int				m_nBuffSize;
	int				m_nBuffRead;

	HBITMAP			m_hBmpImage;
	unsigned char *	m_pBmpBuff;

public:
	static void		pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length);
};

#endif //__CPngDec_H__
