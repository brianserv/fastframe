/*
 * common_heap.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_HEAP_H_
#define COMMON_HEAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_complextype.h"

//声明一个堆管理模板类
template<typename TKEY, typename T, uint32_t CAPACITY>
class CMinHeap: public CObject
{
public:
	enum
	{
		enmIndexFlag_Free				= 0,		//对象索引空闲状态
		enmIndexFlag_Used				= 1,		//对象索引已使用状态
	};

	enum
	{
		enmInvalidHeapIndex = -1
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
			m_nFlag = enmIndexFlag_Free;
			m_pNext = NULL;
			m_pPrev = NULL;

			memset(&m_key, 0, sizeof(m_key));

			m_iHeapIndex = enmInvalidHeapIndex;

			m_object.Initialize();

			return S_OK;
		}
		//恢复对象索引
		virtual int32_t Resume()
		{
			m_object.Resume();

			return S_OK;
		}
		//注销对象索引
		virtual int32_t Uninitialize()
		{
			m_nFlag = enmIndexFlag_Free;
			m_pNext = NULL;
			m_pPrev = NULL;

			memset(&m_key, 0, sizeof(m_key));

			m_object.Uninitialize();

			return S_OK;
		}

	public:

		//获取键值
		inline const TKEY& GetKey() const
		{
			return m_key;
		}
		//获取对象
		inline T& GetObject()
		{
			return m_object.GetValue();
		}
		//设置对象
		inline void SetObject(T& object)
		{
			m_object.SetValue(object);
		}

		inline int32_t & GetHeapIndex()
		{
			return m_iHeapIndex;
		}

		inline void SetHeapIndex(int32_t nIndex)
		{
			m_iHeapIndex = nIndex;
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

		//获取链表的后一节点
		inline CIndex* GetNext() const
		{
			return m_pNext;
		}
		//获取链表的前一节点
		inline CIndex* GetPrev() const
		{
			return m_pPrev;
		}
		//设置链表的后一节点
		inline void SetNext(CIndex* pNext)
		{
			m_pNext = pNext;
		}
		//设置链表的前一节点
		inline void SetPrev(CIndex* pPrev)
		{
			m_pPrev = pPrev;
		}

		//设置键值
		inline void SetKey(TKEY key)
		{
			m_key = key;
		}
	protected:
		uint32_t		m_nFlag;		//索引状态标志

		CIndex*			m_pNext;		//后一节点，用于内存管理
		CIndex*			m_pPrev;		//前一节点，用于内存管理

		TKEY			m_key;			//键值
		CComplexType<T>	m_object;		//关联的对象

		int32_t				m_iHeapIndex;

		friend class CMinHeap;
	};


public:
	CMinHeap()
	{
	}
	virtual ~CMinHeap()
	{
	}

public:
	//初始化堆
	virtual int32_t Initialize()
	{
		m_nObjectCount = 0;
		m_nFreeCount = CAPACITY;
		m_pFreeHead = &m_arrIndex[0];
		m_pFreeTail = &m_arrIndex[CAPACITY - 1];
		m_pUsedHead = NULL;
		m_pUsedTail = NULL;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Initialize();
		}

		BuildIndexList();

		return S_OK;
	}
	//恢复堆
	virtual int32_t Resume()
	{
		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Resume();
		}
		return S_OK;
	}
	//注销堆
	virtual int32_t Uninitialize()
	{
		m_nObjectCount = 0;
		m_nFreeCount = CAPACITY;
		m_pFreeHead = &m_arrIndex[0];
		m_pFreeTail = &m_arrIndex[CAPACITY - 1];
		m_pUsedHead = NULL;
		m_pUsedTail = NULL;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Uninitialize();
		}

		BuildIndexList();

		return S_OK;
	}

public:
	//插入对象
	CIndex* Insert(TKEY key, T object)
	{
		CIndex* pNode = NULL;

		pNode = CreateIndex();
		if (NULL == pNode)
		{
			return NULL;
		}

		//设置object
		pNode->SetObject(object);
		pNode->SetKey(key);

		//把要插入的元素放到数组尾部
		m_arrIndexPrt[m_nObjectCount - 1] = pNode;
		pNode->SetHeapIndex(m_nObjectCount - 1);

		//从尾部开始向上调整
		ShiftUp(m_nObjectCount - 1);

		return pNode;
	}

	void Pop()
	{
		CIndex*	pIndex = NULL;
		if (IsEmpty())
		{
			return;
		}

		pIndex = m_arrIndexPrt[0];
		m_arrIndexPrt[0] = m_arrIndexPrt[m_nObjectCount - 1];
		m_arrIndexPrt[0]->SetHeapIndex(0);

		DestroyIndex(pIndex);

		if (IsEmpty())
		{
			return;
		}
		ShiftDown(0);
	}

	//删除对象
	uint32_t Erase(TKEY key)
	{
		uint32_t		nIndex = 0;
		CIndex*	pIndex = NULL;

		for (nIndex = 0; nIndex < m_nObjectCount; ++nIndex)
		{
			if (m_arrIndexPrt[nIndex]->m_key == key)
			{
				pIndex = m_arrIndexPrt[nIndex];
				break;
			}
		}

		//没有发现有指定KEY
		if (NULL == pIndex)
		{
			return S_OK;
		}

		//销毁CIndex对象, m_nObjectCount会减1
		DestroyIndex(m_arrIndexPrt[nIndex]);


		if (IsEmpty())
		{
			return S_OK;
		}

		//如果删除的是最后一个元素,注意m_nObjectCount已经减1
		if (nIndex == m_nObjectCount)
		{
			return S_OK;
		}

		//将数组最后一个元素放到nIndex位置
		m_arrIndexPrt[nIndex] = m_arrIndexPrt[m_nObjectCount - 1];


		//从nIndex开始向下调整
		ShiftDown(nIndex);

		return S_OK;
	}
	//删除对象
	uint32_t Erase(CIndex* pNode)
	{
		int32_t iHeapIndex = enmInvalidHeapIndex;

		if (IsEmpty())
		{
			return S_OK;
		}

		iHeapIndex = pNode->m_iHeapIndex;

		if (iHeapIndex < 0 || iHeapIndex >= m_nObjectCount)
		{
			return S_OK;
		}

		m_arrIndexPrt[iHeapIndex] = m_arrIndexPrt[m_nObjectCount - 1];
		m_arrIndexPrt[iHeapIndex]->m_iHeapIndex = iHeapIndex;

		DestroyIndex(pNode);

		ShiftDown(iHeapIndex);

		return S_OK;
	}
	//清除所有对象
	void Clear()
	{
		Uninitialize();
	}
	//查找对象
	CIndex* Find(TKEY key)
	{
		return FindNode(key);
	}
	//获取最小对象索引
	CIndex* First() const
	{
		if (IsEmpty())
		{
			return NULL;
		}

		return m_arrIndexPrt[0];
	}
	//获取最大对象索引
	CIndex* Last() const
	{
		if (IsEmpty())
		{
			return NULL;
		}

		if (1 == ObjectCount())
		{
			return m_arrIndexPrt[0];
		}

		//进行堆调整，将大的那个调到最后一个，这边的目标是保护第一个元素。
		RShiftDown(ObjectCount() - 1);

		return m_arrIndexPrt[ObjectCount() - 1];
	}

	//获取对象池容量
	uint32_t Capacity() const
	{
		return CAPACITY;
	}
	//获取对象池中对象数量
	uint32_t ObjectCount() const
	{
		return m_nObjectCount;
	}

	//对象池是否为空
	bool IsEmpty() const
	{
		return (0 == m_nObjectCount);
	}
	//对象池是否已满
	bool IsFull() const
	{
		return (m_nObjectCount == CAPACITY);
	}


	void Debug()
	{
		printf("Index\tKey\tValue\n");
		for (int i = 0; i < ObjectCount(); ++i)
		{
			printf("%d\t%d\t%d\n",
				m_arrIndexPrt[i]->GetHeapIndex(),
				m_arrIndexPrt[i]->GetKey(),
				(int32_t)m_arrIndexPrt[i]->GetObject());
		}
	}
protected:
	//对索引区建立链表
	int32_t BuildIndexList()
	{
		//设置第0个索引
		m_arrIndex[0].SetPrev(NULL);

		for (uint32_t i = 1; i < CAPACITY; ++i)
		{
			m_arrIndex[i - 1].SetNext(&m_arrIndex[i]);
			m_arrIndex[i].SetPrev(&m_arrIndex[i - 1]);
		}

		//设置最后一个索引
		m_arrIndex[CAPACITY - 1].SetNext(NULL);

		return S_OK;
	}

	//创建对象
	CIndex* CreateIndex()
	{
		//对象池已满
		if (IsFull())
		{
			return NULL;
		}

		//从空闲链表头部取出一个空索引
		CIndex* pIndex = m_pFreeHead;
		if (NULL == pIndex)
		{
			return NULL;
		}

		//调整空闲索引链表的头部和尾部指针
		if (m_pFreeTail == pIndex)
		{
			m_pFreeTail = pIndex->GetNext();
		}
		m_pFreeHead = pIndex->GetNext();
		if (NULL != m_pFreeHead)
		{
			m_pFreeHead->SetPrev(NULL);
		}

		//将新索引添加到已使用索引链表尾部
		pIndex->SetPrev(m_pUsedTail);
		pIndex->SetNext(NULL);
		if (NULL != m_pUsedTail)
		{
			m_pUsedTail->SetNext(pIndex);
		}
		m_pUsedTail = pIndex;
		if (NULL == m_pUsedHead)
		{
			m_pUsedHead = m_pUsedTail;
		}

		pIndex->SetUsed();

		++m_nObjectCount;
		--m_nFreeCount;

		return pIndex;
	}
	//销毁对象
	int32_t DestroyIndex(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (ret < 0)
		{
			return ret;
		}

		//调整已使用索引链表的头部和尾部指针
		if (m_pUsedHead == pIndex)
		{
			m_pUsedHead = pIndex->GetNext();
		}
		if (m_pUsedTail == pIndex)
		{
			m_pUsedTail = pIndex->GetPrev();
		}

		//将节点从链表中删除
		if (NULL != pIndex->GetPrev())
		{
			pIndex->GetPrev()->SetNext(pIndex->GetNext());
		}
		if (NULL != pIndex->GetNext())
		{
			pIndex->GetNext()->SetPrev(pIndex->GetPrev());
		}

		//将索引添加到空闲索引链表尾部
		pIndex->SetPrev(m_pFreeTail);
		pIndex->SetNext(NULL);
		if (NULL != m_pFreeTail)
		{
			m_pFreeTail->SetNext(pIndex);
		}
		m_pFreeTail = pIndex;
		if (NULL == m_pFreeHead)
		{
			m_pFreeHead = m_pFreeTail;
		}

		pIndex->SetFree();

		--m_nObjectCount;
		++m_nFreeCount;

		return S_OK;
	}

	//确定对象索引指针是否有效
	int32_t CheckIndex(CIndex* pIndex)
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

	//根据位置获取索引
	CIndex* GetIndex(int32_t nIndex)
	{
		if ((0 > nIndex) || ((int32_t)CAPACITY < nIndex))
		{
			return NULL;
		}
		if (!m_arrIndex[nIndex].IsUsed())
		{
			return NULL;
		}
		return &m_arrIndex[nIndex];
	}

protected:

	inline void SwapIndex(int32_t & iHeapIndex1, int32_t & iHeapIndex2)
	{
		int32_t iTemp = enmInvalidHeapIndex;

		iTemp = iHeapIndex1;
		iHeapIndex1 = iHeapIndex2;
		iHeapIndex2 = iTemp;
	}
	inline void Swap(CIndex*& pRIndex1, CIndex*& pRIndex2)
	{
		CIndex* pTemp = pRIndex2;
		pRIndex2 = pRIndex1;
		pRIndex1 = pTemp;

		int32_t & iHeapIndex1 = pRIndex1->m_iHeapIndex;
		int32_t & iHeapIndex2 = pRIndex2->m_iHeapIndex;

		int32_t iTemp = iHeapIndex1;
		iHeapIndex1 = iHeapIndex2;
		iHeapIndex2 = iTemp;
	}


	//查找节点
	CIndex* FindNode(TKEY key)
	{
		uint32_t		nIndex = 0;
		for (nIndex = 0; nIndex < m_nObjectCount; ++nIndex)
		{
			if (m_arrIndexPrt[nIndex]->m_key == key)
			{
				return m_arrIndexPrt[nIndex];
			}
		}
		return NULL;
	}

	//查找插入点


	int32_t ShiftUp(int32_t nIndex)
	{
		int32_t	iP = 0;
		int32_t	iC = nIndex;
		int32_t ret = enmInvalidHeapIndex;

		if ((iC <= 0) || (iC >= m_nObjectCount))
		{
			return S_OK;//这里应该修改为返回一个溢出错误提示
		}

		while(0 != iC)
		{
			//确定父节点iP,比如A[2]和A[1]的父亲节点是A[0]
			iP = (iC - 1) / 2;

			//如果iC对应的节点大于父节点就停止上翻
			if (m_arrIndexPrt[iP]->m_key <= m_arrIndexPrt[iC]->m_key)
			{
				break;
			}

			//交换数据
			Swap(m_arrIndexPrt[iP], m_arrIndexPrt[iC]);

			//iC的位置上移到之前父节点
			iC = iP;
		}
		return S_OK;
	}
	int32_t ShiftDown(int32_t nIndex)
	{
		int32_t	iL = 0;
		int32_t	iR = 0;
		int32_t	iC = nIndex;
		int32_t	iSmallest = iC;
		bool		bOut = false;

		if ((iC < 0) || (iC >= m_nObjectCount))
		{
			return S_OK;//需要修改成错误返回值
		}

		do
		{
			//确定左子节点
			iL =  (iC << 1) + 1;
			//确定右子节点
			iR =  (iC << 1) + 2;

			//如果左子节点存在
			if (iL < m_nObjectCount)
			{
				if (m_arrIndexPrt[iL]->m_key < m_arrIndexPrt[iC]->m_key)
				{
					iSmallest = iL;
				}
				else
				{
					iSmallest = iC;
				}
			}

			//如果右子节点存在
			if ((iR < m_nObjectCount) && (m_arrIndexPrt[iR]->m_key < m_arrIndexPrt[iSmallest]->m_key))
			{
				iSmallest = iR;
			}

			//如果最小节点不是当前节点，进行交换
			if (iC != iSmallest)
			{
				Swap(m_arrIndexPrt[iC], m_arrIndexPrt[iSmallest]);
				iC = iSmallest;
			}
			else
			{
				bOut = true;
			}

		} while (!bOut);

		return S_OK;
	}


/*
#define ReserseIndex(nIndex) (ObjectCount() - nIndex - 1)

	//逆建最大堆，不包括第一个元素
	int32_t RShiftDown(int32_t nIndex)
	{

			//保护第一个元素
			if ((1 == nIndex) || (0 == nIndex))
			{
				return S_OK;
			}

			int32_t iC = 0;
			int32_t iL = 0;
			int32_t iR = 0;
			int32_t iBiggest = 0;
			bool bOut = false;
			int32_t iTotalSize = ObjectCount() - 1;

			do
			{
				iC = ReverseIndex(nIndex);
				iL = iC * 2 + 1;
				iR = iL + 1;

				if (iL < iTotalSize)
				{
					if (m_arrIndexPrt[ReverseIndex(iL)] > m_arrIndexPrt[ReverseIndex(iC)])
					{
						iBiggest = iL;
					}
					else
					{
						iBiggest = iC;
					}
				}

				if (iC < iTotalSize && (m_arrIndexPrt[ReverseIndex(iC)]->GetKey() >
					m_arrIndexPrt[ReverseIndex(iBiggest)]->GetKey()))
				{
					iBiggest = iR;
				}

				if (iC != iBiggest)
				{
					Swap(m_arrIndexPrt[ReserseIndex(iC)], ReserseIndex(iBiggest));
					iC = iBiggest;
				}else
				{
					bOut = true;
				}
			} while (!bOut);

			return S_OK;
	}*/
protected:
	uint32_t		m_nObjectCount;		//已分配对象的个数
	uint32_t		m_nFreeCount;		//未分配对象的个数
	CIndex*			m_pFreeHead;		//未分配对象链头部对象的索引
	CIndex*			m_pFreeTail;		//未分配对象链尾部对象的索引
	CIndex*			m_pUsedHead;		//已分配对象链头部对象的索引
	CIndex*			m_pUsedTail;		//已分配对象链尾部对象的索引

	CIndex			m_arrIndex[CAPACITY];	//对象索引区
	CIndex*		m_arrIndexPrt[CAPACITY];//指针堆区
};

#endif /* COMMON_HEAP_H_ */
