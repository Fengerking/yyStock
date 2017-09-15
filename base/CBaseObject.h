/*******************************************************************************
	File:		CBaseObject.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CBaseObject_H__
#define __CBaseObject_H__

#include "stdio.h"
#include "string.h"

#include "qcType.h"
#include "qcDef.h"

class CBaseObject
{
public:
	CBaseObject(void);
	virtual ~CBaseObject(void);

protected:
	virtual void	SetObjectName (const char * pObjName);

protected:
	char			m_szObjName[64];
	const char *	m_pClassFileName;
	long long		m_llDbgTime;

public:
	static int		g_ObjectNum;

};

#endif // __CBaseObject_H__
