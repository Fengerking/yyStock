/*******************************************************************************
	File:		CRegMng.cpp

	Contains:	Register op implement code

	Written by:	Fenger King

	Change History (most recent first):
	2013-06-18		Fenger			Create file

*******************************************************************************/
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "CRegMng.h"

#pragma warning (disable : 4996)

CRegMng *	CRegMng::g_pRegMng = NULL;

CRegMng::CRegMng(TCHAR * pSect)
	: CBaseObject ()
	, m_bCU (true)
	, m_pValue (NULL)
	, m_nValue (0)
	, m_pGetValue (NULL)
{
	SetObjectName ("CRegMng");

	_tcscpy (m_szCompnayKey, _T("BangBang"));
	_tcscpy (m_szAppName, _T("bangStock"));
	_tcscpy (m_szSection, pSect);
	_tcscpy (m_szName, _T(""));

	_tcscpy (m_szEmpty, _T(""));

	g_pRegMng = this;
}

CRegMng::~CRegMng(void)
{
	if (m_pGetValue != NULL)
	{
		delete []m_pGetValue;
	}

	g_pRegMng = NULL;
}

bool CRegMng::SetIntValue (const TCHAR * pName, const int nValue)
{
	_tcscpy (m_szName, pName);

	m_pValue = NULL;
	m_nValue = nValue;

	return CreateValue ();
}

int CRegMng::GetIntValue (const TCHAR * pName, int nDefault)
{
	_tcscpy (m_szName, pName);

	m_nValue = nDefault;
	GetValue (false);

	return m_nValue;
}

int CRegMng::GetIntValue (const TCHAR * pSection, const TCHAR * pName, int nDefault)
{
	_tcscpy (m_szSection, pSection);
	_tcscpy (m_szName, pName);

	m_nValue = nDefault;
	GetValue (false);

	return m_nValue;
}

bool CRegMng::SetTextValue (const TCHAR * pName, const TCHAR * pValue)
{
	_tcscpy (m_szName, pName);
	m_pValue = pValue;
	
	return CreateValue ();
}

TCHAR* CRegMng::GetTextValue (const TCHAR * pName)
{
	_tcscpy (m_szName, pName);

	if (!GetValue (true))
		return m_szEmpty;

	return m_pGetValue;
}

TCHAR * CRegMng::GetSDCardName (void)
{
	TCHAR szKey[1024];
	_tcscpy (szKey, _T("HKEY_LOCAL_MACHINE\\System\\StorageManager\\Profiles"));

	HKEY hKey = GetRegKey (szKey);
	if (hKey == NULL)
		return m_szEmpty;

	TCHAR	szSubKey[128];
	DWORD	dwKeySize = 128;
	DWORD	dwIndex = 0;

	bool bFound = false;
	long lRC = ERROR_SUCCESS;
	while (!bFound)
	{
		lRC = RegEnumKeyEx (hKey, dwIndex, szSubKey, &dwKeySize, NULL, NULL, 0, NULL);
		if (lRC != ERROR_SUCCESS)
			break;

		TCHAR * pPos = _tcsstr (szSubKey, _T("SD"));
		if (pPos == szSubKey)
		{
			bFound = true;
			break;
		}

		dwIndex++;
		dwKeySize = 128;
	}
	RegCloseKey(hKey);

	if (!bFound)
		return m_szEmpty;

	_tcscpy (szKey, _T("HKEY_LOCAL_MACHINE\\System\\StorageManager\\Profiles\\"));
	_tcscat (szKey, szSubKey);
	hKey = GetRegKey (szKey);
	if (hKey == NULL)
		return m_szEmpty;

	DWORD dwType,dwLen;

	if (RegQueryValueEx(hKey, _T("Folder"), NULL, &dwType, NULL,&dwLen) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return m_szEmpty;
	}

	if (m_pGetValue != NULL)
		delete []m_pGetValue;

	m_pGetValue = new TCHAR[dwLen+1];
	memset (m_pGetValue, 0, (dwLen+1) * sizeof (TCHAR));
	RegQueryValueEx(hKey, _T("Folder"), NULL, &dwType, (LPBYTE)m_pGetValue,&dwLen);
	
	RegCloseKey(hKey);
	return m_pGetValue;
}

