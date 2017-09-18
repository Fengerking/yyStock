/*******************************************************************************
	File:		UThreadFunc.cpp

	Contains:	The base utility for thread implement file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifdef __QC_OS_WIN32__
#else
#include <stdlib.h>
#ifdef __QC_OS_NDK__
#include <sys/mman.h>
#include <sys/prctl.h>
#elif defined __QC_OS_IOS__
#include <sched.h>
#endif
#endif // __QC_OS_WIN32

#include "qcType.h"
#include "qcErr.h"

#include "UThreadFunc.h"
#include "ULogFunc.h"

#define QC_DEFAULT_STACKSIZE (128 * 1024)

static const char szLogTag[] = "UThreadFunc";

void qcThreadSetName(int uThreadID, const char* threadName)
{
#ifdef __QC_OS_NDK__
	prctl(PR_SET_NAME, (unsigned long)threadName , 0 , 0 , 0);
#elif defined __QC_OS_IOS__
    pthread_setname_np(threadName);
#endif // __QC_OS_NDK__
}

int qcThreadCreate (qcThreadHandle * pHandle, int * pID,
						qcThreadProc fProc, void * pParam, int uFlag)
{
	if (pHandle == NULL)
		return QC_ERR_ARG;

	*pHandle = NULL;
	int nID = 0;
	if (pID != NULL)
		*pID = 0;
	else
		pID = &nID;
#ifdef __QC_OS_WIN32__
	*pHandle = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) fProc, pParam, 0, (LPDWORD)pID);
	if (*pHandle == NULL)
		return QC_ERR_MEMORY;
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
    pthread_t * pThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_attr_t	attr;
	pthread_attr_init(&attr);

	pthread_attr_setstacksize(&attr, QC_DEFAULT_STACKSIZE);
//	pthread_attr_setguardsize(&attr, PAGESIZE);
//	pthread_attr_setschedpolicy(&attr, SCHED_NORMAL);
//	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	
//    struct sched_param param;
//    pthread_attr_getschedparam(&attr, &param);
//	param.sched_priority = 5;
//    pthread_attr_setschedparam(&attr, &param);
    
//    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

	int rs = pthread_create(pThread, &attr, (void*(*)(void*))fProc ,pParam);
	pthread_attr_destroy(&attr);
	if (rs != 0)
    {
        if (pThread)
        {
            free(pThread);
        }
        return QC_ERR_MEMORY;
    }

	rs = pthread_detach(*pThread);
    *pHandle = pThread;
#endif // __QC_OS_WIN32__

	return QC_ERR_NONE;
}

int qcThreadClose (qcThreadHandle hHandle, int uExitCode)
{
	if (hHandle == NULL)
		return QC_ERR_ARG;

#ifdef __QC_OS_WIN32__
	CloseHandle (hHandle);
#else
    if (hHandle)
    {
        free(hHandle);
    }
#endif //__QC_OS_WIN32__

	return QC_ERR_NONE;
}

int qcThreadGetPriority (qcThreadHandle hHandle, qcThreadPriority * pPriority)
{
	if (hHandle == NULL)
		return QC_ERR_ARG;

#ifdef __QC_OS_WIN32__
	int nPriority = 0;
	nPriority = GetThreadPriority (hHandle);
	qcThreadConvertPriority (pPriority, &nPriority, false);
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
	int policy = 0;
    struct sched_param param;
    int nPriority = 0;
	
	int iRet = pthread_getschedparam(*hHandle, &policy, &param);
	if (0 != iRet) {
		QCLOGT(szLogTag, "pthread_getschedparam error :%d", iRet);
		return QC_ERR_FAILED;
	}
	nPriority = param.sched_priority;
	QCLOGT(szLogTag, "get succ hHandle:%ld, policy:%d, param.sched_priority:%d", (long)hHandle, policy, param.sched_priority);
	qcThreadConvertPriority (pPriority, &nPriority, 0);
#endif // __QC_OS_WIN32

	return QC_ERR_NONE;
}


int qcThreadSetPriority (qcThreadHandle hHandle, qcThreadPriority uPriority)
{
	if (hHandle == NULL)
		return QC_ERR_ARG;

#ifdef __QC_OS_WIN32__
	int nPriority = 0;
	qcThreadConvertPriority (&uPriority, &nPriority, true);
	SetThreadPriority (hHandle, nPriority);
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
	int policy = 0;
	struct sched_param param;
    int nPriority = 0;
	
	int iRet = pthread_getschedparam(*hHandle, &policy, &param);
	if (0 != iRet) {
		QCLOGT(szLogTag, "pthread_getschedparam hHandle:%ld, error :%d", (long)hHandle, iRet);
		return QC_ERR_FAILED;
	}

	QCLOGT(szLogTag, "get succ hHandle:%ld, policy:%d, param.sched_priority:%d", (long)hHandle, policy, param.sched_priority);
	
	qcThreadConvertPriority (&uPriority, &nPriority, 1);
	param.sched_priority = nPriority;
	iRet = pthread_setschedparam(*hHandle, policy, &param);
	
	if (0 != iRet) {
		QCLOGT(szLogTag, "pthread_attr_setschedparam hHandle:%ld, error :%d, param.sched_priority:%d", (long)hHandle, iRet, param.sched_priority);
		return QC_ERR_FAILED;
	}

	QCLOGT(szLogTag, "set succ hHandle:%ld, policy:%d, param.sched_priority:%d", (long)hHandle, policy, param.sched_priority);
#endif // __QC_OS_WIN32__
	
	return QC_ERR_NONE;
}

int qcThreadSuspend (qcThreadHandle hHandle)
{
	if (hHandle == NULL)
		return QC_ERR_ARG;

#ifdef __QC_OS_WIN32__
	SuspendThread (hHandle);
#endif //__QC_OS_WIN32__

	return QC_ERR_NONE;
}

int qcThreadResume (qcThreadHandle hHandle)
{
	if (hHandle == NULL)
		return QC_ERR_ARG;

#ifdef __QC_OS_WIN32__
	ResumeThread (hHandle);
#endif //__QC_OS_WIN32__

	return QC_ERR_NONE;
}

int qcThreadProtect (qcThreadHandle hHandle)
{
	return QC_ERR_NONE;
}

int qcThreadConvertPriority (qcThreadPriority * pPriority, int * pPlatform, int bPlatform)
{
	if (bPlatform)
	{
		switch (*pPriority)
		{
#if defined __QC_OS_WIN32__
		case QC_THREAD_PRIORITY_TIME_CRITICAL:
			*pPlatform = THREAD_PRIORITY_TIME_CRITICAL;
			break;

		case QC_THREAD_PRIORITY_HIGHEST:
			*pPlatform = THREAD_PRIORITY_HIGHEST;
			break;

		case QC_THREAD_PRIORITY_ABOVE_NORMAL:
			*pPlatform = THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case QC_THREAD_PRIORITY_NORMAL:
			*pPlatform = THREAD_PRIORITY_NORMAL;
			break;

		case QC_THREAD_PRIORITY_BELOW_NORMAL:
			*pPlatform = THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case QC_THREAD_PRIORITY_LOWEST:
			*pPlatform = THREAD_PRIORITY_LOWEST;
			break;

		case QC_THREAD_PRIORITY_IDLE:
			*pPlatform = THREAD_PRIORITY_IDLE;
			break;
#endif // __QC_OS_WIN32__
		default:
			break;
		}
	}
	else
	{
		switch (*pPlatform)
		{
#if defined __QC_OS_WIN32__
		case THREAD_PRIORITY_TIME_CRITICAL:
			*pPriority = QC_THREAD_PRIORITY_TIME_CRITICAL;
			break;

		case THREAD_PRIORITY_HIGHEST:
			*pPriority = QC_THREAD_PRIORITY_HIGHEST;
			break;

		case THREAD_PRIORITY_ABOVE_NORMAL:
			*pPriority = QC_THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case THREAD_PRIORITY_NORMAL:
			*pPriority = QC_THREAD_PRIORITY_NORMAL;
			break;

		case THREAD_PRIORITY_BELOW_NORMAL:
			*pPriority = QC_THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case THREAD_PRIORITY_LOWEST:
			*pPriority = QC_THREAD_PRIORITY_LOWEST;
			break;

		case THREAD_PRIORITY_IDLE:
			*pPriority = QC_THREAD_PRIORITY_IDLE;
			break;
#endif // __QC_OS_WIN32__
		default:
			break;
		}
	}

	return QC_ERR_NONE;
}

int	qcThreadGetCurrentID (void)
{
#ifdef __QC_OS_WIN32__
	return (int) GetCurrentThreadId ();
#elif defined __QC_OS_NDK__
	return (int) pthread_self ();
#elif defined __QC_OS_IOS__ || defined _OS_MACOS
	return (signed long) pthread_self ();
#endif // __QC_OS_WIN32__
}

qcThreadHandle qcThreadGetCurHandle (void)
{
#ifdef __QC_OS_WIN32__
	return GetCurrentThread ();
#elif defined __QC_OS_NDK__
	return (qcThreadHandle) pthread_self ();
#elif defined __QC_OS_IOS__ || defined _OS_MACOS
	return (qcThreadHandle) pthread_self ();
#endif // __QC_OS_WIN32__
}
