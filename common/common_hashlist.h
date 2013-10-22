/*
 * common_hashlist.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_HASHLIST_H_
#define COMMON_HASHLIST_H_

#include <stdio.h>
#include <memory.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"


//声明一个哈希列表模板类
template<typename T, uint32_t CAPACITY, uint32_t BUCKETSIZE, bool RECYCLETOTAIL = true>
class CHashList : public CObject
{
public:
	class CIndex;

	enum
	{
		enmMaxKeyLength					= 8,		//最大键长度
		enmIndexFlag_Free				= 0,		//对象索引空闲状态
		enmIndexFlag_Used				= 1,		//对象索引已使用状态
		enmInvalidObjectID				= -1,		//无效的对象ID
	};

	//键定义
	typedef struct tagKey
	{
		uint32_t		nKeyLength;					//键长度
		uint8_t			arrKey[enmMaxKeyLength];	//键值
	} Key;

	//冲突链节点定义
	typedef struct tagConflictData
	{
		Key				key;						//键值
		CComplexType<T>	object;
		//CIndex*			pNextConflict;				//冲突链的下一个节点
		int32_t			nNextConflict;				//冲突链的下一个节点
	} ConflictData;

	//桶元素结构
	typedef struct tagBucket
	{
		//CIndex*			pHeadConflict;				//冲突链的首节点
		int32_t			nHeadConflict;				//冲突链的首节点
	} Bucket;


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
			//m_pPrev = NULL;
			//m_pNext = NULL;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;

			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			//m_conflictData.pNextConflict = NULL;
			m_conflictData.nNextConflict = enmInvalidObjectID;

			//m_conflictData.object.Initialize();

			return S_OK;
		}
		//恢复对象索引
		virtual int32_t Resume()
		{
			//m_conflictData.object.Resume();

			return S_OK;
		}
		//注销对象索引
		virtual int32_t Uninitialize()
		{
			m_nFlag = enmIndexFlag_Free;
			//m_pPrev = NULL;
			//m_pNext = NULL;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;

			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			//m_conflictData.pNextConflict = NULL;
			m_conflictData.nNextConflict = enmInvalidObjectID;

			//m_conflictData.object.Uninitialize();

			return S_OK;
		}

	public:
		////获取下一个冲突节点
		//inline CIndex* NextConflict() const
		//{
		//	return m_conflictData.pNextConflict;
		//}
		//获取下一个冲突节点
		inline CIndex* NextConflict() const
		{
			if (enmInvalidObjectID == m_conflictData.nNextConflict)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_conflictData.nNextConflict];
		}

		//获取对象键值
		const Key& GetKey() const
		{
			return m_conflictData.key;
		}

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

		//获取对象
		inline T& GetObject()
		{
			return m_conflictData.object.GetValue();
		}

		//获取对象2(避免错误error C2039: 'GetObjectW' : is not a member of 'CHashList<T,CAPACITY,BUCKETSIZE>::CIndex')
		inline T& Object()
		{
			return m_conflictData.object.GetValue();
		}

		//设置对象
		inline void SetObject(T& object)
		{
			m_conflictData.object.SetValue(object);
		}

		//获取对象ID
		inline int32_t Index()
		{
			return m_nIndex;
		}

	protected:
		//设置父对象指针
		inline void SetContainer(CHashList* pContainer)
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

		//设置节点键值
		inline void SetKey(Key key)
		{
			memcpy(&m_conflictData.key, &key, sizeof(Key));
		}

		////获取下一个冲突节点
		//inline CIndex* GetNextConflict()
		//{
		//	return m_conflictData.pNextConflict;
		//}
		////设置下一个冲突节点
		//inline void SetNextConflict(CIndex* pNextConflict)
		//{
		//	m_conflictData.pNextConflict = pNextConflict;
		//}
		//获取下一个冲突节点
		inline int32_t GetNextConflict()
		{
			return m_conflictData.nNextConflict;
		}
		//设置下一个冲突节点
		inline void SetNextConflict(int32_t nNextConflict)
		{
			m_conflictData.nNextConflict = nNextConflict;
		}

		//重置冲突链节点数据
		inline void ResetConflictData()
		{
			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			//m_conflictData.pNextConflict = NULL;
			m_conflictData.nNextConflict = enmInvalidObjectID;
		}

	protected:
		CHashList*		m_pContainer;

		uint32_t		m_nFlag;		//索引状态标志
		int32_t			m_nIndex;		//对象索引号
		//CIndex*			m_pNext;		//后一节点
		//CIndex*			m_pPrev;		//前一节点
		int32_t			m_nNext;		//后一节点
		int32_t			m_nPrev;		//前一节点

		ConflictData	m_conflictData;	//关联的冲突链元素

		friend class CHashList;
	};


public:
	CHashList()
	{
	}
	virtual ~CHashList()
	{
	}

public:
	//初始化哈希表
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

		for (uint32_t i = 0; i < BUCKETSIZE; ++i)
		{
			//m_arrBucket[i].pHeadConflict = NULL;
			m_arrBucket[i].nHeadConflict = enmInvalidObjectID;
		}

		BuildIndexList();

		return S_OK;
	}
	//恢复哈希表
	virtual int32_t Resume()
	{
		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Resume();
		}
		return S_OK;
	}
	//注销哈希表
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

		for (uint32_t i = 0; i < BUCKETSIZE; ++i)
		{
			//m_arrBucket[i].pHeadConflict = NULL;
			m_arrBucket[i].nHeadConflict = enmInvalidObjectID;
		}

		BuildIndexList();

		return S_OK;
	}

public:
	////向哈希表中插入对象
	//CIndex* Insert(Key key, T object)
	//{
	//	CIndex* pIndex = NULL;

	//	//检查键值是否冲突
	//	pIndex = Find(key);
	//	if (NULL != pIndex)
	//	{
	//		return NULL;
	//	}

	//	//创建索引
	//	pIndex = CreateIndex();
	//	if (NULL == pIndex)
	//	{
	//		return NULL;
	//	}

	//	//计算哈希值
	//	uint32_t nHashCode = GetHashCode(key);
	//	uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

	//	if (NULL == m_arrBucket[nBucketIndex].pHeadConflict)
	//	{
	//		//将新节点加入到冲突链头部
	//		m_arrBucket[nBucketIndex].pHeadConflict = pIndex;
	//	}
	//	else
	//	{
	//		CIndex* pTailConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//		CIndex* pNextConflict = pTailConflict->GetNextConflict();
	//		//找到冲突链的尾部节点
	//		while (NULL != pNextConflict)
	//		{
	//			pTailConflict = pNextConflict;
	//			pNextConflict = pTailConflict->GetNextConflict();
	//		}
	//		//将新节点添加到尾部
	//		pTailConflict->SetNextConflict(pIndex);
	//	}

	//	//设置新节点冲突数据
	//	pIndex->SetKey(key);
	//	pIndex->SetNextConflict(NULL);
	//	pIndex->SetObject(object);

	//	return pIndex;
	//}
	////从哈希表中删除对象
	//int32_t Erase(Key key)
	//{
	//	//计算哈希值
	//	uint32_t nHashCode = GetHashCode(key);
	//	uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

	//	CIndex* pPrevConflict = NULL;
	//	CIndex* pConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//	if (NULL == pConflict)
	//	{
	//		return E_OBJECTNOTEXIST;
	//	}
	//	//查找与指定键值相等的所有节点
	//	while (NULL != pConflict)
	//	{
	//		if (IsEqual(key, pConflict->GetKey()))
	//		{
	//			if (NULL == pPrevConflict)
	//			{
	//				m_arrBucket[nBucketIndex].pHeadConflict = pConflict->GetNextConflict();
	//			}
	//			else
	//			{
	//				pPrevConflict->SetNextConflict(pConflict->GetNextConflict());
	//			}
	//			DestroyIndex(pConflict);
	//			pConflict->ResetConflictData();
	//			if (NULL == pPrevConflict)
	//			{
	//				pConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//			}
	//			else
	//			{
	//				pConflict = pPrevConflict->GetNextConflict();
	//			}
	//			continue;
	//		}
	//		pPrevConflict = pConflict;
	//		pConflict = pConflict->GetNextConflict();
	//	}

	//	return S_OK;
	//}
	////从哈希表中删除对象
	//int32_t Erase(CIndex* pIndex)
	//{
	//	int32_t ret = CheckIndex(pIndex);
	//	if (ret < 0)
	//	{
	//		return ret;
	//	}

	//	//计算哈希值
	//	uint32_t nHashCode = GetHashCode(pIndex->GetKey());
	//	uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

	//	CIndex* pPrevConflict = NULL;
	//	CIndex* pConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//	//查找指定的节点
	//	while (NULL != pConflict)
	//	{
	//		if (pConflict == pIndex)
	//		{
	//			if (NULL == pPrevConflict)
	//			{
	//				m_arrBucket[nBucketIndex].pHeadConflict = pConflict->GetNextConflict();
	//			}
	//			else
	//			{
	//				pPrevConflict->SetNextConflict(pConflict->GetNextConflict());
	//			}
	//			return DestroyIndex(pIndex);
	//		}
	//		pPrevConflict = pConflict;
	//		pConflict = pConflict->GetNextConflict();
	//	}

	//	return E_OBJECTNOTEXIST;
	//}
	////清除所有对象
	//void Clear()
	//{
	//	Uninitialize();
	//}
	////根据键值查找对象
	//CIndex* Find(Key key)
	//{
	//	//计算哈希值
	//	uint32_t nHashCode = GetHashCode(key);
	//	uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

	//	CIndex* pConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//	//找到冲突链的尾部节点
	//	while (NULL != pConflict)
	//	{
	//		if (IsEqual(key, pConflict->GetKey()))
	//		{
	//			return pConflict;
	//		}
	//		pConflict = pConflict->GetNextConflict();
	//	}

	//	return NULL;
	//}
	//向哈希表中插入对象
	CIndex* Insert(Key key, T object)
	{
		CIndex* pIndex = NULL;

		//检查键值是否冲突
		pIndex = Find(key);
		if (NULL != pIndex)
		{
			return NULL;
		}

		//创建索引
		pIndex = CreateIndex();
		if (NULL == pIndex)
		{
			return NULL;
		}

		//计算哈希值
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		if (enmInvalidObjectID == m_arrBucket[nBucketIndex].nHeadConflict)
		{
			//将新节点加入到冲突链头部
			m_arrBucket[nBucketIndex].nHeadConflict = pIndex->Index();
		}
		else
		{
			int32_t nTailConflict = m_arrBucket[nBucketIndex].nHeadConflict;
			int32_t nNextConflict = m_arrIndex[nTailConflict].GetNextConflict();
			//找到冲突链的尾部节点
			while (enmInvalidObjectID != nNextConflict)
			{
				nTailConflict = nNextConflict;
				nNextConflict = m_arrIndex[nTailConflict].GetNextConflict();
			}
			//将新节点添加到尾部
			m_arrIndex[nTailConflict].SetNextConflict(pIndex->Index());
		}

		//设置新节点冲突数据
		pIndex->SetKey(key);
		pIndex->SetNextConflict(enmInvalidObjectID);
		pIndex->SetObject(object);

		return pIndex;
	}
	//从哈希表中删除对象
	int32_t Erase(Key key)
	{
		//计算哈希值
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nPrevConflict = enmInvalidObjectID;
		int32_t nConflict = m_arrBucket[nBucketIndex].nHeadConflict;
		if (enmInvalidObjectID == nConflict)
		{
			return E_OBJECTNOTEXIST;
		}
		//查找与指定键值相等的所有节点
		while (enmInvalidObjectID != nConflict)
		{
			if (IsEqual(key, m_arrIndex[nConflict].GetKey()))
			{
				if (enmInvalidObjectID == nPrevConflict)
				{
					m_arrBucket[nBucketIndex].nHeadConflict = m_arrIndex[nConflict].GetNextConflict();
				}
				else
				{
					m_arrIndex[nPrevConflict].SetNextConflict(m_arrIndex[nConflict].GetNextConflict());
				}
				DestroyIndex(&m_arrIndex[nConflict]);
				m_arrIndex[nConflict].ResetConflictData();
				if (enmInvalidObjectID == nPrevConflict)
				{
					nConflict = m_arrBucket[nBucketIndex].nHeadConflict;
				}
				else
				{
					nConflict = m_arrIndex[nPrevConflict].GetNextConflict();
				}
				continue;
			}
			nPrevConflict = nConflict;
			nConflict = m_arrIndex[nConflict].GetNextConflict();
		}

		return S_OK;
	}
	//从哈希表中删除对象
	int32_t Erase(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (ret < 0)
		{
			return ret;
		}

		//计算哈希值
		uint32_t nHashCode = GetHashCode(pIndex->GetKey());
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nPrevConflict = enmInvalidObjectID;
		int32_t nConflict = m_arrBucket[nBucketIndex].nHeadConflict;
		//查找指定的节点
		while (enmInvalidObjectID != nConflict)
		{
			if (nConflict == pIndex->Index())
			{
				if (enmInvalidObjectID == nPrevConflict)
				{
					m_arrBucket[nBucketIndex].nHeadConflict = m_arrIndex[nConflict].GetNextConflict();
				}
				else
				{
					m_arrIndex[nPrevConflict].SetNextConflict(m_arrIndex[nConflict].GetNextConflict());
				}
				return DestroyIndex(pIndex);
			}
			nPrevConflict = nConflict;
			nConflict = m_arrIndex[nConflict].GetNextConflict();
		}

		return E_OBJECTNOTEXIST;
	}
	//清除所有对象
	void Clear()
	{
		Uninitialize();
	}
	//根据键值查找对象
	CIndex* Find(Key key)
	{
		//计算哈希值
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nConflict = m_arrBucket[nBucketIndex].nHeadConflict;
		//找到冲突链的尾部节点
		while (enmInvalidObjectID != nConflict)
		{
			if (IsEqual(key, m_arrIndex[nConflict].GetKey()))
			{
				return &m_arrIndex[nConflict];
			}
			nConflict = m_arrIndex[nConflict].GetNextConflict();
		}

		return NULL;
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

		pIndex->m_conflictData.object.Initialize();

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

		pIndex->m_conflictData.object.Uninitialize();

		return S_OK;
	}

	//计算哈希值
	inline uint32_t GetHashCode(Key key)
	{
		uint32_t nHashCode = 0;
		uint16_t *tmp1 = (uint16_t*)key.arrKey;

		for (uint32_t i = 0; i < key.nKeyLength / sizeof(uint16_t); ++i)
		{
			nHashCode += tmp1[i];
		}

		uint16_t n = 0;
		uint8_t* tmp2 = NULL;
		if (key.nKeyLength % sizeof(uint16_t) > 0)
		{
			tmp2 = (key.arrKey + (key.nKeyLength - (key.nKeyLength % sizeof(uint16_t))));
			memcpy(&n, tmp2, key.nKeyLength % sizeof(uint16_t));
		}

		nHashCode += n;
		nHashCode = (nHashCode & (uint16_t)0x7FFF);

		return nHashCode;
	}

	//键值比较
	inline bool IsEqual(const Key keySrc, const Key keyDest)
	{
		return ((keySrc.nKeyLength == keyDest.nKeyLength) && (0 == memcmp(keySrc.arrKey, keyDest.arrKey, keySrc.nKeyLength)));
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

protected:
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

	Bucket			m_arrBucket[BUCKETSIZE];//桶
};


#endif /* COMMON_HASHLIST_H_ */
