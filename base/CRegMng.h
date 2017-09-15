/*******************************************************************************
	File:		CRegMng.h

	Contains:	register operator head file.

	Written by:	Fenger King

	Change History (most recent first):
	2013-06-19		Fenger			Create file

*******************************************************************************/
#ifndef __CRegMng_H__
#define __CRegMng_H__

#include "CBaseObject.h"

class  CRegMng : public CBaseObject
{
public:
	static CRegMng *	g_pRegMng;

public:
	CRegMng(TCHAR * pSect);
	virtual ~CRegMng(void);

	bool	SetIntValue (const TCHAR * pName, const int nValue);
	int		GetIntValue (const TCHAR * pName, int nDefault = 0);

	bool	SetTextValue (const TCHAR * pName, const TCHAR * pValue);
	TCHAR*	GetTextValue (const TCHAR * pName);

	int		GetIntValue (const TCHAR * pSection, const TCHAR * pName, int nDefault = 0);

	int		GetRegValueByKey (const TCHAR * inKeyName, const TCHAR * inValueName, BYTE * outValue);
	bool	SetRegValueByKey (const TCHAR * inKeyName, const TCHAR * inValueName, DWORD dwValue);

	TCHAR *	GetSDCardName (void);

private:
	bool	CreateValue (void);
	bool	GetValue (bool bIsText);

	HKEY	GetAppRegistryKey(void);
	HKEY	GetSectionKey(const TCHAR * lpszSection);

	HKEY	GetRegKey (const TCHAR * inKeyName);
	TCHAR * GetRootKey(const TCHAR * svPath,  HKEY * pKey);

	bool	DeleteKey(void);

private:
	bool			m_bCU;
	TCHAR			m_szCompnayKey[256];
	TCHAR			m_szAppName[256];
	TCHAR			m_szSection[256];
	TCHAR			m_szName[256];
	const TCHAR *	m_pValue;
	TCHAR *			m_pGetValue;
	int				m_nValue;

	TCHAR			m_szEmpty[4];
};

#endif // __CRegMng_H__