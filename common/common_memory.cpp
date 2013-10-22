/*
 * common_memory.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#include "common/common_memory.h"
#include "common/common_errordef.h"
#include "common/common_def.h"


uint8_t* CShareMemory::ms_pMemory = NULL;


CShareMemory::CShareMemory()
{
	m_pAvailableMemory = ms_pMemory + sizeof(CShareMemory);
}


CShareMemory::CShareMemory(key_t key, size_t nSize, uint8_t nShmMode)
{
	m_pAvailableMemory = ms_pMemory + sizeof(CShareMemory);

	if (enmShmMode_Resume != nShmMode)
	{
		Initialize(key, nSize);
	}
	else
	{
		Resume(key, nSize);
	}
}

CShareMemory::~CShareMemory()
{

}

//new操作符重载
void* CShareMemory::operator new(size_t nSize) throw()
{
	if (NULL == ms_pMemory)
	{
		return NULL;
	}

	return (void*)ms_pMemory;
}

//delete操作符重载
void CShareMemory::operator delete(void *pShareMemory)
{
}

//分配大小为nSize的内存块
void* CShareMemory::CreateSegment(size_t nSize)
{
	if (nSize <= 0)
	{
		return NULL;
	}
	if (nSize > GetFreeSize())
	{
		return NULL;
	}

	uint8_t* pMemory = m_pAvailableMemory;
	m_pAvailableMemory += nSize;

	return (void*)pMemory;
}

//获取剩余空间大小(单位: Byte)
size_t CShareMemory::GetFreeSize()
{
	size_t nUsedSize = GetUsedSize();

	return (m_nTotalSize - nUsedSize);
}

//获取已使用空间大小(单位: Byte)
size_t CShareMemory::GetUsedSize()
{
	return (size_t)(m_pAvailableMemory - (uint8_t*)this);
}

//设置共享内存时间戳	
void CShareMemory::SetTimeStamp()
{
	time_t tmNow;
	time(&tmNow);
	m_tmLastTimeStamp = tmNow;

	m_nCRC = (uint32_t)m_key ^ (uint32_t)m_nTotalSize ^ (uint32_t)m_tmCreateTime ^ (uint32_t)m_tmLastTimeStamp;
}

uint8_t CShareMemory::GetMode()
{
	return m_nShmMode;
}

int32_t CShareMemory::Initialize(key_t key, size_t nSize)
{
	m_nShmMode = enmShmMode_Init;
	m_key = key;
	m_nTotalSize = nSize;

	time_t tmNow;
	time(&tmNow);
	m_tmCreateTime = tmNow;
	m_tmLastTimeStamp = tmNow;

	//计算校验和
	m_nCRC = (uint32_t)m_key ^ (uint32_t)m_nTotalSize ^ (uint32_t)m_tmCreateTime ^ (uint32_t)m_tmLastTimeStamp;

	return S_OK;
}

int32_t CShareMemory::Resume(key_t key, size_t nSize)
{
	//
	if (key != m_key || nSize != m_nTotalSize)
	{
		return Initialize(key, nSize);
	}

	//检查校验和
	uint32_t nCrc = (uint32_t)m_key ^ (uint32_t)m_nTotalSize ^ (uint32_t)m_tmCreateTime ^ 
		(uint32_t)m_tmLastTimeStamp ^ m_nCRC;
	if (0 != nCrc)
	{
		//校验不通过,则进行初始化
		return Initialize(key, nSize);
	}

	time_t tmNow;
	time(&tmNow);
	if (((tmNow - m_tmCreateTime) < 0) || ((tmNow - m_tmCreateTime) > 1 * SECOND_PER_YEAR))
	{
		return Initialize(key, nSize);
	}
	if ((tmNow - m_tmLastTimeStamp) > 2 * SECOND_PER_HOUR)
	{
		return Initialize(key, nSize);
	}

	SetTimeStamp();
	m_nShmMode = enmShmMode_Resume;

	return S_OK;
}

CShareMemory* CreateShareMemory(key_t key, size_t nSize, uint8_t nShmMode/* = 0*/)
{
	if (nSize <= 0)
	{
		return NULL;
	}

	size_t nRealSize = nSize + sizeof(CShareMemory);

#ifdef WIN32
	CShareMemory::ms_pMemory = new uint8_t[nRealSize];
	if (NULL == CShareMemory::ms_pMemory)
	{
		return NULL;
	}
#else
	int nShmID = 0;

	//创建指定大小的共享内存
	nShmID = shmget(key, nRealSize,  IPC_CREAT | IPC_EXCL | 0666);
	if (nShmID < 0)
	{
		if (errno != EEXIST)
		{
			//共享内存闯将失败
			return NULL;
		}
		
		//共享内存已经存在则直接Attach指定大小的内存
		nShmID = shmget(key, nRealSize, 0666);
		if (nShmID < 0)
		{
			//Attach失败则Attach全部共享内存
			nShmID = shmget(key, 0, 0666);
			if (nShmID < 0)
			{
				//Attach失败
				return NULL;
			}
			else
			{
				//删除Attach到的共享内存
				if (shmctl(nShmID, IPC_RMID, NULL))
				{
					return NULL;
				}

				//重新创建共享内存
				nShmID = shmget(key, nRealSize,  IPC_CREAT | IPC_EXCL | 0666);
				if (nShmID < 0)
				{
					return NULL;
				}
			}

		}
		else
		{
			//Attach到已有内存区域
		}

	}

	//获取共享内存首地址
	CShareMemory::ms_pMemory = (uint8_t*)shmat(nShmID, NULL, 0);
	if (NULL == CShareMemory::ms_pMemory)
	{
		return NULL;
	}
#endif

	//在得到的内存上创建一个CShareMemory对象
	return (new CShareMemory(key, nRealSize, nShmMode));
}

int32_t DestroyShareMemory(key_t key)
{
#ifdef WIN32
	if (NULL != CShareMemory::ms_pMemory)
	{
		delete[] CShareMemory::ms_pMemory;
		CShareMemory::ms_pMemory = NULL;
	}
#else
	int nShmID = 0;

	//Attach到该共享内存
	nShmID = shmget(key, 0, 0666);
	if (nShmID < 0)
	{
		return E_SHMATTACH;
	}
	
	//删除该共享内存
	if (shmctl(nShmID, IPC_RMID, NULL))
	{
		return E_SHMDELETE;
	}
#endif

	return S_OK;
}

