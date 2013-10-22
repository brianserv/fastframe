/*
 * common_objectpool.h
 *
 *  Created on: 2013Äê8ÔÂ22ÈÕ
 *      Author: 1854
 */

#ifndef COMMON_OBJECTPOOL_H_
#define COMMON_OBJECTPOOL_H_

#include "common_typedef.h"

#include <list>
using namespace std;

template<typename T, int CAPACITY>
class ObjectPool
{
public:
	ObjectPool(){init();};
	~ObjectPool(){destroy();};

	void init()
	{
		T* obj = NULL;
		for(uint32_t i = 0; i < CAPACITY; i ++)
		{
			obj = new T;
			m_stObjectList.push_back(obj);
		}
	};

	void destroy()
	{
		T* obj = NULL;
		while(!m_stObjectList.empty())
		{
			obj = m_stObjectList.front();
			if(obj != NULL)
			{
				delete obj;
				obj = NULL;
			}

			m_stObjectList.pop_front();
		}
	}

	void alloc_object(uint32_t number)
	{
		T* obj = NULL;
		for(uint32_t i = 0; i < number; i ++)
		{
			obj = new T;
			m_stObjectList.push_back(obj);
		}
	}

	void push_obj(T* obj)
	{
		if(obj == NULL)
			return ;

		m_stObjectList.push_back(obj);
	}

	T* pop_obj()
	{
		if(m_stObjectList.empty())
		{
			alloc_object(CAPACITY / 2);
		}

		T* obj = m_stObjectList.front();
		m_stObjectList.pop_front();

		return obj;
	}

	size_t size()
	{
		return m_stObjectList.size();
	};
private:
	list<T*>	m_stObjectList;
};

#endif /* COMMON_OBJECTPOOL_H_ */
