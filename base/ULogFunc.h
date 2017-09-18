/*******************************************************************************
	File:		ULogFunc.h

	Contains:	ULogFunc define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __ULogFunc_H__
#define __ULogFunc_H__

#include <string.h>
#include <stdio.h>

#if defined __QC_OS_NDK__ || defined __QC_OS_IOS__
#include <pthread.h>
#endif // __QC_OS_NDK__

#include "qcDef.h"

#if defined __QC_OS_NDK__
#include <android/log.h>
#ifndef LOG_TAG
#define  LOG_TAG "@@@QCLOG"
#endif // LOG_TAG
#if !defined LOGW
#define LOGW(...) ((int)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif
#if !defined LOGI
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif
#if !defined LOGE
#define LOGE(...) ((int)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif
#endif //__QC_OS_NDK__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    
#include "UThreadFunc.h"

void	qcDisplayLog (const char * pLogText);
void	qcDisplayMsg (void * hWnd, const char * pLogText);
    
#define _QCLOG_ERROR   1
#define _QCLOG_WARNING 1
#define _QCLOG_INFO    1
#define _QCLOG_TEXT    1

#ifdef _QCLOG_ERROR
#ifdef __QC_OS_WIN32__
#define QCLOGE(fmt, ...) \
{ \
	char		szLogText[1024]; \
	sprintf (szLogText, "@@@QCLOG Err T%08X %s L%d " fmt "\r\n", GetCurrentThreadId(), m_szObjName, __LINE__, __VA_ARGS__); \
	qcDisplayLog (szLogText); \
}
#elif defined __QC_OS_NDK__
#define QCLOGE(fmt, args...) \
{ \
	LOGE ("Err  T%08X %s L%d " fmt "\r\n", (int)pthread_self(), m_szObjName, __LINE__, ## args); \
}
#elif defined __QC_OS_IOS__
#define QCLOGE(fmt, args...) \
{ \
    printf("Err T%08X %s L%d " fmt "\r\n", qcThreadGetCurrentID(), m_szObjName, __LINE__, ## args); \
}
#endif // __QC_OS_WIN32__
#else
#define QCLOGE(fmt, ...)
#endif // _QCLOG_ERROR

#ifdef _QCLOG_WARNING
#ifdef __QC_OS_WIN32__
#define QCLOGW(fmt, ...) \
{ \
	char		szLogText[1024]; \
	sprintf (szLogText, "@@@QCLOG Warn  T%08X %s L%d " fmt "\r\n", GetCurrentThreadId(), m_szObjName, __LINE__, __VA_ARGS__); \
	qcDisplayLog (szLogText); \
}
#elif defined __QC_OS_NDK__
#define QCLOGW(fmt, args...) \
{ \
	LOGW ("Warn T%08X %s L%d " fmt "\r\n", (int)pthread_self(), m_szObjName, __LINE__, ## args); \
}
#elif defined __QC_OS_IOS__
#define QCLOGW(fmt, args...) \
{ \
    printf("Warn T%08X %s L%d " fmt "\r\n", qcThreadGetCurrentID(), m_szObjName, __LINE__, ## args); \
}
#endif // __QC_OS_WIN32__
#else
#define QCLOGW(fmt, ...)
#endif // _QCLOG_WARNING

#ifdef _QCLOG_INFO
#ifdef __QC_OS_WIN32__
#define QCLOGI(fmt, ...) \
{ \
	char		szLogText[1024]; \
	sprintf (szLogText, "@@@QCLOG Info T%08X %s L%d " fmt "\r\n", GetCurrentThreadId(), m_szObjName, __LINE__, __VA_ARGS__); \
	qcDisplayLog (szLogText); \
}
#elif defined __QC_OS_NDK__
#define QCLOGI(fmt, args...) \
{ \
	LOGI ("Info T%08X %s L%d " fmt "\r\n", (int)pthread_self(), m_szObjName, __LINE__, ## args); \
}
#elif defined __QC_OS_IOS__
#define QCLOGI(fmt, args...) \
{ \
    printf("Info T%08X %s L%d " fmt "\r\n", qcThreadGetCurrentID(), m_szObjName, __LINE__, ## args); \
}
#endif // __QC_OS_WIN32__
#else
#define QCLOGI(fmt, ...)
#endif // _QCLOG_INFO

#ifdef _QCLOG_TEXT
#ifdef __QC_OS_WIN32__
#define QCLOGT(txt, fmt, ...) \
{ \
	char		szLogText[1024]; \
	sprintf (szLogText, "@@@QCLOG Info T%08X %s L%d " fmt "\r\n", GetCurrentThreadId(), txt, __LINE__, __VA_ARGS__); \
	qcDisplayLog (szLogText); \
}
#elif defined __QC_OS_NDK__
#define QCLOGT(txt, fmt, args...) \
{ \
	LOGI ("Info T%08X %s L%d " fmt "\r\n", (int)pthread_self(), txt, __LINE__, ## args); \
}
#elif defined __QC_OS_IOS__
#define QCLOGT(txt, fmt, args...) \
{ \
    printf("Info T%08X %s L%d " fmt "\r\n", qcThreadGetCurrentID(), txt, __LINE__, ## args); \
}
#endif // __QC_OS_WIN32__
#else
#define QCLOGT(txt, fmt, ...)
#endif // _QCLOG_INFO

#ifdef __QC_OS_WIN32__
#define QCMSG(win, fmt, ...) \
{ \
	char	szMsgText[1024]; \
	sprintf (szMsgText, "QCMSG T%08X %s L%d " fmt "\r\n", GetCurrentThreadId(), m_szObjName, __LINE__, __VA_ARGS__); \
	qcDisplayMsg ((void *)win,szMsgText); \
}
#else
#define QCMSG(win, fmt, ...)
#endif // __QC_OS_WIN32__

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ULogFunc_H__
