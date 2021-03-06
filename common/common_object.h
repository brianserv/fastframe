/*
 * common_object.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_OBJECT_H_
#define COMMON_OBJECT_H_

#include "common_typedef.h"


class CObject
{
public:
	CObject();
	virtual ~CObject();
public:
	virtual int32_t Initialize();
	virtual int32_t Resume();
	virtual int32_t Uninitialize();
};


#define ENTITYTYPE_BASE			= 0,		//基础实体类型
#define ENTITYTYPE_OBJECT		= 1,		//对象实体类型

#endif /* COMMON_OBJECT_H_ */
