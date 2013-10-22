/*
 * common_tcpsocket.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#include <errno.h>
#include <string.h>

#include "common/common_tcpsocket.h"
#include "common/common_errordef.h"
#include "common/common_socket.h"


CTCPSocket::CTCPSocket()
{
	m_socketType = enmSocketType_Listen;
	m_socketFD = enmInvalidSocketFD;
	m_socketStatus = enmSocketStatus_Closed;
}

CTCPSocket::~CTCPSocket()
{
	if (enmInvalidSocketFD != m_socketFD)
	{
		CloseSocket();
	}
}

//获取套接字描述符
SocketFD CTCPSocket::GetFD() const
{
	return m_socketFD;
}

//获取套接字状态
SocketStatus CTCPSocket::GetStatus() const
{
	return m_socketStatus;
}

//打开服务器套接字
int32_t CTCPSocket::OpenAsServer(uint16_t nPort, const char* szLocalIP/* = NULL*/, bool nNonBlock/*= true*/)
{
	//若socket连接已存在则先关闭套接字
	if (enmInvalidSocketFD != m_socketFD)
	{
		CloseSocket();
	}

#ifdef WIN32
	//创建socket
	m_socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (enmInvalidSocketFD == m_socketFD)
	{
		return E_SOCKETCREATE;
	}

	//设置套接字属性
	int32_t op = 1;
	int32_t ret = setsockopt(m_socketFD, SOL_SOCKET, SO_REUSEADDR, (const char*)&op, sizeof(op));
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETOPTION;
	}
#else
	//打开套接字
	m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (enmInvalidSocketFD == m_socketFD)
	{
		return E_SOCKETCREATE;
	}

	//设置套接字属性
	int32_t op = 1;
	int32_t ret = setsockopt(m_socketFD, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETOPTION;
	}
#endif

	//填充服务器地址&端口信息
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((NULL != szLocalIP) ? inet_addr(szLocalIP) : htonl(INADDR_ANY));
	addr.sin_port = htons(nPort);

	ret = bind(m_socketFD, (struct sockaddr *)&addr, sizeof(addr));
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETBIND;
	}

	//开始监听
	ret = listen(m_socketFD, SOMAXCONN);
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETLISTEN;
	}

#ifndef WIN32
	//设置为非阻塞
	if(nNonBlock)
	{
		set_non_block(m_socketFD);
	}

#endif


	//更新套接字类型和状态
	m_socketType = enmSocketType_Listen;
	m_socketStatus = enmSocketStatus_Opened;

	return S_OK;

}

//打开客户端套接字
int32_t CTCPSocket::OpenAsClient(const char* szLocalIP/* = NULL*/)
{
	//若socket连接已存在则先关闭套接字
	if (enmInvalidSocketFD != m_socketFD)
	{
		CloseSocket();
	}

#ifdef WIN32
	//创建socket
	m_socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (enmInvalidSocketFD == m_socketFD)
	{
		return E_SOCKETCREATE;
	}
#else
	//打开套接字
	m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (enmInvalidSocketFD == m_socketFD)
	{
		return E_SOCKETCREATE;
	}
#endif

	if (NULL != szLocalIP)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(szLocalIP);
		int32_t ret = bind(m_socketFD, (struct sockaddr *)&addr, sizeof(addr));
		if (0 != ret)
		{
			CloseSocket();
			return E_SOCKETBIND;
		}

	}

	//更新套接字类型和状态
	m_socketType = enmSocketType_Communicate;
	m_socketStatus = enmSocketStatus_Opened;

	return S_OK;
}

//关闭套接字
void CTCPSocket::CloseSocket()
{
	if (enmInvalidSocketFD != m_socketFD)
	{
#ifdef WIN32
		closesocket(m_socketFD);
#else
		close(m_socketFD);
#endif
		m_socketFD = enmInvalidSocketFD;
	}

	//更新套接字状态
	m_socketStatus = enmSocketStatus_Closed;
}

