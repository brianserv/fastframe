/*
 * common_safearray.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_SAFEARRAY_H_
#define COMMON_SAFEARRAY_H_

#include <stdio.h>
#include "common_typedef.h"

template<typename T, uint32_t CAPACITY>
class CSafeArray
{
public:
	CSafeArray()
	{

	}
	~CSafeArray()
	{

	}
	T& Get(uint32_t idx) const
	{
		return arrT[idx];
	}
	void Set(uint32_t idx, const T& t)
	{
		if (idx >= 0 && idx < CAPACITY)
		{
			arrT[idx] = t;
		}
	}
	uint32_t Size() const
	{
		return CAPACITY;
	}
	/**
	返回值的含义：拷贝的数量
	*/
	uint32_t Copy(const T* a, uint32_t size)
	{
		if (NULL == a)
		{
			return 0;
		}

		uint32_t i = 0;

		for (i = 0; i < size && i < CAPACITY; ++i)
		{
			arrT[i] = a[i];
		}

		return i;
	}

	T arrT[CAPACITY];
};

template<typename T, uint32_t CAPACITY>
class CSafeArrayP
{
public:
	CSafeArrayP(T* a) : arrT(a)
	{

	}
	~CSafeArrayP()
	{

	}
	T& Get(uint32_t idx) const
	{
		return arrT[idx];
	}
	void Set(uint32_t idx, const T& t)
	{
		if (idx >= 0 && idx < CAPACITY)
		{
			arrT[idx] = t;
		}
	}
	uint32_t Size() const
	{
		return CAPACITY;
	}
	uint32_t Copy(const T* a, uint32_t size)
	{
		if (NULL == a)
		{
			return  0;
		}
		uint32_t i = 0;

		for ( i = 0; i < size && i < CAPACITY; ++i)
		{
			arrT[i] = a[i];
		}

		return i;
	}

	T* arrT;
private:
	CSafeArrayP();
};

#endif /* COMMON_SAFEARRAY_H_ */
