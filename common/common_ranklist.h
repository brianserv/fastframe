/*
 * common_ranklist.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_RANKLIST_H_
#define COMMON_RANKLIST_H_

#include <stdio.h>
#include <memory.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_hashlist.h"
#include "common_pool.h"
#include "common_datetime.h"


//排行对象实例（参考)
class CRankObject
{
public:
	CRankObject(){}
	~CRankObject(){}

	int32_t GetTargetID(){return nTargetID;}
	int32_t GetRank(){return nRank;}
	void SetRank(int32_t rank) {nRank = rank;}

	bool operator < (const CRankObject& obj) const
	{
		return nValue < obj.nValue;
	}

	bool operator == (const CRankObject& obj) const
	{
		return nValue == obj.nValue;
	}

	void operator = (const CRankObject& obj)
	{
		nTargetID = obj.nTargetID;
		nValue = obj.nValue;
		nRank = obj.nRank;
	}
public:
	int32_t nTargetID;
	int32_t	nRank;
	int32_t nValue;
	int32_t nLastRankUpdateTime;
};

//声明一个排行列表模板类
template<typename T, uint32_t CAPACITY>
class CRankList : public CObject
{
public:

	enum
	{
		enmInvalidObjectID				= -1,		//无效的对象索引ID
		enmIndexFlag_Free				= 0,		//对象索引空闲状态
		enmIndexFlag_Used				= 1,		//对象索引已使用状态
	};

	typedef CHashList<int32_t, CAPACITY, CAPACITY*8>					CTargetHashList;	//索引列表
	typedef typename CHashList<int32_t, CAPACITY, CAPACITY*8>::CIndex 	CHashIndex;
	typedef typename CHashList<int32_t, CAPACITY, CAPACITY*8>::Key		Key;

	class CIndex
	{
	public:
		CIndex()
		{
		}
		~CIndex()
		{
		}

		inline T& Object()
		{
			return m_object;
		}
		inline T* ObjectPtr()
		{
			return (T*)&m_object;
		}

		//设置对象
		inline void SetObject(T& object)
		{
			m_object = object;
		}

		//获取对象ID
		inline int32_t Index()
		{
			return m_nIndex;
		}

	protected:
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

		//设置对象ID
		inline void SetIndex(int32_t nIndex)
		{
			m_nIndex = nIndex;
		}
	protected:
		int32_t			m_nFlag;
		int32_t			m_nIndex;
		T				m_object;

		friend class CRankList;
	};

public:
	CRankList()
	{
	}
	virtual ~CRankList()
	{
	}

public:
	//初始化排行列表
	virtual int32_t Initialize()
	{
		m_nObjectCount = 0;
		m_TargetHashList.Initialize();

		for(uint32_t i = 0; i < CAPACITY; i++)
		{
			m_arrIndex[i].SetIndex(i);
		}

		m_nLastChangeTime = (int32_t)CDateTime::CurrentDateTime().Seconds();
		return S_OK;
	}
	//恢复排行列表
	virtual int32_t Resume()
	{
		return Initialize();
	}
	//注销排行列表
	virtual int32_t Uninitialize()
	{
		m_TargetHashList.Uninitialize();
		m_nObjectCount = 0;
		return S_OK;
	}

public:
	//插入排行对象
	int32_t Append(T& object)
	{
		if(IsFull())
		{
			//如果比最后一名还要小不处理
			CIndex *pLast = Last();
			if(pLast)
			{
				T& lastObj = pLast->Object();
				if( object < lastObj || object == lastObj)
				{
					return S_FALSE;
				}
			}

			//删除最后一个对象
			int32_t ret = EraseLast();
			if(0 > ret)
			{
				return E_UNKNOWN;
			}
		}

		CIndex *pIndex = InsertLast(object);
		if(NULL == pIndex)
		{
			return E_UNKNOWN;
		}

		//更新次序
		pIndex = Update(pIndex);
		return S_OK;
	}

	//根据键值查找对象
	int32_t Find(const int32_t nTargetID, T* &object)
	{
		CIndex *pIndex = NULL;
		int32_t ret = Find(nTargetID, pIndex);
		if(0 > ret)
		{
			return ret;
		}
		object = &(pIndex->Object());
		return S_OK;
	}

	//获取前一名对象(排名不同)
	int32_t FindPrev(const int32_t nTargetID, T* &object)
	{
		object = NULL;
		CIndex *pIndex = NULL;
		int32_t ret = Find(nTargetID, pIndex);
		if(0 > ret)
		{
			return ret;
		}

		CIndex *pPrev = GetPrev(pIndex);
		while(pPrev)
		{
			if(pPrev->Object().GetRank() != pIndex->Object().GetRank())
			{
				break;
			}

			pPrev = GetPrev(pPrev);
		}

		if(NULL == pPrev)
		{
			return E_UNKNOWN;
		}
		object = pPrev->ObjectPtr();
		return S_OK;
	}

	//更新排行对象
	int32_t Update(const int32_t nTargetID)
	{
		CIndex *pIndex = NULL;
		int32_t ret = Find(nTargetID, pIndex);
		if(0 > ret)
		{
			return ret;
		}

		//更新次序
		pIndex = Update(pIndex);
		//不交换也有可能导致排名更新
		Rerank(pIndex);
		return S_OK;
	}

	//获取排行列表
	int32_t GetRankList(T arrTargetID[], int32_t begin, int32_t max, int32_t &count)
	{
		count = 0;
		if(begin < 0)
		{
			return E_UNKNOWN;
		}

		if( begin >= m_nObjectCount)
		{
			return S_OK;
		}

		for(int32_t i = begin; i < m_nObjectCount; i++)
		{
			arrTargetID[count] = m_arrIndex[i].Object();
			count++;

			if(count >= max)
			{
				break;
			}
		}

		return S_OK;
	}

	//获取自然序号
	int32_t GetNormalRank(const int32_t nTargetID)
	{
		CIndex *pIndex = NULL;
		int32_t ret = Find(nTargetID, pIndex);
		if(0 > ret)
		{
			return -1;
		}
		return pIndex->Index();
	}

	//清除所有对象
	void Clear()
	{
		for(int32_t i = 0; i < m_nObjectCount; i++)
		{
			CIndex *pIndex = &m_arrIndex[i];
			pIndex->SetIndex(i);
			pIndex->SetFree();
		}

		//清空Hash表
		m_TargetHashList.Clear();

		m_nObjectCount = 0;

		m_nLastChangeTime = (int32_t)CDateTime::CurrentDateTime().Seconds();
	}

	//获取对象池中对象数量
	uint32_t ObjectCount() const
	{
		return m_nObjectCount;
	}

	//获取最后一名
	int32_t GetLastRank(T& object)
	{
		CIndex *pIndex = Last();
		if(pIndex)
		{
			object = pIndex->Object();
			return S_OK;
		}

		return E_UNKNOWN;
	}

	//获取第一名
	int32_t GetFirstRank(T& object)
	{
		CIndex *pIndex = First();
		if(pIndex)
		{
			object = pIndex->Object();
			return S_OK;
		}
		return E_UNKNOWN;
	}

	int32_t GetLastChangeTime()
	{
		return m_nLastChangeTime;
	}
protected:
	//在末尾插入对象
	CIndex* InsertLast(T& object)
	{
		//尾部插入对象
		int32_t nIndex = m_nObjectCount;
		CIndex *pIndex = &m_arrIndex[nIndex];
		m_nObjectCount ++;

		//插入hash表
		Key key = MakeKey(object.GetTargetID());
		m_TargetHashList.Insert(key, nIndex);

		pIndex->SetObject(object);
		pIndex->SetUsed();

		//设置排行值(如果跟前一节点相等，则使用前一节点排行)
		pIndex->Object().SetRank(nIndex+1);

		CIndex *pPrev = GetPrev(pIndex);
		if((NULL != pPrev) && (pPrev->Object() == pIndex->Object()))
		{
			pIndex->Object().SetRank(pPrev->Object().GetRank());
		}

		m_nLastChangeTime = (int32_t)CDateTime::CurrentDateTime().Seconds();
		return pIndex;
	}

	//删除排行对象
	int32_t EraseLast()
	{
		CIndex *pIndex = Last();
		if(NULL == pIndex)
		{
			return S_OK;
		}

		T &obj = pIndex->Object();

		//从HashList中删除
		Key key = MakeKey(obj.GetTargetID());
		m_TargetHashList.Erase(key);

		//执行清理工作
		pIndex->SetFree();

		m_nObjectCount--;
		return S_OK;
	}

	//根据键值查找对象
	int32_t Find(const int32_t nTargetID, CIndex* &pTarget)
	{
		//HashList取对象索引
		Key key = MakeKey(nTargetID);
		CHashIndex* pHashIndex = m_TargetHashList.Find(key);
		if(NULL == pHashIndex)
		{
			return E_UNKNOWN;
		}

		int32_t nObjectIndex = pHashIndex->Object();
		if(nObjectIndex < 0 || nObjectIndex >= m_nObjectCount)
		{
			return E_UNKNOWN;
		}

		pTarget = &m_arrIndex[nObjectIndex];

		return S_OK;
	}

	//移动对象
	CIndex* Update(CIndex *pIndex)
	{
		T &object = pIndex->Object();

		//跟前面的对比
		CIndex *pPrev = GetPrev(pIndex);
		if(pPrev)
		{
			T& objPrev = pPrev->Object();
			if(objPrev < object)
			{
				CIndex *pNewIndex = SwapUp(pIndex);
				if(pNewIndex)
				{
					return Update(pNewIndex);
				}
				return pIndex;
			}
		}

		//跟后面的对比
		CIndex *pNext = GetNext(pIndex);
		if(pNext)
		{
			T& objNext = pNext->Object();
			if(object < objNext)
			{
				CIndex * pNewIndex = SwapDown(pIndex);
				if(pNewIndex)
				{
					return Update(pNewIndex);
				}
			}
			return pIndex;
		}
		return pIndex;
	}

	//获取最后一名
	inline CIndex* Last()
	{
		return (m_nObjectCount <= 0) ? NULL : &m_arrIndex[m_nObjectCount-1];
	}

	//获取最后一名
	inline CIndex* First()
	{
		return (m_nObjectCount <= 0) ? NULL : &m_arrIndex[0];
	}

	//获取后一个节点
	inline CIndex * GetNext(CIndex *pIndex)
	{
		int32_t nIndex = pIndex->Index();
		if(nIndex >= (m_nObjectCount -1))
		{
			return NULL;
		}

		return &m_arrIndex[nIndex + 1];
	}

	//获取前一个节点
	inline CIndex * GetPrev(CIndex *pIndex)
	{
		int32_t nIndex = pIndex->Index();
		if(nIndex <= 0)
		{
			return NULL;
		}

		return &m_arrIndex[nIndex - 1];
	}

	//获取对象池容量
	inline uint32_t Capacity() const
	{
		return CAPACITY;
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

	void Rerank(CIndex *pIndex)
	{
		T& object = pIndex->Object();

		int32_t oldRank = object.GetRank();

		//计算自己本应该的排名
		object.SetRank(pIndex->Index() + 1);

		//是否需要跟前一节点合并
		CIndex *pPrev = GetPrev(pIndex);
		if(NULL != pPrev)
		{
			if(pPrev->Object() == object)
			{
				object.SetRank(pPrev->Object().GetRank());
			}
		}

		//后面的节点是否需要跟自己合并
		CIndex *pNext = GetNext(pIndex);
		while(NULL != pNext)
		{
			T& nextObj = pNext->Object();
			if(nextObj == object)
			{
				nextObj.SetRank(object.GetRank());
			}
			else
			{
				//如果排名不对则纠正
				if(nextObj.GetRank() != (pNext->Index()+1))
				{
					pPrev = GetPrev(pNext);
					if(pPrev && (nextObj == pPrev->Object()))
					{
						nextObj.SetRank(pPrev->Object().GetRank());
					}
					else
					{
						nextObj.SetRank(pNext->Index()+1);
					}
				}
				else
				{
					break;
				}
			}

			pNext = GetNext(pNext);
		}


		if(object.GetRank() != oldRank)
		{
			m_nLastChangeTime = (int32_t)CDateTime::CurrentDateTime().Seconds();
		}
	}
protected:
	//向下交换
	CIndex* SwapDown(CIndex* pIndex)
	{
		//如果后面没有节点
		CIndex *pNext = GetNext(pIndex);
		if(NULL == pNext)
		{
			return NULL;
		}

		//交换2个节点
		Swap(pIndex, pNext);

		//刷排名
		Rerank(pIndex);
		Rerank(pNext);
		return pNext;
	}

	//向上交换
	CIndex* SwapUp(CIndex* pIndex)
	{
		//如果前面没有节点
		CIndex *pPrev = GetPrev(pIndex);
		if(NULL == pPrev)
		{
			return NULL;
		}

		//交换2个节点
		Swap(pIndex, pPrev);

		//刷排名
		Rerank(pPrev);
		Rerank(pIndex);
		return pPrev;
	}

	//交换2个位置
	void Swap(CIndex *pA, CIndex *pB)
	{
		//修改Hash索引
		Key keyA = MakeKey(pA->Object().GetTargetID());
		Key keyB = MakeKey(pB->Object().GetTargetID());
		CHashIndex *pHashIndexA = m_TargetHashList.Find(keyA);
		int32_t nIndexA = pHashIndexA->Object();

		CHashIndex *pHashIndexB = m_TargetHashList.Find(keyB);
		int32_t nIndexB = pHashIndexB->Object();

		pHashIndexA->SetObject(nIndexB);
		pHashIndexB->SetObject(nIndexA);

		//交换对象
		T obj = pA->Object();
		pA->SetObject(pB->Object());
		pB->SetObject(obj);
	}

	Key MakeKey(const int32_t nTargetID)
	{
		Key key ={0};
		key.nKeyLength = 4;
		*(int32_t*)key.arrKey = nTargetID;
		return key;
	}
protected:
	CIndex				m_arrIndex[CAPACITY];		//排行表
	CTargetHashList		m_TargetHashList;			//排行索引
	int32_t				m_nObjectCount;				//有效对象
	int32_t				m_nLastChangeTime;			//最后一次排行变动事件
};

#endif /* COMMON_RANKLIST_H_ */
