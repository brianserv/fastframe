/*
 * frame_timertask.h
 *
 *  Created on: 2013-1-15
 *      Author: jimm
 */

#ifndef FRAME_TIMERTASK_H_
#define FRAME_TIMERTASK_H_

#include "common/common_singleton.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "lightframe_impl.h"
#include "frame_timer_mgt.h"

FRAME_NAMESPACE_BEGIN

class CFrameTimerTask : public ITimerEvent
{
public:
	CFrameTimerTask();
	~CFrameTimerTask();

	int32_t Start();

	int32_t OnFrameMemTimer(CFrameTimer *pTimer);

	virtual int32_t OnTimerEvent(CFrameTimer *pTimer);
};

#define	CREATE_FRAMETIMERTASK_INSTANCE				CSingleton<CFrameTimerTask>::CreateInstance
#define	g_FrameTimerTask							CSingleton<CFrameTimerTask>::GetInstance()
#define	DESTROY_FRAMETIMERTASK_INSTANCE				CSingleton<CFrameTimerTask>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_TIMERTASK_H_ */