bool CRegMng::CreateValue (void)
{
	HKEY hCreateKey = GetSectionKey (m_szSection);
	if (hCreateKey == NULL)
		return false;

	LONG lResult = -1;

	if (m_pValue != NULL)
		lResult = RegSetValueEx(hCreateKey, m_szName, NULL, REG_SZ, (LPBYTE)m_pValue, _tcslen(m_pValue) * sizeof (TCHAR) + 1);
	else
		lResult = RegSetValueEx(hCreateKey, m_szName, NULL, REG_DWORD, (LPBYTE)&m_nValue, sizeof(m_nValue));

	RegCloseKey(hCreateKey);

	return (lResult == ERROR_SUCCESS);
}

bool CRegMng::GetValue (bool bIsText)
{
	HKEY hGetKey = GetSectionKey (m_szSection);
	if (hGetKey == NULL)
		return false;

	LONG lResult = -1;

	DWORD dwType, dwCount;
	lResult = RegQueryValueEx(hGetKey, m_szName, NULL, &dwType, NULL, &dwCount);
	if (lResult == ERROR_SUCCESS)
	{
		if (bIsText && dwType == REG_SZ)
		{
			if (m_pGetValue != NULL)
				delete []m_pGetValue;
			m_pGetValue = new TCHAR[dwCount + 1];
			memset (m_pGetValue, 0, dwCount + 1);
			lResult = RegQueryValueEx(hGetKey, m_szName, NULL, &dwType,	(unsigned char *)m_pGetValue, &dwCount);
		}
		else if (!bIsText && dwType == REG_DWORD)
		{
			lResult = RegQueryValueEx(hGetKey, m_szName, NULL, &dwType,	(unsigned char *)&m_nValue, &dwCount);
		}
	}

	RegCloseKey(hGetKey);

	return (lResult == ERROR_SUCCESS);
}


