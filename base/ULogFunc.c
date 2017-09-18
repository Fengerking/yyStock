/*******************************************************************************
	File:		ULogFunc.c

	Contains:	Log printf implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "qcType.h"
#include "ULogFunc.h"

int		g_nInitLogTime = 0;

void qcDisplayLog (const char * pLogText)
{
#ifdef __QC_OS_WIN32__
#ifdef _UNICODE
	char wzLogText[1024];
	int	nLogTime =  0;
	if (g_nInitLogTime == 0)
		g_nInitLogTime = GetTickCount ();
	nLogTime = GetTickCount () - g_nInitLogTime;
	_stprintf (wzLogText, _T("%02d:%02d:%03d   "), ((nLogTime / 1000) % 3600) / 60, (nLogTime / 1000) % 60, nLogTime % 1000);
	MultiByteToWideChar (CP_ACP, 0, pLogText, -1, wzLogText + 10, sizeof (wzLogText));
#ifdef _CPU_MSB2531
	RETAILMSG (1, (wzLogText));
#else
	OutputDebugString (wzLogText); 
#endif // _CPU_MSB2531
#else
	OutputDebugString (pLogText); 
#endif // _UNIDCOE
#endif // __QC_OS_WIN32__
}


void qcDisplayMsg (void * hWnd, const char * pLogText)
{
#ifdef __QC_OS_WIN32__
#ifdef _UNICODE
	char wzLogText[1024];
	MultiByteToWideChar (CP_ACP, 0, pLogText, -1, wzLogText, sizeof (wzLogText));
	MessageBox ((HWND)hWnd, wzLogText, _T("QCMSG"), MB_OK); 
#else
	MessageBox ((HWND)hWnd, pLogText, _T("QCMSG"), MB_OK); 
#endif // _UNIDCOE
#endif // __QC_OS_WIN32__
}
