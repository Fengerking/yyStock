/*******************************************************************************
	File:		CIOcurl.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CIOcurl_H__
#define __CIOcurl_H__
#include <string>  
#include <iostream>  

#include "CIOBase.h"
#include "curl.h"

class CIOcurl : public CIOBase
{
public:
    CIOcurl(void);
    virtual ~CIOcurl(void);

	virtual int 		Open(const char * pURL, long long llOffset, int nFlag);
	virtual int 		Close(void);

	virtual char *		GetData(void);

protected:
	virtual int			Request(const char * pURL);

protected:
	curl_slist		m_lstHeader;

    std::string		m_szBufferData;  
    std::string		m_szBufferHead;  
	double			m_dDownSize;
	double			m_dDownTime;
	double			m_dDownSpeed;

public:
	static CURL *	m_pCURL;
	static size_t	CURL_CallWriteHead (const char * pData, size_t nBlock, size_t nSize, std::string * strStream);
	static size_t	CURL_CallWriteData (const char * pData, size_t nBlock, size_t nSize, std::string * strStream);
};

#endif //__CIOcurl_H__
