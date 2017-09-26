/*******************************************************************************
	File:		CMutexLock.cpp

	Contains:	Mutex lock implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "UThreadFunc.h"
#include "CMutexLock.h"

CMutexLock::CMutexLock(void)
{
	SetObjectName ("CMutexLock");

#ifdef __QC_OS_WIN32__
    InitializeCriticalSection(&m_CritSec);
#elif defined _OS_LNXPC
	pthread_mutexattr_t attr;
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init (&m_hMutex, &attr);
#elif defined __QC_OS_NDK__
	pthread_mutexattr_t attr = PTHREAD_MUTEX_RECURSIVE_NP;
	pthread_mutex_init (&m_hMutex, &attr);
#elif defined __QC_OS_IOS__ || defined __QC_OS_MACOS__
    //m_hMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutexattr_t mta;
    pthread_mutexattr_init(&mta);
    pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (&m_hMutex, &mta);
    pthread_mutexattr_destroy(&mta);
    
#endif // __QC_OS_WIN32__

    m_nCurrentOwner = 0;
	m_nLockCount = 0;
}

CMutexLock::~CMutexLock(void)
{
#ifdef __QC_OS_WIN32__
    DeleteCriticalSection(&m_CritSec);
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__ || defined __QC_OS_MACOS__
	pthread_mutex_destroy (&m_hMutex);
#endif // __QC_OS_WIN32__
}

void CMutexLock::Lock(void)
{
	unsigned int nNewOwner = 0;
#ifdef __QC_OS_WIN32__
    nNewOwner = GetCurrentThreadId();
    EnterCriticalSection(&m_CritSec);
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__ || defined __QC_OS_MACOS__
	nNewOwner = qcThreadGetCurrentID();
	pthread_mutex_lock (&m_hMutex);
#endif // __QC_OS_WIN32__

    if (0 == m_nLockCount++)
        m_nCurrentOwner = nNewOwner;
}

void CMutexLock::Unlock(void)
{
    if (0 == --m_nLockCount)
        m_nCurrentOwner = 0;
#ifdef __QC_OS_WIN32__
    LeaveCriticalSection(&m_CritSec);
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__ || defined __QC_OS_MACOS__
	pthread_mutex_unlock (&m_hMutex);
#endif // __QC_OS_WIN32__
}

bool CMutexLock::IsLock (void)
{
	return m_nLockCount > 0 ? true : false;
}

