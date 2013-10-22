/*
 * frame_logqueue.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_LOGQUEUE_H_
#define FRAME_LOGQUEUE_H_

#include "common/common_object.h"
#include "common/common_logqueue.h"
#include "common/common_memory.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "lightframe_impl.h"


FRAME_NAMESPACE_BEGIN


typedef CLogQueue<enmMaxLogQueueSize>		LogQueue;

class CFrameLogQueue
{
public:
	CFrameLogQueue();
	virtual ~CFrameLogQueue();

	virtual size_t GetSize();
	virtual const char* GetName();

public:
	//初始化日志队列
	virtual int32_t Initialize();
	//恢复日志队列
	virtual int32_t Resume();
	//注销日志队列
	virtual int32_t Uninitialize();

public:
	//从队列尾部追加一条日志
	int32_t Push(ThreadType threadType, const char *szLog, const LogSource logSource, const int32_t nID1, const int32_t nID2, const char* szDate);
	//从队列头部读取一条日志
	int32_t Pop(ThreadType threadType, char* szLog, uint32_t nLogSize, LogSource& logSource, int32_t& nID1, int32_t& nID2, char* szDate);
	//判断队列是否已满
	bool IsFull(ThreadType threadType);
	//判读队列是否为空
	bool IsEmpty(ThreadType threadType);

protected:
	LogQueue				m_stLogQueueNetioThread;
	CriticalSection			m_stMainLogLock;
	LogQueue				m_stLogQueueMainThread;
	LogQueue				m_stLogQueueDBAccessThread;
};


#define	CREATE_FRAMELOGQUEUE_INSTANCE		CSingleton<CFrameLogQueue>::CreateInstance
#define	g_FrameLogQueue						CSingleton<CFrameLogQueue>::GetInstance()
#define	DESTROY_FRAMELOGQUEUE_INSTANCE		CSingleton<CFrameLogQueue>::DestroyInstance



FRAME_NAMESPACE_END


#endif /* FRAME_LOGQUEUE_H_ */
