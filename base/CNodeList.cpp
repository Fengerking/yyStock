/*******************************************************************************
	File:		CNodeList.cpp

	Contains:	Node list implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "CNodeList.h"

// set cursor to the position of each element of list in turn  
#define INTERNALTRAVERSELIST(list, cursor)               \
for ( cursor = (list).GetHeadPositionI()           \
    ; cursor!=NULL                               \
    ; cursor = (list).Next(cursor)                \
    )


// set cursor to the position of each element of list in turn
// in reverse order
#define INTERNALREVERSETRAVERSELIST(list, cursor)        \
for ( cursor = (list).GetTailPositionI()           \
    ; cursor!=NULL                               \
    ; cursor = (list).Prev(cursor)                \
    )

CBaseList::CBaseList(int nItems) :
	m_pFirst(NULL),
	m_pLast(NULL),
	m_Count(0),
	m_Cache(nItems)
{
} // constructor

CBaseList::~CBaseList()
{
	RemoveAll();
} // destructor

void CBaseList::RemoveAll()
{
	CNode *pn = m_pFirst;
	while (pn)
	{
		CNode *op = pn;
		pn = pn->Next();
		delete op;
	}

	m_Count = 0;
	m_pFirst = m_pLast = NULL;
} // RemoveAll

NODEPOS CBaseList::GetHeadPositionI() const
{
	return (NODEPOS) m_pFirst;
} // GetHeadPosition


NODEPOS CBaseList::GetTailPositionI() const
{
	return (NODEPOS) m_pLast;
} // GetTailPosition

int CBaseList::GetCountI() const
{
	return m_Count;
} // GetCount

NODEPOS CBaseList::GetPosition (int nIndex)
{
	if (nIndex < 0 || nIndex >= m_Count)
		return NULL;

	int nPosIndex = 0;
	NODEPOS pos = GetHeadPositionI ();
	while (pos)
	{
		if (nPosIndex == nIndex)
			return pos;
		GetNextI (pos);

		nPosIndex++;
	}

	return NULL;
}

void * CBaseList::GetNextI(NODEPOS& rp) const
{
	if (rp == NULL)
	{
		return NULL;
	}

	// Lock the object before continuing
	void * pObject;

	// Copy the original position then step on 
	CNode *pn = (CNode *) rp;
	rp = (NODEPOS) pn->Next();

	// Get the object at the original position from the list 
	pObject = pn->GetData();

	return pObject;
} //GetNext

void * CBaseList::GetPrevI(NODEPOS& rp) const
{
	if (rp == NULL)
	{
		return NULL;
	}

	// Lock the object before continuing
	void * pObject;

	// Copy the original position then step on 
	CNode *pn = (CNode *) rp;
	rp = (NODEPOS) pn->Prev();

	// Get the object at the original position from the list 
	pObject = pn->GetData();

	return pObject;
} //GetNext

void * CBaseList::GetI(NODEPOS p) const
{
	if (p == NULL)
	{
		return NULL;
	}

	CNode * pn = (CNode *) p;
	void * pObject = pn->GetData();
	return pObject;
} //Get


NODEPOS CBaseList::FindI (void * pObj) const
{
	NODEPOS pn;
	INTERNALTRAVERSELIST(*this, pn)
	{
		if (GetI(pn)==pObj)
		{
			return pn;
		}
	}

	return NULL;
} // Find

void * CBaseList::RemoveHeadI()
{
	return RemoveI((NODEPOS)m_pFirst);
} // RemoveHead

void * CBaseList::RemoveTailI()
{
	return RemoveI((NODEPOS)m_pLast);
} // RemoveTail

void * CBaseList::RemoveI(NODEPOS pos)
{
	if (pos==NULL)
		return NULL;

	CNode *pCurrent = (CNode *) pos;

	// Update the previous node
	CNode *pNode = pCurrent->Prev();
	if (pNode == NULL)
	{
		m_pFirst = pCurrent->Next();
	}
	else
	{
		pNode->SetNext(pCurrent->Next());
	}

	// Update the following node
	pNode = pCurrent->Next();
	if (pNode == NULL)
	{
		m_pLast = pCurrent->Prev();
	}
	else
	{
		pNode->SetPrev(pCurrent->Prev());
	}

	// Get the object this node was looking after 
	void * pObject = pCurrent->GetData();
	m_Cache.AddToCache(pCurrent);

	--m_Count;
	return pObject;
} // Remove

bool CBaseList::RemoveObject(void * pObj)
{
	if (pObj==NULL)
		return false;

	NODEPOS posFound = 0;
	NODEPOS pos = GetHeadPositionI ();
	while (pos)
	{
		posFound = pos;
		if (GetNextI (pos) == pObj)
		{
			RemoveI (posFound);
			return true;
		}
	}

	return false;
} // RemoveObject

NODEPOS CBaseList::AddTailI(void * pObject)
{
	CNode *pNode;
	pNode = (CNode *) m_Cache.RemoveFromCache();
	if (pNode == NULL)
	{
		pNode = new CNode;
	}

	if (pNode == NULL)
	{
		return NULL;
	}


	pNode->SetData(pObject);
	pNode->SetNext(NULL);
	pNode->SetPrev(m_pLast);

	if (m_pLast == NULL)
	{
		m_pFirst = pNode;
	}
	else
	{
		m_pLast->SetNext(pNode);
	}

	m_pLast = pNode;
	++m_Count;
	return (NODEPOS) pNode;
} // AddTail(object)

NODEPOS CBaseList::AddHeadI(void * pObject)
{
	CNode *pNode;
	pNode = (CNode *) m_Cache.RemoveFromCache();
	if (pNode == NULL)
	{
		pNode = new CNode;
	}

	if (pNode == NULL)
	{
		return NULL;
	}

	pNode->SetData(pObject);

	pNode->SetPrev(NULL);
	pNode->SetNext(m_pFirst);

	if (m_pFirst == NULL)
	{
		m_pLast = pNode;
	}
	else
	{
		m_pFirst->SetPrev(pNode);
	}
	m_pFirst = pNode;
	++m_Count;

	return (NODEPOS) pNode;
} // AddHead(object)


bool CBaseList::AddTail(CBaseList *pList)
{
	NODEPOS pos = pList->GetHeadPositionI();

	while (pos)
	{
		if (NULL == AddTailI(pList->GetNextI(pos)))
		{
			return false;
		}
	}
	return true;
} // AddTail(list)


bool CBaseList::AddHead(CBaseList *pList)
{
	NODEPOS pos;

	INTERNALREVERSETRAVERSELIST(*pList, pos)
	{
		if (NULL== AddHeadI(pList->GetI(pos)))
		{
			return false;
		}
	}
	return true;
} // AddHead(list)

NODEPOS  CBaseList::AddAfterI(NODEPOS pos, void * pObj)
{
	if (pos==NULL)
		return AddHeadI(pObj);

	CNode *pAfter = (CNode *) pos;
	if (pAfter==m_pLast)
		return AddTailI(pObj);

	CNode *pNode = (CNode *) m_Cache.RemoveFromCache();
	if (pNode == NULL)
	{
		pNode = new CNode;
	}

	if (pNode == NULL)
	{
		return NULL;
	}

	pNode->SetData(pObj);
	CNode * pBefore = pAfter->Next();

	pNode->SetPrev(pAfter);
	pNode->SetNext(pBefore);
	pBefore->SetPrev(pNode);
	pAfter->SetNext(pNode);

	++m_Count;

	return (NODEPOS) pNode;
} // AddAfter(object)

bool CBaseList::AddAfter(NODEPOS p, CBaseList *pList)
{
	NODEPOS pos;
	INTERNALTRAVERSELIST(*pList, pos)
	{
		p = AddAfterI(p, pList->GetI(pos));
		if (p==NULL)
			return false;
	}
	return true;
} // AddAfter(list)

NODEPOS CBaseList::AddBeforeI(NODEPOS pos, void * pObj)
{
	if (pos==NULL)
		return AddTailI(pObj);

	CNode *pBefore = (CNode *) pos;
	if (pBefore==m_pFirst)
		return AddHeadI(pObj);

	CNode * pNode = (CNode *) m_Cache.RemoveFromCache();
	if (pNode == NULL)
	{
		pNode = new CNode;
	}

	/* Check we have a valid object */

	if (pNode == NULL)
	{
		return NULL;
	}
	pNode->SetData(pObj);

	CNode * pAfter = pBefore->Prev();

	pNode->SetPrev(pAfter);
	pNode->SetNext(pBefore);
	pBefore->SetPrev(pNode);
	pAfter->SetNext(pNode);

	++m_Count;

	return (NODEPOS) pNode;
} // Addbefore(object)



