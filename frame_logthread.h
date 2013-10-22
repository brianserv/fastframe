/*
 * frame_logthread.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_LOGTHREAD_H_
#define FRAME_LOGTHREAD_H_

#include "common/common_thread.h"
#include "common/common_singleton.h"
#include "frame_namespace.h"
#include "frame_def.h"

FRAME_NAMESPACE_BEGIN


class CFrameLogThread : public CThread
{
public:
	enum
	{
		enmMaxLogFileNameCount		= 1000,			//日志文件名最大数量
		enmWriteLogPeriod			= 1000*60,		//日志文件的最长非活跃时间(ms),暂时设为1分钟
		enmFlushPeriod				= 1000,			//固定刷新周期，暂时设为1s
		enmInvalidLogFileIndex		= 0xFFFFFFFF,	//无效日志文件序号
	};
public:
	CFrameLogThread();
	virtual ~CFrameLogThread();

public:
	//初始化日志线程
	int32_t Initialize();
	//恢复日志线程
	int32_t Resume();
	//注销日志线程
	int32_t Uninitialize();

public:
	//线程入口函数
	virtual void Execute();

	//将日志写入到文件
	void WriteToLogFile(LogSource logSource, int32_t id1, int32_t id2, const char* szDate, const char* szLog);

	//将日志写入到文件
	void WriteToLogFile(const char *szName, int32_t id1, int32_t id2, const char* szDate, const char* szLog);

protected:
	//各日志文件状态
	LogFileState		m_arrLogFileState[enmMaxLogFileNameCount];

};

#define	CREATE_FRAMELOGTHREAD_INSTANCE		CSingleton<CFrameLogThread>::CreateInstance
#define	g_FrameLogThread					CSingleton<CFrameLogThread>::GetInstance()
#define	DESTROY_FRAMELOGTHREAD_INSTANCE		CSingleton<CFrameLogThread>::DestroyInstance

FRAME_NAMESPACE_END


#endif /* FRAME_LOGTHREAD_H_ */
