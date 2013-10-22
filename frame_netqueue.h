/*
 * frame_netqueue.h
 *
 *  Created on: 2012-11-1
 *      Author: jimm
 */

#ifndef FRAME_NETQUEUE_H_
#define FRAME_NETQUEUE_H_

#include "common/common_object.h"
#include "common/common_messagequeue.h"
#include "common/common_memory.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "frame_protocolhead.h"

#include <list>

using namespace std;

FRAME_NAMESPACE_BEGIN

//typedef CCodeQueue<enmMaxLogicNetQueueSize>		LogicSSCodeQueue;

//typedef CCodeQueue<enmMaxLogicNetQueueSize>		LogicCSCodeQueue;

typedef list<NetPacket *>	 NetQueue;

class CFrameNetQueue
{
public:
	CFrameNetQueue();
	virtual ~CFrameNetQueue();

	virtual size_t GetSize();
	virtual const char* GetName();

public:
	//初始化CS队列
	virtual int32_t Initialize();
	//恢复CS队列
	virtual int32_t Resume();
	//注销CS队列
	virtual int32_t Uninitialize();

public:
	//从接收队列尾部追加一条消息
	int32_t PushRecvSSQueue(NetPacket *pNetPacket);
	//从接收队列头部读取一条消息
	int32_t PopRecvSSQueue(NetPacket *&pNetPacket);
	//判断接收队列是否已满
	bool IsRecvSSQueueFull();
	//判读接收队列是否为空
	bool IsRecvSSQueueEmpty();
	//从发送队列尾部追加一条消息
	int32_t PushSendSSQueue(NetPacket *pNetPacket);
	//从发送队列头部读取一条消息
	int32_t PopSendSSQueue(NetPacket *&pNetPacket);
	//判断发送队列是否已满
	bool IsSendSSQueueFull();
	//判读发送队列是否为空
	bool IsSendSSQueueEmpty();

	//从接收队列尾部追加一条消息
	int32_t PushRecvCSQueue(NetPacket *pNetPacket);
	//从接收队列头部读取一条消息
	int32_t PopRecvCSQueue(NetPacket *&pNetPacket);
	//判断接收队列是否已满
	bool IsRecvCSQueueFull();
	//判读接收队列是否为空
	bool IsRecvCSQueueEmpty();
	//从发送队列尾部追加一条消息
	int32_t PushSendCSQueue(NetPacket *pNetPacket);
	//从发送队列头部读取一条消息
	int32_t PopSendCSQueue(NetPacket *&pNetPacket);
	//判断发送队列是否已满
	bool IsSendCSQueueFull();
	//判读发送队列是否为空
	bool IsSendCSQueueEmpty();

protected:
	NetQueue				m_stRecvSSQueue;
	CriticalSection			m_stRecvSSLock;
	NetQueue				m_stSendSSQueue;
	CriticalSection			m_stSendSSLock;

	NetQueue				m_stRecvCSQueue;
	CriticalSection			m_stRecvCSLock;
	NetQueue				m_stSendCSQueue;
	CriticalSection			m_stSendCSLock;
};


#define	CREATE_FRAMENETQUEUE_INSTANCE	CSingleton<CFrameNetQueue>::CreateInstance
#define	g_FrameNetQueue					CSingleton<CFrameNetQueue>::GetInstance()
#define	DESTROY_FRAMENETQUEUE_INSTANCE	CSingleton<CFrameNetQueue>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_NETQUEUE_H_ */
