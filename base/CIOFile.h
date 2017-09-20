/*******************************************************************************
	File:		CIOFile.h

	Contains:	local file io header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-05		Bangfei			Create file

*******************************************************************************/
#ifndef __CIOFile_H__
#define __CIOFile_H__

#include "CIOBase.h"

class CIOFile : public CIOBase
{
public:
    CIOFile(void);
    virtual ~CIOFile(void);

	virtual int 		Open (const char * pURL, long long llOffset, int nFlag);
	virtual int 		Close (void);

	virtual int			Read (unsigned char * pBuff, int & nSize, bool bFull, int nFlag);
	virtual int			Write (unsigned char * pBuff, int nSize);
	virtual long long 	SetPos (long long llPos, int nFlag);

	virtual char *		GetData(void);
	virtual QCIOType	GetType (void);

protected:
	HANDLE				m_hFile;
};

#endif // __CIOFile_H__