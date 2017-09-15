/*******************************************************************************
	File:		CBodeList.h

	Contains:	the node list header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CBaseList_H__
#define __CBaseList_H__

#include "stdio.h"

#ifndef NODEPOS
#define NODEPOS void *
#endif // NODEPOS

class CBaseList 
{
public:
	class CNode
	{
		CNode *m_pPrev;
		CNode *m_pNext;
		void * m_pObject;

	public:
		CNode()
		{
			m_pPrev = NULL;
			m_pNext = NULL;
			m_pObject = NULL;
		};

		CNode *Prev() const { return m_pPrev; };
		CNode *Next() const { return m_pNext; };

		void SetPrev(CNode *p) { m_pPrev = p; };
		void SetNext(CNode *p) { m_pNext = p; };

		void * GetData() const { return m_pObject; };
		void SetData(void * p) { m_pObject = p; };
	};

	class CNodeCache
	{
	public:
		CNodeCache(int iCacheSize)
			: m_iCacheSize(iCacheSize)
			, m_iUsed(0)
			, m_pHead(NULL)
		{
		};

		~CNodeCache()
		{
			CNode *pNode = m_pHead;
			while (pNode)
			{
				CNode *pCurrent = pNode;
				pNode = pNode->Next();
				delete pCurrent;
				m_iUsed--;
			}
		};

		void AddToCache(CNode *pNode)
		{
			if (m_iUsed < m_iCacheSize)
			{
				pNode->SetNext(m_pHead);
				m_pHead = pNode;
				m_iUsed++;
			}
			else
			{
				delete pNode;
			}
		};

		CNode * RemoveFromCache()
		{
			CNode *pNode = m_pHead;
			if (pNode != NULL)
			{
				m_pHead = pNode->Next();
				m_iUsed--;
			}
			return pNode;
		};

		private:
			int		m_iCacheSize;
			int		m_iUsed;
			CNode *	m_pHead;
	};

protected:
	CNode*	m_pFirst;
	CNode*	m_pLast;
	int		m_Count;

private:
	CNodeCache m_Cache;

private:
	CBaseList(const CBaseList &refList);
	CBaseList &operator=(const CBaseList &refList);

public:
	CBaseList(int nItems = 0);
	virtual ~CBaseList();

	void		RemoveAll();
	NODEPOS	GetHeadPositionI() const;
	NODEPOS	GetTailPositionI() const;
	int		GetCountI() const;

	NODEPOS	GetPosition (int nIndex);

protected:
	void *		GetNextI(NODEPOS& rp) const;
	void *		GetPrevI(NODEPOS& rp) const;
	void *		GetI(NODEPOS p) const;

public:
	NODEPOS Next(NODEPOS pos) const
	{
		if (pos == NULL)
		{
			return (NODEPOS) m_pFirst;
		}
		CNode *pn = (CNode *) pos;
		return (NODEPOS) pn->Next();
	} //Next

	// See Next
	NODEPOS Prev(NODEPOS pos) const
	{
		if (pos == NULL)
		{
			return (NODEPOS) m_pLast;
		}
		CNode *pn = (CNode *) pos;
		return (NODEPOS) pn->Prev();
	} //Prev

protected:
	NODEPOS	FindI( void * pObj) const;
	void *		RemoveHeadI();
	void *		RemoveTailI();
	void *		RemoveI(NODEPOS p);
	bool		RemoveObject(void * pObj);

	NODEPOS	AddTailI(void * pObj);

public:
	bool		AddTail(CBaseList *pList);

protected:
	NODEPOS	AddHeadI(void * pObj);
public:
	bool		AddHead(CBaseList *pList);

protected:
	NODEPOS	AddAfterI(NODEPOS p, void * pObj);
public:
	bool		AddAfter(NODEPOS p, CBaseList *pList);

protected:
	NODEPOS	AddBeforeI(NODEPOS p, void * pObj);
public:
	bool		AddBefore(NODEPOS p, CBaseList *pList);


	bool		MoveToTail(NODEPOS pos, CBaseList *pList);
	bool		MoveToHead(NODEPOS pos, CBaseList *pList);

	void		Reverse();


	// set cursor to the position of each element of list in turn  
	#define TRAVERSELIST(list, cursor)               \
	for ( cursor = (list).GetHeadPosition()           \
		; cursor!=NULL                               \
		; cursor = (list).Next(cursor)                \
		)


	// set cursor to the position of each element of list in turn
	// in reverse order
	#define REVERSETRAVERSELIST(list, cursor)        \
	for ( cursor = (list).GetTailPosition()           \
		; cursor!=NULL                               \
		; cursor = (list).Prev(cursor)                \
		)
}; // end of class declaration


template<class OBJECT> 
class CObjectList : public CBaseList
{
public:
	CObjectList(void) : CBaseList(10)
	{};
	virtual ~CObjectList(){};

	NODEPOS	GetHeadPosition() const { return (NODEPOS)m_pFirst; }
	NODEPOS	GetTailPosition() const { return (NODEPOS)m_pLast; }
	int			GetCount() const { return m_Count; }

	OBJECT *	GetNext(NODEPOS& rp) const { return (OBJECT *) GetNextI(rp); }
	OBJECT *	GetPrev(NODEPOS& rp) const { return (OBJECT *) GetPrevI(rp); }

	OBJECT *	Get(NODEPOS p) const { return (OBJECT *) GetI(p); }
	OBJECT *	GetHead() const  { return Get(GetHeadPosition()); }
	OBJECT *	GetTail() const  { return Get(GetTailPosition()); }

	OBJECT *	RemoveHead() { return (OBJECT *) RemoveHeadI(); }

	OBJECT *	RemoveTail() { return (OBJECT *) RemoveTailI(); }

	OBJECT *	Remove(NODEPOS p) { return (OBJECT *) RemoveI(p); }
	bool		Remove(OBJECT * p) { return RemoveObject(p); }

	NODEPOS	AddBefore(NODEPOS p, OBJECT * pObj) { return AddBeforeI(p, pObj); }
	NODEPOS	AddAfter(NODEPOS p, OBJECT * pObj)  { return AddAfterI(p, pObj); }
	NODEPOS	AddHead(OBJECT * pObj) { return AddHeadI(pObj); }
	NODEPOS	AddTail(OBJECT * pObj)  { return AddTailI(pObj); }
	bool		AddTail(CObjectList<OBJECT> *pList)
					{ return CBaseList::AddTail((CBaseList *) pList); }
	bool		AddHead(CObjectList<OBJECT> *pList)
					{ return CBaseList::AddHead((CBaseList *) pList); }
	bool		AddAfter(NODEPOS p, CObjectList<OBJECT> *pList)
					{ return CBaseList::AddAfter(p, (CBaseList *) pList); };
	bool		AddBefore(NODEPOS p, CObjectList<OBJECT> *pList)
					{ return CBaseList::AddBefore(p, (CBaseList *) pList); };
	NODEPOS	Find( OBJECT * pObj) const { return FindI(pObj); }

}; // end of class declaration
    
#endif //__CBaseList_H__

