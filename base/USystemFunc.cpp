/*******************************************************************************
	File:		USystemFunc.cpp

	Contains:	The base utility for system implement file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifdef __QC_OS_WIN32__
#include "shlobj.h"
#include "shlwapi.h"
#include "stdio.h"
#else
#include <stdio.h>
#include <unistd.h>
#include <time.h>      
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#endif // __QC_OS_WIN32____

#if defined (__QC_OS_IOS__) || defined (__QC_OS_MASOS__)
#import <sys/sysctl.h>
#import <Foundation/Foundation.h>
#import <mach/mach.h>
#endif

#if defined (__QC_OS_IOS__)
#import <UIKit/UIKit.h>
#endif


#include "qcDef.h"
#include "USystemFunc.h"


char g_szWorkPath[1024];
int	qcMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int	qcGetSysTime (void)
{
	int nTime = 0;
#ifdef __QC_OS_WIN32__
	nTime = GetTickCount ();
	if (nTime < 0)
		nTime = nTime & 0X7FFFFFFF;
#elif defined __QC_OS_LINUX__ || defined __QC_OS_NDK__
    timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);

    static timespec stv = {0, 0};
    if ((0 == stv.tv_sec) && (0 == stv.tv_nsec))
	{
		stv.tv_sec = tv.tv_sec;
		stv.tv_nsec = tv.tv_nsec;
	}
    
    nTime = (int)((tv.tv_sec - stv.tv_sec) * 1000 + (tv.tv_nsec - stv.tv_nsec) / 1000000);
#elif defined __QC_OS_IOS__
    nTime = [[NSProcessInfo processInfo] systemUptime] * 1000;
//    struct timeval tv;
//    gettimeofday(&tv, NULL);
//    nTime = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif // __QC_OS_WIN32__

	return nTime;
}

void qcSleep (int nTime)
{
#ifdef __QC_OS_WIN32__
	Sleep (nTime / 1000);
#else
	usleep (nTime);
#endif // __QC_OS_WIN32__
}

int qcGetThreadTime (void * hThread)
{
	int nTime = -1;
#ifdef __QC_OS_WIN32__
	if(hThread == NULL)
		hThread = GetCurrentThread();

	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	BOOL bRC = GetThreadTimes(hThread, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
	if (!bRC)
		return nTime;

	LONGLONG llKernelTime = ftKernelTime.dwHighDateTime;
	llKernelTime = llKernelTime << 32;
	llKernelTime += ftKernelTime.dwLowDateTime;

	LONGLONG llUserTime = ftUserTime.dwHighDateTime;
	llUserTime = llUserTime << 32;
	llUserTime += ftUserTime.dwLowDateTime;

	nTime = int((llKernelTime + llUserTime) / 10000);

#elif defined __QC_OS_LINUX__
    timespec tv;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tv);

    static timespec stvThread = {0, 0};
    if ((0 == stvThread.tv_sec) && (0 == stvThread.tv_nsec))
	{
		stvThread.tv_sec = tv.tv_sec;
		stvThread.tv_nsec = tv.tv_nsec;
	}
    
    nTime = (int)((tv.tv_sec - stvThread.tv_sec) * 1000 + (tv.tv_nsec - stvThread.tv_nsec) / 1000000);
#elif defined __QC_OS_IOS__
    // TODO

#endif // __QC_OS_WIN32__

	return nTime;
}

int	qcGetCPUNum (void)
{
#ifdef __QC_OS_NDK__
	int nTemps[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21};
	char cCpuName[512];
	memset(cCpuName, 0, sizeof(cCpuName));

	for(int i = (sizeof(nTemps)/sizeof(nTemps[0])) - 1 ; i >= 0; i--)
	{
		sprintf(cCpuName, "/sys/devices/system/cpu/cpu%d", nTemps[i]);
		int nOk = access(cCpuName, F_OK);
		if( nOk == 0)
		{
			return nTemps[i]+1;
		}
	}
	return 1;
#elif defined __QC_OS_WIN32__
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
#endif

	return 1;
}

int	qcGetAppPath (void * hInst, char * pPath, int nSize)
{
#ifdef __QC_OS_WIN32__
	GetModuleFileName ((HMODULE)hInst, pPath, nSize);
    char * pPos = _tcsrchr(pPath, _T('/'));
	if (pPos == NULL)
		pPos = _tcsrchr(pPath, _T('\\'));
    int nPos = pPos - pPath;
    pPath[nPos+1] = _T('\0');	
#elif defined __QC_OS_NDK__
	FILE *		hFile = NULL;
	char		szPkgName[256];
	memset (szPkgName, 0, sizeof (szPkgName));
	hFile = fopen("/proc/self/cmdline", "rb");
	if (hFile != NULL)
	{  
		fgets(szPkgName, 256, hFile);
		fclose(hFile);
		strcpy (pPath, "/data/data/");
		strcat (pPath, szPkgName);
		strcat (pPath, "/");
	}
#elif defined __QC_OS_IOS__
    //NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/"];
    strcpy(pPath, [filePath UTF8String]);
    //[pool release];
#endif // __QC_OS_WIN32__

	return 0;
}

bool qcDeleteFolder (char * pFolder)
{
#ifdef __QC_OS_WIN32__
	char	szFolder[1024];
	char	szFilter[1024];
	_tcscpy (szFilter, pFolder);
	_tcscat (szFilter, _T("\\*.*"));
	WIN32_FIND_DATA  data;
	HANDLE  hFind = FindFirstFile(szFilter,&data);
	if (hFind == INVALID_HANDLE_VALUE)
		return true;
	do
	{
		if (!_tcscmp (data.cFileName, _T(".")) || !_tcscmp (data.cFileName, _T("..")))
			continue;	
		
		_tcscpy (szFolder, pFolder);
		_tcscat (szFolder, _T("\\"));
		_tcscat (szFolder, data.cFileName);
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			qcDeleteFolder (szFolder);
		}

		DeleteFile (szFolder);
	}while(FindNextFile(hFind, &data));
	FindClose (hFind);

	BOOL bRC = RemoveDirectory (pFolder);

	return bRC == TRUE ? true : false;
#else
	return false;
#endif // __QC_OS_WIN32__
}


bool qcCreateFolder(char * pFolder)
{
	if (qcPathExist(pFolder))
		return true;

	char szNewPath[2048];
	memset(szNewPath, 0, sizeof(szNewPath));
	strcpy(szNewPath, pFolder);

	char * pPos = strchr(szNewPath, '\\');
	while (pPos != NULL)
	{
		*pPos = '/';
		pPos++;
		pPos = strchr(szNewPath, '\\');
	}

	pPos = strchr(szNewPath, '/');
	if (pPos == NULL)
		return false;
	pPos = strchr(pPos + 1, '/');
	while (pPos != NULL)
	{
#ifdef __QC_OS_WIN32__
		*pPos = 0;
		if (PathFileExists(szNewPath) == FALSE)
			CreateDirectory(szNewPath, NULL);
		*pPos = '/';
		pPos = strchr(pPos + 1, '/');
		if (pPos == NULL)
		{
			if (PathFileExists(szNewPath) == FALSE)
				CreateDirectory(szNewPath, NULL);
		}
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__ || defined __QC_OS_MACOS__
		*pPos = 0;
		if (!qcPathExist(szNewPath))
			mkdir(szNewPath, 0777);
		*pPos = '/';
		pPos = strchr(pPos + 1, '/');
		if (pPos == NULL)
		{
			if (!qcPathExist(szNewPath))
				mkdir(szNewPath, 0777);
		}
#else

#endif // __QC_OS_WIN32__
	}
	return true;
}

bool qcPathExist(char* pPath)
{
	bool bExist = false;
#ifdef __QC_OS_WIN32__
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFile(pPath, &ffd);
	bExist = (hFind != INVALID_HANDLE_VALUE);
#else
	bExist = (-1 != access(pPath, 0));
#endif

	return bExist;
}

bool qcIsTradeTime (void)
{
#ifdef __QC_OS_WIN32__
	// only update the stock info in working time
	SYSTEMTIME tmSys;
	GetLocalTime (&tmSys);
//	tmSys.wHour -= 1;
	if (tmSys.wDayOfWeek == 6 || tmSys.wDayOfWeek == 0)
		return false;

	if (tmSys.wHour >= 9 && tmSys.wHour <= 15)
	{
		int nMinutes = tmSys.wHour * 60 + tmSys.wMinute - (9 * 60 + 30);
		if ((nMinutes >= 0 && nMinutes <= 120) || (nMinutes >= 210 && nMinutes <= 330))
			return true;
	}
#endif // __QC_OS_WIN32__
	return false;
}

bool qcCopyToClipBoard (char * pText)
{
#ifdef __QC_OS_WIN32__
	if (OpenClipboard (NULL) == 0) 
		return false;
	EmptyClipboard();  
	HANDLE	 hClip = GlobalAlloc(GMEM_MOVEABLE, strlen (pText) + 1);  
	char *	pBuf = (char *)GlobalLock(hClip);
	strcpy (pBuf,pText);
	GlobalUnlock (hClip);
	SetClipboardData(CF_TEXT, hClip);  
	CloseClipboard();  
	return true;
#else
	return false;
#endif // __QC_OS_WIN32__
}

int qcPasteFromClipBoard (char * pText, int nSize)
{
#ifdef __QC_OS_WIN32__
	if (OpenClipboard(NULL) == 0)
		return -1;
	  
	if (!IsClipboardFormatAvailable(CF_TEXT))
		return -1;
	  
	HANDLE hClip = GetClipboardData (CF_TEXT);  
	char * pBuf=(char *)GlobalLock (hClip);
	GlobalUnlock (hClip);  
	CloseClipboard();
	if (strlen (pBuf) >= (unsigned int)nSize)
	{
		memset (pText, 0, nSize);
		strncpy (pText, pBuf, nSize - 1);
	}
	else
	{
		strcpy (pText, pBuf);
	}
	return strlen (pBuf);
#else
	return -1;
#endif // __QC_OS_WIN32__
}  

// 2000-1-1 is staturday 6
bool qcIsHoliday (int nYear, int nMonth, int nDay)
{
	int nDays = qcGetDaysFrom2000 (nYear, nMonth, nDay) + 5;
	int nDayOfWeek = nDays % 7;
	if (nDayOfWeek == 6 || nDayOfWeek == 0)
		return true;

	if (nMonth == 5 && nDay == 1)
		return true;

	if (nMonth == 10 && (nDay >= 1 && nDay <= 3))
		return true;

	return false;
}

int qcGetDaysFrom2000 (int nYear, int nMonth, int nDay)
{
	if (nYear < 2000 || nYear > 2020)
		return 0;
	if (nMonth < 1 || nMonth > 12)
		return 0;
	if (nDay < 1 || nDay > 31)
		return 0;

	int nDays = 0;
	int n29Days = (nYear - 2000) / 4;
	if (nMonth > 2)
		n29Days += 1;
	nDays = (nYear - 2000) * 365 + n29Days;
	for (int i = 1; i < nMonth; i++)
		nDays = nDays + qcMonthDays[i-1];
	nDays = nDays + nDay;

	return nDays;
}

int	qcGetTodayFrom2000 (void)
{
	SYSTEMTIME tmToday;
	GetLocalTime (&tmToday);
	return qcGetDaysFrom2000 (tmToday.wYear, tmToday.wMonth, tmToday.wDay);
}

int	qcInRect(int nX, int nY, RECT * pRect)
{
	if (pRect == NULL)
		return 0;
	if (nX < pRect->left || nX > pRect->right)
		return 0;
	if (nY < pRect->top || nY > pRect->bottom)
		return 0;
	return 1;
}

int	qcReadTextLine (char * pData, int nSize, char * pLine, int nLine)
{
	if (pData == NULL)
		return 0;

	char * pBuff = pData;
	while (pBuff - pData < nSize)
	{
		if (*pBuff == '\r' || *pBuff == '\n')
		{
			pBuff++;
			if (*(pBuff)== '\r' || *(pBuff) == '\n')
				pBuff++;
			break;
		}
		pBuff++;
	}
	int nLineLen = pBuff - pData;
	if (nLine > nLineLen)
	{
		memset (pLine, 0, nLine);
		strncpy (pLine, pData, nLineLen);
	}
	return nLineLen;
}
