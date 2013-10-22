/*
 * frame_mainthread.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_MAINTHREAD_H_
#define FRAME_MAINTHREAD_H_

#include "common/common_singleton.h"
#include "common/common_thread.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "common/common_mutex.h"


FRAME_NAMESPACE_BEGIN

class CSocket;
class CFrameMainThread : public CThread
{
public:
	CFrameMainThread();
	virtual ~CFrameMainThread();

public:
	//初始化数据库访问线程
	int32_t Initialize();
	//恢复数据库访问线程
	int32_t Resume();
	//注销数据库访问线程
	int32_t Uninitialize();

public:
	//线程入口函数
	virtual void Execute();

protected:
	bool ProcessCSLogicData();
	//处理定时器事件
	bool ProcessTimerEvent();
	//处理客户端和服务器之间通讯数据
	bool ProcessCSMessage(uint8_t *pBuf, uint32_t nLength);
	//系统事件
	int32_t OnSystemEvent(uint16_t nEventID, void *pParam);
	//客户端消息事件
	int32_t OnMessageEvent(CSocket *pConnection, MessageHeadCS* pMsgHead, const uint16_t nMsgHeadOffset,
			const uint8_t* buf, const uint32_t size, const int32_t nOptionLen, const void *pOption);

	IMsgBody *CreateMsgBody(uint32_t nMsgID);

	void DestroyMsgBody(IMsgBody *pMsgBody);

protected:
//	CriticalSection			m_stMsgBodyLock;
};

//#define	CREATE_FRAMEMAINTHREAD_INSTANCE		CSingleton<CFrameMainThread>::CreateInstance
//#define	g_FrameMainThread					CSingleton<CFrameMainThread>::GetInstance()
//#define	DESTROY_FRAMEMAINTHREAD_INSTANCE	CSingleton<CFrameMainThread>::DestroyInstance

FRAME_NAMESPACE_END


#endif /* FRAME_MAINTHREAD_H_ */