bool CBaseList::AddBefore(NODEPOS p, CBaseList *pList)
{
	NODEPOS pos;
	INTERNALREVERSETRAVERSELIST(*pList, pos)
	{
		p = AddBeforeI(p, pList->GetI(pos));
		if (p==NULL) 
			return false;
	}
	return true;
} // AddBefore(list)

bool CBaseList::MoveToTail (NODEPOS pos, CBaseList *pList)
{
	if (pos==NULL)
		return true;

	CNode * p = (CNode *)pos;
	int cMove = 0;            // number of nodes to move
	while(p!=NULL)
	{
		p = p->Prev();
		++cMove;
	}

	if (pList->m_pLast!=NULL)
		pList->m_pLast->SetNext(m_pFirst);
	if (m_pFirst!=NULL)
		m_pFirst->SetPrev(pList->m_pLast);

	p = (CNode *)pos;

	if (pList->m_pFirst==NULL)
		pList->m_pFirst = m_pFirst;
	m_pFirst = p->Next();
	if (m_pFirst==NULL)
		m_pLast = NULL;
	pList->m_pLast = p;

	if (m_pFirst!=NULL)
		m_pFirst->SetPrev(NULL);
	p->SetNext(NULL);

	m_Count -= cMove;
	pList->m_Count += cMove;

	return true;
} // MoveToTail


bool CBaseList::MoveToHead (NODEPOS pos, CBaseList *pList)
{
	if (pos==NULL)
		return true;  // no-op.  Eliminates special cases later.

	CNode * p = (CNode *)pos;
	int cMove = 0;
	while(p!=NULL)
	{
		p = p->Next();
		++cMove;
	}

	if (pList->m_pFirst!=NULL)
		pList->m_pFirst->SetPrev(m_pLast);
	if (m_pLast!=NULL)
		m_pLast->SetNext(pList->m_pFirst);

	p = (CNode *)pos;

	if (pList->m_pLast==NULL)
		pList->m_pLast = m_pLast;

	m_pLast = p->Prev();
	if (m_pLast==NULL)
		m_pFirst = NULL;
	pList->m_pFirst = p;


	if (m_pLast!=NULL)
		m_pLast->SetNext(NULL);
	p->SetPrev(NULL);


	m_Count -= cMove;
	pList->m_Count += cMove;

	return true;
} // MoveToHead


void CBaseList::Reverse()
{
	CNode * p;

	// Yes we COULD use a traverse, but it would look funny!
	p = m_pFirst;
	while (p!=NULL)
	{
		CNode * q;
		q = p->Next();
		p->SetNext(p->Prev());
		p->SetPrev(q);
		p = q;
	}

	p = m_pFirst;
	m_pFirst = m_pLast;
	m_pLast = p;
} // Reverse
