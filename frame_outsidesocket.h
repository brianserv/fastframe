/*
 * frame_outsidesocket.h
 *
 *  Created on: 2012-11-20
 *      Author: jimm
 */

#ifndef FRAME_OUTSIDESOCKET_H_
#define FRAME_OUTSIDESOCKET_H_

#include "common/common_codeengine.h"
#include "frame_namespace.h"
#include "frame_socket.h"
//#include "frame_socket_mgt.h"
#include "frame_protocolhead.h"

FRAME_NAMESPACE_BEGIN

//来自客户端的数据包经过封装后的最大大小
#define OUTSIDE_PACKET_SIZE			(0xffff + sizeof(ConnUin))

class COutsideSocket : public CSocket
{
public:
	COutsideSocket();

	virtual ~COutsideSocket();

	virtual void Reset();

	virtual void Clear();

	virtual int32_t OnRead(int32_t nErrorCode);

	virtual int32_t OnWrite(int32_t nErrorCode);

	virtual int32_t OnError(int32_t nErrorCode);

	virtual int32_t OnConnected();

	virtual int32_t OnDisconnect(int32_t nErrorCode);
protected:
	//从接收缓存中提取一个消息包
	virtual int32_t MakeMessage();

	//发送缓存中剩余的数据
	//int32_t SendRestData();

	//生成链接关闭事件
	int32_t MakeSystemEvent(uint16_t nEventID);

protected:
	//RoleID			m_nRoleID;
	uint16_t		m_nCurPacketSize;			//当前包的长度
	uint16_t		m_nPacketOffset;			//已经有多少数据了
	NetPacket		*m_pPacket;					//封装给应用层的网络包
};

FRAME_NAMESPACE_END

#endif /* FRAME_OUTSIDESOCKET_H_ */
