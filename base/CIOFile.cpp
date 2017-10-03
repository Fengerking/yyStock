/*******************************************************************************
	File:		CIOFile.cpp

	Contains:	local file io implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-05		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"

#include "CIOFile.h"

CIOFile::CIOFile(void)
	: CIOBase ()
	, m_hFile (NULL)
{
	SetObjectName ("CIOFile");
}

CIOFile::~CIOFile(void)
{
	Close ();
}

int CIOFile::Open (const char * pURL, long long llOffset, int nFlag)
{
	Close();

	char * pNewURL = (char *)pURL;
	if (!strncmp (pURL, "file://", 7))
		pNewURL = (char *)pURL + 7;
	if (nFlag == QCIO_FLAG_READ)
		m_hFile = CreateFile(pNewURL, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	else if (nFlag == QCIO_FLAG_WRITE) 
		m_hFile = CreateFile(pNewURL, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, (DWORD) 0, NULL);
	else
		m_hFile = CreateFile(pNewURL, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		return -1;
	}
	if (nFlag & QCIO_FLAG_READ)
	{
		DWORD dwHigh = 0;
		DWORD dwSize = GetFileSize(m_hFile, &dwHigh);
		m_llSize = dwHigh;
		m_llSize = m_llSize << 32;
		m_llSize += dwSize;
	}
	m_llReadPos = llOffset;
	
	return QC_ERR_NONE;
}

int CIOFile::Close (void)
{
	if (m_hFile != NULL)
		CloseHandle (m_hFile);
	m_hFile = NULL;

	return CIOBase::Close ();
}

int CIOFile::Read (unsigned char * pBuff, int & nSize, bool bFull, int nFlag)
{
	if (m_hFile == NULL)
		return QC_ERR_FAILED;
	if (m_llReadPos >= m_llSize)
		return QC_ERR_FINISH;

	int nRead = -1;
	DWORD	dwRead = 0;
	ReadFile (m_hFile, pBuff, nSize, &dwRead, NULL);
	if (dwRead == 0)
	{
		if (m_llReadPos < m_llSize)
			return QC_ERR_STATUS;
		return QC_ERR_FAILED;
	}
	m_llReadPos += dwRead;
	nSize = (int)dwRead;

	return QC_ERR_NONE;
}

int	CIOFile::Write (unsigned char * pBuff, int nSize)
{
	unsigned int uWrite = 0;

	WriteFile (m_hFile, pBuff, nSize, (DWORD *)&uWrite, NULL);
	
	return uWrite;
}

long long CIOFile::SetPos (long long llPos, int nFlag)
{
	if (m_hFile == NULL)
		return -1;

	long		lPos = (long)llPos;
	long		lHigh = (long)(llPos >> 32);
	
	int sMove = FILE_BEGIN;
	if (nFlag == QCIO_SEEK_BEGIN)
	{
		sMove = FILE_BEGIN;
		m_llReadPos = llPos;

		lPos = (long)llPos;
		lHigh = (long)(llPos >> 32);
	}
	else if (nFlag == QCIO_SEEK_CUR)
	{
		sMove = FILE_CURRENT;
		m_llReadPos = m_llReadPos + llPos;
	}
	else if (nFlag == QCIO_SEEK_END)
	{
		sMove = FILE_END;
		m_llReadPos = m_llSize - llPos;
		lPos = (long)llPos;
		lHigh = (long)(llPos >> 32);
	}

	if (m_llReadPos > m_llSize)
		return QC_ERR_STATUS;

	DWORD dwRC = SetFilePointer (m_hFile, lPos, &lHigh, sMove);
	//modefied by Aiven,return the currect file pointer if finish the seek.
	if(dwRC == INVALID_SET_FILE_POINTER)
		return QC_ERR_FAILED;
	return llPos;
}

char * CIOFile::GetData(void)
{
	if (m_pData != NULL)
		return m_pData;
	if (m_hFile == NULL)
		return NULL;

	int nSize = (int)m_llSize;
	m_pData = new char[nSize];

	long long llPos = m_llReadPos;
	SetPos(0, QCIO_SEEK_BEGIN);
	Read((unsigned char *)m_pData, nSize, true, 0);
	SetPos(llPos, QCIO_SEEK_BEGIN);

	return m_pData;
}

QCIOType CIOFile::GetType (void)
{
	return QC_IOTYPE_FILE;
}

