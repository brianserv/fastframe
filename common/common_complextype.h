/*
 * common_complextype.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_COMPLEXTYPE_H_
#define COMMON_COMPLEXTYPE_H_

#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"


//复合类型模板类的全特化版本
#define CCOMPLEXTYPE_SPECIFICALLY(type)	\
	template<>	\
	class CComplexType<type>	\
	{	\
	public:	\
		inline int32_t Initialize()	\
		{	\
			return m_object.Initialize();	\
		}	\
		inline int32_t Resume()	\
		{	\
			return m_object.Resume();	\
		}	\
		inline int32_t Uninitialize()	\
		{	\
			return m_object.Uninitialize();	\
		}	\
		inline type& GetValue()	\
		{	\
			return m_object;	\
		}	\
		inline void SetValue(type& object)	\
		{	\
			m_object = object;	\
		}	\
	protected:	\
		type		m_object;	\
	};


//声明一个复合类型模板类
template<typename T>
class CComplexType
{
public:
	inline int32_t Initialize()
	{
		return S_OK;
	}
	inline int32_t Resume()
	{
		return S_OK;
	}
	inline int32_t Uninitialize()
	{
		return S_OK;
	}
	inline T& GetValue()
	{
		return m_object;
	}
	inline void SetValue(T& object)
	{
		m_object = object;
	}

protected:
	T			m_object;
};


//CCOMPLEXTYPE_SPECIFICALLY(int8_t)
//CCOMPLEXTYPE_SPECIFICALLY(int16_t)
//CCOMPLEXTYPE_SPECIFICALLY(int32_t)
//CCOMPLEXTYPE_SPECIFICALLY(int64_t)
//CCOMPLEXTYPE_SPECIFICALLY(uint8_t)
//CCOMPLEXTYPE_SPECIFICALLY(uint16_t)
//CCOMPLEXTYPE_SPECIFICALLY(uint32_t)
//CCOMPLEXTYPE_SPECIFICALLY(uint64_t)
//CCOMPLEXTYPE_SPECIFICALLY(float)
//CCOMPLEXTYPE_SPECIFICALLY(double)

#endif /* COMMON_COMPLEXTYPE_H_ */
