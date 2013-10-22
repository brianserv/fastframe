/*
 * common_cache.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_CACHE_H_
#define COMMON_CACHE_H_

#include <stdio.h>
#include <memory.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_complextype.h"


//声明一个Cache模板类
template<typename T, uint32_t CAPACITY, uint32_t BUCKETSIZE>
class CCache : public CObject
{
public:
	class CIndex;

	enum
	{
		enmMaxKeyLength					= 8,		//最大键长度
		//enmIndexFlag_Free				= 0,		//对象索引空闲状态
		//enmIndexFlag_Used				= 1,		//对象索引已使用状态
		enmInvalidObjectID				= -1,		//无效的对象ID
	};

	//定义对象活跃状态
	typedef uint8_t								ActiveState;
	enum
	{
		enmActiveState_Unused					= 0,
		enmActiveState_Active					= 1,
		enmActiveState_Inactive					= 2,
	};
	enum
	{
		enmMaxActiveObjectCount					= CAPACITY / 2,
		enmMaxInactiveObjectCount				= CAPACITY / 2,
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
		int32_t			nNextConflict;				//冲突链的下一个节点
	} ConflictData;

	//桶元素结构
	typedef struct tagBucket
	{
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

	protected:
		//初始化对象索引
		virtual int32_t Initialize()
		{
			m_pContainer = NULL;
			//m_nFlag = enmIndexFlag_Free;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;
			m_activeState = enmActiveState_Unused;

			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			m_conflictData.nNextConflict = enmInvalidObjectID;

			return S_OK;
		}
		//恢复对象索引
		virtual int32_t Resume()
		{
			return S_OK;
		}
		//注销对象索引
		virtual int32_t Uninitialize()
		{
			//m_nFlag = enmIndexFlag_Free;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;
			m_activeState = enmActiveState_Unused;

			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			m_conflictData.nNextConflict = enmInvalidObjectID;

			return S_OK;
		}

	public:
		//获取下一个冲突节点
		CIndex* NextConflict() const
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

		//获取活跃状态
		ActiveState GetActiveState()
		{
			return m_activeState;
		}

		CIndex* Prev()
		{
			if (enmInvalidObjectID == m_nPrev)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nPrev];
		}
		//获取链表的后一节点
		CIndex* Next()
		{
			if (enmInvalidObjectID == m_nNext)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nNext];
		}

		//获取对象
		T& GetObject()
		{
			return m_conflictData.object.GetValue();
		}

		//获取对象2(避免错误error C2039: 'GetObjectW' : is not a member of 'CCache<T,CAPACITY,BUCKETSIZE>::CIndex')
		T& Object()
		{
			return m_conflictData.object.GetValue();
		}

		//设置对象
		void SetObject(T& object)
		{
			m_conflictData.object.SetValue(object);
		}

		//获取对象ID
		int32_t Index()
		{
			return m_nIndex;
		}

	protected:
		//设置父对象指针
		void SetContainer(CCache* pContainer)
		{
			m_pContainer = pContainer;
		}
		////设置节点为已使用
		//void SetUsed()
		//{
		//	m_nFlag = enmIndexFlag_Used;
		//}
		////设置节点为空闲
		//void SetFree()
		//{
		//	m_nFlag = enmIndexFlag_Free;
		//}
		//节点是否已使用
		bool IsUsed() const
		{
			return ((enmActiveState_Active == m_activeState) || (enmActiveState_Inactive == m_activeState));
		}

		//获取链表的后一节点
		int32_t GetNext() const
		{
			return m_nNext;
		}
		//获取链表的前一节点
		int32_t GetPrev() const
		{
			return m_nPrev;
		}
		//设置链表的后一节点
		void SetNext(int32_t nNext)
		{
			m_nNext = nNext;
		}
		//设置链表的前一节点
		void SetPrev(int32_t nPrev)
		{
			m_nPrev = nPrev;
		}

		//设置对象ID
		void SetIndex(int32_t nIndex)
		{
			m_nIndex = nIndex;
		}

		//设置节点键值
		void SetKey(Key key)
		{
			memcpy(&m_conflictData.key, &key, sizeof(Key));
		}

		//设置节点活动状态
		void SetActiveState(ActiveState activeState)
		{
			m_activeState = activeState;
		}

		//获取下一个冲突节点
		int32_t GetNextConflict()
		{
			return m_conflictData.nNextConflict;
		}
		//设置下一个冲突节点
		void SetNextConflict(int32_t nNextConflict)
		{
			m_conflictData.nNextConflict = nNextConflict;
		}

		//重置冲突链节点数据
		void ResetConflictData()
		{
			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			m_conflictData.nNextConflict = enmInvalidObjectID;
		}

	protected:
		CCache*		m_pContainer;

		//uint32_t		m_nFlag;		//索引状态标志
		int32_t			m_nIndex;		//对象索引号
		int32_t			m_nNext;		//后一节点
		int32_t			m_nPrev;		//前一节点
		ActiveState		m_activeState;	//活跃状态

		ConflictData	m_conflictData;	//关联的冲突链元素

		friend class CCache;
	};


public:
	CCache()
	{
	}
	virtual ~CCache()
	{
	}

public:
	//初始化哈希表
	virtual int32_t Initialize()
	{
		m_nFreeCount = CAPACITY;
		m_nFreeHead = 0;
		m_nFreeTail = CAPACITY - 1;

		m_nActiveCount = 0;
		m_nActiveHead = enmInvalidObjectID;
		m_nActiveTail = enmInvalidObjectID;

		m_nInactiveCount = 0;
		m_nInactiveHead = enmInvalidObjectID;
		m_nInactiveTail = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Initialize();
		}

		for (uint32_t i = 0; i < BUCKETSIZE; ++i)
		{
			m_arrActiveBucket[i].nHeadConflict = enmInvalidObjectID;
			m_arrInactiveBucket[i].nHeadConflict = enmInvalidObjectID;
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
		m_nFreeCount = CAPACITY;
		m_nFreeHead = 0;
		m_nFreeTail = CAPACITY - 1;

		m_nActiveCount = 0;
		m_nActiveHead = enmInvalidObjectID;
		m_nActiveTail = enmInvalidObjectID;

		m_nInactiveCount = 0;
		m_nInactiveHead = enmInvalidObjectID;
		m_nInactiveTail = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Uninitialize();
		}

		for (uint32_t i = 0; i < BUCKETSIZE; ++i)
		{
			m_arrActiveBucket[i].nHeadConflict = enmInvalidObjectID;
			m_arrInactiveBucket[i].nHeadConflict = enmInvalidObjectID;
		}

		BuildIndexList();

		return S_OK;
	}

public:
	//向Cache中创建对象
	CIndex* CreateObject(Key key)
	{
		CIndex* pIndex = NULL;

		//检查键值是否冲突
		pIndex = Find(key);
		if (NULL != pIndex)
		{
			return NULL;
		}
		//活跃列表已满
		if (m_nActiveCount >= enmMaxActiveObjectCount)
		{
			MoveFirstActiveToInactive();
		}
		//创建对象
		pIndex = NewActiveIndex(key);
		if (NULL == pIndex)
		{
			return NULL;
		}
		////添加索引
		//AddToBucket(pIndex, enmActiveState_Active, key);

		return pIndex;
	}
	//从Cache中删除对象
	int32_t DeleteObject(Key key)
	{
		CIndex* pIndex = DeleteFromBucket(enmActiveState_Active, key);
		if (NULL == pIndex)
		{
			pIndex = DeleteFromBucket(enmActiveState_Inactive, key);
			if (NULL == pIndex)
			{
				return E_OBJECTNOTEXIST;
			}
		}
		//将对象从活跃链移动到空闲链
		ActiveState activeState = pIndex->GetActiveState();
		int32_t ret = MoveIndex(pIndex, activeState, enmActiveState_Unused);
		if (0 > ret)
		{
			return ret;
		}
		return S_OK;
	}
	//从cache中删除对象
	int32_t DeleteObject(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return ret;
		}

		return DeleteObject(pIndex->GetKey());
	}
	//清除所有对象
	void Clear()
	{
		Uninitialize();
	}
	//获取对象
	CIndex* FindObject(Key key)
	{
		CIndex* pIndex = Find(key);
		if (NULL == pIndex)
		{
			return NULL;
		}
		if (enmActiveState_Inactive == pIndex->GetActiveState())
		{
			int32_t ret = ActivateObject(pIndex);
			if (0 > ret)
			{
				//
			}
		}
		return pIndex;
	}
	//激活对象
	int32_t ActivateObject(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return ret;
		}
		if (enmActiveState_Active == pIndex->GetActiveState())
		{
			return S_OK;
		}
		if (enmActiveState_Inactive != pIndex->GetActiveState())
		{
			return E_NOTINACTIVE;
		}
		//临时移动到空闲链（不销毁对象内容）
		ret = MoveIndex(pIndex, enmActiveState_Inactive, enmActiveState_Unused, false);
		if (0 > ret)
		{
			return ret;
		}
		Key key = pIndex->GetKey();
		//删除对象索引
		DeleteFromBucket(enmActiveState_Inactive, key);
		//确定活跃链是否已满
		if (enmMaxActiveObjectCount <= m_nActiveCount)
		{
			MoveFirstActiveToInactive();
		}
		//将对象移动到活跃链
		ret = MoveIndex(pIndex, enmActiveState_Unused, enmActiveState_Active, false);
		if (0 > ret)
		{
			return ret;
		}
		//添加对象索引
		AddToBucket(pIndex, enmActiveState_Active, key);

		return S_OK;
	}

	//获取活跃头部对象索引
	CIndex* FirstActive()
	{
		return (enmInvalidObjectID == m_nActiveHead) ? NULL : &m_arrIndex[m_nActiveHead];
	}
	//获取活跃尾部对象索引
	CIndex* LastActive()
	{
		return (enmInvalidObjectID == m_nActiveTail) ? NULL : &m_arrIndex[m_nActiveTail];
	}
	//获取非活跃头部对象索引
	CIndex* FirstInactive()
	{
		return (enmInvalidObjectID == m_nInactiveHead) ? NULL : &m_arrIndex[m_nInactiveHead];
	}
	//获取非活跃尾部对象索引
	CIndex* LastInactive()
	{
		return (enmInvalidObjectID == m_nInactiveTail) ? NULL : &m_arrIndex[m_nInactiveTail];
	}

	//获取对象池容量
	uint32_t Capacity() const
	{
		return CAPACITY;
	}
	//获取对象池中对象数量
	uint32_t ObjectCount() const
	{
		return m_nActiveCount + m_nInactiveCount;
	}
	//获取对象池中对象数量
	uint32_t ActiveCount() const
	{
		return m_nActiveCount;
	}
	//获取对象池中对象数量
	uint32_t InactiveCount() const
	{
		return m_nInactiveCount;
	}

	//对象池是否为空
	bool IsEmpty() const
	{
		return (0 == (m_nActiveCount + m_nInactiveCount));
	}
	//对象池是否已满
	bool IsFull() const
	{
		return ((m_nActiveCount + m_nInactiveCount) == CAPACITY);
	}

	//根据位置获取索引
	CIndex* GetIndex(const int32_t nIndex)
	{
		int32_t ret = CheckIndex(nIndex);
		if (0 > ret)
		{
			return NULL;
		}
		return &m_arrIndex[nIndex];
	}
	//根据位置获取索引
	int32_t GetIndex(const CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return enmInvalidObjectID;
		}
		return (int32_t)(((uint8_t*)pIndex - (uint8_t*)m_arrIndex) / sizeof(CIndex));
	}

protected:
	//将活跃链中的第一项移动到非活跃链
	void MoveFirstActiveToInactive()
	{
		if (enmInvalidObjectID == m_nActiveHead)
		{
			return;
		}
		//非活跃列表已满
		if (m_nInactiveCount >= enmMaxInactiveObjectCount)
		{
			DeleteFirstInactive();
		}
		CIndex* pIndex = &m_arrIndex[m_nActiveHead];
		int32_t ret = MoveActiveToInactive(pIndex);
		if (0 > ret)
		{
			//
		}
	}
	//删除非活跃链中的第一项
	void DeleteFirstInactive()
	{
		if (enmInvalidObjectID == m_nInactiveHead)
		{
			return;
		}
		CIndex* pIndex = &m_arrIndex[m_nInactiveHead];
		int32_t ret = FreeIndex(pIndex);
		if (0 > ret)
		{
			//
		}
	}

protected:
	//新建活跃对象
	CIndex* NewActiveIndex(Key key)
	{
		//活跃链已满
		if (enmMaxActiveObjectCount <= m_nActiveCount)
		{
			return NULL;
		}
		//从空闲链表头部取出一个空索引
		if (enmInvalidObjectID == m_nFreeHead)
		{
			return NULL;
		}
		CIndex* pIndex = &m_arrIndex[m_nFreeHead];

		//将对象从空闲链移动到活跃链
		int32_t ret = MoveIndex(pIndex, enmActiveState_Unused, enmActiveState_Active);
		if (0 > ret)
		{
			return NULL;
		}
		//添加活跃对象索引
		AddToBucket(pIndex, enmActiveState_Active, key);

		return pIndex;
	}
	//新建非活跃对象
	CIndex* NewInactiveIndex(Key key)
	{
		//非活跃链已满
		if (enmMaxInactiveObjectCount <= m_nInactiveCount)
		{
			return NULL;
		}
		//从空闲链表头部取出一个空索引
		if (enmInvalidObjectID == m_nFreeHead)
		{
			return NULL;
		}
		CIndex* pIndex = &m_arrIndex[m_nFreeHead];

		//将对象从空闲链移动到非活跃链
		int32_t ret = MoveIndex(pIndex, enmActiveState_Unused, enmActiveState_Inactive);
		if (0 > ret)
		{
			return NULL;
		}
		//添加非活跃对象索引
		AddToBucket(pIndex, enmActiveState_Inactive, key);

		return pIndex;
	}
	//销毁活跃对象
	int32_t FreeIndex(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return ret;
		}
		//将对象从活跃链移动到空闲链
		Key key = pIndex->GetKey();
		ActiveState activeState = pIndex->GetActiveState();
		ret = MoveIndex(pIndex, activeState, enmActiveState_Unused);
		if (0 > ret)
		{
			return ret;
		}
		//删除活跃对象索引
		DeleteFromBucket(activeState, key);

		return S_OK;
	}
	//根据键值查找对象
	CIndex* Find(Key key)
	{
		CIndex* pIndex = FindInBucket(enmActiveState_Active, key);
		if (NULL == pIndex)
		{
			pIndex = FindInBucket(enmActiveState_Inactive, key);
		}

		return pIndex;
	}
	//将活跃对象设置为不活跃对象
	int32_t MoveActiveToInactive(CIndex* pIndex)
	{
		//非活跃链已满
		if (enmMaxInactiveObjectCount <= m_nInactiveCount)
		{
			return E_INACTIVEISFULL;
		}
		//将对象从活跃链移动到非活跃链
		int32_t ret = MoveIndex(pIndex, enmActiveState_Active, enmActiveState_Inactive, false);
		if (0 > ret)
		{
			return ret;
		}
		Key key = pIndex->GetKey();
		//删除活跃对象索引
		DeleteFromBucket(enmActiveState_Active, key);
		//添加非活跃对象索引
		AddToBucket(pIndex, enmActiveState_Inactive, key);

		return S_OK;
	}
	//将活不跃对象设置为活跃对象
	void MoveInactiveToActive(CIndex* pIndex)
	{
		//非活跃链已满
		if (enmMaxActiveObjectCount <= m_nActiveCount)
		{
			return E_ACTIVEISFULL;
		}
		//将对象从非活跃链移动到活跃链
		int32_t ret = MoveIndex(pIndex, enmActiveState_Inactive, enmActiveState_Active, false);
		if (0 > ret)
		{
			return ret;
		}
		Key key = pIndex->GetKey();
		//删除非活跃对象索引
		DeleteFromBucket(enmActiveState_Inactive, key);
		//添加活跃对象索引
		AddToBucket(enmActiveState_Active, key);

		return S_OK;
	}

protected:
	int32_t BuildIndexList()
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

	//对象移动
	int32_t MoveIndex(CIndex* pIndex, ActiveState fromState, ActiveState toState, bool bInit = true)
	{
		int32_t ret = CheckIndex(pIndex, true);
		if (0 > ret)
		{
			return ret;
		}
		if (fromState != pIndex->GetActiveState())
		{
			return E_ACTIVESTATE;
		}
		if (fromState == toState)
		{
			return S_FALSE;
		}

		//为了便于统一操作，获取几个成员变量的指针
		uint32_t *pFromCount = GetCountPointer(fromState);
		int32_t *pFromHead = GetHeadPointer(fromState);
		int32_t *pFromTail = GetTailPointer(fromState);
		uint32_t *pToCount = GetCountPointer(toState);
		int32_t *pToHead = GetHeadPointer(toState);
		int32_t *pToTail = GetTailPointer(toState);

		//调整原索引链表的头部和尾部指针
		if (*pFromHead == pIndex->Index())
		{
			*pFromHead = pIndex->GetNext();
		}
		if (*pFromTail == pIndex->Index())
		{
			*pFromTail = pIndex->GetPrev();
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

		//将索引添加到新索引链表尾部
		pIndex->SetPrev(*pToTail);
		pIndex->SetNext(enmInvalidObjectID);
		if (enmInvalidObjectID != *pToTail)
		{
			m_arrIndex[*pToTail].SetNext(pIndex->Index());
		}
		*pToTail = pIndex->Index();
		if (enmInvalidObjectID == *pToHead)
		{
			*pToHead = *pToTail;
		}

		pIndex->SetActiveState(toState);

		--(*pFromCount);
		++(*pToCount);

		//是否对对象进行初始化操作
		if (bInit)
		{
			if (enmActiveState_Unused == fromState)
			{
				pIndex->m_conflictData.object.Initialize();
			}
			else if (enmActiveState_Unused == toState)
			{
				pIndex->m_conflictData.object.Uninitialize();
			}
		}

		return S_OK;
	}
	//添加索引
	void AddToBucket(CIndex* pIndex, ActiveState activeState, Key key)
	{
		Bucket *pBucket = (activeState == enmActiveState_Active) ? m_arrActiveBucket : m_arrInactiveBucket;

		//计算哈希值
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		if (enmInvalidObjectID == pBucket[nBucketIndex].nHeadConflict)
		{
			//将新节点加入到冲突链头部
			pBucket[nBucketIndex].nHeadConflict = pIndex->Index();
		}
		else
		{
			int32_t nTailConflict = pBucket[nBucketIndex].nHeadConflict;
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
	}
	//删除索引
	CIndex* DeleteFromBucket(ActiveState activeState, Key key)
	{
		Bucket *pBucket = (activeState == enmActiveState_Active) ? m_arrActiveBucket : m_arrInactiveBucket;

		//计算哈希值
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nPrevConflict = enmInvalidObjectID;
		int32_t nConflict = pBucket[nBucketIndex].nHeadConflict;
		if (enmInvalidObjectID == nConflict)
		{
			return NULL;
		}
		//查找与指定键值相等的所有节点
		while (enmInvalidObjectID != nConflict)
		{
			if (IsEqual(key, m_arrIndex[nConflict].GetKey()))
			{
				if (enmInvalidObjectID == nPrevConflict)
				{
					pBucket[nBucketIndex].nHeadConflict = m_arrIndex[nConflict].GetNextConflict();
				}
				else
				{
					m_arrIndex[nPrevConflict].SetNextConflict(m_arrIndex[nConflict].GetNextConflict());
				}
				m_arrIndex[nConflict].ResetConflictData();
				return &m_arrIndex[nConflict];
			}
			nPrevConflict = nConflict;
			nConflict = m_arrIndex[nConflict].GetNextConflict();
		}

		return NULL;
	}
	//根据键值查找对象
	CIndex* FindInBucket(ActiveState activeState, Key key)
	{
		Bucket *pBucket = (activeState == enmActiveState_Active) ? m_arrActiveBucket : m_arrInactiveBucket;

		//计算哈希值
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nConflict = pBucket[nBucketIndex].nHeadConflict;
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

	//计算哈希值
	uint32_t GetHashCode(Key key)
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
	bool IsEqual(const Key keySrc, const Key keyDest)
	{
		return ((keySrc.nKeyLength == keyDest.nKeyLength) && (0 == memcmp(keySrc.arrKey, keyDest.arrKey, keySrc.nKeyLength)));
	}

	//确定对象索引指针是否有效
	int32_t CheckIndex(const CIndex* pIndex, bool bFreeIsOK = false)
	{
		if ((pIndex < m_arrIndex) || (pIndex >= m_arrIndex + CAPACITY))
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (((uint8_t*)pIndex - (uint8_t*)m_arrIndex) % sizeof(CIndex) != 0)
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (!bFreeIsOK && !pIndex->IsUsed())
		{
			return E_OBJECTINDEXNOTUSED;
		}
		return S_OK;
	}
	//确定对象索引是否有效
	int32_t CheckIndex(const int32_t nIndex, bool bFreeIsOK = false)
	{
		if ((0 > nIndex) || ((int32_t)CAPACITY < nIndex))
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (!bFreeIsOK && !m_arrIndex[nIndex].IsUsed())
		{
			return E_OBJECTINDEXNOTUSED;
		}
		return S_OK;
	}

	//获取数量指针
	uint32_t* GetCountPointer(ActiveState activeState)
	{
		switch (activeState)
		{
		case enmActiveState_Active:
			return &m_nActiveCount;
		case enmActiveState_Inactive:
			return &m_nInactiveCount;
		default:
			return &m_nFreeCount;
		}
	}
	//获取链表头指针
	int32_t* GetHeadPointer(ActiveState activeState)
	{
		switch (activeState)
		{
		case enmActiveState_Active:
			return &m_nActiveHead;
		case enmActiveState_Inactive:
			return &m_nInactiveHead;
		default:
			return &m_nFreeHead;
		}
	}
	//获取链表尾指针
	int32_t* GetTailPointer(ActiveState activeState)
	{
		switch (activeState)
		{
		case enmActiveState_Active:
			return &m_nActiveTail;
		case enmActiveState_Inactive:
			return &m_nInactiveTail;
		default:
			return &m_nFreeTail;
		}
	}

protected:
	uint32_t		m_nFreeCount;		//未分配对象的个数
	int32_t			m_nFreeHead;		//未分配对象链头部对象的索引
	int32_t			m_nFreeTail;		//未分配对象链尾部对象的索引

	uint32_t		m_nActiveCount;		//活跃对象数量
	int32_t			m_nActiveHead;		//活跃对象链头部对象的索引
	int32_t			m_nActiveTail;		//活跃对象链尾部对象的索引

	uint32_t		m_nInactiveCount;	//非活跃对象数量
	int32_t			m_nInactiveHead;	//非活跃对象链头部对象的索引
	int32_t			m_nInactiveTail;	//非活跃对象链尾部对象的索引

	CIndex			m_arrIndex[CAPACITY];	//对象索引区

	Bucket			m_arrActiveBucket[BUCKETSIZE];//活跃对象桶
	Bucket			m_arrInactiveBucket[BUCKETSIZE];//非活跃对象桶
};


#endif /* COMMON_CACHE_H_ */
