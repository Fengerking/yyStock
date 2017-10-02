/*******************************************************************************
	File:		UStockTools.h

	Contains:	The base utility for thread header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __UStockTools_H__
#define __UStockTools_H__

#include "qcStock.h"
#include "CNodeList.h"
#include "CIOcurl.h"

int		qcStock_DownLoadData_History(CIOcurl * pIO, const char * pCode);
int		qcStock_DownLoadData_FHSP(CIOcurl * pIO, const char * pCode);
int		qcStock_DownLoadData_Info(CIOcurl * pIO, const char * pCode);
int		qcStock_DownLoadData_CWZB(CIOcurl * pIO, const char * pCode);

int		qcStock_CopyRTInfoToKXTInfo (qcStockKXTInfoItem * pKXTInfo, qcStockRealTimeItem * pRTInfo);

int		qcStock_GetUTF8Text(char * pText, int nSize);
int		qcStock_GetCharText(char * pUTF8Text, int nSize, bool bNewLine);

int		qcStock_FindKeyWord(char * pText, int nSize, char * pKey);
int		qcStock_FillValue_table(char * pLine, int nSize, int nType, char * pValue, int nNum);


#endif // __UStockTools_H__
