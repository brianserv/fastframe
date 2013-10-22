/*
 * common_memory.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_MEMORY_H_
#define COMMON_MEMORY_H_

#include <stdio.h>
#include <time.h>
#include <new>

#ifdef WIN32
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#endif

#include "common_typedef.h"


#ifdef WIN32
typedef int key_t;
#endif


enum
{
	enmShmMode_Init			= 0x00,
	enmShmMode_Resume		= 0x01,
};


class CShareMemory
{
public:
	static uint8_t*	ms_pMemory;

public:
	CShareMemory();
	CShareMemory(key_t key, size_t nSize, uint8_t nShmMode);
	~CShareMemory();

public:
	//new操作符重载
	void*	operator new(size_t nSize) throw();
	//delete操作符重载
	void	operator delete(void *pShareMemory);

	//分配大小为nSize的内存块
	void*	CreateSegment(size_t nSize);

	//获取剩余空间大小(单位: Byte)
	size_t	GetFreeSize();
	//获取已使用空间大小(单位: Byte)
	size_t	GetUsedSize();

	//设置共享内存时间戳
	void	SetTimeStamp();
	uint8_t	GetMode();

protected:
	int32_t Initialize(key_t key, size_t nSize);
	int32_t Resume(key_t key, size_t nSize);

protected:
	key_t			m_key;					//共享内存键
	size_t			m_nTotalSize;			//内存总大小
	uint32_t		m_nCRC;					//内存头校验和
	time_t			m_tmCreateTime;			//内存创建时间
	time_t			m_tmLastTimeStamp;		//最后访问时间
	uint8_t			m_nShmMode;				//内存使用模式
	uint8_t*		m_pAvailableMemory;		//可用内存

};


//创建共享内存
extern CShareMemory*	CreateShareMemory(key_t key, size_t nSize, uint8_t nShmMode = 0);
//销毁共享内存
extern int32_t			DestroyShareMemory(key_t key);


#endif /* COMMON_MEMORY_H_ */
