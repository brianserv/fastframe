/*
 * frame_logqueue.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */


#include "frame_logqueue.h"
#include "frame_errordef.h"


FRAME_NAMESPACE_BEGIN

CFrameLogQueue::CFrameLogQueue()
{
}

CFrameLogQueue::~CFrameLogQueue()
{

}

size_t CFrameLogQueue::GetSize()
{
	return sizeof(m_stLogQueueNetioThread) + sizeof(m_stLogQueueMainThread) + sizeof(m_stLogQueueDBAccessThread);
}

const char* CFrameLogQueue::GetName()
{
	return LOGQUEUE_DATACENTER;
}

//初始化日志队列
int32_t CFrameLogQueue::Initialize()
{
	int32_t nRet = m_stLogQueueNetioThread.Initialize();
	if(0 > nRet)
	{
		return nRet;
	}

	nRet = m_stLogQueueMainThread.Initialize();
	if(0 > nRet)
	{
		return nRet;
	}

	nRet = m_stLogQueueDBAccessThread.Initialize();
	if(0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

//恢复日志队列
int32_t CFrameLogQueue::Resume()
{
	int32_t nRet = m_stLogQueueNetioThread.Resume();
	if(0 > nRet)
	{
		return nRet;
	}

	nRet = m_stLogQueueMainThread.Resume();
	if(0 > nRet)
	{
		return nRet;
	}

	nRet = m_stLogQueueDBAccessThread.Resume();
	if(0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

//注销日志队列
int32_t CFrameLogQueue::Uninitialize()
{
	m_stLogQueueNetioThread.Uninitialize();
	m_stLogQueueMainThread.Uninitialize();
	m_stLogQueueDBAccessThread.Uninitialize();

	return S_OK;
}


//从队列尾部追加一条日志
int32_t CFrameLogQueue::Push(ThreadType threadType, const char *szLog, const LogSource logSource, const int32_t nID1, const int32_t nID2, const char* szDate)
{
	switch (threadType)
	{
	case enmThreadType_NetioThread:
		return m_stLogQueueNetioThread.Push(szLog, logSource, nID1, nID2, szDate);
	case enmThreadType_MainThread:
	{
		MUTEX_GUARD(MainLog, m_stMainLogLock);
		return m_stLogQueueMainThread.Push(szLog, logSource, nID1, nID2, szDate);
	}
	case enmThreadType_DBAccessThread:
		return m_stLogQueueDBAccessThread.Push(szLog, logSource, nID1, nID2, szDate);
	default:
		break;
	}
	return E_LIBF_INVALIDTHREADID;
}

//从队列头部读取一条日志
int32_t CFrameLogQueue::Pop(ThreadType threadType, char* szLog, uint32_t nLogSize, LogSource& logSource, int32_t& nID1, int32_t& nID2, char* szDate)
{
	switch (threadType)
	{
	case enmThreadType_NetioThread:
		return m_stLogQueueNetioThread.Pop(szLog, nLogSize, logSource, nID1, nID2, szDate);
	case enmThreadType_MainThread:
		return m_stLogQueueMainThread.Pop(szLog, nLogSize, logSource, nID1, nID2, szDate);
	case enmThreadType_DBAccessThread:
		return m_stLogQueueDBAccessThread.Pop(szLog, nLogSize, logSource, nID1, nID2, szDate);
	default:
		break;
	}
	return E_LIBF_INVALIDTHREADID;
}


//判断队列是否已满
bool CFrameLogQueue::IsFull(ThreadType threadType)
{
	switch (threadType)
	{
	case enmThreadType_NetioThread:
		return m_stLogQueueNetioThread.IsFull();
	case enmThreadType_MainThread:
		return m_stLogQueueMainThread.IsFull();
	case enmThreadType_DBAccessThread:
		return m_stLogQueueDBAccessThread.IsFull();
	default:
		break;
	}
	return false;
}

//判读队列是否为空
bool CFrameLogQueue::IsEmpty(ThreadType threadType)
{
	switch (threadType)
	{
	case enmThreadType_NetioThread:
		return m_stLogQueueNetioThread.IsEmpty();
	case enmThreadType_MainThread:
		return m_stLogQueueMainThread.IsEmpty();
	case enmThreadType_DBAccessThread:
		return m_stLogQueueDBAccessThread.IsEmpty();
	default:
		break;
	}
	return false;
}

FRAME_NAMESPACE_END
