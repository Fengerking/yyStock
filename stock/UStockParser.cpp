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
#include "ULogFunc.h"

#define QCSTOCK_VALUE_DOUBLE	1
#define QCSTOCK_VALUE_INT		2

int	qcStock_ParseValue(char * pText, char * pItem, void * pValue, int nType)
{
	char * pTextItem = strstr(pText, pItem);
	if (pTextItem == NULL)
		return QC_ERR_FAILED;
	char * pTextEnd = strstr(pTextItem, ",");
	if (pTextEnd == NULL)
		return QC_ERR_FAILED;
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
	if (qcStock_ParseValue(pTextInfo, "\"volume\":", &pStockInfo->m_nTradeNum, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"turnover\":", &pStockInfo->m_nTradeMoney, QCSTOCK_VALUE_INT) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"high\":", &pStockInfo->m_dMaxPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"low\":", &pStockInfo->m_dMinPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"open\":", &pStockInfo->m_dOpenPrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	if (qcStock_ParseValue(pTextInfo, "\"yestclose\":", &pStockInfo->m_dClosePrice, QCSTOCK_VALUE_DOUBLE) != QC_ERR_NONE)
		return QC_ERR_FAILED;



	return QC_ERR_NONE;
}
/*
;
;
m_dSwing;		// zhenfu
m_dDiffRate;
m_dDiffNum;
m_dTurnOver;    // jiaohuanlv

m_dHighLimit;
m_dDownLimit;

m_dBuyPrice1;
m_dBuyPrice2;
m_dBuyPrice3;
m_dBuyPrice4;
m_dBuyPrice5;
m_nBuyNum1;
m_nBuyNum2;
m_nBuyNum3;
m_nBuyNum4;
m_nBuyNum5;
m_dSellPrice1;
m_dSellPrice2;
m_dSellPrice3;
m_dSellPrice4;
m_dSellPrice5;
m_nSellNum1;
m_nSellNum2;
m_nSellNum3;
m_nSellNum4;
m_nSellNum5;

m_nAllNum;
m_dAllMoney;
*/