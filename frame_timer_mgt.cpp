/*
 * frame_timer_mgt.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */


#include "common/common_datetime.h"
#include "frame_errordef.h"
#include "frame_timer_mgt.h"
#include "frame_mem_mgt.h"


FRAME_NAMESPACE_BEGIN

CFrameTimerMgt::CFrameTimerMgt()
{
	m_nTimerSeq = 0;
}

CFrameTimerMgt::~CFrameTimerMgt()
{

}

//初始化数据中心
int32_t CFrameTimerMgt::Initialize()
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerPool.Initialize();
	m_timerMap.Initialize();
	return S_OK;
}

//恢复数据中心
int32_t CFrameTimerMgt::Resume()
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerPool.Resume();
	m_timerMap.Resume();
	return S_OK;
}

//注销定时器管理器
int32_t CFrameTimerMgt::Uninitialize()
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerPool.Uninitialize();
	m_timerMap.Uninitialize();
	return S_OK;
}

int32_t CFrameTimerMgt::CreateTimer(int32_t nTimerID, ITimerEvent *pHandle, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex)
{
	uint8_t *pObj = NULL;
	if(pTimerData != NULL)
	{
		int32_t nTimerDataSize = pTimerData->GetSize();
		pObj = MALLOC(nTimerDataSize);//g_FrameMemMgt.AllocBlock(nTimerDataSize);
		if(pObj == NULL)
		{
			return E_LIBF_ALLOCBLOCKFAILED;
		}

		memcpy(pObj, pTimerData, nTimerDataSize);
	}

	//初始化定时器参数
	FrameTimer timer;
	timer.timerType = 0;//enmFrameTimerType_Timer;
	timer.nTimerID = nTimerID;
	timer.pData = pObj;
	timer.nStartTime = CTimeValue::CurrentTime().Microseconds();
	timer.nCycleTime = nCycleTime;
	timer.nEndTime = timer.nStartTime + timer.nCycleTime;
	timer.bLoop = bLoop;
	timer.pEventHandle = pHandle;
	timer.pTimerProc = NULL;
	timer.nFiredCount = 0;

	MUTEX_GUARD(TimerLock, m_stTimerLock);
	//设置Timer生成时的Seq
	timer.nTimerSeq = m_nTimerSeq;
	//递增TimerSeq;
	++m_nTimerSeq;

	//创建定时器对象
	TimerPool::CIndex* pIndex = m_timerPool.CreateObject();
	if (NULL == pIndex)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		return E_LIBF_CREATETIMER;
	}

	//建立定时器索引
	TimerMap::CIndex* pMapIndex = m_timerMap.Insert(timer.nEndTime, pIndex->Index());
	if (NULL == pMapIndex)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		m_timerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return E_LIBF_CREATETIMERINDEX;
	}

	//将索引保存到附加数据表中
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_RBTreeIndex, (uint64_t)pMapIndex);
	if (0 > ret)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		m_timerMap.Erase(pMapIndex);
		m_timerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return ret;
	}

	//设置定时器参数
	pIndex->ObjectPtr()->SetTimer(timer);
	timerIndex = pIndex->Index();

	return S_OK;
}

int32_t CFrameTimerMgt::CreateTimer(TimerProc Proc, ITimerEvent *pTimer, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex)
{
	uint8_t *pObj = NULL;
	if(pTimerData != NULL)
	{
		int32_t nTimerDataSize = pTimerData->GetSize();
		pObj = MALLOC(nTimerDataSize);//g_FrameMemMgt.AllocBlock(nTimerDataSize);
		if(pObj == NULL)
		{
			return E_LIBF_ALLOCBLOCKFAILED;
		}

		memcpy(pObj, pTimerData, nTimerDataSize);
	}

	//初始化定时器参数
	FrameTimer timer;
	timer.timerType = 0;//enmFrameTimerType_Timer;
	timer.nTimerID = 0;
	timer.pData = pObj;
	timer.nStartTime = CTimeValue::CurrentTime().Microseconds();
	timer.nCycleTime = nCycleTime;
	timer.nEndTime = timer.nStartTime + timer.nCycleTime;
	timer.bLoop = bLoop;
	timer.pEventHandle = pTimer;
	timer.pTimerProc = Proc;
	timer.nFiredCount = 0;

	MUTEX_GUARD(TimerLock, m_stTimerLock);
	//设置Timer生成时的Seq
	timer.nTimerSeq = m_nTimerSeq;
	//递增TimerSeq;
	++m_nTimerSeq;

	//创建定时器对象
	TimerPool::CIndex* pIndex = m_timerPool.CreateObject();
	if (NULL == pIndex)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		return E_LIBF_CREATETIMER;
	}

	//建立定时器索引
	TimerMap::CIndex* pMapIndex = m_timerMap.Insert(timer.nEndTime, pIndex->Index());
	if (NULL == pMapIndex)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		m_timerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return E_LIBF_CREATETIMERINDEX;
	}

	//将索引保存到附加数据表中
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_RBTreeIndex, (uint64_t)pMapIndex);
	if (0 > ret)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		m_timerMap.Erase(pMapIndex);
		m_timerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return ret;
	}

	//设置定时器参数
	pIndex->ObjectPtr()->SetTimer(timer);
	timerIndex = pIndex->Index();

	return S_OK;
}

