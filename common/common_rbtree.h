/*
 * common_rbtree.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_RBTREE_H_
#define COMMON_RBTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"


//声明一个红黑树管理模板类
template<typename TKEY, typename T, uint32_t CAPACITY>
class CRBTree : public CObject
{
public:
	enum
	{
		enmIndexFlag_Free				= 0,		//对象索引空闲状态
		enmIndexFlag_Used				= 1,		//对象索引已使用状态
		enmInvalidObjectID				= -1,		//无效的对象ID
	};

	enum
	{
		enmNodeColor_Red				= 0,		//红节点
		enmNodeColor_Black				= 1,		//黑节点
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
			//m_pNext = NULL;
			//m_pPrev = NULL;
			m_nNext = enmInvalidObjectID;
			m_nPrev = enmInvalidObjectID;

			m_nColor = enmNodeColor_Red;
			//m_pParent = NULL;
			//m_pLeft = NULL;
			//m_pRight = NULL;
			m_nParent = enmInvalidObjectID;
			m_nLeft = enmInvalidObjectID;
			m_nRight = enmInvalidObjectID;

			memset(&m_key, 0, sizeof(m_key));

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
			//m_pNext = NULL;
			//m_pPrev = NULL;
			m_nNext = enmInvalidObjectID;
			m_nPrev = enmInvalidObjectID;

			m_nColor = enmNodeColor_Red;
			//m_pParent = NULL;
			//m_pLeft = NULL;
			//m_pRight = NULL;
			m_nParent = enmInvalidObjectID;
			m_nLeft = enmInvalidObjectID;
			m_nRight = enmInvalidObjectID;

			memset(&m_key, 0, sizeof(m_key));

			//m_object.Uninitialize();

			return S_OK;
		}

	public:
		////获取红黑树的前一节点
		//CIndex* Prev() const
		//{
		//	if (NULL != m_pLeft)
		//	{
		//		return m_pLeft->MaxNode();
		//	}

		//	const CIndex* pNode = this;
		//	const CIndex* pParent = m_pParent;
		//	while (NULL != pParent)
		//	{
		//		if (pNode != pParent->GetLeft())
		//		{
		//			return (CIndex*)pParent;
		//		}
		//		pNode = pParent;
		//		pParent = pParent->GetParent();
		//	}

		//	return NULL;
		//}
		////获取红黑树的后一节点
		//CIndex* Next() const
		//{
		//	if (NULL != m_pRight)
		//	{
		//		return m_pRight->MinNode();
		//	}

		//	const CIndex* pNode = this;
		//	const CIndex* pParent = m_pParent;
		//	while (NULL != pParent)
		//	{
		//		if (pNode != pParent->GetRight())
		//		{
		//			return (CIndex*)pParent;
		//		}
		//		pNode = pParent;
		//		pParent = pParent->GetParent();
		//	}

		//	return NULL;
		//}
		//获取红黑树的前一节点
		CIndex* Prev()
		{
			if (enmInvalidObjectID != m_nLeft)
			{
				return Left()->MaxNode();
			}

			CIndex* pNode = this;
			CIndex* pParent = Parent();
			while (NULL != pParent)
			{
				if (pNode != pParent->Left())
				{
					return pParent;
				}
				pNode = pParent;
				pParent = pParent->Parent();
			}

			return NULL;
		}
		//获取红黑树的后一节点
		CIndex* Next()
		{
			if (enmInvalidObjectID != m_nRight)
			{
				return Right()->MinNode();
			}

			CIndex* pNode = this;
			CIndex* pParent = Parent();
			while (NULL != pParent)
			{
				if (pNode != pParent->Right())
				{
					return pParent;
				}
				pNode = pParent;
				pParent = pParent->Parent();
			}

			return NULL;
		}
		//获取链表的后一节点
		inline CIndex* NextNode()
		{
			if (enmInvalidObjectID == m_nNext)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nNext];
		}
		//获取链表的前一节点
		inline CIndex* PrevNode()
		{
			if (enmInvalidObjectID == m_nPrev)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nPrev];
		}

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
		inline T& Object()
		{
			return m_object.GetValue();
		}
		//设置对象
		inline void SetObject(T& object)
		{
			m_object.SetValue(object);
		}

		//获取对象ID
		inline int32_t Index()
		{
			return m_nIndex;
		}

	protected:
		//设置父对象指针
		inline void SetContainer(CRBTree* pContainer)
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

		//获取节点颜色
		inline uint32_t GetColor()
		{
			return m_nColor;
		}
		//设置节点颜色
		inline void SetColor(uint32_t nColor)
		{
			m_nColor = nColor;
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

		////获取链表的父节点
		//inline CIndex* GetParent() const
		//{
		//	return m_pParent;
		//}
		////获取链表的左子节点
		//inline CIndex* GetLeft() const
		//{
		//	return m_pLeft;
		//}
		////获取链表的右子节点
		//inline CIndex* GetRight() const
		//{
		//	return m_pRight;
		//}
		////设置链表的父节点
		//inline void SetParent(CIndex* pParent)
		//{
		//	m_pParent = pParent;
		//}
		////设置链表的左子节点
		//inline void SetLeft(CIndex* pLeft)
		//{
		//	m_pLeft = pLeft;
		//}
		////设置链表的右子节点
		//inline void SetRight(CIndex* pRight)
		//{
		//	m_pRight = pRight;
		//}
		//获取链表的父节点
		inline int32_t GetParent() const
		{
			return m_nParent;
		}
		//获取链表的左子节点
		inline int32_t GetLeft() const
		{
			return m_nLeft;
		}
		//获取链表的右子节点
		inline int32_t GetRight() const
		{
			return m_nRight;
		}
		//设置链表的父节点
		inline void SetParent(int32_t nParent)
		{
			m_nParent = nParent;
		}
		//设置链表的左子节点
		inline void SetLeft(int32_t nLeft)
		{
			m_nLeft = nLeft;
		}
		//设置链表的右子节点
		inline void SetRight(int32_t nRight)
		{
			m_nRight = nRight;
		}
		//设置链表的父节点
		inline CIndex* Parent()
		{
			if (enmInvalidObjectID == m_nParent)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nParent];
		}
		//获取链表的左子节点
		inline CIndex* Left()
		{
			if (enmInvalidObjectID == m_nLeft)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nLeft];
		}
		//获取链表的右子节点
		inline CIndex* Right()
		{
			if (enmInvalidObjectID == m_nRight)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nRight];
		}

		//设置对象ID
		inline void SetIndex(int32_t nIndex)
		{
			m_nIndex = nIndex;
		}

		//设置键值
		inline void SetKey(TKEY key)
		{
			m_key = key;
		}

		////复位红黑树节点
		//inline void ResetRBNode()
		//{
		//	m_pParent = NULL;
		//	m_pLeft = NULL;
		//	m_pRight = NULL;
		//	memset(&m_key, 0, sizeof(m_key));
		//}
		//复位红黑树节点
		inline void ResetRBNode()
		{
			m_nParent = enmInvalidObjectID;
			m_nLeft = enmInvalidObjectID;
			m_nRight = enmInvalidObjectID;
			memset(&m_key, 0, sizeof(m_key));
		}

		////获取子树的最小节点
		//CIndex* MinNode()
		//{
		//	CIndex* pNode = this;

		//	while (NULL != pNode->GetLeft())
		//	{
		//		pNode = pNode->GetLeft();
		//	}

		//	return pNode;
		//}
		////获取子树的最大节点
		//CIndex* MaxNode()
		//{
		//	CIndex* pNode = this;

		//	while (NULL != pNode->GetRight())
		//	{
		//		pNode = pNode->GetRight();
		//	}

		//	return pNode;
		//}
		//获取子树的最小节点
		CIndex* MinNode()
		{
			CIndex* pNode = this;

			while (NULL != pNode->Left())
			{
				pNode = pNode->Left();
			}

			return pNode;
		}
		//获取子树的最大节点
		CIndex* MaxNode()
		{
			CIndex* pNode = this;

			while (NULL != pNode->Right())
			{
				pNode = pNode->Right();
			}

			return pNode;
		}

	protected:
		CRBTree*		m_pContainer;

		uint32_t		m_nFlag;		//索引状态标志
		int32_t			m_nIndex;		//对象索引号
		//CIndex*			m_pNext;		//后一节点，用于内存管理
		//CIndex*			m_pPrev;		//前一节点，用于内存管理
		int32_t			m_nNext;		//后一节点，用于内存管理
		int32_t			m_nPrev;		//前一节点，用于内存管理

		uint32_t		m_nColor;		//节点颜色
		//CIndex*			m_pParent;		//父节点
		//CIndex*			m_pLeft;		//左子节点
		//CIndex*			m_pRight;		//右子节点
		int32_t			m_nParent;		//父节点
		int32_t			m_nLeft;		//左子节点
		int32_t			m_nRight;		//右子节点

		TKEY			m_key;			//键值
		CComplexType<T>	m_object;		//关联的对象

		friend class CRBTree;
	};


public:
	CRBTree()
	{
	}
	virtual ~CRBTree()
	{
	}

public:
	//初始化红黑树
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

		//m_pRoot = NULL;
		m_nRoot = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Initialize();
		}

		BuildIndexList();

		return S_OK;
	}
	//恢复红黑树
	virtual int32_t Resume()
	{
		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Resume();
		}
		return S_OK;
	}
	//注销红黑树
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

		//m_pRoot = NULL;
		m_nRoot = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Uninitialize();
		}

		BuildIndexList();

		return S_OK;
	}

public:
	////插入对象
	//CIndex* Insert(TKEY key, T object)
	//{
	//	CIndex* pParent = NULL;
	//	CIndex* pNode = NULL;

	//	pParent = FindInsertPostion(key);

	//	//创建索引
	//	pNode = CreateIndex();
	//	if (NULL == pNode)
	//	{
	//		return NULL;
	//	}

	//	pNode->SetParent(pParent);
	//	pNode->SetLeft(NULL);
	//	pNode->SetRight(NULL);
	//	pNode->SetColor(enmNodeColor_Red);

	//	pNode->SetKey(key);
	//	pNode->SetObject(object);

	//	if (NULL != pParent)
	//	{
	//		if (pParent->GetKey() > key)
	//		{
	//			pParent->SetLeft(pNode);
	//		}
	//		else
	//		{
	//			pParent->SetRight(pNode);
	//		}
	//	}
	//	else
	//	{
	//		m_pRoot = pNode;
	//	}

	//	InsertRebalance(pNode);

	//	return pNode;
	//}
	////删除对象
	//uint32_t Erase(TKEY key)
	//{
	//	CIndex* pNode = NULL;
	//	CIndex* pNext = NULL;

	//	pNode = FindNode(key);
	//	while (NULL != pNode)
	//	{
	//		Erase(pNode);
	//		pNode = FindNode(key);
	//	}

	//	return S_OK;
	//}
	////删除对象
	//uint32_t Erase(CIndex* pNode)
	//{
	//	CIndex* pChild = NULL;
	//	CIndex* pParent = NULL;
	//	CIndex* pOld = NULL;
	//	//CIndex* pLeft = NULL;
	//	uint32_t nColor = enmNodeColor_Red;

	//	pOld = pNode;
	//	if ((NULL != pNode->GetLeft()) && (NULL != pNode->GetRight()))
	//	{
	//		pNode = pNode->GetRight();
	//		while (NULL != pNode->GetLeft())
	//		{
	//			pNode = pNode->GetLeft();
	//		}
	//		pChild = pNode->GetRight();
	//		pParent = pNode->GetParent();
	//		nColor = pNode->GetColor();

	//		if (NULL != pChild)
	//		{
	//			pChild->SetParent(pParent);
	//		}
	//		if (NULL != pParent)
	//		{
	//			if (pParent->GetLeft() == pNode)
	//			{
	//				pParent->SetLeft(pChild);
	//			}
	//			else
	//			{
	//				pParent->SetRight(pChild);
	//			}
	//		}
	//		else
	//		{
	//			m_pRoot = pChild;
	//		}

	//		if (pNode->GetParent() == pOld)
	//		{
	//			pParent = pNode;
	//		}

	//		pNode->SetParent(pOld->GetParent());
	//		pNode->SetColor(pOld->GetColor());
	//		pNode->SetRight(pOld->GetRight());
	//		pNode->SetLeft(pOld->GetLeft());

	//		if (NULL != pOld->GetParent())
	//		{
	//			if (pOld->GetParent()->GetLeft() == pOld)
	//			{
	//				pOld->GetParent()->SetLeft(pNode);
	//			}
	//			else
	//			{
	//				pOld->GetParent()->SetRight(pNode);
	//			}
	//		}
	//		else
	//		{
	//			m_pRoot = pNode;
	//		}

	//		pOld->GetLeft()->SetParent(pNode);
	//		if (NULL != pOld->GetRight())
	//		{
	//			pOld->GetRight()->SetParent(pNode);
	//		}
	//	}
	//	else
	//	{
	//		if (NULL == pNode->GetLeft())
	//		{
	//			pChild = pNode->GetRight();
	//		}
	//		else if (NULL == pNode->GetRight())
	//		{
	//			pChild = pNode->GetLeft();
	//		}
	//		pParent = pNode->GetParent();
	//		nColor = pNode->GetColor();

	//		if (NULL != pChild)
	//		{
	//			pChild->SetParent(pParent);
	//		}
	//		if (NULL != pParent)
	//		{
	//			if (pParent->GetLeft() == pNode)
	//			{
	//				pParent->SetLeft(pChild);
	//			}
	//			else
	//			{
	//				pParent->SetRight(pChild);
	//			}
	//		}
	//		else
	//		{
	//			m_pRoot = pChild;
	//		}
	//	}

	//	DestroyIndex(pOld);
	//	pOld->ResetRBNode();

	//	if (enmNodeColor_Black == nColor)
	//	{
	//		EraseRebalance(pChild, pParent);
	//	}

	//	return S_OK;
	//}
	//插入对象
	CIndex* Insert(TKEY key, T object)
	{
		CIndex* pParent = NULL;
		CIndex* pNode = NULL;

		pParent = FindInsertPostion(key);

		//创建索引
		pNode = CreateIndex();
		if (NULL == pNode)
		{
			return NULL;
		}

		pNode->SetParent((NULL == pParent) ? enmInvalidObjectID : pParent->Index());
		pNode->SetLeft(enmInvalidObjectID);
		pNode->SetRight(enmInvalidObjectID);
		pNode->SetColor(enmNodeColor_Red);

		pNode->SetKey(key);
		pNode->SetObject(object);

		if (NULL != pParent)
		{
			if (pParent->GetKey() > key)
			{
				pParent->SetLeft(pNode->Index());
			}
			else
			{
				pParent->SetRight(pNode->Index());
			}
		}
		else
		{
			m_nRoot = pNode->Index();
		}

		InsertRebalance(pNode);

		return pNode;
	}
	//删除对象
	uint32_t Erase(TKEY key)
	{
		CIndex* pNode = NULL;

		pNode = FindNode(key);
		while (NULL != pNode)
		{
			Erase(pNode);
			pNode = FindNode(key);
		}

		return S_OK;
	}
	//删除对象
	uint32_t Erase(CIndex* pNode)
	{
		CIndex* pChild = NULL;
		CIndex* pParent = NULL;
		CIndex* pOld = NULL;
		//CIndex* pLeft = NULL;
		uint32_t nColor = enmNodeColor_Red;

		pOld = pNode;
		if ((NULL != pNode->Left()) && (NULL != pNode->Right()))
		{
			pNode = pNode->Right();
			while (NULL != pNode->Left())
			{
				pNode = pNode->Left();
			}
			pChild = pNode->Right();
			pParent = pNode->Parent();
			nColor = pNode->GetColor();

			if (NULL != pChild)
			{
				pChild->SetParent((NULL == pParent) ? enmInvalidObjectID : pParent->Index());
			}
			if (NULL != pParent)
			{
				if (pParent->Left() == pNode)
				{
					pParent->SetLeft((NULL == pChild) ? enmInvalidObjectID : pChild->Index());
				}
				else
				{
					pParent->SetRight((NULL == pChild) ? enmInvalidObjectID : pChild->Index());
				}
			}
			else
			{
				m_nRoot = (NULL == pChild) ? enmInvalidObjectID : pChild->Index();
			}

			if (pNode->Parent() == pOld)
			{
				pParent = pNode;
			}

			pNode->SetParent(pOld->GetParent());
			pNode->SetColor(pOld->GetColor());
			pNode->SetRight(pOld->GetRight());
			pNode->SetLeft(pOld->GetLeft());

			if (NULL != pOld->Parent())
			{
				if (pOld->Parent()->Left() == pOld)
				{
					pOld->Parent()->SetLeft(pNode->Index());
				}
				else
				{
					pOld->Parent()->SetRight(pNode->Index());
				}
			}
			else
			{
				m_nRoot = pNode->Index();
			}

			pOld->Left()->SetParent(pNode->Index());
			if (NULL != pOld->Right())
			{
				pOld->Right()->SetParent(pNode->Index());
			}
		}
		else
		{
			if (NULL == pNode->Left())
			{
				pChild = pNode->Right();
			}
			else if (NULL == pNode->Right())
			{
				pChild = pNode->Left();
			}
			pParent = pNode->Parent();
			nColor = pNode->GetColor();

			if (NULL != pChild)
			{
				pChild->SetParent((NULL == pParent) ? enmInvalidObjectID : pParent->Index());
			}
			if (NULL != pParent)
			{
				if (pParent->Left() == pNode)
				{
					pParent->SetLeft((NULL == pChild) ? enmInvalidObjectID : pChild->Index());
				}
				else
				{
					pParent->SetRight((NULL == pChild) ? enmInvalidObjectID : pChild->Index());
				}
			}
			else
			{
				m_nRoot = (NULL == pChild) ? enmInvalidObjectID : pChild->Index();
			}
		}

		DestroyIndex(pOld);
		pOld->ResetRBNode();

		if (enmNodeColor_Black == nColor)
		{
			EraseRebalance(pChild, pParent);
		}

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
	////获取最小对象索引
	//CIndex* First() const
	//{
	//	if (NULL == m_pRoot)
	//	{
	//		return NULL;
	//	}

	//	return m_pRoot->GetMin();
	//}
	////获取最大对象索引
	//CIndex* Last() const
	//{
	//	if (NULL == m_pRoot)
	//	{
	//		return NULL;
	//	}

	//	return m_pRoot->GetMax();
	//}
	//获取最小对象索引
	inline CIndex* First()
	{
		if (enmInvalidObjectID == m_nRoot)
		{
			return NULL;
		}

		return m_arrIndex[m_nRoot].MinNode();
	}
	//获取最大对象索引
	inline CIndex* Last()
	{
		if (enmInvalidObjectID == m_nRoot)
		{
			return NULL;
		}

		return m_arrIndex[m_nRoot].MaxNode();
	}
	//获取头部对象索引
	inline CIndex* FirstNode()
	{
		return (enmInvalidObjectID == m_nUsedHead) ? NULL : &m_arrIndex[m_nUsedHead];
	}
	//获取尾部对象索引
	inline CIndex* LastNode()
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
	//	m_arrIndex[0].SetPrev(NULL);

	//	for (uint32_t i = 1; i < CAPACITY; ++i)
	//	{
	//		m_arrIndex[i - 1].SetNext(&m_arrIndex[i]);
	//		m_arrIndex[i].SetPrev(&m_arrIndex[i - 1]);
	//	}

	//	//设置最后一个索引
	//	m_arrIndex[CAPACITY - 1].SetNext(NULL);

	//	return S_OK;
	//}
	//对索引区建立链表
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
	CIndex* CreateIndex()
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
	int32_t DestroyIndex(CIndex* pIndex)
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
			pIndex->PrevNode()->SetNext(pIndex->GetNext());
		}
		if (enmInvalidObjectID != pIndex->GetNext())
		{
			pIndex->NextNode()->SetPrev(pIndex->GetPrev());
		}

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

	//获取根节点
	inline CIndex* Root()
	{
		if (enmInvalidObjectID == m_nRoot)
		{
			return NULL;
		}
		return &m_arrIndex[m_nRoot];
	}

protected:
	//void RotateLeft(CIndex* pNode)
	//{
	//	CIndex* pRight = pNode->GetRight();

	//	pNode->SetRight(pRight->GetLeft());
	//	if (NULL != pNode->GetRight())
	//	{
	//		pRight->GetLeft()->SetParent(pNode);
	//	}
	//	pRight->SetLeft(pNode);

	//	pRight->SetParent(pNode->GetParent());
	//	if (NULL != pRight->GetParent())
	//	{
	//		if (pNode == pNode->GetParent()->GetRight())
	//		{
	//			pNode->GetParent()->SetRight(pRight);
	//		}
	//		else
	//		{
	//			pNode->GetParent()->SetLeft(pRight);
	//		}
	//	}
	//	else
	//	{
	//		m_pRoot = pRight;
	//	}
	//	pNode->SetParent(pRight);
	//}
	//void RotateRight(CIndex* pNode)
	//{
	//	CIndex* pLeft = pNode->GetLeft();

	//	pNode->SetLeft(pLeft->GetRight());
	//	if (NULL != pNode->GetLeft())
	//	{
	//		pLeft->GetRight()->SetParent(pNode);
	//	}
	//	pLeft->SetRight(pNode);

	//	pLeft->SetParent(pNode->GetParent());
	//	if (NULL != pLeft->GetParent())
	//	{
	//		if (pNode == pNode->GetParent()->GetRight())
	//		{
	//			pNode->GetParent()->SetRight(pLeft);
	//		}
	//		else
	//		{
	//			pNode->GetParent()->SetLeft(pLeft);
	//		}
	//	}
	//	else
	//	{
	//		m_pRoot = pLeft;
	//	}
	//	pNode->SetParent(pLeft);
	//}
	/*-----------------------------------------------------------
	|   node           right
	|   / \    ==>     / \
	|   a  right     node  y
	|       / \           / \
	|       b  y         a   b
	-----------------------------------------------------------*/
	void RotateLeft(CIndex* pNode)
	{
		CIndex* pRight = pNode->Right();

		pNode->SetRight(pRight->GetLeft());
		if (NULL != pNode->Right())
		{
			pRight->Left()->SetParent(pNode->Index());
		}
		pRight->SetLeft(pNode->Index());

		pRight->SetParent(pNode->GetParent());
		if (NULL != pRight->Parent())
		{
			if (pNode == pNode->Parent()->Right())
			{
				pNode->Parent()->SetRight(pRight->Index());
			}
			else
			{
				pNode->Parent()->SetLeft(pRight->Index());
			}
		}
		else
		{
			m_nRoot = pRight->Index();
		}
		pNode->SetParent(pRight->Index());
	}
	/*-----------------------------------------------------------
	|       node           left
	|       / \            / \
	|    left  y   ==>    a   node
	|   / \               / \
	|  a   b             b   y
	-----------------------------------------------------------*/
	void RotateRight(CIndex* pNode)
	{
		CIndex* pLeft = pNode->Left();

		pNode->SetLeft(pLeft->GetRight());
		if (NULL != pNode->Left())
		{
			pLeft->Right()->SetParent(pNode->Index());
		}
		pLeft->SetRight(pNode->Index());

		pLeft->SetParent(pNode->GetParent());
		if (NULL != pLeft->Parent())
		{
			if (pNode == pNode->Parent()->Right())
			{
				pNode->Parent()->SetRight(pLeft->Index());
			}
			else
			{
				pNode->Parent()->SetLeft(pLeft->Index());
			}
		}
		else
		{
			m_nRoot = pLeft->Index();
		}
		pNode->SetParent(pLeft->Index());
	}

	////插入节点后重新平衡
	//void InsertRebalance(CIndex* pNode)
	//{
	//	CIndex* pParent = NULL;
	//	CIndex* pGParent = NULL;
	//	CIndex* pUncle = NULL;
	//	CIndex* p = NULL;

	//	pParent = pNode->GetParent();
	//	while ((NULL != pParent) && (enmNodeColor_Red == pParent->GetColor()))
	//	{
	//		pGParent = pParent->GetParent();

	//		if (pParent == pGParent->GetLeft())
	//		{
	//			pUncle = pGParent->GetRight();
	//			if ((NULL != pUncle) && (enmNodeColor_Red == pUncle->GetColor()))
	//			{
	//				pUncle->SetColor(enmNodeColor_Black);
	//				pParent->SetColor(enmNodeColor_Black);
	//				pGParent->SetColor(enmNodeColor_Red);
	//				pNode = pGParent;
	//			}
	//			else
	//			{
	//				if (pParent->GetRight() == pNode)
	//				{
	//					RotateLeft(pParent);
	//					p = pParent;
	//					pParent = pNode;
	//					pNode = p;
	//				}

	//				pParent->SetColor(enmNodeColor_Black);
	//				pGParent->SetColor(enmNodeColor_Red);
	//				RotateRight(pGParent);
	//			}
	//		}
	//		else
	//		{
	//			pUncle = pGParent->GetLeft();
	//			if ((NULL != pUncle) && (enmNodeColor_Red == pUncle->GetColor()))
	//			{
	//				pUncle->SetColor(enmNodeColor_Black);
	//				pParent->SetColor(enmNodeColor_Black);
	//				pGParent->SetColor(enmNodeColor_Red);
	//				pNode = pGParent;
	//			}
	//			else
	//			{
	//				if (pParent->GetLeft() == pNode)
	//				{
	//					RotateRight(pParent);
	//					p = pParent;
	//					pParent = pNode;
	//					pNode = p;
	//				}

	//				pParent->SetColor(enmNodeColor_Black);
	//				pGParent->SetColor(enmNodeColor_Red);
	//				RotateLeft(pGParent);
	//			}
	//		}

	//		pParent = pNode->GetParent();
	//	}

	//	m_pRoot->SetColor(enmNodeColor_Black);
	//}

	////删除节点后重新平衡
	//void EraseRebalance(CIndex* pNode, CIndex* pParent)
	//{
	//	CIndex* pOther = NULL;
	//	CIndex* pOtherLeft = NULL;
	//	CIndex* pOtherRight = NULL;

	//	while (((NULL == pNode) || (enmNodeColor_Black == pNode->GetColor())) && (pNode != m_pRoot))
	//	{
	//		if (pParent->GetLeft() == pNode)
	//		{
	//			pOther = pParent->GetRight();
	//			if (enmNodeColor_Red == pOther->GetColor())
	//			{
	//				pOther->SetColor(enmNodeColor_Black);
	//				pParent->SetColor(enmNodeColor_Red);
	//				RotateLeft(pParent);
	//				pOther = pParent->GetRight();
	//			}
	//			if (((NULL == pOther->GetLeft()) || (enmNodeColor_Black == pOther->GetLeft()->GetColor())) &&
	//				((NULL == pOther->GetRight()) || (enmNodeColor_Black == pOther->GetRight()->GetColor())))
	//			{
	//				pOther->SetColor(enmNodeColor_Red);
	//				pNode = pParent;
	//				pParent = pNode->GetParent();
	//			}
	//			else
	//			{
	//				if ((NULL == pOther->GetRight()) || (enmNodeColor_Black == pOther->GetRight()->GetColor()))
	//				{
	//					pOtherLeft = pOther->GetLeft();
	//					if (NULL != pOtherLeft)
	//					{
	//						pOtherLeft->SetColor(enmNodeColor_Black);
	//					}
	//					pOther->SetColor(enmNodeColor_Red);
	//					RotateRight(pOther);
	//					pOther = pParent->GetRight();
	//				}
	//				pOther->SetColor(pParent->GetColor());
	//				pParent->SetColor(enmNodeColor_Black);
	//				if (NULL != pOther->GetRight())
	//				{
	//					pOther->GetRight()->SetColor(enmNodeColor_Black);
	//				}
	//				RotateLeft(pParent);
	//				pNode = m_pRoot;
	//				break;
	//			}
	//		}
	//		else
	//		{
	//			pOther = pParent->GetLeft();
	//			if (enmNodeColor_Red == pOther->GetColor())
	//			{
	//				pOther->SetColor(enmNodeColor_Black);
	//				pParent->SetColor(enmNodeColor_Red);
	//				RotateRight(pParent);
	//				pOther = pParent->GetLeft();
	//			}
	//			if (((NULL == pOther->GetLeft()) || (enmNodeColor_Black == pOther->GetLeft()->GetColor())) &&
	//				 ((NULL == pOther->GetRight()) || (enmNodeColor_Black == pOther->GetRight()->GetColor())))
	//			{
	//				pOther->SetColor(enmNodeColor_Red);
	//				pNode = pParent;
	//				pParent = pNode->GetParent();
	//			}
	//			else
	//			{
	//				if ((NULL == pOther->GetLeft()) || (enmNodeColor_Black == pOther->GetLeft()->GetColor()))
	//				{
	//					pOtherRight = pOther->GetRight();
	//					if (NULL != pOtherRight)
	//					{
	//						pOtherRight->SetColor(enmNodeColor_Black);
	//					}
	//					pOther->SetColor(enmNodeColor_Red);
	//					RotateLeft(pOther);
	//					pOther = pParent->GetLeft();
	//				}
	//				pOther->SetColor(pParent->GetColor());
	//				pParent->SetColor(enmNodeColor_Black);
	//				if (NULL != pOther->GetLeft())
	//				{
	//					pOther->GetLeft()->SetColor(enmNodeColor_Black);
	//				}
	//				RotateRight(pParent);
	//				pNode = m_pRoot;
	//				break;
	//			}
	//		}
	//	}

	//	if (NULL != pNode)
	//	{
	//		pNode->SetColor(enmNodeColor_Black);
	//	}
	//}
	//插入节点后重新平衡
	void InsertRebalance(CIndex* pNode)
	{
		CIndex* pParent = NULL;
		CIndex* pGParent = NULL;
		CIndex* pUncle = NULL;
		CIndex* p = NULL;

		pParent = pNode->Parent();
		while ((NULL != pParent) && (enmNodeColor_Red == pParent->GetColor()))
		{
			pGParent = pParent->Parent();

			if (pParent == pGParent->Left())
			{
				pUncle = pGParent->Right();
				if ((NULL != pUncle) && (enmNodeColor_Red == pUncle->GetColor()))
				{
					pUncle->SetColor(enmNodeColor_Black);
					pParent->SetColor(enmNodeColor_Black);
					pGParent->SetColor(enmNodeColor_Red);
					pNode = pGParent;
				}
				else
				{
					if (pParent->Right() == pNode)
					{
						RotateLeft(pParent);
						p = pParent;
						pParent = pNode;
						pNode = p;
					}

					pParent->SetColor(enmNodeColor_Black);
					pGParent->SetColor(enmNodeColor_Red);
					RotateRight(pGParent);
				}
			}
			else
			{
				pUncle = pGParent->Left();
				if ((NULL != pUncle) && (enmNodeColor_Red == pUncle->GetColor()))
				{
					pUncle->SetColor(enmNodeColor_Black);
					pParent->SetColor(enmNodeColor_Black);
					pGParent->SetColor(enmNodeColor_Red);
					pNode = pGParent;
				}
				else
				{
					if (pParent->Left() == pNode)
					{
						RotateRight(pParent);
						p = pParent;
						pParent = pNode;
						pNode = p;
					}

					pParent->SetColor(enmNodeColor_Black);
					pGParent->SetColor(enmNodeColor_Red);
					RotateLeft(pGParent);
				}
			}

			pParent = pNode->Parent();
		}

		Root()->SetColor(enmNodeColor_Black);
	}

	//删除节点后重新平衡
	void EraseRebalance(CIndex* pNode, CIndex* pParent)
	{
		CIndex* pOther = NULL;
		CIndex* pOtherLeft = NULL;
		CIndex* pOtherRight = NULL;

		while (((NULL == pNode) || (enmNodeColor_Black == pNode->GetColor())) && (pNode != Root()))
		{
			if (pParent->Left() == pNode)
			{
				pOther = pParent->Right();
				if (enmNodeColor_Red == pOther->GetColor())
				{
					pOther->SetColor(enmNodeColor_Black);
					pParent->SetColor(enmNodeColor_Red);
					RotateLeft(pParent);
					pOther = pParent->Right();
				}
				if (((NULL == pOther->Left()) || (enmNodeColor_Black == pOther->Left()->GetColor())) &&
					((NULL == pOther->Right()) || (enmNodeColor_Black == pOther->Right()->GetColor())))
				{
					pOther->SetColor(enmNodeColor_Red);
					pNode = pParent;
					pParent = pNode->Parent();
				}
				else
				{
					if ((NULL == pOther->Right()) || (enmNodeColor_Black == pOther->Right()->GetColor()))
					{
						pOtherLeft = pOther->Left();
						if (NULL != pOtherLeft)
						{
							pOtherLeft->SetColor(enmNodeColor_Black);
						}
						pOther->SetColor(enmNodeColor_Red);
						RotateRight(pOther);
						pOther = pParent->Right();
					}
					pOther->SetColor(pParent->GetColor());
					pParent->SetColor(enmNodeColor_Black);
					if (NULL != pOther->Right())
					{
						pOther->Right()->SetColor(enmNodeColor_Black);
					}
					RotateLeft(pParent);
					pNode = Root();
					break;
				}
			}
			else
			{
				pOther = pParent->Left();
				if (enmNodeColor_Red == pOther->GetColor())
				{
					pOther->SetColor(enmNodeColor_Black);
					pParent->SetColor(enmNodeColor_Red);
					RotateRight(pParent);
					pOther = pParent->Left();
				}
				if (((NULL == pOther->Left()) || (enmNodeColor_Black == pOther->Left()->GetColor())) &&
					((NULL == pOther->Right()) || (enmNodeColor_Black == pOther->Right()->GetColor())))
				{
					pOther->SetColor(enmNodeColor_Red);
					pNode = pParent;
					pParent = pNode->Parent();
				}
				else
				{
					if ((NULL == pOther->Left()) || (enmNodeColor_Black == pOther->Left()->GetColor()))
					{
						pOtherRight = pOther->Right();
						if (NULL != pOtherRight)
						{
							pOtherRight->SetColor(enmNodeColor_Black);
						}
						pOther->SetColor(enmNodeColor_Red);
						RotateLeft(pOther);
						pOther = pParent->Left();
					}
					pOther->SetColor(pParent->GetColor());
					pParent->SetColor(enmNodeColor_Black);
					if (NULL != pOther->Left())
					{
						pOther->Left()->SetColor(enmNodeColor_Black);
					}
					RotateRight(pParent);
					pNode = Root();
					break;
				}
			}
		}

		if (NULL != pNode)
		{
			pNode->SetColor(enmNodeColor_Black);
		}
	}

	////查找节点
	//CIndex* FindNode(TKEY key)
	//{
	//	CIndex* pNode = m_pRoot;
	//	CIndex* pParent = NULL;

	//	while (NULL != pNode)
	//	{
	//		pParent = pNode;
	//		if (pNode->GetKey() > key)
	//		{
	//			pNode = pNode->GetLeft();
	//		}
	//		else if (pNode->GetKey() < key)
	//		{
	//			pNode = pNode->GetRight();
	//		}
	//		else
	//		{
	//			return pNode;
	//		}
	//	}

	//	return NULL;
	//}
	//查找节点
	CIndex* FindNode(TKEY key)
	{
		CIndex* pNode = Root();
		CIndex* pParent = NULL;

		while (NULL != pNode)
		{
			pParent = pNode;
			if (pNode->GetKey() > key)
			{
				pNode = pNode->Left();
			}
			else if (pNode->GetKey() < key)
			{
				pNode = pNode->Right();
			}
			else
			{
				return pNode;
			}
		}

		return NULL;
	}

	////查找插入点
	//CIndex* FindInsertPostion(TKEY key)
	//{
	//	CIndex* pNode = m_pRoot;
	//	CIndex* pParent = NULL;

	//	while (NULL != pNode)
	//	{
	//		pParent = pNode;
	//		if (pNode->GetKey() > key)
	//		{
	//			pNode = pNode->GetLeft();
	//		}
	//		else
	//		{
	//			pNode = pNode->GetRight();
	//		}
	//	}

	//	return pParent;
	//}
	//查找插入点
	CIndex* FindInsertPostion(TKEY key)
	{
		CIndex* pNode = Root();
		CIndex* pParent = NULL;

		while (NULL != pNode)
		{
			pParent = pNode;
			if (pNode->GetKey() > key)
			{
				pNode = pNode->Left();
			}
			else
			{
				pNode = pNode->Right();
			}
		}

		return pParent;
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

	//CIndex*			m_pRoot;			//根节点
	int32_t			m_nRoot;			//根节点

	CIndex			m_arrIndex[CAPACITY];	//对象索引区
};

#endif /* COMMON_RBTREE_H_ */
