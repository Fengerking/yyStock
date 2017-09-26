/*******************************************************************************
	File:		CIOcurl.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include <assert.h>  

#include "qcErr.h"
#include "CIOcurl.h"
#include "CIOFile.h"

#include "USystemFunc.h"

#define		SKIP_PEER_VERIFICATION		1  
//#define	SKIP_HOSTNAME_VERFICATION	1  

int		qcg_curl_read_times = 0;

CIOcurl::CIOcurl(void)
	: CIOBase()
{
	SetObjectName ("CIOcurl");

//	m_lstHeader.data = new char[256];
//	strcpy(m_lstHeader.data, "");// "Authorization:APPCODE 4779d46346404086ae4d077b9bfe9177");
//	m_lstHeader.next = NULL;
}

CIOcurl::~CIOcurl(void)
{
	Close();
    curl_global_cleanup();  
}

int CIOcurl::Open(const char * pURL, long long llOffset, int nFlag)
{
	if (m_pCURL == NULL)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		m_pCURL = curl_easy_init();
	}
	if (m_pCURL == NULL)
		return QC_ERR_STATUS;

	int nTryTimes = 1;
	int nRC = Request(pURL);
	while (nRC != QC_ERR_NONE)
	{
		qcSleep(200000 * nTryTimes);
		nRC = Request(pURL);
		nTryTimes++;
		if (nTryTimes > 5)
			break;
	}

	if (0)
	{
		CIOFile ioFile;
		ioFile.Open("c:\\work\\Temp\\0000.csv", 0, QCIO_FLAG_WRITE);
		ioFile.Write((unsigned char *)m_szBufferData.c_str(), m_szBufferData.length());
	}
	return QC_ERR_NONE;
}

int CIOcurl::Close(void)
{
	if (m_pCURL != NULL)
	{
		curl_easy_cleanup(m_pCURL);
		m_pCURL = NULL;
	}
	return CIOBase::Close();
}

char * CIOcurl::GetData(void)
{
	return (char *)m_szBufferData.c_str();
}

int	CIOcurl::Request(const char * pURL)
{
	qcg_curl_read_times = 0;
	m_szBufferHead.clear();
	m_szBufferData.clear();

	curl_easy_setopt(m_pCURL, CURLOPT_URL, pURL);
#ifdef SKIP_PEER_VERIFICATION  
	curl_easy_setopt(m_pCURL, CURLOPT_SSL_VERIFYPEER, 0L);
#endif  
	//	curl_easy_setopt(m_pCURL, CURLOPT_HTTPHEADER, &m_lstHeader);  

	curl_easy_setopt(m_pCURL, CURLOPT_HEADERFUNCTION, CURL_CallWriteHead);
	curl_easy_setopt(m_pCURL, CURLOPT_HEADERDATA, &m_szBufferHead);
	curl_easy_setopt(m_pCURL, CURLOPT_WRITEFUNCTION, CURL_CallWriteData);
	curl_easy_setopt(m_pCURL, CURLOPT_WRITEDATA, &m_szBufferData);

	CURLcode nRC = curl_easy_perform(m_pCURL);
	if (nRC == CURLE_OK)
	{
		m_dDownSize = 0;
		nRC = curl_easy_getinfo(m_pCURL, CURLINFO_SIZE_DOWNLOAD, &m_dDownSize);

		m_dDownTime = 0;
		nRC = curl_easy_getinfo(m_pCURL, CURLINFO_TOTAL_TIME, &m_dDownTime);

		m_dDownSpeed = 0;
		nRC = curl_easy_getinfo(m_pCURL, CURLINFO_SPEED_DOWNLOAD, &m_dDownSpeed);

		m_llSize = m_szBufferData.length();
	}
	else
	{
		//CURLE_OK						任务完成一切都好 
		//CURLE_UNSUPPORTED_PROTOCOL	不支持的协议，由URL的头部指定 
		//CURLE_COULDNT_CONNECT			不能连接到remote 主机或者代理 
		//CURLE_REMOTE_ACCESS_DENIED	访问被拒绝 
		//CURLE_HTTP_RETURNED_ERROR		Http返回错误 
		//CURLE_READ_ERROR				读本地文件错误  
		//CURLE_SSL_CACERT				访问HTTPS时需要CA证书路径 
		return QC_ERR_FAILED;
	}

	return QC_ERR_NONE;
}

size_t CIOcurl::CURL_CallWriteHead (const char * pData, size_t nBlock, size_t nSize, std::string * strStream)  
{  
	assert(strStream != NULL);  
	size_t nLen  = nBlock * nSize;  
	strStream->append(pData, nLen);  
	return nLen;  
} 

size_t CIOcurl::CURL_CallWriteData (const char * pData, size_t nBlock, size_t nSize, std::string * strStream)  
{  
	assert(strStream != NULL);  
	size_t nLen  = nBlock * nSize;  
	strStream->append(pData, nLen);  
	if (qcg_curl_read_times % 10 == 0)
		qcSleep(1000);
	qcg_curl_read_times++;
	return nLen;  
} 
