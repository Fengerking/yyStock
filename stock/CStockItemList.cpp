/*******************************************************************************
	File:		CStockItemList.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockItemList.h"

#include "CIOFile.h"
#include "CIOcurl.h"
#include "USystemFunc.h"

CStockItemList * CStockItemList::g_stkList = NULL;

CStockItemList::CStockItemList(void)
	: CBaseObject ()
{
	SetObjectName ("CStockItemList");

	strcpy(m_szStockListURL, "http://quote.eastmoney.com/stocklist.html");
	qcGetAppPath(NULL, m_szStockListFile, sizeof(m_szStockListFile));
	strcat(m_szStockListFile, "data\\");
	qcCreateFolder(m_szStockListFile);

	strcat(m_szStockListFile, "qcStockList.txt");

	g_stkList = this;
}

CStockItemList::~CStockItemList(void)
{
	ReleaseList();
	g_stkList = NULL;
}

int CStockItemList::OpenHttpList(void)
{
	CIOcurl ioURL;
	char *	pDataBuff = NULL;
	int		nDataSize = 0;

	int nRC = ioURL.Open(m_szStockListURL, 0, 0);
	if (nRC != QC_ERR_NONE)
		return nRC;
	pDataBuff = ioURL.GetData();
	nDataSize = (int)ioURL.GetSize();

	char * pStart = strstr(pDataBuff, "(600000)");
	if (pStart == NULL)
		return QC_ERR_FAILED;

	ReleaseList();

	char * pData = pStart;
	char * pCode = pStart;
	char * pName = strstr(pStart - 16, ">");
	if (pName == NULL)
		return QC_ERR_FAILED;
	qcStockInfoItem	* pItem = new qcStockInfoItem();
	memset(pItem, 0, sizeof(qcStockInfoItem));
	m_lstStock.AddTail(pItem);
	strcpy(pItem->m_szCode, "600000");
	strncpy(pItem->m_szName, pName+1, pCode - pName -1);

	char	szLine[1024];
	int		nRestSize = nDataSize - (pData - pDataBuff);
	int		nLineLen = qcReadTextLine(pData, nRestSize, szLine, sizeof(szLine));
	pData += nLineLen;
	nRestSize -= nLineLen;

	while (nRestSize > 0)
	{
		nLineLen = qcReadTextLine(pData, nRestSize, szLine, sizeof(szLine));
		pData += nLineLen;
		nRestSize -= nLineLen;
		if (nLineLen > 32)
		{
			pName = strstr(szLine, "html\">");
			if (pName == NULL)
				continue;
			pCode = strstr(pName, "(60");
			if (pCode == NULL)
				pCode = strstr(pName, "(00");
			if (pCode == NULL)
				pCode = strstr(pName, "(30");
			if (pCode == NULL)
				continue;

			pItem = new qcStockInfoItem();
			memset(pItem, 0, sizeof(qcStockInfoItem));
			m_lstStock.AddTail(pItem);
			strncpy(pItem->m_szCode, pCode+1, 6);
			strncpy(pItem->m_szName, pName + 6, pCode - pName - 6);
		}
	}

	SaveList();

	return QC_ERR_NONE;
}

int CStockItemList::OpenFileList(void)
{
	CIOFile ioFile;
	if (ioFile.Open(m_szStockListFile, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
	{
		if (OpenHttpList() != QC_ERR_NONE)
			return QC_ERR_FAILED;
		return QC_ERR_NONE;
	}
	char *	pData = ioFile.GetData();
	if (pData == NULL)
		return QC_ERR_FAILED;
	int		nSize = (int)ioFile.GetSize();
	if (nSize < 1024)
		return QC_ERR_FAILED;

	ReleaseList();

	qcStockInfoItem	* pItem = new qcStockInfoItem();
	char	szLine[64];
	int		nRestSize = nSize;
	int		nLineLen = 0;
	char *	pStart = NULL;
	char *	pEnd = NULL;
	while (nRestSize > 0)
	{
		nLineLen = qcReadTextLine(pData, nRestSize, szLine, sizeof(szLine));
		pData += nLineLen;
		nRestSize -= nLineLen;
		pItem = new qcStockInfoItem();
		memset(pItem, 0, sizeof(qcStockInfoItem));
		m_lstStock.AddTail(pItem);
		pStart = strstr(szLine, ",");
		strncpy(pItem->m_szCode, szLine, pStart - szLine);
		pStart++;

		strncpy(pItem->m_szName, pStart, nLineLen - (pStart - szLine) - 2);
	}
	ioFile.Close();

	return QC_ERR_NONE;
}

int	CStockItemList::SaveList(void)
{
	CIOFile ioFile;
	ioFile.Open(m_szStockListFile, 0, QCIO_FLAG_WRITE);
	char szTxt[64];
	qcStockInfoItem	* pItem = NULL;
	NODEPOS pPos = m_lstStock.GetHeadPosition();
	while (pPos != NULL)
	{
		pItem = m_lstStock.GetNext(pPos);
		strcpy(szTxt, pItem->m_szCode);
		strcat(szTxt, ",");
		strcat(szTxt, pItem->m_szName);
		strcat(szTxt, "\r\n");
		ioFile.Write((unsigned char *)szTxt, strlen(szTxt));
	}
	ioFile.Close();
	return QC_ERR_NONE;
}

int	CStockItemList::ReleaseList(void)
{
	qcStockInfoItem * pItem = m_lstStock.RemoveTail();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstStock.RemoveTail();
	}
	return QC_ERR_NONE;
}
