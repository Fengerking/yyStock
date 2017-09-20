/*******************************************************************************
	File:		UStockParser.h

	Contains:	The base utility for thread header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __UStockParser_H__
#define __UStockParser_H__

#include "qcStock.h"
#include "CNodeList.h"


int		qcStock_ParseRealTimeInfo(const char * pCode, qcStockRealTimeItem * pStockInfo);

int		qcStock_ParseHistoryData(const char * pCode, CObjectList<qcStockKXTInfoItem> * pList);

int		qcStock_ParseValue(char * pText, char * pItem, void * pValue, int nType);


#endif // __UStockParser_H__
