/*
 * frame_timer.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_TIMER_H_
#define FRAME_TIMER_H_

#include "common/common_object.h"
#include "common/common_complextype.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_errordef.h"
#include "lightframe_impl.h"

FRAME_NAMESPACE_BEGIN

typedef struct tagFrameTimer : public Timer
{
	uint32_t					nTimerSeq;
	ITimerEvent*				pEventHandle;			//Timer接口指针
	TimerProc					pTimerProc;		//Timer处理函数
} FrameTimer;


class CFrameTimer : public CObject
{
public:
	CFrameTimer()
	{
	}
	virtual ~CFrameTimer()
	{
	}

public:
	//初始化定时器对象
	virtual int32_t Initialize()
	{
		memset(&m_stTimer, 0, sizeof(m_stTimer));
		return S_OK;
	}

	//恢复定时器对象
	virtual int32_t Resume()
	{
		return S_OK;
	}

	//注销定时器对象
	virtual int32_t Uninitialize()
	{
		memset(&m_stTimer, 0, sizeof(m_stTimer));
		return S_OK;
	}


public:
	//设置定时器信息
	void SetTimer(const FrameTimer& stTimer)
	{
		memcpy(&m_stTimer, &stTimer, sizeof(m_stTimer));
	}

	//获取定时器信息
	void GetTimer(FrameTimer& stTimer)
	{
		memcpy(&stTimer, &m_stTimer, sizeof(m_stTimer));
	}

	//获取定时器类型
	TimerType GetTimerType()
	{
		return m_stTimer.timerType;
	}

	//获取定时器标识
	int32_t GetTimerID()
	{
		return m_stTimer.nTimerID;
	}

	//获取定时器数据
	void *GetTimerData()
	{
		return m_stTimer.pData;
	}

	//获取定时器结束时间
	int64_t GetEndTime()
	{
		return m_stTimer.nEndTime;
	}

	uint32_t GetTimerSeq()
	{
		return m_stTimer.nTimerSeq;
	}

	//定时器是否循环触发
	bool IsLoop()
	{
		return m_stTimer.bLoop;
	}

	//获取已触发次数
	uint32_t GetFiredCount()
	{
		return m_stTimer.nFiredCount;
	}

	//获取触发回调
	TimerProc GetEventProc()
	{
		return m_stTimer.pTimerProc;
	}

	ITimerEvent* GetEventHandler()
	{
		return m_stTimer.pEventHandle;
	}

protected:
	FrameTimer					m_stTimer;
};


FRAME_NAMESPACE_END


CCOMPLEXTYPE_SPECIFICALLY(FRAME_NAMESPACE::CFrameTimer)

#endif /* FRAME_TIMER_H_ */
