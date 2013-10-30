/*
 * frame_netthread.cpp
 *
 *  Created on: 2012-11-1
 *      Author: jimm
 */

#include "frame_protocolhead.h"
#include "common/common_api.h"
#include "common/common_datetime.h"
#include "common/common_message.h"
#include "common/common_socket.h"
#include "common/common_tcpsocket.h"
#include "common/common_codeengine.h"
#include "frame_netthread.h"
#include "frame_netqueue.h"
#include "frame_configmgt.h"
//#include "frame_socket_mgt.h"
#include "frame_mem_mgt.h"
#include "frame_eventid.h"
#include "frame_sockettimer.h"
#include "frame_msgevent_mgt.h"

#ifndef WIN32
#include<cstring>
#include <errno.h>
#endif

FRAME_NAMESPACE_BEGIN

CFrameNetThread::CFrameNetThread()
{
	m_nReconnectTime = 10;
	m_nLastConnectTime = 0;
}

CFrameNetThread::~CFrameNetThread()
{

}

int32_t CFrameNetThread::Initialize()
{
	int32_t nRet = m_stEpoll.Create(0xffff);
	if(nRet < 0)
	{
		return -1;
	}

	return S_OK;
}

int32_t CFrameNetThread::Resume()
{
	return S_OK;
}

int32_t CFrameNetThread::Uninitialize()
{
	return S_OK;
}

bool CFrameNetThread::Execute()
{
	bool bHasData = false;
	//while(!GetTerminated())
	//{
		//消息泵
		if(MessagePump() > 0)
		{
			bHasData = true;
		}
		//处理socket超时事件
		HandleTimeOutEvent();
	//}
	return bHasData;
}

CEpoll *CFrameNetThread::GetEpollObj()
{
	return &m_stEpoll;
}

int32_t CFrameNetThread::MessagePump()
{
	int32_t nMessageResult = SendMessage();

	int32_t nWaitTimeout = 0;
	if(nMessageResult == S_FALSE)
	{
		nWaitTimeout = 50;
	}
	int32_t nEventCount = m_stEpoll.HandleMessage(nWaitTimeout);

	return nEventCount;
}

int32_t CFrameNetThread::SendMessage()
{
	int32_t ret = S_OK;
	NetPacket *pNetPacket = NULL;
	//从队列中取出数据
	ret = g_FrameNetQueue.PopSendCSQueue(pNetPacket);
	if ((0 > ret) || (pNetPacket == NULL))
	{
		return S_FALSE;
	}

	int32_t nSocketCount = 0;
	CSocket *arrSocket[enmMaxConnectCount];

//	GetSocket(enmTransType_P2P, enmEntityType_Client, pConnInfo->nTunnelIndex,
//			arrSocket, enmMaxConnectCount, nSocketCount);

	ConnInfo *pConnInfo = (ConnInfo *)&pNetPacket->m_pNetPacket[0];
	if(pConnInfo->pSocket == NULL)
	{
		FREE((uint8_t *)pNetPacket);
		return E_NULLPOINTER;
	}

	arrSocket[nSocketCount++] = (CSocket *)pConnInfo->pSocket;

	uint8_t *pBufCS = &pNetPacket->m_pNetPacket[sizeof(ConnInfo)];
	uint32_t nBufSizeCS = pNetPacket->m_nNetPacketLen/* - sizeof(ConnInfo)*/;

	uint32_t nOffset = 0;
	MessageHeadCS stMsgHead;
	ret = stMsgHead.MessageDecode(pBufCS, nBufSizeCS, nOffset);
	if(ret < 0)
	{
		FREE((uint8_t *)pNetPacket);
		return E_UNKNOWN;
	}

	if(stMsgHead.nMessageID == 18)
	{
		char arrTimeString[1024];
		GetTimeString(arrTimeString);

		uint32_t offset = 0;
		char szLog[enmMaxLogInfoLength];
		szLog[0] = 0;

		sprintf(szLog + offset, "{SendTime : %s} MessageHeadCS=", arrTimeString);
		offset = (uint32_t)strlen(szLog);
		stMsgHead.Dump(szLog, enmMaxLogInfoLength, offset);

		uint8_t *pBody = pBufCS + nOffset;
		uint32_t nUserID = *(uint32_t *)pBody;
		uint8_t nNameLen = *(uint8_t *)(pBody + 5);
		uint8_t nChatLen = *(uint8_t *)(pBody + 6);
		char arrContent[1024];
		memcpy(arrContent, pBody + 7 + nNameLen, nChatLen);
		arrContent[nChatLen] = '\0';
		sprintf(szLog + offset, " MessageBody={userid[%u] send world chat content[%s]}",
				nUserID, arrContent);
		offset = (uint32_t)strlen(szLog);

		WRITE_MAIN_LOG(enmLogLevel_Notice, "%s\n", szLog);
	}



	int32_t nSendBytes = 0;
	for(int32_t i = 0; i < nSocketCount; ++i)
	{
		ret = arrSocket[i]->Send(pBufCS, nBufSizeCS, nSendBytes);
		if(ret < 0)
		{
			g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "send data failed!{nSendBytes=%d, "
					"len=%d, PeerAddress=%s, PeerPort=%d, errno=%d, desc=%s}\n",
					nSendBytes, pNetPacket->m_nNetPacketLen, /*stMessageHead.nMessageID,*/
					arrSocket[i]->GetPeerAddressStr(), arrSocket[i]->GetPeerPort(), errno, strerror(errno));

			if(ret < 0)
			{
				//关闭连接
				arrSocket[i]->CloseSocket(SYS_EVENT_CONN_ERROR);
			}
		}
		else
		{
			if((uint32_t)nSendBytes < nBufSizeCS)
			{
				if(errno != EAGAIN)
				{
					arrSocket[i]->CloseSocket(SYS_EVENT_CONN_ERROR);
				}
			}
		}
	}

	FREE((uint8_t *)pNetPacket);

	return S_OK;
}

int32_t CFrameNetThread::HandleTimeOutEvent()
{
	int32_t nTimerCount = g_FrameSocketTimerMgt.GetTimerCount();

	for(int32_t i = 0; i < nTimerCount; ++i)
	{
		CFrameSocketTimer *pTimer = g_FrameSocketTimerMgt.GetFirstSocketTimer();
		if(pTimer->m_nEndTime > CDateTime::CurrentDateTime().Seconds())
		{
			break;
		}

		CSocket *pSocket = pTimer->pSocket;
		SocketTimerProc pTimerProc = pTimer->pTimerProc;

		(pSocket->*pTimerProc)(pTimer);
	}

	return S_OK;
}

FRAME_NAMESPACE_END

