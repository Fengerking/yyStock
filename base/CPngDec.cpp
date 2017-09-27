/*******************************************************************************
	File:		CPngDec.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include <assert.h>  

#include "qcErr.h"
#include "CPngDec.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

#define PNG_BYTES_TO_CHECK 4  

CPngDec::CPngDec(void)
	: CBaseObject ()
	, m_pIO(NULL)
	, m_hHandle(NULL)
	, m_hInfo(NULL)
	, m_pData(NULL)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_hBmpImage(NULL)
	, m_pBmpBuff(NULL)
{
	SetObjectName ("CPngDec");
}

CPngDec::~CPngDec(void)
{
	Close();
	QC_DEL_P(m_pIO);
}

int	CPngDec::OpenSource(const char * pURL)
{
	if (m_pIO == NULL)
		m_pIO = new CIOcurl();
	if (m_pIO->Open(pURL, 0, 0) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	m_pBuffData = (unsigned char *)m_pIO->GetData();
	m_nBuffSize = (int)m_pIO->GetSize();
	m_nBuffRead = 0;

	Close();
	m_hHandle = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (m_hHandle == NULL)
		return QC_ERR_FAILED;
	m_hInfo = png_create_info_struct(m_hHandle);
	if (m_hInfo == NULL)
		return QC_ERR_FAILED;
	if (setjmp(png_jmpbuf(m_hHandle)))
		return QC_ERR_FAILED;

	png_set_read_fn(m_hHandle, this, pngReadCallback);
	png_read_png(m_hHandle, m_hInfo, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, 0);

	m_nColorType = png_get_color_type(m_hHandle, m_hInfo);
	m_nWidth = png_get_image_width(m_hHandle, m_hInfo);
	m_nHeight = png_get_image_height(m_hHandle, m_hInfo);
	m_pData = png_get_rows(m_hHandle, m_hInfo);

	if (m_hBmpImage == NULL)
		m_hBmpImage = CreateBitmap(m_nWidth, m_nHeight, 1, 32, NULL);
	if (m_pBmpBuff == NULL)
		m_pBmpBuff = new unsigned char[m_nWidth * m_nHeight * 4];
	unsigned char * pBmpBuff = m_pBmpBuff;
	int				nBytesPixel = 3;
	if (m_nColorType == PNG_COLOR_TYPE_RGB_ALPHA)
		nBytesPixel = 4;

	switch (m_nColorType)
	{
	case PNG_COLOR_TYPE_RGB:
	case PNG_COLOR_TYPE_RGB_ALPHA:
		for (int y = 0; y < m_nHeight; y++)
		{
			pBmpBuff = m_pBmpBuff + y * m_nWidth * 4;
			for (int x = 0; x < m_nWidth * nBytesPixel;)
			{
				//QCLOGI("Y   X  R  G  B %d  %d  %d  %d  %d", y, x, m_pData[y][x], m_pData[y][x+1], m_pData[y][x+2])
				if (x > 456 * nBytesPixel && x < 498 * nBytesPixel && y > 18 && y < 27)
				{
					if (m_pData[y][x] == 204 && m_pData[y][x + 1] == 204 && m_pData[y][x + 2] == 204)
					{
						*pBmpBuff++ = 0; // blue
						*pBmpBuff++ = 0; // green
						*pBmpBuff++ = 0; // red
					}
					else if (m_pData[y][x] == 255 && m_pData[y][x + 1] == 255 && m_pData[y][x + 2] == 255)
					{
						*pBmpBuff++ = 0;
						*pBmpBuff++ = 0;
						*pBmpBuff++ = 0;
					}
					else
					{
						*pBmpBuff++ = m_pData[y][x];
						*pBmpBuff++ = m_pData[y][x + 1];
						*pBmpBuff++ = m_pData[y][x + 2];
					}
				}
				else if (m_pData[y][x] == 255 && m_pData[y][x+1] == 255 && m_pData[y][x+2] == 255)
				{
					*pBmpBuff++ = 0; 
					*pBmpBuff++ = 0; 
					*pBmpBuff++ = 0; 
				}
				else if (m_pData[y][x] == 221 && m_pData[y][x + 1] == 233 && m_pData[y][x + 2] == 253)
				{
					*pBmpBuff++ = 0;
					*pBmpBuff++ = 0;
					*pBmpBuff++ = 0;
				}
				else if (m_pData[y][x] == 254 && m_pData[y][x + 1] == 10 && m_pData[y][x + 2] == 10)
				{
					*pBmpBuff++ = 0; 
					*pBmpBuff++ = 0; 
					*pBmpBuff++ = 255; 
				}
				else
				{
					*pBmpBuff++ = m_pData[y][x]; 
					*pBmpBuff++ = m_pData[y][x+1]; 
					*pBmpBuff++ = m_pData[y][x+2]; 
				}
				x += 3;

				if (m_nColorType == PNG_COLOR_TYPE_RGB_ALPHA)
					*pBmpBuff++ = m_pData[y][x++]; // alpha
				else
					*pBmpBuff++ = 0;
			}
		}
		break;

	default:
		break;
	}
	SetBitmapBits(m_hBmpImage, m_nWidth * m_nHeight * 4, m_pBmpBuff);

	return QC_ERR_NONE;
}

void CPngDec::pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	CPngDec* pDec = (CPngDec*)png_get_io_ptr(png_ptr);
	if ((int)(pDec->m_nBuffRead + length) <= pDec->m_nBuffSize)
	{
		memcpy(data, pDec->m_pBuffData + pDec->m_nBuffRead, length);
		pDec->m_nBuffRead += length;
	}
	else
		png_error(png_ptr, "pngReaderCallback failed");
}

int	CPngDec::Close(void)
{
	if (m_hHandle != NULL)
	{
		if (m_hInfo != NULL)
			png_destroy_read_struct(&m_hHandle, &m_hInfo, 0);
		else
			png_destroy_read_struct(&m_hHandle, 0, 0);
		m_hInfo = NULL;
		m_hHandle = NULL;
	}
	QC_DEL_A(m_pBmpBuff);
	if (m_hBmpImage != NULL)
	{
		DeleteObject(m_hBmpImage);
		m_hBmpImage = NULL;
	}

	return QC_ERR_NONE;
}
