/*
 * frame_timertask.cpp
 *
 *  Created on: 2013-1-15
 *      Author: jimm
 */

#include "frame_timertask.h"
#include "frame_mem_mgt.h"

FRAME_NAMESPACE_BEGIN

CFrameTimerTask::CFrameTimerTask()
{

}

CFrameTimerTask::~CFrameTimerTask()
{

}

int32_t CFrameTimerTask::Start()
{
	TimerIndex nTimerIndex = enmInvalidTimerIndex;
	int32_t nRet = g_FrameTimerMgt.CreateTimer(static_cast<TimerProc>(&CFrameTimerTask::OnFrameMemTimer), this,
			NULL, 1 * US_PER_MINUTE, true, nTimerIndex);
	if(nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CFrameTimerTask::OnFrameMemTimer(CFrameTimer *pTimer)
{
	g_FrameMemMgt.PrintMemBlockInfo();

	return S_OK;
}

int32_t CFrameTimerTask::OnTimerEvent(CFrameTimer *pTimer)
{
	return S_OK;
}


FRAME_NAMESPACE_END
