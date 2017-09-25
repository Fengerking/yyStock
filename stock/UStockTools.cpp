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

int	qcStock_DownLoadHistoryData(const char * pCode)
{
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
	CIOcurl ioHttp;
	int nRC = ioHttp.Open(szHttpName, 0, 0);
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

	ioFile.Write((unsigned char*)ioHttp.GetData(), (int)ioHttp.GetSize());
	if (pRestData != NULL)
	{
		ioFile.Write((unsigned char *)pRestData, nFileSize - (pRestData - pFileData));
	}
	ioFile.Close();

	return QC_ERR_NONE;
}

