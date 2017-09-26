/*******************************************************************************
	File:		CThreadWork.h

	Contains:	the mutex lock header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CThreadWork_H__
#define __CThreadWork_H__

#include "qcErr.h"

#include "CBaseObject.h"
#include "CNodeList.h"
#include "CMutexLock.h"

#include "UThreadFunc.h"

typedef enum {
	QCWORK_Init		= 0,
	QCWORK_Run		= 1,
	QCWORK_Pause	= 2,
	QCWORK_Stop		= 3,
	QCWORK_MAX		= 0X7FFFFFFF
} QCWORK_STATUS;

class CThreadEvent;

class CThreadFunc 
{
public:
	CThreadFunc (void){}
	virtual ~CThreadFunc (void){}

public:
	virtual int		OnEvent (void * pEvent) {return OnHandleEvent ((CThreadEvent *)pEvent);}
	virtual int		OnWork (void) {return OnWorkItem ();}
	virtual int		OnStart (void) {return OnStartFunc ();}
	virtual int		OnStop (void) {return OnStopFunc ();}

protected:
	virtual int		OnHandleEvent (CThreadEvent * pEvent) {return 0;}
	virtual int		OnWorkItem (void) {return 0;}
	virtual int		OnStartFunc (void) {return 0;}
	virtual int		OnStopFunc (void) {return 0;}
};

class CThreadEvent
{
public:
	CThreadEvent (int nID, int nValue, long long llValue, const char * pName)
	{
		m_nTime = 0;
		m_nID = nID;
		m_nValue = nValue;
		m_llValue = llValue;
		m_pName = NULL;
		if (pName != NULL)
		{
			m_pName = new char [strlen (pName)+1];
			strcpy (m_pName, pName);
		}
	}
	virtual ~CThreadEvent (void)
	{
		QC_DEL_A (m_pName);
	}

	virtual int	Fire (void) 
	{
		return (m_pThreadFunc->*m_fOnEvent) (this);
	}

	virtual int	SetEventFunc (CThreadFunc * pThreadFunc, int (CThreadFunc::* fOnEvent) (void *))
	{
		m_pThreadFunc = pThreadFunc;
		m_fOnEvent = fOnEvent;
		return 0;
	}

	virtual int SetName (const char * pName)
	{
		QC_DEL_A (m_pName);
		if (pName != NULL)
		{
			m_pName = new char [strlen (pName)+1];
			strcpy (m_pName, pName);
		}
		return QC_ERR_NONE;
	}
	
public:
	int			m_nTime;
	int			m_nID;
	char *		m_pName;
	int			m_nValue;
	long long	m_llValue;

protected:
	CThreadFunc *		m_pThreadFunc;
	int	(CThreadFunc::*	m_fOnEvent) (void *);

};

class CThreadWork : public CBaseObject
{
public:
    CThreadWork (void);
    virtual ~CThreadWork(void);

	virtual int				SetOwner (char * pOwner);
	virtual int				Start (void);
	virtual int				Pause (void);
	virtual int				Stop (void);

	virtual int				SetWorkProc (CThreadFunc * pThreadFunc, int (CThreadFunc::* fOnWork) (void));
	virtual int				SetStartStopFunc (int (CThreadFunc::* fOnStart) (void), int (CThreadFunc::* fOnStop) (void));
	virtual int				SetPriority (qcThreadPriority nPriority);
	QCWORK_STATUS			GetStatus (void) {return m_nStatus;}

	virtual CThreadEvent *	GetFree (void);
	virtual int				PostEvent (CThreadEvent * pEvent, int nDelay);


protected:
	static	int		WorkProc (void * pParam);
	virtual int		WorkLoop (void);

protected:
	char				m_szOwner[64];
	CThreadFunc *		m_pThreadFunc;
	int	(CThreadFunc::*	m_fOnWork) (void);
	int	(CThreadFunc::*	m_fOnStart) (void);
	int	(CThreadFunc::*	m_fOnStop) (void);

	qcThreadHandle		m_hThread;
	QCWORK_STATUS		m_nStatus;
	bool				m_bWorking;

	qcThreadPriority	m_nPriority;
	bool				m_bPriority;

	CMutexLock					m_mtTask;
	CObjectList<CThreadEvent>	m_lstFree;
	CObjectList<CThreadEvent>	m_lstFull;

};

#endif //__CThreadWork_H__
