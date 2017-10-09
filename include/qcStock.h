/*******************************************************************************
	File:		qcStock.h

	Contains:	qc player type define header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __qcStock_H__
#define __qcStock_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
struct qcStockInfoItem
{
	char		m_szCode[16];
	char		m_szName[16];
};

struct qcStockRealTimeItem
{
	char		m_szCode[16];
	char		m_szDate[16];
	char		m_szName[16];
	wchar_t		m_wzName[16];
	double		m_dNowPrice;
	int			m_nTradeNum;
	int			m_nTradeMoney;

	double		m_dMaxPrice;
	double		m_dMinPrice;
	double		m_dOpenPrice;
	double		m_dClosePrice;

	double		m_dSwing;		// zhenfu
	double		m_dDiffRate;
	double		m_dDiffNum;
	double		m_dTurnOver;    // jiaohuanlv

	double		m_dHighLimit;
	double		m_dDownLimit;

	double		m_dBuyPrice1;
	double		m_dBuyPrice2;
	double		m_dBuyPrice3;
	double		m_dBuyPrice4;
	double		m_dBuyPrice5;
	int			m_nBuyNum1;
	int			m_nBuyNum2;
	int			m_nBuyNum3;
	int			m_nBuyNum4;
	int			m_nBuyNum5;
	double		m_dSellPrice1;
	double		m_dSellPrice2;
	double		m_dSellPrice3;
	double		m_dSellPrice4;
	double		m_dSellPrice5;
	int			m_nSellNum1;
	int			m_nSellNum2;
	int			m_nSellNum3;
	int			m_nSellNum4;
	int			m_nSellNum5;

	int			m_nAllNum;
	double		m_dAllMoney;

	double		m_dLastPrice[5];
	int			m_nLastIndex;
};

struct qcStockDayInfoItem
{
	int			m_nHour;
	int			m_nMinute;
	double		m_dNowPrice;
	double		m_dAvgPrice;
	int			m_nVolume;
};
   
struct qcDayLine
{
	double		m_dLine5;
	double		m_dLine10;
	double		m_dLine20;
	double		m_dLine30;
	double		m_dLine60;
	double		m_dLine120;
};

struct qcMACD
{
	double	m_dEMA12;
	double	m_dEMA26;
	double	m_dDIFF;
	double	m_dDEA;
	double	m_dBAR;
};

struct qcStockKXTInfoItem
{
	int			m_nYear;
	int			m_nMonth;
	int			m_nDay;
	double		m_dMin;
	double		m_dMax;
	double		m_dOpen;
	double		m_dClose;
	int			m_nVolume;
	double		m_dMoney;
	double		m_dDiffNum;
	double		m_dDiffRate;
	double		m_dSwing;
	double		m_dExchange;
	qcDayLine *	m_pDayLine;
	qcMACD *	m_pMACD;
};

struct qcStockIndexInfoItem
{
	char		m_szCode[16];
	char		m_szName[16];
	char		m_szTime[32];
	double		m_dClose;
	double		m_dOpen;
	double		m_dNow;
	double		m_dMax;
	double		m_dMin;
	double		m_dDiffRate;
	double		m_dDiffMoney;
	char		m_szTradeNum[32];
	char		m_szTradeMoney[32];
};

struct qcStockFHSPInfoItem
{
	int			m_nYear;
	int			m_nMonth;
	int			m_nDay;
	double		m_dGive;
	double		m_dGain;
	double		m_dDividend;
	double		m_dRation;
};

struct qcStockFinanceInfoItem
{
	int			m_nYear;
	int			m_nMonth;
	int			m_nDay;
	double		m_dValue;
	double		m_dIncome;
	double		m_dCash;
	double		m_dFund;
};


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __qcStock_H__
