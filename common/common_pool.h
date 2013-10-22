/*
 * common_pool.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_POOL_H_
#define COMMON_POOL_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_complextype.h"
#include "common_api.h"


//声明一个对象池管理模板类
template<typename T, uint32_t CAPACITY, bool RECYCLETOTAIL = true>
class CPool : public CObject
{
public:
	enum
	{
		enmMaxAdditionalDataSize		= 4,		//最大附加字段长度
		enmIndexFlag_Free				= 0,		//对象索引空闲状态
		enmIndexFlag_Used				= 1,		//对象索引已使用状态
		enmInvalidObjectID				= -1,		//无效的对象ID
	};

public:
	//索引对象
	class CIndex : public CObject
	{
	public:
		CIndex()
		{
		}
		virtual ~CIndex()
		{
		}

	public:
		//初始化对象索引
		virtual int32_t Initialize()
		{
			m_pContainer = NULL;
			m_nFlag = enmIndexFlag_Free;
			m_nIndex = enmInvalidObjectID;
			//m_pPrev = NULL;
			//m_pNext = NULL;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;

			ClearAdditionalData();

			//m_object.Initialize();

			return S_OK;
		}
		//恢复对象索引
		virtual int32_t Resume()
		{
			//m_object.Resume();

			return S_OK;
		}
		//注销对象索引
		virtual int32_t Uninitialize()
		{
			m_nFlag = enmIndexFlag_Free;
			m_nIndex = enmInvalidObjectID;
			//m_pPrev = NULL;
			//m_pNext = NULL;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;

			ClearAdditionalData();

			//m_object.Uninitialize();

			return S_OK;
		}

	public:
		////获取链表的前一节点
		//inline CIndex* Prev() const
		//{
		//	return m_pPrev;
		//}
		////获取链表的后一节点
		//inline CIndex* Next() const
		//{
		//	return m_pNext;
		//}
		//获取链表的前一节点
		inline CIndex* Prev()
		{
			if (enmInvalidObjectID == m_nPrev)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nPrev];
		}
		//获取链表的后一节点
		inline CIndex* Next()
		{
			if (enmInvalidObjectID == m_nNext)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nNext];
		}

		//获取对象指针
		inline T& Object()
		{
			return m_object.GetValue();
		}

		//获取对象指针
		inline T* ObjectPtr()
		{
			return (T*)&m_object.GetValue();
		}

		//获取对象ID
		inline int32_t Index()
		{
			return m_nIndex;
		}

	protected:
		//设置父对象指针
		inline void SetContainer(CPool* pContainer)
		{
			m_pContainer = pContainer;
		}
		//设置节点为已使用
		inline void SetUsed()
		{
			m_nFlag = enmIndexFlag_Used;
		}
		//设置节点为空闲
		inline void SetFree()
		{
			m_nFlag = enmIndexFlag_Free;
		}
		//节点是否已使用
		inline bool IsUsed() const
		{
			return (enmIndexFlag_Used == m_nFlag);
		}

		////获取链表的后一节点
		//inline CIndex* GetNext() const
		//{
		//	return m_pNext;
		//}
		////获取链表的前一节点
		//inline CIndex* GetPrev() const
		//{
		//	return m_pPrev;
		//}
		////设置链表的后一节点
		//inline void SetNext(CIndex* pNext)
		//{
		//	m_pNext = pNext;
		//}
		////设置链表的前一节点
		//inline void SetPrev(CIndex* pPrev)
		//{
		//	m_pPrev = pPrev;
		//}
		//获取链表的后一节点
		inline int32_t GetNext() const
		{
			return m_nNext;
		}
		//获取链表的前一节点
		inline int32_t GetPrev() const
		{
			return m_nPrev;
		}
		//设置链表的后一节点
		inline void SetNext(int32_t nNext)
		{
			m_nNext = nNext;
		}
		//设置链表的前一节点
		inline void SetPrev(int32_t nPrev)
		{
			m_nPrev = nPrev;
		}

		//设置对象ID
		inline void SetIndex(int32_t nIndex)
		{
			m_nIndex = nIndex;
		}

	public:
		//获取附加数据
		int32_t GetAdditionalData(int32_t nIndex, uint64_t& nValue)
		{
			if (nIndex < 0 || nIndex >= enmMaxAdditionalDataSize)
			{
				return E_UNKNOWN;
			}

			nValue = m_arrAdditionalData[nIndex];

			return S_OK;
		}
		//设置附加数据
		int32_t SetAdditionalData(int32_t nIndex, uint64_t nValue)
		{
			if (nIndex < 0 || nIndex >= enmMaxAdditionalDataSize)
			{
				return E_UNKNOWN;
			}

			m_arrAdditionalData[nIndex] = nValue;

			return S_OK;
		}
		//获取附加数据
		int32_t GetAdditionalData(int32_t nIndex, uint32_t& nValue)
		{
			if (nIndex < 0 || nIndex >= enmMaxAdditionalDataSize)
			{
				return E_UNKNOWN;
			}

			nValue = (uint32_t)m_arrAdditionalData[nIndex];

			return S_OK;
		}
		//设置附加数据
		int32_t SetAdditionalData(int32_t nIndex, uint32_t nValue)
		{
			if (nIndex < 0 || nIndex >= enmMaxAdditionalDataSize)
			{
				return E_UNKNOWN;
			}

			m_arrAdditionalData[nIndex] = (uint64_t)nValue;

			return S_OK;
		}
		//清除附加数据
		void ClearAdditionalData()
		{
			memset(m_arrAdditionalData, 0, sizeof(m_arrAdditionalData));
		}


	//protected:
	public:
		CPool*			m_pContainer;

		uint32_t		m_nFlag;		//索引状态标志
		int32_t			m_nIndex;		//对象索引号
		//CIndex*			m_pNext;		//后一节点
		//CIndex*			m_pPrev;		//前一节点
		int32_t			m_nNext;		//后一节点
		int32_t			m_nPrev;		//前一节点
		uint64_t		m_arrAdditionalData[enmMaxAdditionalDataSize];	//附加数据

		CComplexType<T>	m_object;		//关联的对象

		friend class CPool;
	};


public:
	CPool()
	{
	}
	virtual ~CPool()
	{
	}

public:
	//初始化对象池
	virtual int32_t Initialize()
	{
		m_nObjectCount = 0;
		m_nFreeCount = CAPACITY;
		//m_pFreeHead = &m_arrIndex[0];
		//m_pFreeTail = &m_arrIndex[CAPACITY - 1];
		//m_pUsedHead = NULL;
		//m_pUsedTail = NULL;
		m_nFreeHead = 0;
		m_nFreeTail = CAPACITY - 1;
		m_nUsedHead = enmInvalidObjectID;
		m_nUsedTail = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Initialize();
		}

		BuildIndexList();

		return S_OK;
	}
	//恢复对象池
	virtual int32_t Resume()
	{
		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Resume();
		}
		return S_OK;
	}
	//注销对象池
	virtual int32_t Uninitialize()
	{
		m_nObjectCount = 0;
		m_nFreeCount = CAPACITY;
		//m_pFreeHead = &m_arrIndex[0];
		//m_pFreeTail = &m_arrIndex[CAPACITY - 1];
		//m_pUsedHead = NULL;
		//m_pUsedTail = NULL;
		m_nFreeHead = 0;
		m_nFreeTail = CAPACITY - 1;
		m_nUsedHead = enmInvalidObjectID;
		m_nUsedTail = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Uninitialize();
		}

		BuildIndexList();

		return S_OK;
	}

public:
	//创建对象
	CIndex* CreateObject()
	{
		return CreateIndex();
	}
	//销毁对象
	int32_t DestroyObject(CIndex* pIndex)
	{
		return DestroyIndex(pIndex);
	}
	//销毁对象
	int32_t DestroyObject(int32_t nIndex)
	{
		CIndex* pIndex = GetIndex(nIndex);
		if (NULL == pIndex)
		{
			return E_INVALIDARGUMENT;
		}
		return DestroyIndex(pIndex);
	}
	//清除所有对象
	void Clear()
	{
		Uninitialize();
	}

	////获取头部对象索引
	//CIndex* First() const
	//{
	//	return m_pUsedHead;
	//}
	////获取尾部对象索引
	//CIndex* Last() const
	//{
	//	return m_pUsedTail;
	//}
	//获取头部对象索引
	inline CIndex* First()
	{
		return (enmInvalidObjectID == m_nUsedHead) ? NULL : &m_arrIndex[m_nUsedHead];
	}
	//获取尾部对象索引
	inline CIndex* Last()
	{
		return (enmInvalidObjectID == m_nUsedTail) ? NULL : &m_arrIndex[m_nUsedTail];
	}

	//获取对象池容量
	inline uint32_t Capacity() const
	{
		return CAPACITY;
	}
	//获取对象池中对象数量
	inline uint32_t ObjectCount() const
	{
		return m_nObjectCount;
	}

	//对象池是否为空
	inline bool IsEmpty() const
	{
		return (0 == m_nObjectCount);
	}
	//对象池是否已满
	inline bool IsFull() const
	{
		return (m_nObjectCount == CAPACITY);
	}

	//根据位置获取索引
	inline CIndex* GetIndex(const int32_t nIndex)
	{
		int32_t ret = CheckIndex(nIndex);
		if (0 > ret)
		{
			return NULL;
		}
		return &m_arrIndex[nIndex];
	}
	//根据位置获取索引
	inline int32_t GetIndex(const CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return enmInvalidObjectID;
		}
		return ((uint8_t*)pIndex - (uint8_t*)m_arrIndex) / sizeof(CIndex);
	}
	//随机获取索引
	inline CIndex* GetRandomIndex()
	{
		if (0 == m_nObjectCount)
		{
			return NULL;
		}

		//srand((unsigned int)time(NULL)); 程序启动初始化一次就可以了
		uint32_t nIndex = (rand() % m_nObjectCount);
		CIndex* pIndex = First();
		while ((nIndex > 0) && (NULL != pIndex))
		{
			pIndex = pIndex->Next();
			--nIndex;
		}

		return pIndex;
	}

protected:
	////对索引区建立链表
	//int32_t BuildIndexList()
	//{
	//	//设置第0个索引
	//	m_arrIndex[0].SetIndex(0);
	//	m_arrIndex[0].SetPrev(NULL);

	//	for (uint32_t i = 1; i < CAPACITY; ++i)
	//	{
	//		m_arrIndex[i].SetIndex(i);
	//		m_arrIndex[i - 1].SetNext(&m_arrIndex[i]);
	//		m_arrIndex[i].SetPrev(&m_arrIndex[i - 1]);
	//	}

	//	//设置最后一个索引
	//	m_arrIndex[CAPACITY - 1].SetNext(NULL);

	//	return S_OK;
	//}
	//对索引区建立链表
	inline int32_t BuildIndexList()
	{
		//设置第0个索引
		m_arrIndex[0].SetContainer(this);
		m_arrIndex[0].SetIndex(0);
		m_arrIndex[0].SetPrev(enmInvalidObjectID);

		for (uint32_t i = 1; i < CAPACITY; ++i)
		{
			m_arrIndex[i].SetContainer(this);
			m_arrIndex[i].SetIndex(i);
			m_arrIndex[i].SetPrev(i - 1);
			m_arrIndex[i - 1].SetNext(i);
		}

		//设置最后一个索引
		m_arrIndex[CAPACITY - 1].SetNext(enmInvalidObjectID);

		return S_OK;
	}

	////创建对象
	//CIndex* CreateIndex()
	//{
	//	//对象池已满
	//	if (IsFull())
	//	{
	//		return NULL;
	//	}

	//	//从空闲链表头部取出一个空索引
	//	CIndex* pIndex = m_pFreeHead;
	//	if (NULL == pIndex)
	//	{
	//		return NULL;
	//	}

	//	//调整空闲索引链表的头部和尾部指针
	//	if (m_pFreeTail == pIndex)
	//	{
	//		m_pFreeTail = pIndex->GetNext();
	//	}
	//	m_pFreeHead = pIndex->GetNext();
	//	if (NULL != m_pFreeHead)
	//	{
	//		m_pFreeHead->SetPrev(NULL);
	//	}

	//	//将新索引添加到已使用索引链表尾部
	//	pIndex->SetPrev(m_pUsedTail);
	//	pIndex->SetNext(NULL);
	//	if (NULL != m_pUsedTail)
	//	{
	//		m_pUsedTail->SetNext(pIndex);
	//	}
	//	m_pUsedTail = pIndex;
	//	if (NULL == m_pUsedHead)
	//	{
	//		m_pUsedHead = m_pUsedTail;
	//	}

	//	pIndex->SetUsed();

	//	++m_nObjectCount;
	//	--m_nFreeCount;

	//	pIndex->m_object.Initialize();

	//	return pIndex;
	//}
	////销毁对象
	//int32_t DestroyIndex(CIndex* pIndex)
	//{
	//	int32_t ret = CheckIndex(pIndex);
	//	if (ret < 0)
	//	{
	//		return ret;
	//	}

	//	//调整已使用索引链表的头部和尾部指针
	//	if (m_pUsedHead == pIndex)
	//	{
	//		m_pUsedHead = pIndex->GetNext();
	//	}
	//	if (m_pUsedTail == pIndex)
	//	{
	//		m_pUsedTail = pIndex->GetPrev();
	//	}

	//	//将节点从链表中删除
	//	if (NULL != pIndex->GetPrev())
	//	{
	//		pIndex->GetPrev()->SetNext(pIndex->GetNext());
	//	}
	//	if (NULL != pIndex->GetNext())
	//	{
	//		pIndex->GetNext()->SetPrev(pIndex->GetPrev());
	//	}

	//	//将索引添加到空闲索引链表尾部
	//	pIndex->SetPrev(m_pFreeTail);
	//	pIndex->SetNext(NULL);
	//	if (NULL != m_pFreeTail)
	//	{
	//		m_pFreeTail->SetNext(pIndex);
	//	}
	//	m_pFreeTail = pIndex;
	//	if (NULL == m_pFreeHead)
	//	{
	//		m_pFreeHead = m_pFreeTail;
	//	}

	//	pIndex->SetFree();

	//	--m_nObjectCount;
	//	++m_nFreeCount;

	//	pIndex->m_object.Uninitialize();
	//
	//	return S_OK;
	//}
	//创建对象
	inline CIndex* CreateIndex()
	{
		//对象池已满
		if (IsFull())
		{
			return NULL;
		}

		//从空闲链表头部取出一个空索引
		if (enmInvalidObjectID == m_nFreeHead)
		{
			return NULL;
		}
		CIndex* pIndex = &m_arrIndex[m_nFreeHead];

		//调整空闲索引链表的头部和尾部指针
		if (m_nFreeTail == pIndex->Index())
		{
			m_nFreeTail = pIndex->GetNext();
		}
		m_nFreeHead = pIndex->GetNext();
		if (enmInvalidObjectID != m_nFreeHead)
		{
			m_arrIndex[m_nFreeHead].SetPrev(enmInvalidObjectID);
		}

		//将新索引添加到已使用索引链表尾部
		pIndex->SetPrev(m_nUsedTail);
		pIndex->SetNext(enmInvalidObjectID);
		if (enmInvalidObjectID != m_nUsedTail)
		{
			m_arrIndex[m_nUsedTail].SetNext(pIndex->Index());
		}
		m_nUsedTail = pIndex->Index();
		if (enmInvalidObjectID == m_nUsedHead)
		{
			m_nUsedHead = m_nUsedTail;
		}

		pIndex->SetUsed();

		++m_nObjectCount;
		--m_nFreeCount;

		pIndex->m_object.Initialize();

		return pIndex;
	}
	//销毁对象
	inline int32_t DestroyIndex(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (ret < 0)
		{
			return ret;
		}

		//调整已使用索引链表的头部和尾部指针
		if (m_nUsedHead == pIndex->Index())
		{
			m_nUsedHead = pIndex->GetNext();
		}
		if (m_nUsedTail == pIndex->Index())
		{
			m_nUsedTail = pIndex->GetPrev();
		}

		//将节点从链表中删除
		if (enmInvalidObjectID != pIndex->GetPrev())
		{
			pIndex->Prev()->SetNext(pIndex->GetNext());
		}
		if (enmInvalidObjectID != pIndex->GetNext())
		{
			pIndex->Next()->SetPrev(pIndex->GetPrev());
		}

		if (RECYCLETOTAIL)
		{
			//将索引添加到空闲索引链表尾部
			pIndex->SetPrev(m_nFreeTail);
			pIndex->SetNext(enmInvalidObjectID);
			if (enmInvalidObjectID != m_nFreeTail)
			{
				m_arrIndex[m_nFreeTail].SetNext(pIndex->Index());
			}
			m_nFreeTail = pIndex->Index();
			if (enmInvalidObjectID == m_nFreeHead)
			{
				m_nFreeHead = m_nFreeTail;
			}
		}
		else
		{
			//将索引添加到空闲索引链表头部
			pIndex->SetNext(m_nFreeHead);
			pIndex->SetPrev(enmInvalidObjectID);
			if (enmInvalidObjectID != m_nFreeHead)
			{
				m_arrIndex[m_nFreeHead].SetPrev(pIndex->Index());
			}
			m_nFreeHead = pIndex->Index();
			if (enmInvalidObjectID == m_nFreeTail)
			{
				m_nFreeTail = m_nFreeHead;
			}
		}

		pIndex->SetFree();

		--m_nObjectCount;
		++m_nFreeCount;

		pIndex->m_object.Uninitialize();

		return S_OK;
	}

	//确定对象索引指针是否有效
	inline int32_t CheckIndex(CIndex* pIndex)
	{
		if ((pIndex < m_arrIndex) || (pIndex >= m_arrIndex + CAPACITY))
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (((uint8_t*)pIndex - (uint8_t*)m_arrIndex) % sizeof(CIndex) != 0)
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (!pIndex->IsUsed())
		{
			return E_OBJECTINDEXNOTUSED;
		}
		return S_OK;
	}
	//确定对象索引是否有效
	inline int32_t CheckIndex(int32_t nIndex)
	{
		if ((0 > nIndex) || ((int32_t)CAPACITY < nIndex))
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (!m_arrIndex[nIndex].IsUsed())
		{
			return E_OBJECTINDEXNOTUSED;
		}
		return S_OK;
	}

//protected:
	public:
	uint32_t		m_nObjectCount;		//已分配对象的个数
	uint32_t		m_nFreeCount;		//未分配对象的个数
	//CIndex*			m_pFreeHead;		//未分配对象链头部对象的索引
	//CIndex*			m_pFreeTail;		//未分配对象链尾部对象的索引
	//CIndex*			m_pUsedHead;		//已分配对象链头部对象的索引
	//CIndex*			m_pUsedTail;		//已分配对象链尾部对象的索引
	int32_t			m_nFreeHead;		//未分配对象链头部对象的索引
	int32_t			m_nFreeTail;		//未分配对象链尾部对象的索引
	int32_t			m_nUsedHead;		//已分配对象链头部对象的索引
	int32_t			m_nUsedTail;		//已分配对象链尾部对象的索引

	CIndex			m_arrIndex[CAPACITY];	//对象索引区
};


#endif /* COMMON_POOL_H_ */
