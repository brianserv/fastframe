/*
 * frame_sockettimer.cpp
 *
 *  Created on: 2013-5-20
 *      Author: jimm
 */

#include "frame_socket.h"
#include "frame_sockettimer.h"
#include "common/common_datetime.h"
#include "frame_mem_mgt.h"

FRAME_NAMESPACE_BEGIN

CFrameSocketTimer *CFrameSocketTimerMgt::CreateSocketTimer(CSocket *pSocket, SocketTimerProc pTimerProc,
		int64_t nTimeOut)
{
	if((pSocket == NULL) || (nTimeOut <= 0))
	{
		return NULL;
	}

	uint8_t *pMem = MALLOC(sizeof(CFrameSocketTimer));
	if(pMem == NULL)
	{
		return NULL;
	}

	CFrameSocketTimer *pTimer = new(pMem)CFrameSocketTimer();
	pTimer->pSocket = pSocket;
	pTimer->pTimerProc = pTimerProc;
	pTimer->m_nStartTime = CDateTime::CurrentDateTime().Seconds();
	pTimer->m_nEndTime = CDateTime::CurrentDateTime().Seconds() + nTimeOut;

	m_stFrameSocketTimerList.push_back(pTimer);

	return pTimer;
}

CFrameSocketTimer *CFrameSocketTimerMgt::GetSocketTimer(CSocket *pSocket)
{
	CFrameSocketTimer *pTimer = NULL;

	list<CFrameSocketTimer *>::iterator it = m_stFrameSocketTimerList.begin();
	for(;it != m_stFrameSocketTimerList.end(); it++)
	{
		if(pSocket == (*it)->pSocket)
		{
			pTimer = *it;
			break;
		}
	}

	return pTimer;
}

CFrameSocketTimer *CFrameSocketTimerMgt::GetSocketTimer(SocketFD nSocketFD)
{
	CFrameSocketTimer *pTimer = NULL;

	list<CFrameSocketTimer *>::iterator it = m_stFrameSocketTimerList.begin();
	for(;it != m_stFrameSocketTimerList.end(); it++)
	{
		if(nSocketFD == (*it)->pSocket->GetSocketFD())
		{
			pTimer = *it;
			break;
		}
	}

	return pTimer;
}

CFrameSocketTimer *CFrameSocketTimerMgt::GetFirstSocketTimer()
{
	CFrameSocketTimer *pTimer = NULL;

	if(m_stFrameSocketTimerList.size() > 0)
	{
		pTimer = m_stFrameSocketTimerList.front();
	}

	return pTimer;
}

int32_t CFrameSocketTimerMgt::DestroySocketTimer(CFrameSocketTimer *pTimer)
{
	if(pTimer == NULL)
	{
		return S_OK;
	}

	list<CFrameSocketTimer *>::iterator it = m_stFrameSocketTimerList.begin();
	for(;it != m_stFrameSocketTimerList.end(); it++)
	{
		if(pTimer == *it)
		{
			m_stFrameSocketTimerList.erase(it);
			FREE((uint8_t *)pTimer);
			break;
		}
	}

	return S_OK;
}

int32_t CFrameSocketTimerMgt::GetTimerCount()
{
	return m_stFrameSocketTimerList.size();
}

FRAME_NAMESPACE_END

