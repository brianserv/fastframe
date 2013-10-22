/*
 * lightframe_impl.h
 *
 *  Created on: 2012-10-30
 *      Author: jimm
 */

#ifndef LIGHTFRAME_IMPL_H_
#define LIGHTFRAME_IMPL_H_

#include "frame_typedef.h"
#include "frame_msg_impl.h"
#include "common/common_memory.h"
#include "frame_protocolhead.h"
#include "common/common_tcpsocket.h"

FRAME_NAMESPACE_BEGIN

//Server配置接口
class IServerConfig_Impl
{
public:
	virtual ~IServerConfig_Impl() {};

	virtual const char* GetServerName()		= 0;
	//获取Server配置
	virtual int32_t GetZoneID()				= 0;

	//TODO(ServerType固化到Frame或配置文件, 这个版本不动)
	virtual EntityType GetServerType()			= 0;
	virtual ServerID GetServerID()			= 0;
	virtual LogLevel GetLogLevel()			= 0;
	virtual int32_t GetLogFileSize()		= 0;
};

//配置中心接口
class IConfigCenter
{
public:
	virtual ~IConfigCenter() {};
	virtual int32_t Initialize(const char* szFileName = NULL, const int32_t type=0) = 0;
	virtual int32_t Reload(const char* szFileName = NULL, const int32_t type=0) = 0;
	virtual int32_t Uninitialize() = 0;
};

//数据中心接口
class IDataCenter
{
public:
	virtual ~IDataCenter() {};

	virtual const char* GetName() = 0;

	//向Frame提供自己所需内存大小的查询接口
	virtual size_t GetSize() = 0;

	//初始化接口调用的时候 共享内存已分配
	virtual int32_t Initialize() = 0;
	virtual int32_t Resume() = 0;
	virtual int32_t Uninitialize() = 0;
};


//所有继承ISessionData的子类都要在声明中加上下面的宏
#define SESSION_INTERFACE() 	\
	public:	\
	int32_t GetSize(){return sizeof(*this);}

#define SESSION_DATA_BEGIN(class_name)		\
	class class_name : public ISessionData	\
	{	\
		SESSION_INTERFACE()

#define SESSION_DATA_MEMBER(type, member)	\
	type		member;

#define SESSION_DATA_END()	};

class CFrameTimer;

//定时器事件接口
class ITimerEvent
{
public:
	virtual ~ITimerEvent() {};
	//定时器事件
	virtual int32_t OnTimerEvent(CFrameTimer *pTimer) = 0;
};

typedef int32_t (ITimerEvent::*TimerProc)(CFrameTimer *pTimer);

class CSocket;

//Message事件接口
class IMessageEventCS
{
public:
	virtual ~IMessageEventCS() {};

	//消息事件
	virtual int32_t OnMessageEvent(CSocket *pConnection, MessageHeadCS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL) = 0;

};

typedef int32_t (IMessageEventCS::*CS_MSG_PROC)(CSocket *pConnection, MessageHeadCS *pMsgHead, IMsgBody *pMsgBody,
			const uint16_t nOptionLen, const void *pOptionData);


class IIOHandler
{
public:
	virtual ~IIOHandler(){};

	virtual int32_t ReadEvent() = 0;

	virtual int32_t WriteEvent() = 0;

	virtual int32_t ErrorEvent() = 0;
};

class ISystemEvent
{
public:
	virtual ~ISystemEvent() {};

	virtual int32_t OnSystemEvent(uint16_t nEventID, void *pParam) = 0;
};

typedef int32_t (*SYS_EVENT_PROC)(uint16_t nEventID, void *pParam);


class ITimerData
{
public:
	virtual ~ITimerData(){};

	virtual int32_t GetSize() = 0;
};
//所有继承ITimerData的子类都要在声明中加上下面的宏
#define TIMER_INTERFACE() 	\
	public:	\
	int32_t GetSize(){return sizeof(*this);}

#define TIMER_DATA_BEGIN(class_name)	\
	class class_name : public ITimerData	\
	{	\
		TIMER_INTERFACE()

#define TIMER_DATA_END()	};


class CSocket;

//Frame逻辑功能接口
class IFrame_Impl
{
public:
	virtual ~IFrame_Impl() {};

	virtual void RegistMsgHandler(uint32_t nMsgID, IMessageEventCS* pHandle, CS_MSG_PROC proc = NULL) = 0;

	//注册系统事件
	virtual void RegistSysEvent(uint16_t nEventID, ISystemEvent* pHandle) = 0;
	//发送消息
	virtual int32_t PostMessage(MessageHeadCS* pMessageHead, IMsgBody *pMsgBody, CSocket *pSocket) = 0;

	//发送消息
	virtual int32_t PostMessage(MessageHeadCS* pMessageHead, const uint8_t *pMsgBodyBuf, const uint32_t nMsgBodyLen, CSocket *pSocket) = 0;

	//写日志(DEBUG模式直接写文件)
	virtual int32_t WriteLog(LogLevel loglevel, const char *szFunc, const int32_t lineno,
			const char *szFormat, ...) = 0;

	//设置定时器
	virtual int32_t CreateTimer(int32_t nTimerID, ITimerEvent* pHandler, ITimerData *pTimerData,
			int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex) = 0;

	//删除定时器
	virtual int32_t RemoveTimer(const TimerIndex timerIndex) = 0;

	virtual void SetDaemon() = 0;
};

FRAME_NAMESPACE_END

#endif /* LIGHTFRAME_IMPL_H_ */
