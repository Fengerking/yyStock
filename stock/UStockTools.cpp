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
	qcCreateFolder(szFileName);

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

	char		szFileName[256];
	char		szHttpName[256];
	qcGetAppPath(NULL, szFileName, sizeof(szFileName));
	strcat(szFileName, "data\\fhsp\\");
	qcCreateFolder(szFileName);

	strcat(szFileName, pCode);
	strcat(szFileName, ".txt");

	sprintf(szHttpName, "http://quotes.money.163.com/f10/fhpg_%s.html#00000", pCode);
	int nRC = pIO->Open(szHttpName, 0, 0);
	if (nRC != QC_ERR_NONE)
		return nRC;

	int			nFileSize = (int)pIO->GetSize();
	char *		pFileData = pIO->GetData();
	char *		pBuff = pFileData;
	char *		pText = NULL;
	char *		pStop = NULL;
	int			nRest = nFileSize;
	char		szLine[20480];
	int			nLine = 0;

	int			i = 0;
	char		szKeyWord[256];
	strcpy(szKeyWord, "派息");
	qcStock_GetUTF8Text(szKeyWord, sizeof(szKeyWord));
	int nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));

	CObjectList<qcStockFHSPInfoItem>	lstItem;
	qcStockFHSPInfoItem *				pItem = NULL;
	pText = szLine;
	while (pText - szLine < nLine - 12)
	{
		pItem = new qcStockFHSPInfoItem();
		memset(pItem, 0, sizeof(qcStockFHSPInfoItem));
		for (i = 0; i < 8; i++)
		{
			pText = strstr(pText, "<td");
			if (pText == NULL)
				break;
			pText = strstr(pText, ">");
			if (pText == NULL)
				break;
			pText++;
			pStop = strstr(pText, "</td>");
			if (pStop == NULL)
				break;
			*pStop = 0;

			if (i == 2)
			{
				pItem->m_dGive = atof(pText) / 10;
			}
			else if (i == 3)
			{
				pItem->m_dGain = atof(pText) / 10;
			}
			else if (i == 4)
			{
				pItem->m_dRation = atof(pText) / 10;
			}
			else if (i == 6)
			{
				nRC = sscanf(pText, "%d-%d-%d", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
				if (nRC > 0)
				{
					lstItem.AddTail(pItem);
				}
				else
				{
					delete pItem;
					pItem = NULL;
				}
			}

			pText = pStop + 4;
		}

		if (pText == NULL || pStop == NULL)
		{
			if (pItem != NULL)
				delete pItem;
			break;
		}
	}

	CIOFile ioFile;
	if (ioFile.Open(szFileName, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
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

	char szURL[1024];
	CIOFile ioFile;
	qcGetAppPath(NULL, szURL, sizeof(szURL));
	strcat(szURL, "data\\info\\");
	qcCreateFolder(szURL);

	sprintf(szURL, "%s%s.txt", szURL, pCode);
//	if (ioFile.Open(szURL, 0, QCIO_FLAG_READ) == QC_ERR_NONE)
//		return QC_ERR_NONE;

	sprintf(szURL, "http://quotes.money.163.com/f10/gdfx_%s.html#00000", pCode);
	int nRC = pIO->Open(szURL, 0, 0);
	if (nRC != QC_ERR_NONE)
		return nRC;

	int			nFileSize = (int)pIO->GetSize();
	char *		pFileData = pIO->GetData();
	char *		pBuff = pFileData;
	char *		pText = NULL;
	char *		pStop = NULL;
	int			nRest = nFileSize;
	char		szLine[10240];
	int			nLine = 0;
	char		szKeyWord[256];

	strcpy(szKeyWord, "总股本");
	qcStock_GetUTF8Text(szKeyWord, sizeof(szKeyWord));
	int nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof (szLine));
	pBuff += nLine; nRest -= nLine;
	pText = strstr(szLine, "<td>") + 4;
	double dTotalNum = atof(pText) * 100000000;

	strcpy(szKeyWord, "流通A股");
	qcStock_GetUTF8Text(szKeyWord, sizeof(szKeyWord));
	nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	pText = strstr(szLine, "<td>") + 4;
	double dTradeNum = atof(pText) * 100000000;

	char	szCWZBDate[1024];
	char	szCWZBLucre[1024];
	char	szCWZBCash[1024];
	char	szCWZBValue[1024];
	char	szCWZBDebt[1024];

	sprintf(szURL, "http://quotes.money.163.com/f10/zycwzb_%s.html#00000", pCode);
	nRC = pIO->Open(szURL, 0, 0);
	if (nRC != QC_ERR_NONE)
		return nRC;
	nFileSize = (int)pIO->GetSize();
	pFileData = pIO->GetData();
	pBuff = pFileData;
	nRest = nFileSize;

	strcpy(szKeyWord, "table_bg001 border_box limit_sale scr_table");
	nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	// Get Date 
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	qcStock_FillValue_table(szLine, nLine, 0, szCWZBDate, 5);

	// Get Lucre 
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	qcStock_FillValue_table(szLine, nLine, 1, szCWZBLucre, 5);

	// Get Cash 
	int i = 0;
	for (i = 0; i < 11; i++)
	{
		nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
		pBuff += nLine; nRest -= nLine;
		pText = szLine;
	}
	qcStock_FillValue_table(szLine, nLine, 1, szCWZBCash, 5);

	// Get Value 
	for (i = 0; i < 2; i++)
	{
		nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
		pBuff += nLine; nRest -= nLine;
		pText = szLine;
	}
	qcStock_FillValue_table(szLine, nLine, 1, szCWZBValue, 5);
	// Get Debt 
	for (i = 0; i < 2; i++)
	{
		nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
		pBuff += nLine; nRest -= nLine;
		pText = szLine;
	}
	qcStock_FillValue_table(szLine, nLine, 1, szCWZBDebt, 5);

	sprintf(szURL, "http://quotes.money.163.com/f10/gszl_%s.html#00000", pCode);
	nRC = pIO->Open(szURL, 0, 0);
	if (nRC != QC_ERR_NONE)
		return nRC;
	nFileSize = (int)pIO->GetSize();
	pFileData = pIO->GetData();
	pBuff = pFileData;
	nRest = nFileSize;

	strcpy(szKeyWord, "公司全称");
	qcStock_GetUTF8Text(szKeyWord, sizeof(szKeyWord));
	nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	char  pTextCompName[1024];
	qcStock_FillValue_table(szLine, nLine, 1, pTextCompName, 1);

	strcpy(szKeyWord, "主营业务");
	qcStock_GetUTF8Text(szKeyWord, sizeof(szKeyWord));
	nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	char  pTextField[8192];
	qcStock_FillValue_table(szLine, nLine, 1, pTextField, 1);

	strcpy(szKeyWord, "经营范围");
	qcStock_GetUTF8Text(szKeyWord, sizeof(szKeyWord));
	nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	char  pTextRange[8192];
	qcStock_FillValue_table(szLine, nLine, 1, pTextRange, 1);

	strcpy(szKeyWord, "上市日期");
	qcStock_GetUTF8Text(szKeyWord, sizeof(szKeyWord));
	nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	char  pTextCreateDate[64];
	qcStock_FillValue_table(szLine, nLine, 1, pTextCreateDate, 1);
	pTextCreateDate[strlen(pTextCreateDate) - 1] = 0;

	strcpy(szKeyWord, "发行价格");
	qcStock_GetUTF8Text(szKeyWord, sizeof(szKeyWord));
	nFind = qcStock_FindKeyWord(pBuff, nRest, szKeyWord);
	pBuff += nFind; nRest -= nFind;
	nLine = qcReadTextLine(pBuff, nRest, szLine, sizeof(szLine));
	pBuff += nLine; nRest -= nLine;
	char  pTextPrice[64];
	qcStock_FillValue_table(szLine, nLine, 1, pTextPrice, 1);
	if (strlen(pTextPrice) > 0)
		pTextPrice[strlen(pTextPrice) - 1] = 0;
	double dOpenPrice = 0;
	sscanf(pTextPrice, "%lf", &dOpenPrice);

	qcGetAppPath(NULL, szURL, sizeof(szURL));
	sprintf(szURL, "%sdata\\info\\%s.txt", szURL, pCode);
	if (ioFile.Open(szURL, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	char szPrevName[1024];
	strcpy(szPrevName, "公司全称：");
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));
	qcStock_GetCharText(pTextCompName, strlen(pTextCompName), true);
	ioFile.Write((unsigned char *)pTextCompName, strlen(pTextCompName));
	sprintf(szPrevName, "流通股本:%lld\r\n", (long long)dTradeNum);
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));
	sprintf(szPrevName, "总共股本:%lld\r\n", (long long)dTotalNum);
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));
	sprintf(szPrevName, "上市日期:%s\r\n", pTextCreateDate);
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));
	sprintf(szPrevName, "发行价格:%.2lf\r\n", dOpenPrice);
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));

	strcpy(szPrevName, "\r\n财务指标：\r\n");
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));
	strcpy(szPrevName, "  日  期     收  益    现金（万）  净资产  \r\n");
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));

	strcpy(szPrevName, "\r\n");
	char * pTextStart1 = szCWZBDate;
	char * pTextStart2 = szCWZBLucre;
	char * pTextStart3 = szCWZBCash;
	char * pTextStart4 = szCWZBValue;
	char * pTextStart5 = szCWZBDebt;

	double dValue = 0;
	double dDebt = 0;
	char * pValue = NULL;
	char   szValue[32];

	int    nSpaceNum = 0;
	char * pTextSplt = strstr(pTextStart1, "|");
	while (pTextSplt != NULL)
	{
		nSpaceNum = 0;
		ioFile.Write((unsigned char *)pTextStart1, pTextSplt - pTextStart1);
		nSpaceNum += pTextSplt - pTextStart1;
		pTextStart1 = pTextSplt + 1;

		if (pTextStart2[0] == '-')
		{
			ioFile.Write((unsigned char *)"      ", 3);
			nSpaceNum += 3;
		}
		else
		{
			ioFile.Write((unsigned char *)"      ", 4);
			nSpaceNum += 4;
		}
		pTextSplt = strstr(pTextStart2, "|");
		ioFile.Write((unsigned char *)pTextStart2, pTextSplt - pTextStart2);
		nSpaceNum += pTextSplt - pTextStart2;
		pTextStart2 = pTextSplt + 1;

		if (nSpaceNum < 24)
		{
			ioFile.Write((unsigned char *)"          ", 22 - nSpaceNum);
			nSpaceNum = 24;
		}
		if (pTextStart3[0] != '-')
		{
			ioFile.Write((unsigned char *)"      ", 1);
			nSpaceNum += 1;
		}
		pTextSplt = strstr(pTextStart3, "|");
		ioFile.Write((unsigned char *)pTextStart3, pTextSplt - pTextStart3);
		nSpaceNum += pTextSplt - pTextStart3;
		pTextStart3 = pTextSplt + 1;

		pTextSplt = strstr(pTextStart4, "|");
		pValue = pTextStart4;
		i = 0;
		memset(szValue, 0, sizeof(szValue));
		while (pValue < pTextSplt)
		{
			if (*pValue != ',')
				szValue[i++] = *pValue;
			pValue++;
		}
		sscanf(szValue, "%lf", &dValue);
		pTextStart4 = pTextSplt + 1;

		pTextSplt = strstr(pTextStart5, "|");
		pValue = pTextStart5;
		i = 0;
		memset(szValue, 0, sizeof(szValue));
		while (pValue < pTextSplt)
		{
			if (*pValue != ',')
				szValue[i++] = *pValue;
			pValue++;
		}
		sscanf(szValue, "%lf", &dDebt);
		pTextStart5 = pTextSplt + 1;

		if (nSpaceNum < 34)
			ioFile.Write((unsigned char *)"          ", 34 - nSpaceNum);
		if (dValue > dDebt)
			sprintf(szPrevName, "    %.2lf", (dValue - dDebt) / (dTotalNum / 10000));
		else
			sprintf(szPrevName, "   %.2lf", (dValue - dDebt) / (dTotalNum / 10000));
		ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));

		pTextSplt = strstr(pTextStart1, "|");

		ioFile.Write((unsigned char *)"\r\n", 2);
	}

	strcpy(szPrevName, "\r\n主营业务：\r\n");
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));
	qcStock_GetCharText(pTextField, strlen(pTextField), true);
	ioFile.Write((unsigned char *)pTextField, strlen(pTextField));
	strcpy(szPrevName, "\r\n经营范围：\r\n");
	ioFile.Write((unsigned char *)szPrevName, strlen(szPrevName));
	qcStock_GetCharText(pTextRange, strlen(pTextRange), true);
	ioFile.Write((unsigned char *)pTextRange, strlen(pTextRange));

	ioFile.Close();

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

	sscanf(pRTInfo->m_szDate, "%d/%d/%d", &pKXTInfo->m_nYear, &pKXTInfo->m_nMonth, &pKXTInfo->m_nDay);
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

