/*
 * frame_log.h
 *
 *  Created on: 2013-1-2
 *      Author: jimm
 */

#ifndef FRAME_LOG_H_
#define FRAME_LOG_H_

#include "common/common_object.h"
#include "common/common_logqueue.h"
#include "common/common_singleton.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "frame_logqueue.h"


FRAME_NAMESPACE_BEGIN

typedef CLogQueue<enmMaxLogQueueSize>		UserLogQueue;

class CFrameLog : public CObject
{
public:
	CFrameLog();
	virtual ~CFrameLog();

public:
	//初始化日志队列
	virtual int32_t Initialize();
	//恢复日志队列
	virtual int32_t Resume();
	//注销日志队列
	virtual int32_t Uninitialize();

public:
	//从队列尾部追加一条日志
	virtual int32_t Push(const char *szLog, const char *szName, const int32_t nID1, const int32_t nID2, const char* szDate);
	//从队列头部读取一条日志
	virtual int32_t Pop(char* szLog, uint32_t nLogSize, char *szName, int32_t& nID1, int32_t& nID2, char* szDate);
	//判断队列是否已满
	virtual bool IsFull();
	//判读队列是否为空
	virtual bool IsEmpty();

protected:
	UserLogQueue		m_stUserLogQueue;
};

FRAME_NAMESPACE_END

#endif /* FRAME_LOG_H_ */
