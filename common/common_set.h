/*
 * common_set.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_SET_H_
#define COMMON_SET_H_

#include <stdio.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_complextype.h"

template<typename T, uint32_t	CAPACITY>
class CSet : public CObject
{
public:
	CSet()
	{
		memset(m_arrObject, 0, sizeof(m_arrObject));
	}
	~CSet()
	{

	}

	int32_t Initialize()
	{
		m_nCount = 0;
		return S_OK;
	}
	int32_t Resume()
	{
		return S_OK;
	}
	int32_t Uninitialize()
	{
		m_nCount = 0;
		return S_OK;
	}

	//插入数据一个元素
	int32_t Insert(const T & object)
	{
		if (CAPACITY == m_nCount)
		{
			return E_SETISFULL;
		}
		if (! Check(object))
		{
			m_arrObject[m_nCount++] = object;
		}
		return S_OK;
	}

	//删除一个元素
	int32_t Remove(const T & object)
	{
		if (0 == m_nCount)
		{
			return S_OK;
		}
		for (uint32_t i = 0; i < m_nCount; ++i)
		{
			if (object == m_arrObject[i])
			{
				m_arrObject[i] = m_arrObject[m_nCount - 1];
				--m_nCount;
				break;
			}
		}
		return S_OK;
	}

	//检查一个元素是否存在
	bool Check(const T & object)
	{
		for (uint32_t i = 0; i < m_nCount; ++i)
		{
			if (object == m_arrObject[i])
			{
				return true;
			}
		}
		return false;
	}

	//设置集合
	int32_t SetObjects(const uint32_t nCount, const T arrObject[])
	{
		if (CAPACITY < nCount)
		{
			return E_SETOUTOFCAPACITY;
		}

		for (uint32_t i = 0; i < nCount; ++i)
		{
			m_arrObject[i] = arrObject[i];
		}

		m_nCount = nCount;

		return S_OK;
	}

	//获取集合中的元素
	void GetObjects(uint32_t& nCount, T arrObject[], const uint32_t size)
	{
		for (nCount = 0; (nCount < size) && (nCount < m_nCount); ++nCount)
		{
			arrObject[nCount] = m_arrObject[nCount];
		}
	}

	//清空集合
	void Clear()
	{
		m_nCount = 0;
	}

	//获取集合大小
	uint32_t Size()
	{
		return m_nCount;
	}

	uint32_t Capacity()
	{
		return CAPACITY;
	}

	T* GetArray()
	{
		return m_arrObject;
	}
protected:
	T				m_arrObject[CAPACITY];
	uint32_t		m_nCount;
};

#endif /* COMMON_SET_H_ */
