/*
 * frame_socket.h
 *
 *  Created on: 2012-11-20
 *      Author: jimm
 */

#ifndef FRAME_SOCKET_H_
#define FRAME_SOCKET_H_

#include "common/common_socket.h"
#include "common/common_tcpsocket.h"
#include "common/common_errordef.h"
#include "common/common_cyclebuffer.h"
#include "common/common_message.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_epoll.h"
#include "lightframe_impl.h"
#include "frame_sockettimer.h"

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

FRAME_NAMESPACE_BEGIN

class CSocket: public IIOHandler
{
public:
	CSocket();
	virtual ~CSocket();

	virtual void Reset()
	{
		Clear();

		m_nSocketAttr = enmSocketAttr_Unknown;
		m_nPeerType = 0;
		m_nPeerID = enmInvalidServerID;
		m_nBelongPort = 0;
	}

	//子类socket如果需要重置数据成员，最好派生此函数
	virtual void Clear()
	{
		m_nSocketFD = enmInvalidSocketFD;
		m_nSocketType = enmSocketType_Communicate;
		m_nSocketStatus = enmSocketStatus_Closed;
		memset(m_szClientAddress, 0, sizeof(m_szClientAddress));
		m_nPeerAddress = 0;
		m_nPeerPort = 0;
		m_nCreateTime = 0;
		m_nTotalRecvBytes = 0;
		m_nLastRecvTime = 0;
		m_nTotalSendBytes = 0;
		m_nLastSendTime = 0;
		m_stRecvBuffer.Reset();
		m_stSendBuffer.Reset();
//		m_nConnectTimerIndex = enmInvalidTimerIndex;
		m_pSocketTimer = NULL;

		m_nIOEvents = 0;
	}
	//设置套接字描述符
	void SetSocketFD(SocketFD nSocketFD);
	//获取套接字描述符
	SocketFD GetSocketFD() const;
	//设置套接字状态
	void SetSocketStatus(SocketStatus nSocketStatus);
	//获取套接字状态
	SocketStatus GetStatus() const;

	void SetSocketType(SocketType nSocketType);

	SocketType GetSocketType();

	void SetSocketAttr(SocketAttr nSocketAttr);

	SocketAttr GetSocketAttr();

	void SetPeerID(ServerID nPeerID);

	ServerID GetPeerID();

	void SetPeerType(EntityType nPeerType);

	EntityType GetPeerType();

	char *GetPeerAddressStr();

	void SetPeerAddress(uint32_t nAddress);

	uint32_t GetPeerAddress();

	void SetPeerPort(uint16_t nPort);

	uint16_t GetPeerPort();

	uint32_t GetListenAddress();

	uint16_t GetListenPort();

	void SetBelongPort(uint16_t nPort);

	uint16_t GetBelongPort();

	void SetCreateTime(time_t nCreateTime);

	void SetEpollObj(CEpoll *pEpoll);

	CEpoll *GetEpollObj();

	virtual int32_t ReadEvent();

	virtual int32_t WriteEvent();

	virtual int32_t ErrorEvent();

	//打开套接字
	virtual int32_t OpenSocketAsServer(const char* szLocalIP, uint16_t nPort);
	//打开套接字
	virtual int32_t OpenSocketAsClient(const char* szLocalIP = NULL);
	//关闭套接字
	virtual void CloseSocket(int32_t nCloseCode = 0);
	//与服务端建立连接
	virtual int32_t Connect(const char* szRemoteIP, uint16_t nPort);
	//接收数据
	virtual int32_t Recv(uint8_t *pBuffer, int32_t nSize, int32_t& nRecvBytes);
	//发送数据
	virtual int32_t Send(const uint8_t *pBuffer, const int32_t nLength, int32_t& nSendBytes);

	uint32_t GetIOEvents();

	void SetIOEvents(uint32_t nEvents);

protected:
	//读事件回调
	virtual int32_t OnRead(int32_t nErrorCode) = 0;
	//写事件回调
	virtual int32_t OnWrite(int32_t nErrorCode) = 0;
	//异常事件回调
	virtual int32_t OnError(int32_t nErrorCode) = 0;
	//接收连接回调(重载此函数，可以在这里做些连接信息初始化的工作)
	virtual int32_t OnAccept(SocketFD nAcceptFD, struct sockaddr_in stPeerAddress,
			socklen_t nPeerAddressLen);
	//连接成功回调(重载此函数，可以在这里做些连接信息初始化的工作)
	virtual int32_t OnConnected();
	//连接超时回调
	virtual int32_t OnConnectTimeout();
	//断开连接回调(重载此函数，可以在这里做些资源回收的工作)
	virtual int32_t OnDisconnect(int32_t nCloseCode);
	//发送缓存中剩余的数据
	virtual int32_t SendRestData();
	//尝试尽最大努力读取想读的数据
	int32_t nRead(uint8_t *pBuffer, const int32_t nLength,bool &isConnClose);
	//尝试尽最大努力写入想写的数据
	int32_t nWrite(const uint8_t *pBuffer, const int32_t nLength);
	//改变epoll事件
	void ChangeWriteEvent();

private:
	//定时器事件
	int32_t OnTimerEvent(CFrameSocketTimer *pTimer);
	//连接超时
	int32_t ConnectTimeout();
	//连接成功
	int32_t Connected();
	//连接关闭
	int32_t Disconnected(int32_t nCloseCode);

protected:
	SocketFD		m_nSocketFD;
	SocketType		m_nSocketType;
	SocketStatus	m_nSocketStatus;
	SocketAttr		m_nSocketAttr;
	char			m_szClientAddress[enmMaxIPAddressLength];
	EntityType		m_nPeerType;
	ServerID		m_nPeerID;
	uint32_t		m_nPeerAddress;				//对端ip
	uint16_t		m_nPeerPort;				//对端port
	uint32_t		m_nListenAddress;			//监听地址
	uint16_t		m_nListenPort;				//监听端口
	uint16_t		m_nBelongPort;				//所属端口(此套接字只在此端口连接上)
	time_t			m_nCreateTime;				//连接创建时间
	int32_t			m_nTotalRecvBytes;			//收到的数据大小
	time_t			m_nLastRecvTime;			//最近一次从对端接收数据的时间
	int32_t			m_nTotalSendBytes;			//发送的数据大小
	time_t			m_nLastSendTime;			//最近一次发给对端数据的时间
	CycleBuffer<enmRecvBufferSize>	m_stRecvBuffer;	//接收缓冲区
	CycleBuffer<enmSendBufferSize>	m_stSendBuffer;	//发送缓存，在套接字异常或者系统发送缓冲满的情况下，数据将会被放进此缓存

//	TimerIndex		m_nConnectTimerIndex;		//连接超时定时器索引
	CFrameSocketTimer *m_pSocketTimer;
	CEpoll			*m_pEpoll;
	uint32_t		m_nIOEvents;
};

FRAME_NAMESPACE_END

#endif /* FRAME_SOCKET_H_ */
