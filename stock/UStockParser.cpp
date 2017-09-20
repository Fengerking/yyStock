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

#include "UStockParser.h"
#include "USystemFunc.h"
#include "ULogFunc.h"

#define QCSTOCK_VALUE_DOUBLE	1
#define QCSTOCK_VALUE_INT		2

int	qcStock_ParseRealTimeInfo(const char * pCode, qcStockRealTimeItem * pStockInfo)
{
	if (pCode == NULL || pStockInfo == NULL)
		return QC_ERR_ARG;

	char szURL[256];
	if (pCode[0] == '6')
		sprintf(szURL, "http://api.money.126.net/data/feed/0%s", pCode);
	else
		sprintf(szURL, "http://api.money.126.net/data/feed/1%s", pCode);

	CIOcurl ioURL;
	if (ioURL.Open(szURL, 0, 0) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	char *	pTextInfo = ioURL.GetData();
	int		nTextSize = (int)ioURL.GetSize();
	if (pTextInfo == NULL || nTextSize <= 0)
		return QC_ERR_FAILED;

	strcpy(pStockInfo->m_szCode, pCode);

	if (qcStock_ParseValue(pTextInfo, "\"price\":", &pStockInfo->m_dNowPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"high\":", &pStockInfo->m_dMaxPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"low\":", &pStockInfo->m_dMinPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"open\":", &pStockInfo->m_dOpenPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"yestclose\":", &pStockInfo->m_dClosePrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	if (qcStock_ParseValue(pTextInfo, "\"volume\":", &pStockInfo->m_nTradeNum, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"turnover\":", &pStockInfo->m_nTradeMoney, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	if (qcStock_ParseValue(pTextInfo, "\"bid1\":", &pStockInfo->m_dBuyPrice1, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bid2\":", &pStockInfo->m_dBuyPrice2, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bid3\":", &pStockInfo->m_dBuyPrice3, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bid4\":", &pStockInfo->m_dBuyPrice4, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bid5\":", &pStockInfo->m_dBuyPrice5, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bidvol1\":", &pStockInfo->m_nBuyNum1, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bidvol2\":", &pStockInfo->m_nBuyNum2, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bidvol3\":", &pStockInfo->m_nBuyNum3, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bidvol4\":", &pStockInfo->m_nBuyNum4, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"bidvol5\":", &pStockInfo->m_nBuyNum5, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	if (qcStock_ParseValue(pTextInfo, "\"ask1\":", &pStockInfo->m_dSellPrice1, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"ask2\":", &pStockInfo->m_dSellPrice2, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"ask3\":", &pStockInfo->m_dSellPrice3, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"ask4\":", &pStockInfo->m_dSellPrice4, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"ask5\":", &pStockInfo->m_dSellPrice5, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"askvol1\":", &pStockInfo->m_nSellNum1, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"askvol2\":", &pStockInfo->m_nSellNum2, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"askvol3\":", &pStockInfo->m_nSellNum3, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"askvol4\":", &pStockInfo->m_nSellNum4, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"askvol5\":", &pStockInfo->m_nSellNum5, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;

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
		pList->AddHead(pItem);

		//日期, 股票代码, 名称, 收盘价, 最高价, 最低价, 开盘价, 前收盘, 涨跌额, 涨跌幅, 换手率, 成交量, 成交金额, 总市值, 流通市值, 成交笔数
		//2017-09-19, '000001,平安银行,11.13,11.34,11.08,11.25,11.25,-0.12,-1.0667,0.4517,76421262,853815999.65,1.91106678504e+11,1.88297420559e+11,26241
		sscanf(szLine, "%d-%d-%d", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
		pItemText = strstr(szLine + 24, ",") + 1;
		sscanf(pItemText, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%lf", 
			&pItem->m_dClose, &pItem->m_dMax, &pItem->m_dMin, &pItem->m_dOpen, &dYesClose,
			&pItem->m_dDiffNum, &pItem->m_dDiffRate, &pItem->m_dExchange, &pItem->m_nVolume, &pItem->m_dMoney);
		pItem->m_dSwing = (pItem->m_dMax - pItem->m_dMin) / dYesClose;
	}

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
