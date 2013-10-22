/*
 * common_list.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_LIST_H_
#define COMMON_LIST_H_

#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "frame_errordef.h"
#include "common_pool.h"
#include <assert.h>

#define INVAILD_LIST_INDEX (-1)

template<typename T,uint32_t CAPACITY>
class CNodePool
{
public:
	struct TNode
	{
	public:
		TNode():m_nNext(INVAILD_LIST_INDEX), m_nPrev(INVAILD_LIST_INDEX)
		{
		}

		~TNode()
		{

		}

		void Initialize()
		{
			m_object.~T();
			m_nNext = INVAILD_LIST_INDEX;
			m_nPrev = INVAILD_LIST_INDEX;
		}

		void Uninitialize()
		{
			m_object.~T();
			m_nNext = INVAILD_LIST_INDEX;
			m_nPrev = INVAILD_LIST_INDEX;
		}

	public:
		int32_t			m_nNext;
		int32_t			m_nPrev;
		T				m_object;
	};


//	typedef typename CPool<TNode, CAPACITY>		NodePool;
//	typedef	typename NodePool::CIndex			PoolIndex;

	class CIndex{
		//极其猥琐的结构，主要用途在于取出NodePool::CIndex类的m_pContainer指针
		struct TPoolIndexImg:public CObject{
			friend class CIndex;
		protected:
			CPool<TNode, CAPACITY>*		m_pContainer;

			uint32_t		m_nFlag;		//索引状态标志
			int32_t			m_nIndex;		//对象索引号
			int32_t			m_nNext;		//后一节点
			int32_t			m_nPrev;		//前一节点
			uint64_t		m_arrAdditionalData[CPool<TNode, CAPACITY>::enmMaxAdditionalDataSize];	//附加数据

			CComplexType<T>	m_object;		//关联的对象
		};
	public:

		friend class Clist;

		CIndex(const typename CPool<TNode, CAPACITY>::CIndex& poolIndex):m_cPoolIndex(poolIndex)
		{

		}

		int32_t Initialize()
		{
			m_cPoolIndex.Object().Initialize();
			return S_OK;
		}
		//恢复对象索引
		int32_t Resume()
		{
			return S_OK;
		}
		//注销对象索引
		int32_t Uninitialize()
		{
			m_cPoolIndex.Object().Uninitialize();
			return S_OK;
		}

		inline int32_t SetPrev(int32_t nPrev)
		{
			m_cPoolIndex.Object().m_nPrev = nPrev;
			return S_OK;
		}

		inline CIndex* Prev()
		{
			TNode tNode = m_cPoolIndex.Object();
			if(INVAILD_LIST_INDEX == tNode.m_nPrev)
			{
				return NULL;
			}
			TPoolIndexImg* pIndex = (TPoolIndexImg*)&m_cPoolIndex;
			return (CIndex*)pIndex->m_pContainer->GetIndex(m_cPoolIndex.Object().m_nPrev);
		}

		inline int32_t SetNext(int32_t nNext)
		{
			m_cPoolIndex.Object().m_nNext = nNext;
			return S_OK;
		}

		//获取链表的后一节点
		inline CIndex* Next()
		{
			TNode tNode = m_cPoolIndex.Object();
			if(INVAILD_LIST_INDEX == tNode.m_nNext)
			{
				return NULL;
			}
			TPoolIndexImg* pIndex = (TPoolIndexImg*)&m_cPoolIndex;
			return (CIndex*)pIndex->m_pContainer->GetIndex(m_cPoolIndex.Object().m_nNext);
		}

		//获取对象指针
		inline T& Object()
		{
			return m_cPoolIndex.Object().m_object;
		}

		//获取对象指针
		inline T* ObjectPtr()
		{
			return (T*)&(m_cPoolIndex.Object().m_object);
		}

		//获取对象ID
		inline int32_t Index()
		{
			return m_cPoolIndex.Index();
		}

		//获取附加数据
		int32_t GetAdditionalData(int32_t nIndex, uint64_t& nValue)
		{
			return m_cPoolIndex.GetAdditionalData(nIndex, nValue);
		}
		//设置附加数据
		int32_t SetAdditionalData(int32_t nIndex, uint64_t nValue)
		{
			return m_cPoolIndex.SetAdditionalData(nIndex, nValue);
		}
		//获取附加数据
		int32_t GetAdditionalData(int32_t nIndex, uint32_t& nValue)
		{
			return m_cPoolIndex.GetAdditionalData(nIndex, nValue);
		}
		//设置附加数据
		int32_t SetAdditionalData(int32_t nIndex, uint32_t nValue)
		{
			return m_cPoolIndex.SetAdditionalData(nIndex, nValue);
		}
		//清除附加数据
		void ClearAdditionalData()
		{
			m_cPoolIndex.ClearAdditionalData();
		}

	private:

		typename CPool<TNode, CAPACITY>::CIndex	m_cPoolIndex;
	};

public:

	//初始化对象索引
	int32_t Initialize()
	{
		return m_nodePool.Initialize();
	}
	//恢复对象索引
	int32_t Resume()
	{
		return m_nodePool.Resume();;
	}
	//注销对象索引
	int32_t Uninitialize()
	{
		return m_nodePool.Uninitialize();
	}
	CIndex* CreateObject()
	{
		CIndex *pIndex = (CIndex*)m_nodePool.CreateObject();
		if(pIndex)
		{
			pIndex->Initialize();
		}
		return pIndex;
	}

	int32_t DestroyObject(CIndex* pIndex)
	{
		if(pIndex)
		{
			pIndex->Uninitialize();
		}
		return m_nodePool.DestroyObject((typename CPool<TNode, CAPACITY>::CIndex *)pIndex);
	}

	CIndex* GetIndex(int32_t nIndex)
	{
		CIndex *pIndex = (CIndex*)m_nodePool.GetIndex(nIndex);
		return pIndex;
	}

private:
	CPool<TNode, CAPACITY>			m_nodePool;
};


template<typename T, uint32_t CAPACITY>
class CList{
public:
	typedef CNodePool<T, CAPACITY> POOL;

	typedef typename POOL::CIndex CIndex;


	CList(uint32_t unCapacity = CAPACITY, POOL* pPool = NULL):m_nHead( INVAILD_LIST_INDEX ),
			m_nTail( INVAILD_LIST_INDEX ),
			m_unSize( 0 ),
			m_unCapacity( unCapacity ),
			m_cPool( pPool )
	{

	}

	int32_t Initialize(uint32_t unCapacity, POOL* pPool)
	{
		if(!m_cPool && pPool)
		{
			m_nHead		 = INVAILD_LIST_INDEX;
			m_nTail		 = INVAILD_LIST_INDEX;
			m_unSize	 = 0;
			m_unCapacity = unCapacity;
			m_cPool		 = pPool;
			return	S_OK;
		}
		return E_NULLPOINTER;
	}

	bool Initialized()
	{
		return m_cPool? true: false;
	}

	~CList()
	{
		for(; m_unSize ;)
		{
			PopFront();
		}
		assert(INVAILD_LIST_INDEX == m_nHead);

	}

	int32_t Uninitialize()
	{
		for(; m_unSize ;)
		{
			PopFront();
		}
		assert(INVAILD_LIST_INDEX == m_nHead && m_nTail	== INVAILD_LIST_INDEX && !m_unSize);
		m_cPool		 = NULL;
		return S_OK;
	}

	void Clear()
	{
		for(; m_unSize ;)
		{
			PopFront();
		}
		assert(INVAILD_LIST_INDEX == m_nHead && m_nTail	== INVAILD_LIST_INDEX && !m_unSize);
	}

	int32_t SetCapacity(uint32_t unCapacity)
	{
		m_unCapacity = unCapacity;
		return S_OK;
	}

	uint32_t Capacity()
	{
		return m_unCapacity;
	}



	CIndex* CreateObject()
	{
		if(m_unSize >= m_unCapacity || !m_cPool)
		{
			return NULL;
		}
		CIndex *pIndex = m_cPool->CreateObject();
		if(pIndex)
		{
			if(INVAILD_LIST_INDEX == m_nHead)
			{
				m_nHead = pIndex->Index();
				m_nTail = pIndex->Index();
			}
			else
			{
				CIndex* pLast = Last();
				assert(pLast);
				pLast->SetNext(pIndex->Index());
				pIndex->SetPrev(pLast->Index());
				pIndex->SetNext(INVAILD_LIST_INDEX);
				m_nTail = pIndex->Index();
			}
			++m_unSize;
			return pIndex;
		}
		return NULL;
	}

	int32_t DestroyObject(CIndex* pIndex)
	{
		if( !m_cPool)
		{
			return E_NULLPOINTER;
		}
		assert(pIndex);
		int32_t nPos = Find(pIndex);
		if(INVAILD_LIST_INDEX != nPos)
		{
			CIndex* pPrev = pIndex->Prev();
			CIndex* pNext = pIndex->Next();

			if(pPrev)
			{
				if(pNext)
				{
					pPrev->SetNext(pNext->Index());
				}
				else
				{
					m_nTail = pPrev->Index();
					pPrev->SetNext(INVAILD_LIST_INDEX);
				}
			}

			if(pNext)
			{
				if(pPrev)
				{
					pNext->SetPrev(pPrev->Index());
				}
				else
				{
					m_nHead = pNext->Index();
					pNext->SetPrev(INVAILD_LIST_INDEX);
				}
			}
			m_cPool->DestroyObject((CIndex*)pIndex);
			--m_unSize;
			if(!m_unSize)
			{
				m_nHead = INVAILD_LIST_INDEX;
				m_nTail = INVAILD_LIST_INDEX;
			}
			return S_OK;
		}
		return E_UNKNOWN;
	}

	//获取头部对象索引
	inline CIndex* First()
	{
		return (INVAILD_LIST_INDEX == m_nHead) ? NULL : (CIndex*)m_cPool->GetIndex(m_nHead);
	}

	//获取尾部对象索引
	inline CIndex* Last()
	{
		return (INVAILD_LIST_INDEX == m_nTail) ? NULL : (CIndex*)m_cPool->GetIndex(m_nTail);
	}

	int32_t	PushBack(const T& data)
	{
		if(m_unSize >= m_unCapacity || !m_cPool)
		{
			return E_UNKNOWN;
		}
		CIndex *pIndex = m_cPool->CreateObject();
		if(pIndex)
		{
			if(INVAILD_LIST_INDEX == m_nHead)
			{
				m_nHead = pIndex->Index();
				m_nTail = pIndex->Index();
				pIndex->SetPrev(INVAILD_LIST_INDEX);
				pIndex->SetNext(INVAILD_LIST_INDEX);
			}
			else
			{
				CIndex* pLast = Last();
				assert(pLast);
				pLast->SetNext(pIndex->Index());
				pIndex->SetPrev(pLast->Index());
				pIndex->SetNext(INVAILD_LIST_INDEX);
				m_nTail = pIndex->Index();
			}
			pIndex->Object() = data;
			++m_unSize;
			return S_OK;
		}
		return E_UNKNOWN;
	}

	int32_t PopBack()
	{
		if(m_unSize)
		{
			CIndex* pLast = Last();
			assert(pLast);
			CIndex* pPrev = pLast->Prev();
			if(pPrev)
			{
				pPrev->SetNext(INVAILD_LIST_INDEX);
				m_nTail = pPrev->Index();
			}
			m_cPool->DestroyObject((CIndex*)pLast);
			--m_unSize;
			if(!m_unSize)
			{
				m_nTail = INVAILD_LIST_INDEX;
				m_nHead = INVAILD_LIST_INDEX;
			}

		}
		return S_OK;
	}

	int32_t PushFront(const T& data)
	{
		if(m_unSize >= m_unCapacity || !m_cPool)
		{
			return E_UNKNOWN;
		}
		CIndex *pIndex = m_cPool->CreateObject();
		if(pIndex)
		{
			if(INVAILD_LIST_INDEX == m_nHead)
			{
				m_nHead = pIndex->Index();
				m_nTail = pIndex->Index();
				pIndex->SetPrev(INVAILD_LIST_INDEX);
				pIndex->SetNext(INVAILD_LIST_INDEX);
			}
			else
			{
				CIndex* pFirst = First();
				assert(pFirst);
				pFirst->SetPrev(pIndex->Index());
				pIndex->SetNext(pFirst->Index());
				pIndex->SetPrev(INVAILD_LIST_INDEX);
				m_nHead = pIndex->Index();
			}
			pIndex->Object() = data;
			++m_unSize;
			return S_OK;
		}
		return E_UNKNOWN;
	}

	int32_t PopFront()
	{
		if(m_unSize)
		{
			CIndex* pFirst = First();
			assert(pFirst);
			CIndex* pNext = pFirst->Next();
			if(pNext)
			{
				pNext->SetPrev(INVAILD_LIST_INDEX);
				m_nHead = pNext->Index();
			}
			m_cPool->DestroyObject((CIndex*)pFirst);
			--m_unSize;
			if(!m_unSize)
			{
				m_nTail = INVAILD_LIST_INDEX;
				m_nHead = INVAILD_LIST_INDEX;
			}

		}
		return S_OK;
	}

	//链表是否为空
	inline bool IsFull() const
	{
		return (m_unCapacity == m_unSize);
	}

	//链表是否为空
	inline bool IsEmpty() const
	{
		return (0 == m_unSize);
	}

	uint32_t ObjectCount() const
	{
		return m_unSize;
	}

	uint32_t Size() const
	{
		return m_unSize;
	}


private:
	int32_t		Find(CIndex* pIndex)
	{
		CIndex* pPos = First();
		for(; pPos; )
		{
			if(pPos->Index() == pIndex->Index())
			{
				return pIndex->Index();
			}
			pPos = pPos->Next();
		}
		return INVAILD_LIST_INDEX;
	}

	int32_t		m_nHead;		//头对象索引值
	int32_t		m_nTail;		//尾对象索引值
	uint32_t	m_unSize;		//总共的对象数目
	uint32_t	m_unCapacity;	//最大容量
	POOL*		m_cPool;		//对象池
};

typedef	int32_t			NodeIndex;
enum
{
	enmInvalidNodeIndex					= -1,
};

template<typename T, typename CLessOp, uint32_t CAPACITY>
class CSList
{
public:
	struct ListNode
	{
		NodeIndex					nPrev;
		NodeIndex					nNext;
		T							data;
	};
public:
	CSList():
	  m_nNodeCount(CAPACITY + 2)	//冗余头结点的前一个结点和尾结点
	{

	}
	virtual ~CSList()
	{

	}

	int32_t Initialize()
	{
		m_nUsed = 0;
		m_nHead = 1;
		m_nTail = 1;
		for (uint32_t i = 0; i < m_nNodeCount; ++i)
		{
			m_arrNode[i].nPrev = i - 1;
			m_arrNode[i].nNext = i + 1;
		}
		m_arrNode[0].nPrev = enmInvalidNodeIndex;
		m_arrNode[m_nNodeCount - 1].nNext = enmInvalidNodeIndex;
		return S_OK;
	}

	int32_t Uninitialize()	{	return S_OK;	}

	void Clear()	{	Initialize();	}

	bool IsEmputy()	{	return m_nHead == m_nTail;	}

	bool IsFull()	{	return enmInvalidNodeIndex == m_arrNode[m_nTail].nNext;	}

	bool IsExist(T& data){	return !(enmInvalidNodeIndex == GetIndex(data));	}

	int32_t Size()	{ return m_nUsed;	}
public:

	int32_t Get(const NodeIndex nodeIndex, T& data)
	{
		if (CheckIndex(nodeIndex))
		{
			return enmInvalidNodeIndex;
		}
		data = m_arrNode[nodeIndex].data;
		return S_OK;
	}

	int32_t PushBack(const T& data)
	{
		if (IsFull())
		{
			return E_LIBF_CONTAINERFULL;
		}
		m_arrNode[m_nTail].data = data;
		m_nTail = m_arrNode[m_nTail].nNext;
		++m_nUsed;
		return S_OK;
	}

	int32_t Remove(T& data)
	{
		NodeIndex nIndex = GetIndex(data);
		if (enmInvalidNodeIndex == nIndex)
		{
			return E_OBJECTNOTEXIST;
		}

		--m_nUsed;
		//删除头结点
		if (m_nHead == nIndex)
		{
			m_nHead = m_arrNode[m_nHead].nNext;
		}

		//删除尾结点
		if (m_arrNode[m_nTail].nPrev == nIndex)
		{
			m_nTail = nIndex;
			return S_OK;
		}

		const NodeIndex nPrevIndex = m_arrNode[nIndex].nPrev;
		const NodeIndex nNextIndex = m_arrNode[nIndex].nNext;
		m_arrNode[nPrevIndex].nNext = nNextIndex;
		m_arrNode[nNextIndex].nPrev = nPrevIndex;

		m_arrNode[nIndex].nPrev = m_nTail;
		m_arrNode[nIndex].nNext = m_arrNode[m_nTail].nNext;

		m_arrNode[m_nTail].nNext = nIndex;
		return S_OK;
	}

public:
	NodeIndex GetIndex(T& data)
	{
		NodeIndex nIndex = m_nHead;
		while(nIndex != m_nTail)
		{
			if (Equal(data, m_arrNode[nIndex].data))
			{
				return nIndex;
			}
			nIndex = m_arrNode[nIndex].nNext;
		}
		return enmInvalidNodeIndex;
	}

	NodeIndex GetFirstIndex()
	{
		return IsEmputy() ? enmInvalidNodeIndex : m_nHead;
	}

	NodeIndex GetPrexIndex(const NodeIndex nIndex)
	{
		if (!CheckIndex(nIndex))
		{
			return enmInvalidNodeIndex;
		}
		return m_arrNode[nIndex].nPrev;
	}

	NodeIndex GetNextIndex(const NodeIndex nIndex)
	{
		if (!CheckIndex(nIndex))
		{
			return enmInvalidNodeIndex;
		}
		return m_arrNode[nIndex].nNext;
	}

protected:
	bool CheckIndex(const NodeIndex nodeIndex)	{	return (nodeIndex >= 0) && (nodeIndex < (int32_t)m_nNodeCount);	}

	bool Less(T& data1, T& data2)	{	return m_lessOp(data1, data2);	}
	bool Great(T& data1, T& data2)	{	return m_lessOp(data2, data1);	}
	bool Equal(T& data1, T& data2)	{	return !(m_lessOp(data1, data2) || m_lessOp(data2, data2));	}

protected:

	const uint32_t			m_nNodeCount;
	int32_t					m_nUsed;
	NodeIndex				m_nHead;
	NodeIndex				m_nTail;
	ListNode				m_arrNode[CAPACITY + 2]; //冗余头结点的前一个结点和尾结点

	CLessOp					m_lessOp;
};

#endif /* COMMON_LIST_H_ */