//关联到套接字
int32_t CTCPSocket::Attach(SocketFD socketFD)
{
	//若socket连接已存在则先关闭套接字
	if (enmInvalidSocketFD != m_socketFD)
	{
		CloseSocket();
	}

	m_socketFD = socketFD;
	m_socketType = enmSocketType_Communicate;
	m_socketStatus = enmSocketStatus_Connected;

	return S_OK;
}
//与服务端建立连接
int32_t CTCPSocket::Connect_With_Timeout(const char* szRemoteIP, uint16_t nPort, uint16_t secs, bool bNonBlock/* = true*/)
{
	if (NULL == szRemoteIP)
	{
		return E_REMOTEIP;
	}

	//判断套接字类型
	if (enmSocketType_Communicate != m_socketType)
	{
		return E_SOCKETTYPE;
	}

	//套接字是否打开
	if ((enmInvalidSocketFD == m_socketFD) || (enmSocketStatus_Opened != m_socketStatus))
	{
		return E_SOCKETNOTCREATED;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(szRemoteIP);
	addr.sin_port = htons(nPort);
	int32_t ret; 
#ifdef WIN32
	if(bNonBlock)
	{
		unsigned long ul = 1;
		ret = ioctlsocket(m_socketFD,FIONBIO,(unsigned long*)&ul);
		if(ret == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
	}
#else
	if (bNonBlock)
	{
		set_non_block(m_socketFD);
	}
#endif

	//与服务器端建立连接
	ret = connect(m_socketFD, (const struct sockaddr*)&addr, sizeof(addr));
	if (0 != ret)
	{
#ifdef WIN32
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#else
		if(errno != EINPROGRESS)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#endif

	}
	struct timeval to;
	fd_set rSet,wSet,eSet;
	FD_ZERO(&rSet);
	FD_ZERO(&wSet);
	FD_ZERO(&eSet);
	FD_SET(m_socketFD,&rSet);
	FD_SET(m_socketFD,&wSet);
	FD_SET(m_socketFD,&eSet);
	to.tv_sec = secs;
	to.tv_usec= 0;
#ifdef WIN32
	ret = select(0,&rSet,&wSet,&eSet,&to);
#else
	ret = select(m_socketFD+1,&rSet,&wSet,0,&to);
#endif
	if(ret <= 0)
	{
		//select error or timeout
		CloseSocket();
		return E_SOCKETCONNECT;
	}
#ifdef WIN32
	if (FD_ISSET(m_socketFD,&eSet))
	{
		//error happened
		CloseSocket();
		return E_SOCKETCONNECT;
	}
#else
	if (FD_ISSET(m_socketFD,&rSet) && FD_ISSET(m_socketFD,&wSet))
	{
		//error happened
		CloseSocket();
		return E_SOCKETCONNECT;
	}
#endif

	//更新套接字状态
	m_socketStatus = enmSocketStatus_Connected;

	return S_OK;
}

//与服务端建立连接
int32_t CTCPSocket::Connect(const char* szRemoteIP, uint16_t nPort, bool bNonBlock/* = true*/, int32_t nTimeout/* = 0*/)
{
	if (NULL == szRemoteIP)
	{
		return E_REMOTEIP;
	}

	//判断套接字类型
	if (enmSocketType_Communicate != m_socketType)
	{
		return E_SOCKETTYPE;
	}

	//套接字是否打开
	if ((enmInvalidSocketFD == m_socketFD) || (enmSocketStatus_Opened != m_socketStatus))
	{
		return E_SOCKETNOTCREATED;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(szRemoteIP);
	addr.sin_port = htons(nPort);

	if (bNonBlock)
	{
#ifndef WIN32
		set_non_block(m_socketFD);
		//int flags = fcntl(m_socketFD, F_GETFL, 0);
		//fcntl(m_socketFD, F_SETFL, flags | O_NONBLOCK);
#else
		unsigned long ul = 1;
		int32_t ret = ioctlsocket(m_socketFD, FIONBIO, (unsigned long*)&ul);
		if (ret == SOCKET_ERROR)
		{
			return E_SOCKETCONTROL;
		}
#endif
	}

	//与服务器端建立连接
	int32_t ret = connect(m_socketFD, (const struct sockaddr*)&addr, sizeof(addr));
	if (0 != ret)
	{
#ifdef WIN32
		DWORD error = WSAGetLastError();
		if (error != WSAEINPROGRESS && error != WSAEWOULDBLOCK)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#else
		if (errno != EINPROGRESS)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
		else if (bNonBlock)
		{
			m_socketStatus = enmSocketStatus_Connecting;
			return E_SOCKET_CONNECTING;
		}
#endif
	}

	//非阻塞并等待超时
	if (bNonBlock && (0 < nTimeout))
	{
		timeval tv;
		tv.tv_sec = nTimeout / MS_PER_SECOND;
		tv.tv_usec = (nTimeout % MS_PER_SECOND) * US_PER_MS;

		fd_set rset;
		fd_set wset;
		FD_ZERO(&rset);
		FD_SET(m_socketFD, &rset);
		wset = rset;

		ret = select((int)m_socketFD + 1, &rset, &wset, NULL, &tv);
		if (0 >= ret)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#ifndef WIN32
		if ((FD_ISSET(m_socketFD, &rset)) && (FD_ISSET(m_socketFD, &wset)))
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#endif
	}

	//更新套接字状态
	m_socketStatus = enmSocketStatus_Connected;

	return S_OK;
}

//与服务端建立连接
int32_t CTCPSocket::Connect(uint32_t nRemoteIP, uint16_t nPort, bool bNonBlock/* = true*/, int32_t nTimeout/* = 0*/)
{
	//判断套接字类型
	if (enmSocketType_Communicate != m_socketType)
	{
		return E_SOCKETTYPE;
	}

	//套接字是否打开
	if ((enmInvalidSocketFD == m_socketFD) || (enmSocketStatus_Opened != m_socketStatus))
	{
		return E_SOCKETNOTCREATED;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = nRemoteIP;
	addr.sin_port = htons(nPort);

	if (bNonBlock)
	{
#ifndef WIN32
		set_non_block(m_socketFD);
#else
		unsigned long ul = 1;
		int32_t ret = ioctlsocket(m_socketFD, FIONBIO, (unsigned long*)&ul);
		if (ret == SOCKET_ERROR)
		{
			return E_SOCKETCONTROL;
		}
#endif
	}

	//与服务器端建立连接
	int32_t ret = connect(m_socketFD, (const struct sockaddr*)&addr, sizeof(addr));
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETCONNECT;
	}

	//非阻塞并等待超时
	if (bNonBlock && (0 < nTimeout))
	{
		timeval tv;
		tv.tv_sec = nTimeout / MS_PER_SECOND;
		tv.tv_usec = (nTimeout % MS_PER_SECOND) * US_PER_MS;

		fd_set rset;
		fd_set wset;
		FD_ZERO(&rset);
		FD_SET(m_socketFD, &rset);
		wset = rset;

		ret = select((int)m_socketFD + 1, &rset, &wset, NULL, &tv);
		if (0 >= ret)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#ifndef WIN32
		if ((FD_ISSET(m_socketFD, &rset)) && (FD_ISSET(m_socketFD, &wset)))
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#endif
	}

	//更新套接字状态
	m_socketStatus = enmSocketStatus_Connected;

	return S_OK;
}

//断开连接
int32_t CTCPSocket::Disconnect()
{
	CloseSocket();
	return S_OK;
}

//
int32_t CTCPSocket::Accept(SocketFD& socketFD)
{
	socketFD = accept(m_socketFD, NULL, NULL);
	if (0 > socketFD)
	{
		return E_SOCKETACCEPT;
	}

	return S_OK;
}

//接收数据
int32_t CTCPSocket::ReceiveData(uint8_t *pBuffer, uint32_t nSize, uint32_t& nReceived)
{
	//套接字是否打开
	if ((enmInvalidSocketFD == m_socketFD) || ((enmSocketStatus_Available != m_socketStatus)&&(enmSocketStatus_Connected != m_socketStatus)))
	{
		return E_SOCKETNOTCREATED;
	}

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	//检查可读状态
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_socketFD, &fds);
	int ret = select((int)m_socketFD + 1, &fds, NULL, NULL, &tv);
	if (0 >= ret)
	{
		nReceived = 0;
		return S_NODATA;
	}

	//接收数据
#ifdef WIN32
	int32_t n = recv(m_socketFD, (char*)pBuffer, nSize, 0);
#else
	int32_t n = recv(m_socketFD, pBuffer, nSize, 0);
#endif
	if (0 < n)
	{
		nReceived = (uint32_t)n;
		return S_OK;
	}
	else if (0 == n)
	{	
		//连接已断开
		CloseSocket();
		return E_SOCKETDISCONNECTED;
	}
#ifdef WIN32
	else if (WSAGetLastError() != ERROR_IO_PENDING)
	{
		//接收出错
		CloseSocket();
		return E_SOCKETERROR;
	}
#else
	else if (EAGAIN != errno)
	{
		//接收出错
		CloseSocket();
		return E_SOCKETERROR;
	}
#endif

	//数据正在接收
	nReceived = 0;

	return S_FALSE;
}

//发送数据
int32_t CTCPSocket::SendData(const uint8_t *pBuffer, const uint32_t nLength, uint32_t& nSent)
{
	if ((NULL == pBuffer) || (0 == nLength))
	{
		return E_INVALIDARGUMENT;
	}

	//套接字是否打开
	if ((enmInvalidSocketFD == m_socketFD) || ((enmSocketStatus_Connected != m_socketStatus)&&(enmSocketStatus_Available != m_socketStatus)))
	{
		return E_SOCKETNOTCREATED;
	}

#ifdef WIN32
	int32_t n = send(m_socketFD, (const char*)pBuffer, nLength, 0);
#else
	int32_t n = send(m_socketFD, pBuffer, nLength, 0);
#endif
	if (n > 0)
	{
		nSent = n;
		return S_OK;
	}
#ifdef WIN32
	else if ((WSAGetLastError() != ERROR_IO_PENDING) && (WSAGetLastError() != WSAENOBUFS))
	{
		//发送出错
		CloseSocket();
		return E_SOCKETERROR;
	}
#else
	else if (EAGAIN != errno)
	{
		//发送出错
		CloseSocket();
		return E_SOCKETERROR;
	}
#endif

	//缓冲区满
	nSent = 0;

	return S_SENDNOTCOMPLETED;

}