int	qcStock_GetUTF8Text(char * pText, int nSize)
{
	wchar_t wzWord[64];
	memset(wzWord, 0, sizeof(wzWord));
	MultiByteToWideChar(CP_ACP, 0, pText, -1, wzWord, 32);
	memset(pText, 0, nSize);
	WideCharToMultiByte(CP_UTF8, 0, wzWord, -1, pText, nSize, NULL, NULL);

	return QC_ERR_NONE;
}

int	qcStock_GetCharText(char * pUTF8Text, int nSize, bool bNewLine)
{
	wchar_t * wzText = new wchar_t[nSize];
	memset(wzText, 0, nSize * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, pUTF8Text, -1, wzText, nSize);
	memset(pUTF8Text, 0, nSize);
	WideCharToMultiByte(CP_ACP, 0, wzText, -1, pUTF8Text, nSize, NULL, NULL);
	if (bNewLine)
		strcat(pUTF8Text, "\r\n");
	delete[]wzText;

	return QC_ERR_NONE;
}

int	qcStock_FindKeyWord(char * pText, int nSize, char * pKey)
{
	char *		pBuff = pText;
	int			nRest = nSize;
	char		szLine[2048];
	int			nLine = 0;

	while (nRest > 0)
	{
		nLine = qcReadTextLine(pBuff, nRest, szLine, 2048);
		if (nLine <= 0)
			break;
		pBuff += nLine;
		nRest -= nLine;
		if (strstr(szLine, pKey) != NULL)
			break;
	}
	return nSize - nRest;
}

