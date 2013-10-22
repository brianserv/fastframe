/*
 * common_socket.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_SOCKET_H_
#define COMMON_SOCKET_H_

#include <stdio.h>


#ifndef WIN32

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

typedef int SOCKET;
#define INVALID_SOCKET -1
#define closesocket close

//block flag
enum enmSocketBlockFlag
{
	enmSocketBlockFlag_lock = 0,
	enmSocketBlockFlag_nonblock = 1,
};

#ifdef __cplusplus
extern "C"
{
#endif

int32_t socket_address_to_string(struct sockaddr_in& stAddress, char* pszIPPort, size_t size);

int32_t set_non_block(int32_t fd);

//设置socket的接收&发送缓冲区大小.特别注意的是设置socket缓冲区大小的时机
//对于client来说:必须在调用connect函数前进行对接收缓冲区的设置
//对于server来说:必须在调用listen函数前进行对接收缓冲区的设置
int32_t set_socket_buffer_size(int32_t fd, size_t size);

int32_t create_udp_socket(uint16_t port, enmSocketBlockFlag flag = enmSocketBlockFlag_lock, char* pszIP = NULL);

//简单的接收&发送
//ip_size : pszSrcIP的长度
int32_t recv_data_from(int32_t fd, char* pszBuffer, size_t size, char* pszSrcIP, size_t ip_size, uint16_t& unPort);
int32_t send_data_to(int32_t fd, char* pszBuffer, size_t size, char* pszDstIP, uint16_t unPort);

int32_t get_nic_addrs(struct sockaddr_in* pstNicAddrs, int32_t& count);

int32_t ip_string_to_addr(const char* string_ip, int32_t& ip);
int32_t ip_addr_to_string(int32_t ip, char* string_ip, int16_t buflen);

#ifdef __cplusplus
}
#endif



#else

/**
define epoll data on windows platform
*/
#include "common/common_typedef.h"

typedef int socklen_t;

int32_t set_non_block(int32_t fd);

enum EPOLL_EVENTS
{
	EPOLLIN = 0x001,
 #define EPOLLIN EPOLLIN
	EPOLLPRI = 0x002,
 #define EPOLLPRI EPOLLPRI
	EPOLLOUT = 0x004,
 #define EPOLLOUT EPOLLOUT
	EPOLLRDNORM = 0x040,
 #define EPOLLRDNORM EPOLLRDNORM
	EPOLLRDBAND = 0x080,
 #define EPOLLRDBAND EPOLLRDBAND
	EPOLLWRNORM = 0x100,
 #define EPOLLWRNORM EPOLLWRNORM
	EPOLLWRBAND = 0x200,
 #define EPOLLWRBAND EPOLLWRBAND
	EPOLLMSG = 0x400,
 #define EPOLLMSG EPOLLMSG
	EPOLLERR = 0x008,
 #define EPOLLERR EPOLLERR
	EPOLLHUP = 0x010,
 #define EPOLLHUP EPOLLHUP
	EPOLLONESHOT = (1 << 30),
 #define EPOLLONESHOT EPOLLONESHOT
	EPOLLET = (1 << 31)
 #define EPOLLET EPOLLET
};

typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct epoll_event
{
	uint32_t events;      /* Epoll events */
	epoll_data_t data;    /* User data variable */
};

#ifndef EPOLL_CTL_ADD
#define EPOLL_CTL_ADD 1 /* Add a file decriptor to the interface.  */
#endif

#ifndef EPOLL_CTL_DEL
#define EPOLL_CTL_DEL 2 /* Remove a file decriptor from the interface.  */
#endif

#ifndef EPOLL_CTL_MOD
#define EPOLL_CTL_MOD 3 /* Change file decriptor epoll_event structure.  */
#endif



int epoll_create (int __size);

int epoll_ctl (int __epfd, int __op, int __fd, struct epoll_event * __ev);

int epoll_wait (int __epfd, struct epoll_event * __events, int __maxevents, int __timeout);

#endif

#endif /* COMMON_SOCKET_H_ */
