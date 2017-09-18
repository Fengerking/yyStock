/*******************************************************************************
	File:		UThreadFunc.h

	Contains:	The base utility for thread header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __UThreadFunc_H__
#define __UThreadFunc_H__

#if __QC_OS_WIN32__

#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
#include <pthread.h>
#endif // __QC_OS_WIN32__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if __QC_OS_WIN32__
typedef void * qcThreadHandle;
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
typedef pthread_t *qcThreadHandle;
#endif
typedef int (* qcThreadProc) (void * pParam);

typedef enum QCTHRD_Status
{
	QCTHRD_Stop			= 0,
	QCTHRD_Pause		= 1,
	QCTHRD_Run			= 2,
	QCTHRD_Work			= 3,
} QCTHRD_Status;

typedef enum qcThreadPriority
{
	QC_THREAD_PRIORITY_TIME_CRITICAL	= 0x0,
	QC_THREAD_PRIORITY_HIGHEST			= 0x1,
	QC_THREAD_PRIORITY_ABOVE_NORMAL		= 0x2,
	QC_THREAD_PRIORITY_NORMAL			= 0x3,
	QC_THREAD_PRIORITY_BELOW_NORMAL		= 0x4,
	QC_THREAD_PRIORITY_LOWEST			= 0x5,
	QC_THREAD_PRIORITY_IDLE				= 0x6,
	QC_THREAD_PRIORITY_MAX				= 0x7FFFFFFF,
} qcThreadPriority;

/**
 * Create the thread
 * \param pHandle [out] the thread handle
 * \param pID [out] the thread id
 * \param fProc [in] the thread start address
 * \param pParam [in] the thread call back parameter
 * \param uFlag [in] the thread was create with the flagr
 * \return value qc_ErrorNone
 */
int	qcThreadCreate (qcThreadHandle * pHandle, int * pID, 
						qcThreadProc fProc, void * pParam, int uFlag);


/**
 * close the thread
 * \param pHandle [in] the thread handle was created by qcThreadCreate
 * \param uExitCode [in] the return code after exit the thread
 * \return value qc_ErrorNone
 */
int	qcThreadClose (qcThreadHandle hHandle, int uExitCode);


/**
 * Get the thread priority
 * \param pHandle [in] the thread handle was created by qcThreadCreate
 * \param pPriority [out] the priority to get
 * \return value qc_ErrorNone
 */
int	qcThreadGetPriority (qcThreadHandle hHandle, qcThreadPriority * pPriority);


/**
 * Set the thread priority
 * \param pHandle [in] the thread handle was created by qcThreadCreate
 * \param uPriority [in] the priority to set
 * \return value qc_ErrorNone
 */
int	qcThreadSetPriority (qcThreadHandle hHandle, qcThreadPriority uPriority);


/**
 * Suspend the thread
 * \param pHandle [in] the thread handle was created by qcThreadCreate
 * \return value qc_ErrorNone
 */
int	qcThreadSuspend (qcThreadHandle hHandle);


/**
 * Resume the thread
 * \param pHandle [in] the thread handle was created by qcThreadCreate
 * \return value qc_ErrorNone
 */
int	qcThreadResume (qcThreadHandle hHandle);

/**
 * Protect the thread
 * \param pHandle [in] the thread handle was created by qcThreadCreate
 * \return value qc_ErrorNone
 */
int	qcThreadProtect (qcThreadHandle hHandle);

/**
 * set the thread name
 * \param uThreadID [in] the thread id was created by qcThreadCreate
 * \return value qc_ErrorNone
 */
void qcThreadSetName(int uThreadID, const char* threadName);

/**
 * convert the thread priority
 * \param uPriority [in] the thread priority
 * \return value 
 */
int	qcThreadConvertPriority (qcThreadPriority * pPriority, int * pPlatform, int bPlatform);

/**
 * Get the current thread ID
 * \return current thread ID 
 */
int	qcThreadGetCurrentID (void);

/**
 * Get the current thread handle
 * \return current thread handle 
 */
qcThreadHandle	qcThreadGetCurHandle (void);
    
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __UThreadFunc_H__
