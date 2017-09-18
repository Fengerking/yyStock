/*******************************************************************************
	File:		CIOBase.cpp

	Contains:	base io implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CIOBase.h"

CIOBase::CIOBase(void)
	: CBaseObject ()
	, m_sStatus (QCIO_Init)
	, m_llSize (0)
	, m_pData (NULL)
	, m_llReadPos (-1)
	, m_llDownPos (-1)
{
	SetObjectName ("CIOBase");
}

CIOBase::~CIOBase(void)
{
	Close ();
}

int CIOBase::Open (const char * pURL, long long llOffset, int nFlag)
{
	return QC_ERR_IMPLEMENT;
}

int CIOBase::Reconnect (const char * pNewURL, long long llOffset)
{
	return QC_ERR_IMPLEMENT;
}

int CIOBase::Close (void)
{
	m_llSize = 0;
	m_llReadPos = -1;
	m_llDownPos = -1;
	QC_DEL_A (m_pData);
	return QC_ERR_IMPLEMENT;
}

int CIOBase::Run (void)
{
	m_sStatus = QCIO_Run;
	return QC_ERR_NONE;
}

int CIOBase::Pause (void)
{
	m_sStatus = QCIO_Pause;
	return QC_ERR_NONE;
}

int CIOBase::Stop (void)
{
	m_sStatus = QCIO_Stop;
	return QC_ERR_NONE;
}

long long CIOBase::GetSize (void)
{
	return m_llSize;
}

char * CIOBase::GetData(void)
{
	return m_pData;
}

int CIOBase::Read (unsigned char * pBuff, int & nSize, bool bFull, int nFlag)
{
	return QC_ERR_IMPLEMENT;
}

int	CIOBase::ReadAt (long long llPos, unsigned char * pBuff, int & nSize, bool bFull, int nFlag)
{
	if (llPos != m_llReadPos)
		SetPos(llPos, QCIO_SEEK_BEGIN);
	return Read(pBuff, nSize, bFull, nFlag);
}

int	CIOBase::ReadSync (long long llPos, unsigned char * pBuff, int nSize, int nFlag)
{
	int nRead = nSize;
	int nRC = ReadAt (llPos, pBuff, nRead, true, nFlag);
	if (nRC == QC_ERR_NONE)
		return nRead;
	else
		return 0;
}

int	CIOBase::Write (unsigned char * pBuff, int & nSize)
{
	return QC_ERR_IMPLEMENT;
}

long long CIOBase::SetPos (long long llPos, int nFlag)
{
	return -1;
}

long long CIOBase::GetDownPos (void)
{
	return m_llDownPos;
}

long long CIOBase::GetReadPos (void)
{
	return m_llReadPos;
}

int CIOBase::GetSpeed (int nLastSecs)
{
	return 1024 * 1024 * 8;
}

QCIOType CIOBase::GetType (void)
{
	return QC_IOTYPE_NONE;
}

int CIOBase::GetParam (int nID, void * pParam)
{
	return QC_ERR_IMPLEMENT;
}

int CIOBase::SetParam (int nID, void * pParam)
{
	return QC_ERR_IMPLEMENT;
}

