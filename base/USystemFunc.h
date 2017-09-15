/*******************************************************************************
	File:		USystemFunc.h

	Contains:	The base utility for system header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __USystemFunc_H__
#define __USystemFunc_H__

#include "qcType.h"

extern int qcMonthDays[];

int		qcGetSysTime (void);
void	qcSleep (int nTime);
int		qcGetThreadTime (void * hThread);
int		qcGetCPUNum (void);

int		qcGetAppPath (void * hInst, char * pPath, int nSize);
bool	qcDeleteFolder (char * pFolder);

bool	qcIsTradeTime (void);
bool	qcCopyToClipBoard (char * pText);
int		qcPasteFromClipBoard (char * pText, int nSize);

bool	qcIsHoliday (int nYear, int nMonth, int nDay);
int		qcGetDaysFrom2000 (int nYear, int nMonth, int nDay);
int		qcGetTodayFrom2000 (void);

int		qcReadTextLine (char * pData, int nSize, char * pLine, int nLine);


extern	char g_szWorkPath[1024];

#endif // __USystemFunc_H__
