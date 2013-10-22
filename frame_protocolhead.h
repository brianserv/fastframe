/*
 * frame_protocolhead.h
 *
 *  Created on: 2011-11-23
 *      Author: jimm
 */

#ifndef FRAME_PROTOCOLHEAD_H_
#define FRAME_PROTOCOLHEAD_H_

#include "common/common_typedef.h"
#include "common/common_codeengine.h"
#include "common/common_string.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_msg_impl.h"
#include <string.h>
#include <stdio.h>

FRAME_NAMESPACE_BEGIN

typedef uint8_t						MessageType;		//消息类型
enum
{
	enmMessageType_None				= 0x00,				//无效值
	enmMessageType_Request			= 0x01,				//请求
	enmMessageType_Response			= 0x02,				//响应
	enmMessageType_Notify			= 0x03,				//通知
	enmMessageType_Other			= 0x04,				//其他
};

typedef uint8_t						TransType;			//传输类型
enum
{
	enmTransType_None				= 0xFF,				//无效值
	enmTransType_P2P				= 0,				//点对点
	enmTransType_P2G				= 1,				//组播
	enmTransType_Broadcast			= 2,   				//广播
	enmTransType_ByKey				= 3,				//根据关键字转发
	enmTransType_Regist				= 4,				//Server注册
	enmTransType_Ramdom				= 5,				//在指定的目的servertype中随机选择一个发送
	enmTransType_Disconnect         = 6,				//server链接断开
};

enum
{
	enmMaxOptionalCount				= 128,				//最大可选字段大小
	enmMaxDestinationCout			= 126,				//组播时最大目的地数量
};

enum
{
	enmMinCSHeadMessageLength		= 28,			//CS消息头部最小长度
};

/*********************************************************************
* 通信协议相关数据结构定义
*********************************************************************/


//CS消息头部
class MessageHeadCS
{
public:
	uint32_t		nTotalSize;		//代表整个包的长度,如果有packSecHead包，包含packSecHead的长度
	uint8_t			nHaveSecHead;	//是否有第二个头
	uint16_t		nMessageID;		//消息id

	enum
	{
		enmInvalidFlag		= 0,
		enmHaveSecondHead	= 210,
	};

	MessageHeadCS()
	{
		nTotalSize = 0;
		nHaveSecHead = enmHaveSecondHead;
		nMessageID = 0;
	}

	//返回没有空隙的结构体长度
	static int32_t CompactSize()
	{
		return (sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint16_t));
	}

	static int32_t HeadSize()
	{
		return CompactSize() - sizeof(uint32_t);
	}

	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;
		*((uint32_t *)(buf + offset)) = nTotalSize;
		offset += sizeof(nTotalSize);

		*(buf + offset) = nHaveSecHead;
		offset += sizeof(nHaveSecHead);

		*((uint16_t *)(buf + offset)) = nMessageID;
		offset += sizeof(nMessageID);

		return ret;
	}

	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;
		nTotalSize = *((uint32_t *)(buf + offset));
		offset += sizeof(nTotalSize);

		nHaveSecHead = *(buf + offset);
		offset += sizeof(nHaveSecHead);

		nMessageID = *((uint16_t *)(buf + offset));
		offset += sizeof(nMessageID);

		return ret;
	}

	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
	    sprintf(buf + offset, "{nTotalSize=%u, nHaveSecHead=%d, nMessageID=%d}",
	    		nTotalSize, nHaveSecHead, nMessageID);
	    offset = (uint32_t)strlen(buf);
	}
};



class ConnUin
{
public:
	void					*pSocket;
	uint16_t				nTunnelIndex;
	uint8_t					nErrorCode;				//错误码

	ConnUin()
	{
		Reset();
	}

	void Reset()
	{
		pSocket			= NULL;
		nTunnelIndex	= 0;
		nErrorCode		= 0;
	}

	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
	    sprintf(buf + offset, "{nTunnelIndex=%d, nErrorCode=%d}",
	    		nTunnelIndex, nErrorCode);
	    offset = (uint32_t)strlen(buf);
	}
};

typedef ConnUin		ConnInfo;


class NetPacket
{
public:
	NetPacket()
	{
		m_nNetPacketLen = 0;
	}

	virtual ~NetPacket()
	{
	}

	int32_t 	m_nNetPacketLen;	//不包含自身4字节的长度
	uint8_t		m_pNetPacket[0];
};

FRAME_NAMESPACE_END

#endif /* FRAME_PROTOCOLHEAD_H_ */
