/*
 * common_message.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_MESSAGE_H_
#define COMMON_MESSAGE_H_


#include "common_typedef.h"


enum
{
	enmMaxMessageSize					= 0x40000,	//最大消息长度, 1024*256, 256k
};

typedef struct tagMessageContent
{
	uint32_t				m_nMessageSize;			//消息长度
	uint8_t					m_arrMessageBuffer[enmMaxMessageSize];	//消息缓冲区
} MessageContent;

#endif /* COMMON_MESSAGE_H_ */
