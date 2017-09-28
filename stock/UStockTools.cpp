/*******************************************************************************
	File:		UStockTools.cpp

	Contains:	The base utility for thread implement file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "qcType.h"
#include "qcErr.h"

#include "CIOcurl.h"
#include "CIOFile.h"

#include "UStockTools.h"
#include "UStockParser.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

int	qcStock_DownLoadData_History(CIOcurl * pIO, const char * pCode)
{
	if (pIO == NULL || pCode == NULL)
		return QC_ERR_ARG;

	char		szFileName[256];
	char		szHttpName[256];
	SYSTEMTIME	tmStart;
	SYSTEMTIME	tmEnd;
	memset(&tmStart, 0, sizeof(SYSTEMTIME));
	tmStart.wYear = 1996;
	tmStart.wMonth = 1;
	tmStart.wDay = 1;
	GetLocalTime(&tmEnd);

	qcGetAppPath(NULL, szFileName, sizeof(szFileName));
	strcat(szFileName, "data\\history\\");
	strcat(szFileName, pCode);
	strcat(szFileName, ".csv");

	CIOFile	ioFile;
	char *	pFileData = NULL;
	char *	pRestData = NULL;
	int		nFileSize = 0;
	if (ioFile.Open(szFileName, 0, QCIO_FLAG_READ) == QC_ERR_NONE)
	{
		char	szLine[1024];
		nFileSize = (int)ioFile.GetSize();
		pFileData = new char[nFileSize];
		ioFile.Read((unsigned char *)pFileData, nFileSize, true, 0);
		int nLen = qcReadTextLine(pFileData, nFileSize, szLine, sizeof(szLine));
		pRestData = pFileData + nLen;
		nLen = qcReadTextLine(pRestData, nFileSize - nLen, szLine, sizeof(szLine));
		pRestData = pRestData + nLen;

		// 2015-05-08
		szLine[10] = 0;
		sscanf(szLine, "%d-%02d-%02d", &tmStart.wYear, &tmStart.wMonth, &tmStart.wDay);
		ioFile.Close();
	}

	if (strlen(pCode) == 7)
		sprintf(szHttpName, "http://quotes.money.163.com/service/chddata.html?code=%s&start=%d%02d%02d&end=%d%02d%02d",
							pCode, tmStart.wYear, tmStart.wMonth, tmStart.wDay, tmEnd.wYear, tmEnd.wMonth, tmEnd.wDay);
	else if (pCode[0] == '6')
		sprintf(szHttpName, "http://quotes.money.163.com/service/chddata.html?code=0%s&start=%d%02d%02d&end=%d%02d%02d", 
							pCode, tmStart.wYear, tmStart.wMonth, tmStart.wDay, tmEnd.wYear, tmEnd.wMonth, tmEnd.wDay);
	else
		sprintf(szHttpName, "http://quotes.money.163.com/service/chddata.html?code=1%s&start=%d%02d%02d&end=%d%02d%02d",
							pCode, tmStart.wYear, tmStart.wMonth, tmStart.wDay, tmEnd.wYear, tmEnd.wMonth, tmEnd.wDay);

	int nRC = pIO->Open(szHttpName, 0, 0);
	if (nRC != QC_ERR_NONE)
	{
		QC_DEL_A(pFileData);
		return QC_ERR_FAILED;
	}

	if (ioFile.Open(szFileName, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
	{
		QC_DEL_A(pFileData);
		return QC_ERR_FAILED;
	}

	ioFile.Write((unsigned char*)pIO->GetData(), (int)pIO->GetSize());
	if (pRestData != NULL)
	{
		ioFile.Write((unsigned char *)pRestData, nFileSize - (pRestData - pFileData));
	}
	ioFile.Close();

	return QC_ERR_NONE;
}

int	qcStock_DownLoadData_FHSP(CIOcurl * pIO, const char * pCode)
{
	if (pIO == NULL || pCode == NULL)
		return QC_ERR_ARG;

	char szURL[1024];
	qcGetAppPath(NULL, szURL, sizeof(szURL));
	sprintf(szURL, "%sdata\\fhsp\\%s.txt", szURL, pCode);
	CIOFile ioFile;
	if (ioFile.Open(szURL, 0, QCIO_FLAG_READ) == QC_ERR_NONE)
	{
		ioFile.Close();
		return QC_ERR_FINISH;
	}

	sprintf(szURL, "http://vip.stock.finance.sina.com.cn/corp/go.php/vISSUE_ShareBonus/stockid/%s.phtml", pCode);
	int nRC = pIO->Open(szURL, 0, 0);
	if (nRC != QC_ERR_NONE)
		return nRC;

	CObjectList<qcStockFHSPInfoItem>	lstItem;

	int		nFileSize = (int)pIO->GetSize();
	char *	pFileData = pIO->GetData ();

	int			nRest = nFileSize;
	char *		pBuff = pFileData;
	char		szLine[2048];
	char *		pPos = NULL;
	char *		pTxt = NULL;
	int			nLine = 0;
	qcStockFHSPInfoItem  * pItem = NULL;
	while (pBuff - pFileData < nFileSize)
	{
		nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
		if (nLine <= 0)
			break;
		pBuff += nLine;
		if (strstr(szLine, "<tbody>") != NULL)
			break;
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
		if (nLine <= 0)
			break;
		pBuff += nLine;
		if (strstr(szLine, "</tbody>") != NULL)
			break;
		if (strstr(szLine, "<tr>") == NULL)
			continue;

		nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
		if (nLine <= 0)
			break;
		pBuff += nLine;
		pTxt = strstr(szLine, "<td>");
		if (pTxt == NULL)
			break;
		pTxt += 4;
		pPos = strstr(szLine, "</td>");
		if (pPos == NULL)
			break;
		*pPos = 0;

		pItem = new qcStockFHSPInfoItem();
		memset(pItem, 0, sizeof(qcStockFHSPInfoItem));
		lstItem.AddTail(pItem);
		sscanf(pTxt, "%d-%d-%d", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);

		nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
		pBuff += nLine;
		pTxt = strstr(szLine, "<td>");
		pTxt += 4;
		pPos = strstr(szLine, "</td>");
		*pPos = 0;
		pItem->m_dGive = atof(pTxt) / 10;

		nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
		pBuff += nLine;
		pTxt = strstr(szLine, "<td>");
		pTxt += 4;
		pPos = strstr(szLine, "</td>");
		*pPos = 0;
		pItem->m_dGain = atof(pTxt) / 10;

		nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
		pBuff += nLine;
		pTxt = strstr(szLine, "<td>");
		pTxt += 4;
		pPos = strstr(szLine, "</td>");
		*pPos = 0;
		pItem->m_dRation = atof(pTxt) / 10;

		strcpy(szLine, "");
		while (strstr(szLine, "</td>") == NULL)
		{
			nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
			pBuff += nLine;
		}
		nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
		pBuff += nLine;
		pTxt = strstr(szLine, "<td>");
		pTxt += 4;
		pPos = strstr(szLine, "</td>");
		*pPos = 0;
		nRC = sscanf(pTxt, "%d-%d-%d", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
		if (nRC == 0)
		{
			pItem->m_dGive = 0;
			pItem->m_dGain = 0;
			pItem->m_dDividend = 0;
			pItem->m_dRation = 0;
		}

		while (strstr(szLine, "</tr>") == NULL)
		{
			nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
			pBuff += nLine;
		}
	}
	if (lstItem.GetCount() <= 0)
		return QC_ERR_FAILED;

	qcGetAppPath(NULL, szURL, sizeof(szURL));
	sprintf(szURL, "%sdata\\fhsp\\%s.txt", szURL, pCode);
	if (ioFile.Open(szURL, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	NODEPOS pos = lstItem.GetHeadPosition();
	while (pos != NULL)
	{
		pItem = lstItem.GetNext(pos);
		sprintf(szLine, "%d-%02d-%02d,%.2f,%.2f,%.2f\r\n", pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay,
			pItem->m_dGive, pItem->m_dGain, pItem->m_dRation);
		ioFile.Write((unsigned char *)szLine, strlen(szLine));
	}
	ioFile.Close();

	pItem = lstItem.RemoveTail();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = lstItem.RemoveTail();
	}

	return QC_ERR_NONE;
}

int	qcStock_DownLoadData_Info(CIOcurl * pIO, const char * pCode)
{
	if (pIO == NULL || pCode == NULL)
		return QC_ERR_ARG;

	return QC_ERR_NONE;
}

int	qcStock_DownLoadData_CWZB(CIOcurl * pIO, const char * pCode)
{
	if (pIO == NULL || pCode == NULL)
		return QC_ERR_ARG;

	return QC_ERR_NONE;
}

int	qcStock_CopyRTInfoToKXTInfo(qcStockKXTInfoItem * pKXTInfo, qcStockRealTimeItem * pRTInfo)
{
	if (pKXTInfo == NULL || pRTInfo == NULL)
		return QC_ERR_ARG;

	sscanf(pRTInfo->m_szDate, "%d/%d%/%d", &pKXTInfo->m_nYear, &pKXTInfo->m_nMonth, &pKXTInfo->m_nDay);
	pKXTInfo->m_dMin = pRTInfo->m_dMinPrice;
	pKXTInfo->m_dMax = pRTInfo->m_dMaxPrice;
	pKXTInfo->m_dOpen = pRTInfo->m_dOpenPrice;
	pKXTInfo->m_dClose = pRTInfo->m_dNowPrice;
	pKXTInfo->m_nVolume = pRTInfo->m_nTradeNum;
	pKXTInfo->m_dMoney = pRTInfo->m_nTradeMoney;
	pKXTInfo->m_dDiffNum = pRTInfo->m_dDiffNum;
	pKXTInfo->m_dDiffRate = pRTInfo->m_dDiffRate;
	pKXTInfo->m_dSwing = pRTInfo->m_dSwing;
	pKXTInfo->m_dExchange = 0;

	return QC_ERR_NONE;
}