HKEY CRegMng::GetAppRegistryKey()
{
	HKEY hAppKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;

	if (RegOpenKeyEx(m_bCU ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, _T("software"), 0, 
					KEY_WRITE|KEY_READ,	&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dwDisp;
		if (RegCreateKeyEx(hSoftKey, m_szCompnayKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, 
							KEY_WRITE|KEY_READ, NULL, &hCompanyKey, &dwDisp) == ERROR_SUCCESS)
		{
			RegCreateKeyEx (hCompanyKey, m_szAppName, 0, REG_NONE, REG_OPTION_NON_VOLATILE, 
							KEY_WRITE|KEY_READ, NULL, &hAppKey, &dwDisp);
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if (hCompanyKey != NULL)
		RegCloseKey(hCompanyKey);

	return hAppKey;
}

int CRegMng::GetRegValueByKey (const TCHAR * inKeyName, const TCHAR * inValueName, BYTE * outValue)
{
	TCHAR szKey[1024];
	_tcscpy (szKey, inKeyName);

	HKEY hKey = GetRegKey (szKey);
	if (hKey == NULL)
		return -1;

	BYTE *pData;
	DWORD dwType,dwLen;
	if( RegQueryValueEx(hKey,inValueName,NULL,&dwType,NULL,&dwLen) != ERROR_SUCCESS )
	{
		RegCloseKey(hKey);
		return -1;
	}

	if(dwLen>=8192) 
	{
		RegCloseKey(hKey);
		return -1;
	}

	if (outValue == NULL)
	{
		RegCloseKey(hKey);
		return dwLen;
	}

	bool bTypeCorrect = false;

	pData=(BYTE *)new BYTE[dwLen];
	memset (pData, 0, dwLen);
	RegQueryValueEx(hKey,inValueName,NULL,&dwType,pData,&dwLen);
	switch(dwType)
	{
	case REG_BINARY:
		break;

	case REG_DWORD:
	case REG_EXPAND_SZ:
	case REG_SZ:
		memcpy (outValue, pData, dwLen);
		bTypeCorrect = true;
		break;

	case REG_MULTI_SZ:
		break;
	default :
		break;
	}
		
	RegCloseKey(hKey);
	delete []pData;

	return bTypeCorrect;
}

bool CRegMng::SetRegValueByKey (const TCHAR * inKeyName, const TCHAR * inValueName, DWORD dwValue)
{
	TCHAR szKey[1024];
	_tcscpy (szKey, inKeyName);

	HKEY hKey = GetRegKey (szKey);
	if (hKey == NULL)
		return false;

	LONG lResult = -1;

	lResult = RegSetValueEx(hKey, inValueName, NULL, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));

	RegCloseKey(hKey);

	return (lResult == ERROR_SUCCESS);
}


HKEY CRegMng::GetRegKey (const TCHAR * inKeyName)
{
	// Get root key
	TCHAR * svKey;
	TCHAR * svNext;
	HKEY key = NULL;
	svKey = GetRootKey(inKeyName, &key);
	if(svKey == NULL) 
	{
		if(key)
			RegCloseKey(key);
		return NULL;
	}

	// Open key hierarchy
	LONG	lResult = -1;
	HKEY	subkey;
	DWORD	dwPerm=KEY_READ;
	DWORD	dwDisp;
	while(svKey != NULL)
	{
		svNext = _tcsstr (svKey, _T("\\"));
		if (svNext)
		{
			*svNext = 0;
			svNext++;
		}
		if(svNext==NULL) 
			dwPerm = KEY_WRITE | KEY_READ;

		if(RegOpenKeyEx (key, svKey, 0, dwPerm, &subkey) != ERROR_SUCCESS)
		{
			lResult = RegCreateKeyEx (key, svKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, 
												KEY_WRITE|KEY_READ, NULL, &subkey, &dwDisp);
			if (lResult != ERROR_SUCCESS)
			{
				RegCloseKey(key);
				return NULL;
			}
		}
		if(key!=NULL)
			RegCloseKey(key);

		key = subkey;
		svKey = svNext;
	} 
	
	return key;
}

TCHAR * CRegMng::GetRootKey(const TCHAR * svPath,  HKEY * pKey)
{ 
	TCHAR * svNext;

	if(svPath == NULL)
		return NULL;
	

	TCHAR * pStrPathNew = new TCHAR[_tcslen (svPath) + 1];
	TCHAR * pStrPath = pStrPathNew;//new TCHAR[_tcslen (svPath) + 1];

	_tcscpy (pStrPath, svPath);

	if(_tcsncmp(pStrPath, _T("\\\\"),2) == 0)
		pStrPath+=2;
	else if(_tcsncmp(pStrPath, _T("\\"), 1) == 0)
		pStrPath++;

	svNext = _tcsstr (pStrPath, _T("\\"));
	if (svNext)
	{
		*svNext = 0;
		svNext++;
	}

	if((_tcsicmp (pStrPath, _T("HKEY_CLASSES_ROOT")) == 0) ||
		(_tcsicmp(pStrPath, _T("HKCR")) == 0) ) *pKey = HKEY_CLASSES_ROOT;
	else if((_tcsicmp(pStrPath, _T("HKEY_CURRENT_USER")) == 0) ||
		(_tcsicmp(pStrPath, _T("HKCU")) == 0) ) *pKey = HKEY_CURRENT_USER;
	else if((_tcsicmp(pStrPath, _T("HKEY_LOCAL_MACHINE")) == 0) ||
		(_tcsicmp(pStrPath, _T("HKLM")) ==0 ) ) *pKey = HKEY_LOCAL_MACHINE;
	else if((_tcsicmp(pStrPath, _T("HKEY_USERS"))==0) ||
		(_tcsicmp(pStrPath, _T("HKU")) == 0) ) *pKey = HKEY_USERS;
//	else if((_tcsicmp(svPath, _T("HKEY_CURRENT_CONFIG")) == 0) ||
//		(_tcsicmp(svPath, _T("HKCC")) == 0) ) *pKey = HKEY_CURRENT_CONFIG;
//	else if((_tcsicmp(svPath, _T("HKEY_DYN_DATA")) == 0) ||
//		(_tcsicmp(svPath, _T("HKDD")) == 0) ) *pKey = HKEY_DYN_DATA;
	else 
	{
		delete []pStrPathNew;
		return NULL;
	}

	svNext = (TCHAR *)svPath + (svNext - pStrPathNew);
	delete []pStrPathNew;

	return svNext;
}

HKEY CRegMng::GetSectionKey(const TCHAR * lpszSection)
{
	HKEY hSectionKey = NULL;
	HKEY hAppKey = GetAppRegistryKey();
	if (hAppKey == NULL)
		return NULL;

	DWORD dwDisp;
	RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE, REG_OPTION_NON_VOLATILE, 
					KEY_WRITE|KEY_READ, NULL, &hSectionKey, &dwDisp);
	RegCloseKey(hAppKey);

	return hSectionKey;
}

bool CRegMng::DeleteKey(void)
{
	HKEY hSecKey = GetSectionKey(m_szSection);
	if (hSecKey)
	{
		RegDeleteValue(hSecKey, m_szName);
		RegCloseKey(hSecKey);
		return true;
	}

	return false;
}

