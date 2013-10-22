/*
 * frame_def.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_DEF_H_
#define FRAME_DEF_H_

#include "frame_namespace.h"
#include "frame_typedef.h"
#include "common/common_def.h"
#include <stdio.h>

FRAME_NAMESPACE_BEGIN

//内存分配大小定义
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
enum
{
	enmMaxCenterNetQueueSize		= 0x00800000/32,				//256K
	enmMaxCSChannelSize				= 0x01000000/64,				//256K
	enmMaxSCChannelSize				= 0x01000000/64,				//256K
	enmMaxLogicNetQueueSize			= 0x02000000/*/32*/,				//1M
	enmMaxCmdQueueSize				= 0x00010000,					//64K
};
#define MAX_FRAMESESSION_COUNT		0x00001000		//4096
#define MAX_FRAMETIMER_COUNT		0x00001000		//4096
#define MAX_FRAMEONLINEPLAYER_COUNT	0x00001000		//4096
//#define MAX_BINDING_SESSION_COUNT	0x00001000		//4096
#define MAX_BINDING_SESSION_COUNT	0x00000100		//256
#else
enum
{
	enmMaxCenterNetQueueSize		= 0x00800000,				//8M
	enmMaxCSChannelSize				= 0x01000000,				//16M
	enmMaxSCChannelSize				= 0x01000000,				//16M
	enmMaxLogicNetQueueSize			= 0x02000000,				//32M
	enmMaxCmdQueueSize				= 0x00010000,				//64K
};
#define MAX_FRAMESESSION_COUNT		0x00010000		//64K
#define MAX_FRAMETIMER_COUNT		0x00010000		//64K
#define MAX_FRAMEONLINEPLAYER_COUNT	0x00010000		//64K
#define MAX_BINDING_SESSION_COUNT	0x00008000		//32K
#endif

#define FRAME_DATACENTER			"FrameDataCenter"
#define LOGQUEUE_DATACENTER			"LOGQUEUEDataCenter"
#define CENTERQUEUE_DATACENTER		"CENTERQUEUEDataCenter"
#define LOGICQUEUE_DATACENTER		"LOGICQUEUEDataCenter"
#define WEBCSQUEUE_DATACENTER		"WEBCSQUEUEDataCenter"
#define WEBSCQUEUE_DATACENTER		"WEBSCQUEUEDataCenter"
#define COMMANDQUEUE_DATACENTER		"COMMANDQUEUEDataCenter"

enum
{
	enmMaxDataCenterCount			= 64,
	enmMaxConfigCenterCount			= 32,
	enmMaxSubConfigCount			= 32,
	enmMaxConfigCenterNameLength	= 256,
	enmMaxDataCenterNameLength		= 256,
	enmMaxMsgBodyInstanceSize		= 256,

//	enmMaxQueueIndex				= 0x08,
};

////队列优先级定义
//typedef  int8_t		PriorityType;
//enum
//{
//	enmHightPRI = 0,
//	enmLowPRI	= 1,
//	enmMaxPRI	= 2
//};

//定时器定义
//enum
//{
//	enmFrameTimerType_Session,
//	enmFrameTimerType_Timer,
//};

enum
{
	enmRouterType_Default			= 0x00,				//默认
	enmRouterType_Sync				= 0x01,				//同步
};

// 日志相关类型定义
enum
{
	enmMaxLogFileSize						= 0x2000000,	//最大日志文件大小
};

//日志文件状态
typedef struct tagLogFileState
{
	FILE*				pFile;								//文件描述符
	int64_t				nActiveTime;						//文件上次活跃时间
	int64_t				nLastFlushTime;						//上次刷新时间
	char				szFileName[enmMaxFullPathLength];	//文件名
} LogFileState;

#ifdef WIN32
#define snprintf _snprintf
#endif

FRAME_NAMESPACE_END

#include "frame_timer_mgt.h"
//#include "frame_msgevent_mgt.h"

#endif /* FRAME_DEF_H_ */
