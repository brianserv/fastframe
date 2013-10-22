/*
 * frame_epoll.h
 *
 *  Created on: 2012-12-3
 *      Author: jimm
 */

#ifndef FRAME_EPOLL_H_
#define FRAME_EPOLL_H_

#include "frame_namespace.h"
#include "frame_typedef.h"
#include "lightframe_impl.h"

#include <sys/epoll.h>
#include <string.h>

FRAME_NAMESPACE_BEGIN

class CEpoll
{
public:
	CEpoll();

	virtual ~CEpoll();

	int32_t Create(int32_t nMaxConn);

//	int32_t Control(int32_t nSocketFD, void *pObject, uint32_t nOperation, uint32_t nEvents);

	int32_t RegistEvent(CSocket *pSocket, uint32_t nEvents);

	int32_t RemoveEvent(CSocket *pSocket, uint32_t nEvents);

	int32_t DeleteEvent(CSocket *pSocket);

	int32_t HandleMessage(int32_t nWaitTimeout);

	int32_t GetEpollFD();

protected:
	int32_t				m_nEpollEventTimeout;
	int32_t				m_nMaxFD;
	int32_t				m_nEpollFD;
	struct epoll_event	*m_pEpollEvent;
};

FRAME_NAMESPACE_END

#endif /* FRAME_EPOLL_H_ */
