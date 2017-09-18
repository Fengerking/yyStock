/*******************************************************************************
	File:		CIOBase.h

	Contains:	the base io header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CIOBase_H__
#define __CIOBase_H__

#include "CBaseObject.h"

#define	QCIO_FLAG_READ			1                                   /**< read-only */
#define QCIO_FLAG_WRITE			2                                   /**< write-only */
#define QCIO_FLAG_READ_WRITE	(QCIO_FLAG_READ | QCIO_FLAG_WRITE)	/**< read-write pseudo flag */
#define	QCIO_FLAG_URL			8                                   /**< connect with URL  */

#define QCIO_SEEK_SIZE			1
#define QCIO_SEEK_BEGIN			2
#define QCIO_SEEK_CUR			3
#define QCIO_SEEK_END			4

// the IO source type
typedef enum {
	QC_IOTYPE_NONE,
	QC_IOTYPE_FILE,
	QC_IOTYPE_HTTP_VOD,
	QC_IOTYPE_HTTP_LIVE,
	QC_IOTYPE_RTMP,
	QC_IOTYPE_MAX = 0X7FFFFFFF
}QCIOType;

typedef enum {
    QCIO_Init		= 0,
	QCIO_Open		= 1,
    QCIO_Run		= 2,
    QCIO_Pause	    = 3,
    QCIO_Stop		= 4,
    QCIO_MAX		= 0X7FFFFFFF
} QCIO_STATUS;

class CIOBase : public CBaseObject
{
public:
    CIOBase(void);
    virtual ~CIOBase(void);

	virtual int 		Open (const char * pURL, long long llOffset, int nFlag);
	virtual int 		Reconnect (const char * pNewURL, long long llOffset);
	virtual int 		Close (void);

	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);

	virtual long long 	GetSize (void);
	virtual char *		GetData (void);
	virtual int			Read (unsigned char * pBuff, int & nSize, bool bFull, int nFlag);
	virtual int			ReadAt (long long llPos, unsigned char * pBuff, int & nSize, bool bFull, int nFlag);
	virtual int			ReadSync (long long llPos, unsigned char * pBuff, int nSize, int nFlag);
	virtual int			Write (unsigned char * pBuff, int & nSize);
	virtual long long 	SetPos (long long llPos, int nFlag);

	virtual long long 	GetReadPos (void);
	virtual long long 	GetDownPos (void);
	virtual int			GetSpeed (int nLastSecs);
	virtual QCIOType	GetType (void);

	virtual int 		GetParam (int nID, void * pParam);
	virtual int 		SetParam (int nID, void * pParam);

protected:
	QCIO_STATUS			m_sStatus;
	long long			m_llSize;
	char *				m_pData;
	long long			m_llReadPos;
	long long			m_llDownPos;
};

#endif // __CIOBase_H__
