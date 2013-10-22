/*
 * common_codequeue.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_CODEQUEUE_H_
#define COMMON_CODEQUEUE_H_

#include <stdio.h>
#include <memory.h>

#include "common_typedef.h"
#include "common_object.h"
#include "common_errordef.h"


template<uint32_t CAPACITY>
class CCodeQueue
{
public:
	enum
	{
		enmReservedCodeLength		= 8,	//预留空间长度
	};
public:
	CCodeQueue()
	{
	}
	~CCodeQueue()
	{
	}

public:
	//初始化队列
	int32_t Initialize()
	{
		m_nSize = CAPACITY;
		m_nHead = 0;
		m_nTail = 0;
		m_nOffset = sizeof(uint32_t) * 4;
		memset(m_arrCodeBuffer, 0, sizeof(m_arrCodeBuffer));

		return S_OK;
	}
	//恢复队列
	int32_t Resume()
	{
		return S_OK;
	}
	//注销队列
	int32_t Uninitialize()
	{
		return S_OK;
	}

public:
	//从队列尾部追加一个数据包
	int32_t Push(const uint8_t* pBuffer, uint32_t nLength)
	{
		if ((NULL == pBuffer) || (0 >= nLength))
		{
			return E_INVALIDARGUMENT;
		}

		//获取队列首地址和队列空间总大小
		uint8_t* pCodeBuffer = GetCodeBuffer();
		uint32_t nCapacity = GetCapacity();

		if ((NULL == pCodeBuffer) || (nCapacity <= 0))
		{
			return E_QUEUENOTINITIALIZED;
		}
		if (IsFull())
		{
			return E_FULLQUEUE;
		}

		//获取队列首尾位置
		uint32_t nHead = 0;
		uint32_t nTail = 0;
		uint32_t nFreeSize = 0;
		GetFreeInfo(nHead, nTail, nFreeSize);

		//检查首尾位置是否有效
		if ((nHead >= nCapacity) || (nTail >= nCapacity))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}

		if ((uint32_t)sizeof(uint32_t) + nLength > nFreeSize)
		{
			//队列空间不够
			return E_SPACENOTENOUGH;
		}

		uint8_t* p = (uint8_t*)&nLength;

		//将数据包长度保存到队列中
		for (size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			pCodeBuffer[nTail] = p[i];
			nTail = (nTail + 1) % nCapacity;
		}

		//将数据包保存到队列中
		if (nHead > nTail)
		{
			memcpy(&pCodeBuffer[nTail], pBuffer, nLength);
		}
		else
		{
			if (nLength > (nCapacity - nTail))
			{
				//需要分段拷贝
				memcpy(pCodeBuffer + nTail, pBuffer, nCapacity - nTail);
				memcpy(pCodeBuffer, pBuffer + (nCapacity - nTail), nLength - (nCapacity - nTail));
			}
			else
			{
				memcpy(pCodeBuffer + nTail, pBuffer, nLength);
			}
		}

		nTail = (nTail + nLength) % nCapacity;

		//更改队列尾部位置
		return SetTail(nTail);
	}

	//从队列头部读取一个数据包,但数据任保留在缓冲区中,返回值为新的头指针位置
	int32_t Front(uint8_t* pBuffer, uint32_t nSize, uint32_t& nLength)
	{
		if ((NULL == pBuffer) || (0 >= nSize))
		{
			return E_INVALIDARGUMENT;
		}

		//获取队列首地址和队列空间总大小
		uint8_t* pCodeBuffer = GetCodeBuffer();
		uint32_t nCapacity = GetCapacity();

		if ((NULL == pCodeBuffer) || (nCapacity <= 0))
		{
			return E_QUEUENOTINITIALIZED;
		}
		if (IsEmpty())
		{
			return E_EMPTYQUEUE;
		}

		//获取队列首尾位置
		uint32_t nHead = 0;
		uint32_t nTail = 0;
		uint32_t nUsedSize = 0;
		GetUsedInfo(nHead, nTail, nUsedSize);

		//检查首尾位置是否有效
		if ((nHead >= nCapacity) || (nTail >= nCapacity))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}
		//队列已使用空间小于4个字节
		if (nUsedSize < sizeof(uint32_t))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}

		//使用nRealSize保存数据包长度
		uint32_t nRealSize = 0;
		uint8_t* p = (uint8_t*)&nRealSize;

		//获取数据包长度
		for (size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			p[i] = pCodeBuffer[nHead];
			nHead = (nHead + 1) % nCapacity;
		}

		//从队列中读取的数据包长度长度不符合要求
		if ((nRealSize <= 0) || (nRealSize > (nUsedSize - sizeof(uint32_t))))
		{
			SetBoundary(0, 0);
			return E_INVALIDPACKAGE;
		}

		//输出缓冲区长度不够
		if (nSize < nRealSize)
		{
			//丢弃该数据包
			nHead = (nHead + nRealSize) % nCapacity;
			SetHead(nHead);
			return E_INVALIDPACKAGE;
		}

		//从队列中取出数据包
		nLength = nRealSize;

		if (nTail > nHead)
		{
			memcpy(pBuffer, pCodeBuffer + nHead, nRealSize);
		}
		else
		{
			if (nRealSize > (nCapacity - nHead))
			{
				//需要分段拷贝
				memcpy(pBuffer, pCodeBuffer + nHead, nCapacity - nHead);
				memcpy(pBuffer + (nCapacity - nHead), pCodeBuffer, nRealSize - (nCapacity - nHead));
			}
			else
			{
				memcpy(pBuffer, pCodeBuffer + nHead, nRealSize);
			}

		}

		nHead = (nHead + nRealSize) % nCapacity;

		//更改队列头部位置
		return (int32_t)nHead;
	}

	//将前len个字节丢掉
	int32_t Pop(uint32_t len)
	{
		uint32_t nHead = GetHead();
		//len是上层用户数据的长度，sizeof(uint32_t)是数据时间长度字节在缓冲区中的长度
		nHead = (nHead + len + sizeof(uint32_t))%GetCapacity();

		return SetHead(nHead);
	}


	//从队列头部读取一个数据包
	int32_t Pop(uint8_t* pBuffer, uint32_t nSize, uint32_t& nLength)
	{
		if ((NULL == pBuffer) || (0 >= nSize))
		{
			return E_INVALIDARGUMENT;
		}

		//获取队列首地址和队列空间总大小
		uint8_t* pCodeBuffer = GetCodeBuffer();
		uint32_t nCapacity = GetCapacity();

		if ((NULL == pCodeBuffer) || (nCapacity <= 0))
		{
			return E_QUEUENOTINITIALIZED;
		}
		if (IsEmpty())
		{
			return E_EMPTYQUEUE;
		}

		//获取队列首尾位置
		uint32_t nHead = 0;
		uint32_t nTail = 0;
		uint32_t nUsedSize = 0;
		GetUsedInfo(nHead, nTail, nUsedSize);

		//检查首尾位置是否有效
		if ((nHead >= nCapacity) || (nTail >= nCapacity))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}
		//队列已使用空间小于4个字节
		if (nUsedSize < sizeof(uint32_t))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}

		//使用nRealSize保存数据包长度
		uint32_t nRealSize = 0;
		uint8_t* p = (uint8_t*)&nRealSize;

		//获取数据包长度
		for (size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			p[i] = pCodeBuffer[nHead];
			nHead = (nHead + 1) % nCapacity;
		}

		//从队列中读取的数据包长度长度不符合要求
		if ((nRealSize <= 0) || (nRealSize > (nUsedSize - sizeof(uint32_t))))
		{
			SetBoundary(0, 0);
			return E_INVALIDPACKAGE;
		}

		//输出缓冲区长度不够
		if (nSize < nRealSize)
		{
			//丢弃该数据包
			nHead = (nHead + nRealSize) % nCapacity;
			SetHead(nHead);
			return E_INVALIDPACKAGE;
		}

		//从队列中取出数据包
		nLength = nRealSize;

		if (nTail > nHead)
		{
			memcpy(pBuffer, pCodeBuffer + nHead, nRealSize);
		}
		else
		{
			if (nRealSize > (nCapacity - nHead))
			{
				//需要分段拷贝
				memcpy(pBuffer, pCodeBuffer + nHead, nCapacity - nHead);
				memcpy(pBuffer + (nCapacity - nHead), pCodeBuffer, nRealSize - (nCapacity - nHead));
			}
			else
			{
				memcpy(pBuffer, pCodeBuffer + nHead, nRealSize);
			}

		}

		nHead = (nHead + nRealSize) % nCapacity;

		//更改队列头部位置
		return SetHead(nHead);
	}

	//判断队列是否已满
	bool IsFull()
	{
		return (GetFreeSize() <= 0);
	}
	//判读队列是否为空
	bool IsEmpty()
	{
		return (GetUsedSize() <= 0);
	}

protected:
	//设置队列头部位置
	int32_t SetHead(uint32_t nHead)
	{
		uint32_t nCapacity = GetCapacity();

		if (nHead > nCapacity)
		{
			return E_INVALIDHEAD;
		}

		m_nHead = nHead;
		return S_OK;
	}
	//设置队列尾部位置
	int32_t SetTail(uint32_t nTail)
	{
		uint32_t nCapacity = GetCapacity();

		if (nTail > nCapacity)
		{
			return E_INVALIDTAIL;
		}

		m_nTail = nTail;
		return S_OK;
	}
	//设置队列首尾位置
	int32_t SetBoundary(uint32_t nHead, uint32_t nTail)
	{
		uint32_t nCapacity = GetCapacity();

		if (nHead > nCapacity)
		{
			return E_INVALIDHEAD;
		}
		if (nTail > nCapacity)
		{
			return E_INVALIDTAIL;
		}

		m_nHead = nHead;
		m_nTail = nTail;
		return S_OK;
	}
	//获取队列头部位置
	uint32_t GetHead() const
	{
		return m_nHead;
	}
	//获取队列尾部位置
	uint32_t GetTail() const
	{
		return m_nTail;
	}
	//获取队列首尾位置
	void GetBoundary(uint32_t& nHead, uint32_t& nTail) const
	{
		nHead = m_nHead;
		nTail = m_nTail;
	}

	void GetFreeInfo(uint32_t& nHead, uint32_t& nTail, uint32_t& nFreeSize)
	{
		uint32_t nCapacity = GetCapacity();
		if (nCapacity <= enmReservedCodeLength)
		{
			return;
		}
		nHead = GetHead();
		nTail = GetTail();
		nFreeSize = ((nHead == nTail) ? nCapacity : ((nHead + nCapacity - nTail) % nCapacity));

		//去掉预留的空间
		if (nFreeSize < (uint32_t)enmReservedCodeLength)
		{
			nFreeSize = 0;
		}
		else
		{
			nFreeSize = nFreeSize - (uint32_t)enmReservedCodeLength;
		}
	}

	//返回队列可用空间大小
	uint32_t GetFreeSize() const
	{
		uint32_t nHead = GetHead();
		uint32_t nTail = GetTail();
		uint32_t nCapacity = GetCapacity();

		if (nCapacity <= enmReservedCodeLength)
		{
			return 0;
		}

		uint32_t nFreeSize = ((nHead == nTail) ? nCapacity : ((nHead + nCapacity - nTail) % nCapacity));

		//去掉预留的空间
		if (nFreeSize < (uint32_t)enmReservedCodeLength)
		{
			return 0;
		}
		return nFreeSize - (uint32_t)enmReservedCodeLength;
	}
	void GetUsedInfo(uint32_t& nHead, uint32_t& nTail, uint32_t& nUsedSize)
	{
		uint32_t nCapacity = GetCapacity();

		if (nCapacity <= enmReservedCodeLength)
		{
			return;
		}

		nHead = GetHead();
		nTail = GetTail();

		nUsedSize = (nTail + nCapacity - nHead) % nCapacity;
	}
	//返回队列已使用空间大小
	uint32_t GetUsedSize()
	{
		uint32_t nHead = GetHead();
		uint32_t nTail = GetTail();
		uint32_t nCapacity = GetCapacity();

		if (nCapacity <= enmReservedCodeLength)
		{
			return 0;
		}

		uint32_t nUsedSize = (nTail + nCapacity - nHead) % nCapacity;

		return nUsedSize;
	}
	//返回队列空间总大小
	uint32_t GetCapacity() const
	{
		return CAPACITY;
	}
	//返回数据区首地址
	uint8_t* GetCodeBuffer() const
	{
		return (uint8_t*)m_arrCodeBuffer;
	}


protected:
	//缓冲区总长度
	uint32_t				m_nSize;
	//可用数据起始位置
	uint32_t				m_nHead;
	//可用数据结束位置
	uint32_t				m_nTail;
	//偏移量
	uint32_t				m_nOffset;
	//数据区
	uint8_t					m_arrCodeBuffer[CAPACITY];

};

#endif /* COMMON_CODEQUEUE_H_ */