int	qcStock_FillValue_table(char * pLine, int nSize, int nType, char * pValue, int nNum)
{
	if (pLine == NULL || pValue == NULL)
		return QC_ERR_ARG;
	strcpy(pValue, "");
	int		i = 0;
	char *	pText = pLine;
	char *	pStop = NULL;
	for (i = 0; i < nNum; i++)
	{
		if (nType == 0)
			pText = strstr(pText, "<th");
		else
			pText = strstr(pText, "<td");
		if (pText == NULL)
			break;
		pText = strstr(pText + 3, ">");
		if (pText == NULL)
			break;
		pText++;
		if (nType == 0)
			pStop = strstr(pText, "</th>");
		else
			pStop = strstr(pText, "</td>");
		if (pStop == NULL)
		{
			pLine[nSize - 2] = 0;
			strcat(pValue, pText);
			strcat(pValue, "|");
			break;
		}
		*pStop = 0;
		strcat(pValue, pText);
		strcat(pValue, "|");
		pText = pStop + 4;
	}
	return QC_ERR_NONE;
}

/*
char szURL[1024];
qcGetAppPath(NULL, szURL, sizeof(szURL));
strcat(szURL, "data\\fhsp\\");
qcCreateFolder(szURL);
sprintf(szURL, "%s%s.txt", szURL, pCode);
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
char *	pFileData = pIO->GetData();

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
*/