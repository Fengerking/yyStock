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

int		qcStock_ParseValue(char * pText, char * pItem, void * pValue, int nType);

int		qcStock_ParseRealTimeInfo(const char * pCode, qcStockRealTimeItem * pStockInfo);

#endif // __UStockParser_H__
