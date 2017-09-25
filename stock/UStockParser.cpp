/*******************************************************************************
	File:		UStockParser.cpp

	Contains:	The base utility for thread implement file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "qcType.h"
#include "qcErr.h"

#include "CIOcurl.h"
#include "CIOFile.h"

#include "CRegMng.h"

#include "UStockParser.h"
#include "USystemFunc.h"
#include "ULogFunc.h"

#define QCSTOCK_VALUE_DOUBLE	1
#define QCSTOCK_VALUE_INT		2

int	qcStock_ParseRTItemInfo(const char * pCode, qcStockRealTimeItem * pStockInfo)
{
	if (pCode == NULL || pStockInfo == NULL)
		return QC_ERR_ARG;

	char szURL[256];
	char szCode[32];
	if (strlen(pCode) == 7)
		strcpy(szCode, pCode);
	else if (pCode[0] == '6')
		sprintf(szCode, "0%s", pCode);
	else
		sprintf(szCode, "1%s", pCode);
	sprintf(szURL, "http://api.money.126.net/data/feed/%s", szCode);

	CIOcurl ioURL;
	if (ioURL.Open(szURL, 0, 0) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	char *	pTextInfo = ioURL.GetData();
	int		nTextSize = (int)ioURL.GetSize();
	if (pTextInfo == NULL || nTextSize <= 0)
		return QC_ERR_FAILED;

	char	szItemInfo[4096];
	char *	pItemStart = strstr(pTextInfo, szCode);
	if (pItemStart == NULL)
		return QC_ERR_FAILED;
	char *	pItemEnd = strstr(pItemStart, "}");
	if (pItemEnd == NULL)
		return QC_ERR_FAILED;
	memset(szItemInfo, 0, sizeof(szItemInfo));
	strncpy(szItemInfo, pItemStart, (pItemEnd - pItemStart)+1);

	return qcStock_ParseRTItem(szItemInfo, pStockInfo);
}

int	qcStock_ParseRTListInfo(const char ** ppCode, int nNum, qcStockRealTimeItem ** ppStockInfo)
{
	if (ppCode == NULL || ppStockInfo == NULL || nNum <= 0)
		return QC_ERR_ARG;

	char szURL[4096];
	char szCode[1024];
	char szItem[32];

	if (strlen(ppCode[0]) == 7)
		strcpy(szCode, ppCode[0]);
	else if (ppCode[0][0] == '6')
		sprintf(szCode, "0%s", ppCode[0]);
	else
		sprintf(szCode, "1%s", ppCode[0]);
	int i = 1;
	for (i = 1; i < nNum; i++)
	{
		strcat(szCode, ",");
		if (strlen(ppCode[i]) == 7)
			strcpy(szItem, ppCode[i]);
		else if (ppCode[i][0] == '6')
			sprintf(szItem, "0%s", ppCode[i]);
		else
			sprintf(szItem, "1%s", ppCode[i]);
		strcat(szCode, szItem);
	}
	sprintf(szURL, "http://api.money.126.net/data/feed/%s", szCode);

	CIOcurl ioURL;
	if (ioURL.Open(szURL, 0, 0) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	char *	pTextInfo = ioURL.GetData();
	int		nTextSize = (int)ioURL.GetSize();
	if (pTextInfo == NULL || nTextSize <= 0)
		return QC_ERR_FAILED;

	char	szItemInfo[4096];
	char *	pItemStart = NULL;
	char *	pItemEnd = NULL;
	for (i = 0; i < nNum; i++)
	{
		if (strlen(ppCode[i]) == 7)
			strcpy(szCode, ppCode[i]);
		else if (ppCode[i][0] == '6')
			sprintf(szCode, "0%s", ppCode[i]);
		else
			sprintf(szCode, "1%s", ppCode[i]);

		pItemStart = strstr(pTextInfo, szCode);
		if (pItemStart == NULL)
			return QC_ERR_FAILED;
		pItemEnd = strstr(pItemStart, "}");
		if (pItemEnd == NULL)
			return QC_ERR_FAILED;
		memset(szItemInfo, 0, sizeof(szItemInfo));
		strncpy(szItemInfo, pItemStart, (pItemEnd - pItemStart) + 1);

		ppStockInfo[i]->m_dLastPrice = ppStockInfo[i]->m_dNowPrice;

		if (qcStock_ParseRTItem(szItemInfo, ppStockInfo[i]) != QC_ERR_NONE)
			return QC_ERR_FAILED;

		if (ppStockInfo[i]->m_dLastPrice == 0)
			ppStockInfo[i]->m_dLastPrice = ppStockInfo[i]->m_dNowPrice;
	}

	return QC_ERR_NONE;
}

int	qcStock_ParseHistoryData(const char * pCode, CObjectList<qcStockKXTInfoItem> * pList)
{
	if (pCode == NULL || pList == NULL)
		return QC_ERR_ARG;

	qcStockKXTInfoItem * pItem = pList->RemoveTail();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = pList->RemoveTail();
	}

	char	szFileName[256];
	qcGetAppPath(NULL, szFileName, sizeof(szFileName));
	strcat(szFileName, "data\\history\\");
	strcat(szFileName, pCode);
	strcat(szFileName, ".csv");

	CIOFile ioFile;
	if (ioFile.Open(szFileName, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	char *	pFileData = ioFile.GetData();
	int		nFileSize = (int)ioFile.GetSize();
	int		nRestSize = nFileSize;
	char	szLine[1024];
	char *	pItemText = szLine;
	double	dYesClose = 0;
	int nLen = qcReadTextLine(pFileData, nRestSize, szLine, sizeof(szLine));
	nRestSize -= nLen;
	pFileData += nLen;
	while (nRestSize > 0)
	{
		nLen = qcReadTextLine(pFileData, nRestSize, szLine, sizeof(szLine));
		nRestSize -= nLen;
		pFileData += nLen;

		pItem = new qcStockKXTInfoItem();
		memset(pItem, 0, sizeof(qcStockKXTInfoItem));

		//����, ��Ʊ����, ����, ���̼�, ��߼�, ��ͼ�, ���̼�, ǰ����, �ǵ���, �ǵ���, ������, �ɽ���, �ɽ����, ����ֵ, ��ͨ��ֵ, �ɽ�����
		//2017-09-19, '000001,ƽ������,11.13,11.34,11.08,11.25,11.25,-0.12,-1.0667,0.4517,76421262,853815999.65,1.91106678504e+11,1.88297420559e+11,26241
		sscanf(szLine, "%d-%d-%d", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
		pItemText = strstr(szLine + 24, ",") + 1;
		sscanf(pItemText, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%lf", 
			&pItem->m_dClose, &pItem->m_dMax, &pItem->m_dMin, &pItem->m_dOpen, &dYesClose,
			&pItem->m_dDiffNum, &pItem->m_dDiffRate, &pItem->m_dExchange, &pItem->m_nVolume, &pItem->m_dMoney);
		pItem->m_dSwing = (pItem->m_dMax - pItem->m_dMin) / dYesClose;

		if (pItem->m_dClose != 0 && dYesClose != 0)
		{
			if (pItem->m_dClose > dYesClose * 1.11 || pItem->m_dClose < dYesClose * 0.89)
				dYesClose = dYesClose;
		}

		if (pItem->m_dClose > 0)
			pList->AddHead(pItem);
		else
			delete pItem;
	}

	return QC_ERR_NONE;
}

int	qcStock_CreateDayLineMACD(CObjectList<qcStockKXTInfoItem> * pList)
{
	int nDay5 = CRegMng::g_pRegMng->GetIntValue ("Line5Days", 5);
	int nDay10 = CRegMng::g_pRegMng->GetIntValue("Line5Days", 10);
	int nDay20 = CRegMng::g_pRegMng->GetIntValue("Line5Days", 20);
	int nDay30 = CRegMng::g_pRegMng->GetIntValue("Line5Days", 30);
	int nDay60 = CRegMng::g_pRegMng->GetIntValue("Line5Days", 60);
	int nDay120 = CRegMng::g_pRegMng->GetIntValue("Line5Days", 120);

	int			nIndex = 0;
	double		dLine5 = 0;
	double *	pLine5 = new double[nDay5];
	double		dLine10 = 0;
	double *	pLine10 = new double[nDay10];
	double		dLine20 = 0;
	double *	pLine20 = new double[nDay20];
	double		dLine30 = 0;
	double *	pLine30 = new double[nDay30];
	double		dLine60 = 0;
	double *	pLine60 = new double[nDay60];
	double		dLine120 = 0;
	double *	pLine120 = new double[nDay120];

	double		dPrevEMA12 = 0;
	double		dPrevEMA26 = 0;
	double		dPrevDEA = 0;

	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = pList->GetHeadPosition();
	while (pos != NULL)
	{
		pItem = pList->GetNext(pos);
		pItem->m_pDayLine = new qcDayLine();
		pItem->m_pMACD = new qcMACD();

		dLine5 += pItem->m_dClose;
		pLine5[nIndex%nDay5] = pItem->m_dClose;
		if (nIndex < nDay5 - 1)
		{
			pItem->m_pDayLine->m_dLine5 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine5 = dLine5 / nDay5;
			dLine5 = dLine5 - pLine5[(nIndex + 1) % nDay5];
		}

		dLine10 += pItem->m_dClose;
		pLine10[nIndex%nDay10] = pItem->m_dClose;
		if (nIndex < nDay10 - 1)
		{
			pItem->m_pDayLine->m_dLine10 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine10 = dLine10 / nDay10;
			dLine10 = dLine10 - pLine10[(nIndex + 1) % nDay10];
		}

		dLine20 += pItem->m_dClose;
		pLine20[nIndex%nDay20] = pItem->m_dClose;
		if (nIndex < nDay20 - 1)
		{
			pItem->m_pDayLine->m_dLine20 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine20 = dLine20 / nDay20;
			dLine20 = dLine20 - pLine20[(nIndex + 1) % nDay20];
		}

		dLine30 += pItem->m_dClose;
		pLine30[nIndex%nDay30] = pItem->m_dClose;
		if (nIndex < nDay30 - 1)
		{
			pItem->m_pDayLine->m_dLine30 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine30 = dLine30 / nDay30;
			dLine30 = dLine30 - pLine30[(nIndex + 1) % nDay30];
		}

		dLine60 += pItem->m_dClose;
		pLine60[nIndex%nDay60] = pItem->m_dClose;
		if (nIndex < nDay60 - 1)
		{
			pItem->m_pDayLine->m_dLine60 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine60 = dLine60 / nDay60;
			dLine60 = dLine60 - pLine60[(nIndex + 1) % nDay60];
		}

		dLine120 += pItem->m_dClose;
		pLine120[nIndex%nDay120] = pItem->m_dClose;
		if (nIndex < nDay120 - 1)
		{
			pItem->m_pDayLine->m_dLine120 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine120 = dLine120 / nDay120;
			dLine120 = dLine120 - pLine120[(nIndex + 1) % nDay120];
		}

		// circulate MACD data
		if (nIndex == 0)
		{
			pItem->m_pMACD->m_dEMA12 = 0;
			pItem->m_pMACD->m_dEMA26 = 0;
			pItem->m_pMACD->m_dDIFF = 0;
			pItem->m_pMACD->m_dDEA = 0;
			pItem->m_pMACD->m_dBAR = 0;
		}
		else
		{
			pItem->m_pMACD->m_dEMA12 = (2 * pItem->m_dClose + 11 * dPrevEMA12) / 13;
			pItem->m_pMACD->m_dEMA26 = (2 * pItem->m_dClose + 25 * dPrevEMA26) / 27;;
			pItem->m_pMACD->m_dDIFF = pItem->m_pMACD->m_dEMA12 - pItem->m_pMACD->m_dEMA26;
			pItem->m_pMACD->m_dDEA = dPrevDEA * 8 / 10 + pItem->m_pMACD->m_dDIFF * 2 / 10;
			pItem->m_pMACD->m_dBAR = 2 * (pItem->m_pMACD->m_dDIFF - pItem->m_pMACD->m_dDEA);
		}
		dPrevEMA12 = pItem->m_pMACD->m_dEMA12;
		dPrevEMA26 = pItem->m_pMACD->m_dEMA26;
		dPrevDEA = pItem->m_pMACD->m_dDEA;

		nIndex++;
	}

	delete[]pLine5;
	delete[]pLine10;
	delete[]pLine20;
	delete[]pLine30;
	delete[]pLine60;
	delete[]pLine120;

	return QC_ERR_NONE;
}

int	qcStock_ParseRTItem(char * pItemInfo, qcStockRealTimeItem * pStockInfo)
{
	if (pItemInfo == NULL || pStockInfo == NULL)
		return QC_ERR_ARG;

	strncpy(pStockInfo->m_szCode, pItemInfo + 1, 6);

	if (qcStock_ParseValue(pItemInfo, "\"price\":", &pStockInfo->m_dNowPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"high\":", &pStockInfo->m_dMaxPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"low\":", &pStockInfo->m_dMinPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"open\":", &pStockInfo->m_dOpenPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"yestclose\":", &pStockInfo->m_dClosePrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	if (qcStock_ParseValue(pItemInfo, "\"volume\":", &pStockInfo->m_nTradeNum, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"turnover\":", &pStockInfo->m_nTradeMoney, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	if (qcStock_ParseValue(pItemInfo, "\"bid1\":", &pStockInfo->m_dBuyPrice1, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bid2\":", &pStockInfo->m_dBuyPrice2, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bid3\":", &pStockInfo->m_dBuyPrice3, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bid4\":", &pStockInfo->m_dBuyPrice4, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bid5\":", &pStockInfo->m_dBuyPrice5, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bidvol1\":", &pStockInfo->m_nBuyNum1, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bidvol2\":", &pStockInfo->m_nBuyNum2, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bidvol3\":", &pStockInfo->m_nBuyNum3, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bidvol4\":", &pStockInfo->m_nBuyNum4, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"bidvol5\":", &pStockInfo->m_nBuyNum5, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	if (qcStock_ParseValue(pItemInfo, "\"ask1\":", &pStockInfo->m_dSellPrice1, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"ask2\":", &pStockInfo->m_dSellPrice2, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"ask3\":", &pStockInfo->m_dSellPrice3, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"ask4\":", &pStockInfo->m_dSellPrice4, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"ask5\":", &pStockInfo->m_dSellPrice5, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"askvol1\":", &pStockInfo->m_nSellNum1, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"askvol2\":", &pStockInfo->m_nSellNum2, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"askvol3\":", &pStockInfo->m_nSellNum3, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"askvol4\":", &pStockInfo->m_nSellNum4, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pItemInfo, "\"askvol5\":", &pStockInfo->m_nSellNum5, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	char * pNamePos = strstr(pItemInfo, "\"name\":");
	if (pNamePos != NULL)
	{
		pNamePos = strstr(pNamePos + 7, "\"") + 1;
		int	nPos = 0;
		while (*pNamePos != '\"')
		{
			if (*pNamePos == ' ')
			{
				pNamePos++;
			}
			if (!strncmp(pNamePos, "\\u", 2))
			{
				sscanf(pNamePos + 2, "%x", &pStockInfo->m_wzName[nPos++]);
				pNamePos += 6;
			}
			else
			{
				pStockInfo->m_wzName[nPos++] = *pNamePos;
				pNamePos++;
			}
		}
	}

	pStockInfo->m_dSwing = (pStockInfo->m_dMaxPrice - pStockInfo->m_dMinPrice) * 100 / pStockInfo->m_dClosePrice;
	pStockInfo->m_dDiffRate = (pStockInfo->m_dNowPrice - pStockInfo->m_dClosePrice) * 100 / pStockInfo->m_dClosePrice;
	pStockInfo->m_dDiffNum = pStockInfo->m_dNowPrice - pStockInfo->m_dClosePrice;
	pStockInfo->m_dTurnOver = 1;// pStockInfo->m_nTradeMoney / pStockInfo-> ? ;

	pStockInfo->m_dHighLimit = pStockInfo->m_dClosePrice * 1.1;
	pStockInfo->m_dDownLimit = pStockInfo->m_dClosePrice * 0.9;

	pStockInfo->m_nAllNum = 100000;
	pStockInfo->m_dAllMoney = (int)(pStockInfo->m_nAllNum * pStockInfo->m_dNowPrice);

	return QC_ERR_NONE;
}

int	qcStock_ParseValue(char * pText, char * pItem, void * pValue, int nType)
{
	char * pTextItem = strstr(pText, pItem);
	if (pTextItem == NULL)
		return QC_ERR_FAILED;
	char * pTextEnd = strstr(pTextItem, ",");
	if (pTextEnd == NULL)
	{
		pTextEnd = strstr(pTextItem, "}");
		if (pTextEnd == NULL)
			return QC_ERR_FAILED;
	}
	*pTextEnd = 0;
	pTextItem += strlen(pItem);
	while (*++pTextItem == ' ')
	{
		if (*pTextItem == 0)
			return QC_ERR_FAILED;
	}
	if (nType == QCSTOCK_VALUE_DOUBLE)
		*((double *)pValue) = atof(pTextItem);
	else if (nType == QCSTOCK_VALUE_INT)
		*((int *)pValue) = atoi(pTextItem);

	*pTextEnd = ',';

	return QC_ERR_NONE;
}
