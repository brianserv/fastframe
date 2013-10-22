/*
 * frame_sockettimer.h
 *
 *  Created on: 2013-5-20
 *      Author: jimm
 */

#ifndef FRAME_SOCKETTIMER_H_
#define FRAME_SOCKETTIMER_H_

#include "frame_namespace.h"
#include "common/common_singleton.h"

#include <list>

using namespace std;

FRAME_NAMESPACE_BEGIN

class CSocket;
class CFrameSocketTimer;

typedef int32_t (CSocket::*SocketTimerProc)(CFrameSocketTimer *pTimer);

class CFrameSocketTimer
{
public:
	CFrameSocketTimer()
	{
		Reset();
	}

	void Reset()
	{
		pSocket = NULL;
		pTimerProc = NULL;
		m_nStartTime = 0;
		m_nEndTime = 0;
	}

	CSocket			*pSocket;
	SocketTimerProc	pTimerProc;
	int64_t			m_nStartTime;
	int64_t			m_nEndTime;
};

class CFrameSocketTimerMgt
{
public:

	CFrameSocketTimer *CreateSocketTimer(CSocket *pSocket, SocketTimerProc pTimerProc, int64_t nTimeOut);

	CFrameSocketTimer *GetSocketTimer(CSocket *pSocket);

	CFrameSocketTimer *GetSocketTimer(SocketFD nSocketFD);

	CFrameSocketTimer *GetFirstSocketTimer();

	int32_t DestroySocketTimer(CFrameSocketTimer *pTimer);

	int32_t GetTimerCount();

protected:
	list<CFrameSocketTimer *>		m_stFrameSocketTimerList;
};

#define	g_FrameSocketTimerMgt					CSingleton<CFrameSocketTimerMgt>::GetInstance()

FRAME_NAMESPACE_END

#endif /* FRAME_SOCKETTIMER_H_ */
