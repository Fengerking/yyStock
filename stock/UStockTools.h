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

int		qcStock_DownLoadHistoryData (const char * pCode);

#endif // __UStockTools_H__
