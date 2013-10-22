/*
 * common_socket.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef WIN32

#include <fcntl.h>
#include <sys/ioctl.h>

#include <sys/param.h>
#include <sys/cdefs.h>

#include "common/common_socket.h"


//
//int32_t ip_string_to_addr( const char* string_ip, int32_t& ip )
//{
//	struct in_addr stAddr;
//	if(!string_ip || inet_pton(AF_INET, string_ip, &stAddr) <=0)
//	{
//		return -1;
//	}
//	ip = (int32_t)stAddr.s_addr;
//	return 0;
//}
//
//int32_t ip_addr_to_string(int32_t ip, char* string_ip, int16_t buflen)
//{
//	if (NULL == string_ip || buflen <= 0 || buflen < INET_ADDRSTRLEN)
//	{
//		return -1;
//	}
//
//	if (NULL == inet_ntop(AF_INET, (const void*)&ip, string_ip, socklen_t(buflen)))
//	{
//		return -1;
//	}
//
//	return 0;
//}
//
//int32_t socket_address_to_string( struct sockaddr_in& stAddress, char* pszIPPort, size_t size)
//{
//	if (NULL == pszIPPort || 0 == size)
//	{
//		return -1;
//	}
//
//	char* pszTmpIP = NULL;
//	pszTmpIP = inet_ntoa(stAddress.sin_addr);
//	if( NULL == pszTmpIP)
//	{
//		return -1;
//	}
//
//	u_short port = ntohs(stAddress.sin_port);
//	snprintf(pszIPPort, size-1, "%s:%d", pszTmpIP, port);
//	pszTmpIP[size-1] = '\0';
//
//	return 0;
//}

int32_t set_non_block( int32_t fd )
{

	int flag = 0;
	flag = fcntl(fd, F_GETFL, 0);
	if (flag < 0)
	{
		return -1;
	}

	flag |= O_NONBLOCK;
	flag |= O_NDELAY;
	
	if (fcntl(fd, F_SETFL, flag) < 0)
	{
		return -1;
	}

	return 0;
}
//
//int32_t set_socket_buffer_size( int32_t fd, size_t size )
//{	
//	if (fd < 0 )
//	{
//		return -1;
//	}
//
//	int result = 0;
//	size_t recv_buffer_size = size;
//	socklen_t opt_length = (socklen_t)sizeof(recv_buffer_size);
//	result = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &recv_buffer_size, opt_length);
//	if(0 != result)
//	{
//		return -1;
//	}
//
//	size_t send_buffer_size = size;
//	opt_length = (socklen_t)sizeof(send_buffer_size);
//	result = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &send_buffer_size, opt_length);
//	if (0 != result)
//	{
//		return -1;
//	}
//
//	return 0;
//}
//
//int32_t create_udp_socket( uint16_t port, enmSocketBlockFlag flag /*= enmSocketBlockFlag_lock*/, char* pszIP /*= NULL*/ )
//{
//	if (0 == port)
//	{
//		return -1;
//	}
//
//	int32_t fd = -1;
//	fd = socket(AF_INET, SOCK_DGRAM, 0);
//	if( fd < 0 )
//	{
//		return -1;
//	}
//
//	if(enmSocketBlockFlag_nonblock == flag )
//	{
//		set_non_block(fd);
//	}
//
//	//bind ip address info
//	struct sockaddr_in stSocketAddr;
//	memset((void *)&stSocketAddr, 0x0, sizeof(stSocketAddr));
//	stSocketAddr.sin_family = AF_INET;
//	if(NULL != pszIP)
//	{
//		stSocketAddr.sin_addr.s_addr = inet_addr(pszIP);
//	}
//	else
//	{
//		stSocketAddr.sin_addr.s_addr = INADDR_ANY;
//	}
//	stSocketAddr.sin_port = htons(port);
//
//	//
//	if (bind(fd, (struct sockaddr*) &stSocketAddr, (socklen_t)sizeof(stSocketAddr)) < 0)
//	{
//		close(fd);
//		return -1;
//	}
//
//	return fd;
//}
//
////
//int32_t recv_data_from( int32_t fd, char* pszBuffer, size_t size, char* pszSrcIP, size_t ip_size, uint16_t& unPort )
//{	
//	//
//	if (fd < 0 || NULL == pszBuffer || 0 == size || NULL == pszSrcIP || 0 == ip_size)
//	{
//		return -1;
//	}
//
//	struct sockaddr_in stAddr;
//	memset(&stAddr, 0x0, sizeof(stAddr));
//	socklen_t addrSize = (socklen_t)sizeof(stAddr);
//	ssize_t tmpSize = 0;
//
//	tmpSize = recvfrom(fd, pszBuffer, size, 0, (struct sockaddr *)&stAddr, &addrSize);
//	if (tmpSize < 0 && errno != EAGAIN)
//	{
//		return -1;
//	}
//
//	char* pszTmp = inet_ntoa(stAddr.sin_addr);
//	strncpy(pszSrcIP, pszTmp, ip_size);
//	pszSrcIP[ip_size-1] = '\0';
//	unPort = (uint16_t)ntohs(stAddr.sin_port);
//	
//	return (int32_t)tmpSize;
//}
//
//int32_t send_data_to( int32_t fd, char* pszBuffer, size_t size, char* pszDstIP, uint16_t unPort )
//{
//	if (fd < 0 || NULL == pszBuffer || 0 == size || NULL == pszDstIP)
//	{
//		return -1;
//	}
//
//	struct sockaddr_in stAddr;
//	memset(&stAddr, 0x0, sizeof(stAddr));
//	stAddr.sin_family = AF_INET;
//	stAddr.sin_addr.s_addr = inet_addr(pszDstIP);
//	stAddr.sin_port = htons(unPort);
//
//	ssize_t iSendedSize = 0;
//	socklen_t addrSize = (socklen_t)sizeof(stAddr);
//	iSendedSize = sendto(fd, pszBuffer, size, 0, (const struct sockaddr*)&stAddr, addrSize);
//	if (iSendedSize < 0)
//	{
//		return -1;
//	}
//
//	return (int32_t)iSendedSize;
//	
//}
//
//int32_t get_nic_addrs( struct sockaddr_in* pstNicAddrs, int32_t& count )
//{
//	if (NULL == pstNicAddrs || 0 >= count)
//	{
//		return -1;
//	}
//
//	int32_t fd = -1;
//
//	fd = socket(AF_INET, SOCK_STREAM, 0);
//	if (fd < 0)
//	{
//		return -1;
//	}
//
//	struct ifconf nicConfig;
//	struct ifreq astNics[1024];
//
//	nicConfig.ifc_buf = (char*)&astNics[0];
//	nicConfig.ifc_len = sizeof(astNics);
//
//	int32_t ret = -1;
//	ret = ioctl(fd, SIOCGIFCONF, &nicConfig);
//	if (-1 == ret)
//	{
//		close(fd);
//		return -1;
//	}
//
//	int32_t tmpCount = int32_t(nicConfig.ifc_len/sizeof(ifreq));
//	if(tmpCount < count)
//	{
//		count = tmpCount;
//	}
//
//	for(int32_t i = 0; i < count; ++i)
//	{
//		memcpy(&pstNicAddrs[i], &(astNics[i].ifr_addr), sizeof(struct sockaddr_in));
//	}
//
//	close(fd);
//	
//	return 0;
//}

#else

#include "common/common_socket.h"

int32_t set_non_block(int32_t fd)
{
	return 0;
}

int epoll_create(int __size)
{
	return -1;
}

int epoll_ctl(int __epfd, int __op, int __fd, struct epoll_event *__ev)
{
	return -1;
}


int epoll_wait(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout)
{
	return -1;
}

#endif






