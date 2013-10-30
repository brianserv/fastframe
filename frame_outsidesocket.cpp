/*
 * frame_clientsocket.cpp
 *
 *  Created on: 2012-12-3
 *      Author: jimm
 */

#include "frame_netqueue.h"
#include "frame_outsidesocket.h"
#include "common/common_codeengine.h"
#include "frame_eventid.h"
#include "frame_configmgt.h"
//#include "frame_connection_mgt.h"
#include "frame_mem_mgt.h"
#include "frame_msgevent_mgt.h"

FRAME_NAMESPACE_BEGIN

COutsideSocket::COutsideSocket()
{
	Reset();
}

COutsideSocket::~COutsideSocket()
{

}

void COutsideSocket::Reset()
{
	m_nSocketAttr = enmSocketAttr_Outside;

	Clear();
}

void COutsideSocket::Clear()
{
	CSocket::Clear();

	//m_nRoleID = enmInvalidRoleID;
	if(m_pPacket != NULL)
	{
		FREE((uint8_t *)m_pPacket);
	}
	m_pPacket = NULL;
	m_nCurPacketSize = 0;
	m_nPacketOffset = 0;
}

int32_t COutsideSocket::OnRead(int32_t nErrorCode)
{
	uint8_t arrBuf[enmMaxMessageSize];

	int32_t nCloseCode = 0;
	int32_t nRecvBytes = 0;
	int32_t nRet = Recv(arrBuf, enmMaxMessageSize, nRecvBytes);
	if(nRet != S_OK)
	{
		nCloseCode = nRet;
	}

	m_stRecvBuffer.Write(arrBuf, nRecvBytes);
	//提取消息包
	MakeMessage();

	if(nCloseCode != 0)
	{
		CloseSocket(nCloseCode);
		return E_SOCKETERROR;
	}

	return S_OK;
}

int32_t COutsideSocket::OnWrite(int32_t nErrorCode)
{
	if(nErrorCode != 0)
	{
		CloseSocket(SYS_EVENT_CONN_ERROR);
		return E_SOCKETERROR;
	}

	int32_t nRet = SendRestData();
	if(nRet < 0)
	{
		CloseSocket(SYS_EVENT_CONN_ERROR);
		return E_SOCKETERROR;
	}

	return S_OK;
}

int32_t COutsideSocket::OnError(int32_t nErrorCode)
{
	CloseSocket(SYS_EVENT_CONN_ERROR);
	return S_OK;
}

int32_t COutsideSocket::OnConnected()
{
	//刚连接成功是不会有数据的
	MakeMessage();

	MakeSystemEvent(SYS_EVENT_INITIATIVE_CONNECT_SUCCESS);

	return S_OK;
}

int32_t COutsideSocket::OnDisconnect(int32_t nCloseCode)
{
	m_pEpoll->DeleteEvent(this);

	//没有通过认证的就不给应用层发送断开连接事件
	//if(g_FrameConnectionMgt.GetConnection(m_nSocketFD) != NULL)
	{
		MakeMessage();

		//客户端主动关闭
		if(nCloseCode == (int32_t)E_SOCKETDISCONNECTED)
		{
			MakeSystemEvent(SYS_EVENT_CONN_CLIENT_CLOSED);
		}
		else
		{
			MakeSystemEvent(SYS_EVENT_CONN_ERROR);
		}
	}

//	//解除peer端与此socket对象的映射
//	g_FrameSocketMgt.DelSocket(m_nPeerType, m_nSocketFD);
//	//回收此socket对象
//	g_FrameSocketMgt.DestroySocketObject(this);

	return S_OK;
}

//生成链接关闭事件
int32_t COutsideSocket::MakeSystemEvent(uint16_t nEventID)
{
	int32_t nPacketLen = sizeof(NetPacket) + sizeof(ConnInfo);
	uint8_t *pMem = MALLOC(nPacketLen);//g_FrameMemMgt.AllocBlock(nPacketLen);
	if(pMem == NULL)
	{
		return E_NULLPOINTER;
	}

	NetPacket *pNetPacket = new(pMem)NetPacket();
	pNetPacket->m_nNetPacketLen = nPacketLen;

	ConnInfo *pConnInfo = new(&pNetPacket->m_pNetPacket[0])ConnInfo;
	pConnInfo->pSocket = this;
	pConnInfo->nTunnelIndex = (uint16_t)m_nSocketFD;
	pConnInfo->nErrorCode = nEventID;

	g_FrameNetQueue.PushRecvCSQueue(pNetPacket);

	return S_OK;
}

int32_t COutsideSocket::MakeMessage()
{
	//开始组包
	while(m_stRecvBuffer.Size() > 0)
	{
		MessageHeadCS stMsgHead;
		if(m_nCurPacketSize <= 0)
		{
			//获取包大小字段所占的字节数
			int32_t nPacketLengthSize = sizeof(stMsgHead.nTotalSize);
			if(m_stRecvBuffer.Size() < nPacketLengthSize)
			{
				break;
			}

			//取出包头前面的包大小字段，然后解码出来
			uint8_t arrPacketSize[nPacketLengthSize];
			m_stRecvBuffer.PeekRead(arrPacketSize, nPacketLengthSize);

			uint32_t nOffset = 0;
			uint32_t nPacketSize = *((uint32_t *)arrPacketSize);
			m_nCurPacketSize = nPacketSize + nPacketLengthSize;
			//CCodeEngine::Decode(arrPacketSize, nPacketLengthSize, nOffset, m_nCurPacketSize);
		}

		int32_t nWantDataSize = (int32_t)(m_nCurPacketSize - m_nPacketOffset);
		int32_t nRealGetDataSize = (m_stRecvBuffer.Size() >= nWantDataSize ? nWantDataSize : m_stRecvBuffer.Size());
		if(nRealGetDataSize <= 0)
		{
			break;
		}

		if(m_pPacket == NULL)
		{
			int32_t nPacketLen = m_nCurPacketSize + sizeof(ConnInfo);
			uint8_t *pMem = MALLOC(sizeof(NetPacket) + nPacketLen);
			if(pMem == NULL)
			{
				break;
			}

			m_pPacket = new(pMem)NetPacket();
			m_pPacket->m_nNetPacketLen = nPacketLen;
		}

		//前面存放conninfo结构
		uint8_t *pPacketHead = &m_pPacket->m_pNetPacket[sizeof(ConnInfo)];

		if(m_stRecvBuffer.Read(&pPacketHead[m_nPacketOffset], nRealGetDataSize) <= 0)
		{
			break;
		}

		m_nPacketOffset += nRealGetDataSize;
		if(m_nPacketOffset >= m_nCurPacketSize)
		{
			uint32_t nOffset = 0;
			int32_t nRet = stMsgHead.MessageDecode(pPacketHead, m_nPacketOffset, nOffset);
			if(nRet < 0)
			{
				FREE((uint8_t *)m_pPacket);
				m_pPacket = NULL;
				m_nPacketOffset = 0;
				m_nCurPacketSize = 0;
				return nRet;
			}
			//m_nRoleID = stMsgHead.nRoleID;

			ConnInfo *pConnInfo = new(&m_pPacket->m_pNetPacket[0]) ConnInfo;
			pConnInfo->pSocket = this;
			pConnInfo->nTunnelIndex = (uint16_t)m_nSocketFD;
			pConnInfo->nErrorCode = 0;

			g_FrameNetQueue.PushRecvCSQueue(m_pPacket);
			m_pPacket = NULL;
			m_nPacketOffset = 0;
			m_nCurPacketSize = 0;
		}
	}

	return S_OK;
}

FRAME_NAMESPACE_END
