/*
 * frame_timer_mgt.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_TIMER_MGT_H_
#define FRAME_TIMER_MGT_H_

#include "common/common_object.h"
#include "common/common_pool.h"
#include "common/common_rbtree.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "frame_timer.h"


FRAME_NAMESPACE_BEGIN

//定义定时器对象池和索引类型
typedef CPool<CFrameTimer, MAX_FRAMETIMER_COUNT>	TimerPool;
typedef CRBTree<int64_t, TimerIndex, MAX_FRAMETIMER_COUNT>	TimerMap;

class CFrameTimerMgt : public CObject
{
protected:
	enum
	{
		enmAdditionalIndex_RBTreeIndex			= 0,
	};
public:
	CFrameTimerMgt();
	virtual ~CFrameTimerMgt();

public:
	//初始化定时器管理器
	virtual int32_t Initialize();
	//恢复定时器管理器
	virtual int32_t Resume();
	//注销定时器管理器
	virtual int32_t Uninitialize();

public:

	int32_t CreateTimer(int32_t nTimerID, ITimerEvent *pHandle, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex);

	//使用方法为CreateTimer(MEMBER_FUNC(timer_class, timer_func), pTimer, pTimerData, nCycleTime, bLoop, timerIndex);
	int32_t CreateTimer(TimerProc Proc, ITimerEvent *pTimer, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex);

	//删除定时器
	int32_t RemoveTimer(TimerIndex timerIndex);
	//清空定时器管理器
	int32_t Clear();
	//定时器已触发
	int32_t TimerFired(TimerIndex timerIndex, uint32_t timerSeq);
	//获取结束时间最小的定时器
	int32_t GetFirstTimer(CFrameTimer*& pTimer, TimerIndex& timerIndex);
	//获取定时器
	int32_t GetTimer(TimerIndex timerIndex, CFrameTimer*& pTimer);
	//删除定时器
	int32_t RemoveTimer(TimerPool::CIndex* pIndex);
	//定时器已触发
	int32_t TimerFired(TimerIndex timerIndex);
	//获取结束时间最小的定时器
	int32_t GetFirstTimer(TimerPool::CIndex*& pIndex);

protected:
	uint32_t				m_nTimerSeq;			//Tiemr生成序号
	CriticalSection			m_stTimerLock;
//	CriticalSection			m_stTimerPoolLock;
	TimerPool				m_timerPool;			//定时器对象池
//	CriticalSection			m_stTimerMapLock;
	TimerMap				m_timerMap;				//定时器索引

};

#define	CREATE_FRAMETIMERMGT_INSTANCE	CSingleton<CFrameTimerMgt>::CreateInstance
#define	g_FrameTimerMgt					CSingleton<CFrameTimerMgt>::GetInstance()
#define	DESTROY_FRAMETIMERMGT_INSTANCE	CSingleton<CFrameTimerMgt>::DestroyInstance


FRAME_NAMESPACE_END


#endif /* FRAME_TIMER_MGT_H_ */
