/*
 * frame_netthread.h
 *
 *  Created on: 2012-11-1
 *      Author: jimm
 */

#ifndef FRAME_NETTHREAD_H_
#define FRAME_NETTHREAD_H_

#include "common/common_singleton.h"
#include "common/common_thread.h"
#include "common/common_message.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_errordef.h"
#include "frame_logengine.h"
#include "frame_socket.h"
#include "frame_baseconfig.h"
#include "frame_epoll.h"


FRAME_NAMESPACE_BEGIN

//enum enmSocketType
//{
//	enmSocketType_invalid = 0,
//	enmSocketType_listen =	1,			//监听socket
//	enmSocketType_common =  2,			//普通socket
//};

//最大的服务器类型个数
//#define MAX_PEERTYPE_COUNT 64

#define RECONNECT_TIME		10//(s)

class CFrameNetThread// : public CThread
{
public:
	CFrameNetThread();
	virtual ~CFrameNetThread();

	int32_t Initialize();

	int32_t Resume();

	int32_t Uninitialize();

	bool Execute();

	CEpoll *GetEpollObj();

protected:
	int32_t MessagePump();

	int32_t SendMessage();

	int32_t HandleTimeOutEvent();

protected:
	CEpoll		m_stEpoll;

	int64_t		m_nReconnectTime;
	int64_t		m_nLastConnectTime;
};


#define	CREATE_FRAMENETTHREAD_INSTANCE		CSingleton<CFrameNetThread>::CreateInstance
#define	g_FrameNetThread					CSingleton<CFrameNetThread>::GetInstance()
#define	DESTROY_FRAMENETTHREAD_INSTANCE		CSingleton<CFrameNetThread>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_NETTHREAD_H_ */
