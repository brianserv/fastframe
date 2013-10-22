/*
 * common_tcpsocket.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_TCPSOCKET_H_
#define COMMON_TCPSOCKET_H_


#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "common_typedef.h"
#include "common_def.h"


#ifdef WIN32
typedef SOCKET		SocketFD;
enum
{
	enmInvalidSocketFD		= INVALID_SOCKET,		//无效套接字
};
#else
typedef int32_t			SocketFD;
enum
{
	enmInvalidSocketFD		= -1,					//无效套接字
};
#endif

//套接字类型
typedef enum enmSocketType
{
	enmSocketType_Listen				= 0,		//监听套接字
	enmSocketType_Communicate			= 1,		//通信套接字
} SocketType;

//套接字属性
typedef enum enmSocketAttr
{
	enmSocketAttr_Unknown				= 0,		//未知套接字
	enmSocketAttr_Outside				= 1,		//外部套接字
	enmSocketAttr_Inside				= 2,		//内部套接字
} SocketAttr;

//套接字连接状态
typedef enum enmSocketStatus
{
	enmSocketStatus_Closed				= 0,		//
	enmSocketStatus_Opened				= 1,
	enmSocketStatus_Connecting			= 2,
	enmSocketStatus_Connected			= 3,
	enmSocketStatus_Error				= 4,
	enmSocketStatus_Available			= 5,
} SocketStatus;

enum
{
	enmRecvBufferSize				= 0x00040000,		//接收缓冲区大小
	enmSendBufferSize				= 0x00040000,		//发送缓冲区大小
};


enum
{
	enmMaxConnectCount					= 500,			//最大的连接数
	enmDefaultConnectionTimeout			= 1000,			//默认连接超时时间
};


class CTCPSocket
{
public:
	CTCPSocket();
	virtual ~CTCPSocket();

private:
	//禁止拷贝构造函数和赋值操作符
	CTCPSocket(const CTCPSocket& soc);
	CTCPSocket& operator = (CTCPSocket& soc);

public:
	//获取套接字描述符
	SocketFD GetFD() const;
	//获取套接字状态
	SocketStatus GetStatus() const;

	//打开服务器套接字
	int32_t OpenAsServer(uint16_t nPort, const char* szLocalIP = NULL, bool bNonBlock = true);
	//打开客户端套接字
	int32_t OpenAsClient(const char* szLocalIP = NULL);
	//关闭套接字
	void CloseSocket();
	//关联到套接字
	int32_t Attach(SocketFD socketFD);

	//与服务端建立连接
	int32_t Connect_With_Timeout(const char* szRemoteIP, uint16_t nPort, uint16_t secs, bool bNonBlock = true);

	//与服务端建立连接
	int32_t Connect(const char* szRemoteIP, uint16_t nPort, bool bNonBlock = true, int32_t nTimeout = 0);

	//与服务端建立连接
	int32_t Connect(uint32_t nRemoteIP, uint16_t nPort, bool bNonBlock = true, int32_t nTimeout = 0);
	//断开连接
	int32_t Disconnect();

	//
	int32_t Accept(SocketFD& socketFD);

	//接收数据
	int32_t ReceiveData(uint8_t *pBuffer, uint32_t nSize, uint32_t& nReceived);
	//发送数据
	int32_t SendData(const uint8_t *pBuffer, const uint32_t nLength, uint32_t& nSent);

protected:
	SocketType			m_socketType;
	SocketFD			m_socketFD;
	SocketStatus		m_socketStatus;
};

#endif /* COMMON_TCPSOCKET_H_ */