//删除定时器
int32_t CFrameTimerMgt::RemoveTimer(TimerIndex timerIndex)
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	TimerPool::CIndex* pIndex = m_timerPool.GetIndex(timerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_TIMERNOTEXIST;
	}

	return RemoveTimer(pIndex);
}

//清空定时器管理器
int32_t CFrameTimerMgt::Clear()
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerPool.Clear();
	m_timerMap.Clear();
	return S_OK;
}

//定时器已触发
int32_t CFrameTimerMgt::TimerFired(TimerIndex timerIndex, uint32_t timerSeq)
{
	CFrameTimer *pTimer = NULL;
	int32_t nRet = GetTimer(timerIndex,pTimer);
	if( nRet == S_OK && pTimer->GetTimerSeq() == timerSeq)
	{
		return TimerFired(timerIndex);
	}

	return S_FALSE;
}


//获取定时器
int32_t CFrameTimerMgt::GetTimer(TimerIndex timerIndex, CFrameTimer*& pTimer)
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	TimerPool::CIndex* pIndex = m_timerPool.GetIndex(timerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_TIMERNOTFOUND;
	}

	pTimer = pIndex->ObjectPtr();

	return S_OK;
}

//删除定时器
int32_t CFrameTimerMgt::RemoveTimer(TimerPool::CIndex* pIndex)
{
	if (NULL == pIndex)
	{
		return E_LIBF_TIMERNOTEXIST;
	}

	CFrameTimer* pTimer = pIndex->ObjectPtr();
	if(pTimer == NULL)
	{
		return E_NULLPOINTER;
	}
	//回收定时器所申请的内存资源
	FrameTimer timer;
	pTimer->GetTimer(timer);
	if(timer.pData != NULL)
	{
		FREE((uint8_t *)timer.pData);//g_FrameMemMgt.RecycleBlock((uint8_t *)timer.pData);
	}

	uint64_t nAddtionalValue = 0;
	pIndex->GetAdditionalData(enmAdditionalIndex_RBTreeIndex, nAddtionalValue);
	TimerMap::CIndex* pMapIndex = (TimerMap::CIndex*)nAddtionalValue;

	//将定时器从对象池及索引表中删除
//	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerMap.Erase(pMapIndex);
	m_timerPool.DestroyObject(pIndex);

	return S_OK;
}

//定时器已触发
int32_t CFrameTimerMgt::TimerFired(TimerIndex timerIndex)
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	TimerPool::CIndex* pIndex = m_timerPool.GetIndex(timerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_TIMERNOTFOUND;
	}

	CFrameTimer* pTimer = pIndex->ObjectPtr();
	if(pTimer == NULL)
	{
		return E_NULLPOINTER;
	}

	if (!pTimer->IsLoop())
	{
		//若不是循环触发则删除定时器
		return RemoveTimer(pIndex);
	}

	FrameTimer timer;
	pTimer->GetTimer(timer);

	//更新定时器参数
	timer.nEndTime = CTimeValue::CurrentTime().Microseconds() + timer.nCycleTime;
	++timer.nFiredCount;
	pTimer->SetTimer(timer);

	//调整定时器索引
	uint64_t nAddtionalValue = 0;
	pIndex->GetAdditionalData(enmAdditionalIndex_RBTreeIndex, nAddtionalValue);
	TimerMap::CIndex* pMapIndex = (TimerMap::CIndex*)nAddtionalValue;

	m_timerMap.Erase(pMapIndex);
	pMapIndex = m_timerMap.Insert(timer.nEndTime, pIndex->Index());
	if (NULL == pMapIndex)
	{
		m_timerPool.DestroyObject(pIndex);
		return E_LIBF_CREATETIMERINDEX;
	}

	//将索引保存到附加数据表中
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_RBTreeIndex, (uint64_t)pMapIndex);
	if (0 > ret)
	{
		m_timerMap.Erase(pMapIndex);
		m_timerPool.DestroyObject(pIndex);
		return ret;
	}

	return S_OK;
}

//获取结束时间最小的定时器
int32_t CFrameTimerMgt::GetFirstTimer(CFrameTimer*& pTimer, TimerIndex& timerIndex)
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	TimerMap::CIndex* pMapIndex = m_timerMap.First();
	if (NULL == pMapIndex)
	{
		return E_LIBF_NOTIMER;
	}

	TimerPool::CIndex* pIndex = m_timerPool.GetIndex( pMapIndex->Object() );
	if (NULL == pIndex)
	{
		//索引列表中存在无效索引则删除
		m_timerMap.Erase(pMapIndex);
		return E_LIBF_INVALIDTIMERINDEX;
	}

	pTimer = pIndex->ObjectPtr();
	timerIndex = pIndex->Index();

	return S_OK;
}


FRAME_NAMESPACE_END
