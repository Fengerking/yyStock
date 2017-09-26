/*******************************************************************************
	File:		CMutexLock.h

	Contains:	the mutex lock header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-02		Bangfei			Create file

*******************************************************************************/
#ifndef __CMutexLock_H__
#define __CMutexLock_H__

#include "CBaseObject.h"
#if defined __QC_OS_NDK__ || defined __QC_OS_IOS__
#include <pthread.h>
#endif // __QC_OS_NDK__

class CMutexLock : public CBaseObject
{
public:
    CMutexLock(void);
    virtual ~CMutexLock(void);

    void	Lock(void);
    void	Unlock(void);
	bool	IsLock (void);

public:
#ifdef __QC_OS_WIN32__
    CRITICAL_SECTION	m_CritSec;
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
	pthread_mutex_t		m_hMutex;
#endif // __QC_OS_WIN32__

	unsigned int		m_nCurrentOwner;
	unsigned int		m_nLockCount;
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class CAutoLock
{
protected:
    CMutexLock * m_pLock;

public:
    CAutoLock(CMutexLock * plock)
    {
        m_pLock = plock;
        if (m_pLock) {
            m_pLock->Lock();
        }
    };

    ~CAutoLock()
	{
        if (m_pLock) {
            m_pLock->Unlock();
        }
    };
};

#endif //__CMutexLock_H__
