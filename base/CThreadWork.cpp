/*******************************************************************************
	File:		CThreadWork.cpp

	Contains:	Mutex lock implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "qcType.h"
#include "qcErr.h"

#include "CThreadWork.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

CThreadWork::CThreadWork(void)
	: CBaseObject ()
	, m_hThread (NULL)
	, m_nStatus (QCWORK_Init)
	, m_bWorking (false)
	, m_nPriority (QC_THREAD_PRIORITY_NORMAL)
	, m_bPriority (false)
	, m_pThreadFunc (NULL)
	, m_fOnWork (NULL)
	, m_fOnStart (NULL)
	, m_fOnStop (NULL)

{
	SetObjectName ("CThreadWork");
	strcpy (m_szOwner, "Thread Work");
}

CThreadWork::~CThreadWork(void)
{
	Stop ();
	CAutoLock lock (&m_mtTask);
	CThreadEvent * pEvent = m_lstFull.RemoveHead ();
	while (pEvent != NULL)
	{
		delete pEvent;
		pEvent = m_lstFull.RemoveHead ();
	}
	pEvent = m_lstFree.RemoveHead ();
	while (pEvent != NULL)
	{
		delete pEvent;
		pEvent = m_lstFree.RemoveHead ();
	}
}

int CThreadWork::SetOwner (char * pOwner)
{
	strcpy (m_szOwner, pOwner);
	return QC_ERR_NONE;
}

int CThreadWork::Start (void)
{
	m_nStatus = QCWORK_Run;
	if (m_hThread == NULL)
	{
		int nID = 0;
		qcThreadCreate (&m_hThread, &nID, WorkProc, this, 0);
	}
	return QC_ERR_NONE;
}

int CThreadWork::Pause (void)
{
	m_nStatus = QCWORK_Pause;
	int nStart = qcGetSysTime ();
	int	nTryTimes = 0;
	while (m_bWorking)
	{
		qcSleep (5000);
		nTryTimes++;
		if (qcGetSysTime () - nStart > 5000)
		{
			if (nTryTimes % 100 == 0)
				QCLOGW ("The %s can't Pause in work thread! It used Time %8d", m_szOwner, qcGetSysTime () - nStart);
		}
	}
	return QC_ERR_NONE;
}

int CThreadWork::Stop (void)
{
	m_nStatus = QCWORK_Stop;
	int nStart = qcGetSysTime ();
	int	nTryTimes = 0;
	while (m_hThread != NULL)
	{
		qcSleep (5000);
		nTryTimes++;
		if (qcGetSysTime () - nStart > 5000)
		{
			if (nTryTimes % 100 == 0)
				QCLOGW ("The %s can't Stop in work thread! It used Time %8d", m_szOwner, qcGetSysTime () - nStart);
		}
	}
	return QC_ERR_NONE;
}

int CThreadWork::SetWorkProc (CThreadFunc * pThreadFunc, int (CThreadFunc::* fOnWork) (void))
{
	m_pThreadFunc = pThreadFunc;
	m_fOnWork = fOnWork;
	return QC_ERR_NONE;
}

int CThreadWork::SetStartStopFunc (int (CThreadFunc::* fOnStart) (void), int (CThreadFunc::* fOnStop) (void))
{
	m_fOnStart = fOnStart;
	m_fOnStop = fOnStop;
	return QC_ERR_NONE;
}

int CThreadWork::SetPriority (qcThreadPriority nPriority)
{
	if (m_nPriority == nPriority)
		return QC_ERR_NONE;

	m_nPriority = nPriority;
	m_bPriority = true;
	return QC_ERR_NONE;
}

CThreadEvent *	CThreadWork::GetFree (void)
{
	if (m_lstFree.GetCount () <= 0)
		return NULL;
	CAutoLock lock (&m_mtTask);
	CThreadEvent * pEvent = m_lstFree.RemoveHead ();
	return pEvent;
}

int CThreadWork::PostEvent (CThreadEvent * pEvent, int nDelay)
{
	if (pEvent == NULL)
		return QC_ERR_ARG;
	CAutoLock lock (&m_mtTask);
	if (nDelay > 0)
		pEvent->m_nTime = qcGetSysTime () + nDelay;
	else
		pEvent->m_nTime = 0;
	m_lstFull.AddTail (pEvent);

	return QC_ERR_NONE;
}

int CThreadWork::WorkProc (void * pParam)
{
	CThreadWork * pWork = (CThreadWork *)pParam;
	pWork->WorkLoop ();
	return 0;
}

int CThreadWork::WorkLoop (void)
{
	if (m_pThreadFunc != NULL && m_fOnStart != NULL)
		(m_pThreadFunc->*m_fOnStart)();
		
	while (m_nStatus == QCWORK_Run || m_nStatus == QCWORK_Pause)
	{
		m_bWorking = false;
		if (m_nStatus == QCWORK_Pause)
		{
			qcSleep (5000);
			continue;
		}
		m_bWorking = true;

		if (m_bPriority)
		{
			qcThreadSetPriority (qcThreadGetCurHandle (), m_nPriority);
			m_bPriority = false;
		}

		if (m_fOnWork != NULL)
			(m_pThreadFunc->*m_fOnWork)();

		// handle the thread event
		CThreadEvent *	pEventTask = NULL;
		CThreadEvent *	pEventTemp = NULL;
		NODEPOS			pPos = m_lstFull.GetHeadPosition ();
		while (pPos != NULL)
		{
			CAutoLock lock (&m_mtTask);
			pEventTemp = m_lstFull.GetNext (pPos);
			if (pEventTemp->m_nTime <= 0 || pEventTemp->m_nTime <= qcGetSysTime ())
			{
				m_lstFull.Remove (pEventTemp);
				m_lstFree.AddTail (pEventTemp);
				pEventTask = pEventTemp;
				break;
			}
		}
		if (pEventTask != NULL)
			pEventTask->Fire ();
		else if (m_fOnWork == NULL)
			qcSleep (1000);
	}
	if (m_fOnStop != NULL)
		(m_pThreadFunc->*m_fOnStop)();
    qcThreadClose(m_hThread, 0);
	m_hThread = NULL;
	return QC_ERR_NONE;
}
