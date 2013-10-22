/*
 * common_messagequeue.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_MESSAGEQUEUE_H_
#define COMMON_MESSAGEQUEUE_H_

#include <stdio.h>
#include <memory.h>

#include "common_typedef.h"
#include "common_object.h"
#include "common_codequeue.h"
#include "common_message.h"


template<uint32_t CAPACITY>
class CMessageQueue : public CObject
{
public:
	CMessageQueue()
	{
	}
	virtual ~CMessageQueue()
	{
	}

public:
	//初始化队列
	virtual int32_t Initialize()
	{
		return m_codeQueue.Initialize();
	}
	//恢复队列
	virtual int32_t Resume()
	{
		return m_codeQueue.Resume();
	}
	//注销队列
	virtual int32_t Uninitialize()
	{
		return m_codeQueue.Uninitialize();
	}

public:
	//从队列尾部追加一个数据包
	int32_t Push(const MessageContent *pMessageContent)
	{
		if (NULL == pMessageContent)
		{
			return E_INVALIDARGUMENT;
		}

		return m_codeQueue.Push(pMessageContent->m_arrMessageBuffer, pMessageContent->m_nMessageSize);
	}
	//从队列头部读取一个数据包
	int32_t Pop(MessageContent *pMessageContent)
	{
		if (NULL == pMessageContent)
		{
			return E_INVALIDARGUMENT;
		}

		return m_codeQueue.Pop(pMessageContent->m_arrMessageBuffer, enmMaxMessageSize, pMessageContent->m_nMessageSize);
	}

	//判断队列是否已满
	bool IsFull()
	{
		return m_codeQueue.IsFull();
	}
	//判读队列是否为空
	bool IsEmpty()
	{
		return m_codeQueue.IsEmpty();
	}

protected:
	CCodeQueue<CAPACITY>	m_codeQueue;		//数据流队列

};

#endif /* COMMON_MESSAGEQUEUE_H_ */
