/*******************************************************************************
	File:		CStockItemList.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockItemList_H__
#define __CStockItemList_H__
#include "qcStock.h"

#include "CBaseObject.h"
#include "CNodeList.h"

class CStockItemList : public CBaseObject
{
public:
    CStockItemList(void);
    virtual ~CStockItemList(void);

	virtual int OpenHttpList(void);
	virtual int OpenFileList(void);


protected:
	virtual int	SaveList(void);

protected:
	char	m_szStockListURL[256];
	char	m_szStockListFile[256];

public:
	CObjectList<qcStockInfoItem>	m_lstStock;
	static CStockItemList *			g_stkList;
	static char						g_szStkCode[16];
};

#endif //__CStockItemList_H__
