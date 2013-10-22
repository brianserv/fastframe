/*
 * frame_logengine.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_LOGENGINE_H_
#define FRAME_LOGENGINE_H_

#include "common/common_string.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "common/common_singleton.h"
#include "frame_log.h"
#include <map>
#include <string>

using namespace std;

FRAME_NAMESPACE_BEGIN

class CFrameLog;
typedef map<string, CFrameLog *> CFrameLogMap;

class CFrameLogEngine
{
public:
	CFrameLogEngine();
	~CFrameLogEngine();

public:
	//写通信线程日志
	int32_t WriteNetioLog(LogLevel logLevel, const char* szFormat, ...);

	//写主业务线程日志
	int32_t WriteMainLog(LogLevel logLevel, const char* szFormat, ...);
	int32_t WriteMainLog(LogLevel logLevel, const char *szFunc, int32_t nLineNO, const char *szMessage);

	//写数据库访问线程日志
	int32_t WriteDBAccessLog(LogLevel logLevel, const char* szFormat, ...);

	int32_t WriteBaseLog(LogLevel logLevel, const char* szFormat, ...);

	//读取日志
	int32_t ReadLog(ThreadType threadType, char* szLog, uint32_t nLogSize, LogSource& logSource, int32_t& id1, int32_t& id2, char* szDate);

	//写日志
	int32_t WriteLog(ThreadType threadType, LogLevel logLevel, LogSource logSource, int32_t id1, int32_t id2, const char* szFormat, va_list vaList);

	//写日志
	int32_t WriteLog(ThreadType threadType, const char* szLog, LogSource logSource, int32_t id1, int32_t id2, const char* szDate);

	//写日志
	static void WriteLog(const char* szFilePrefix, const LogLevel logLevel, const char* szFormat, ...);

	CFrameLog *GetFrameLog(const char *szPrefixName);

	//写日志
	int32_t WriteUserLog(LogLevel logLevel, const char *szName, const char *szFunc, int32_t nLineNO, const char *szMessage);

	//读取日志
	int32_t ReadUserLog(const char *szName, char* szLog, uint32_t nLogSize, int32_t& nID1, int32_t& nID2, char* szDate);

	int32_t GetAllUserLogObj(char szPrefixName[][enmMaxFileNameLength], int32_t &nObjCount);

	int32_t RegistLog(const char *szPrefixName);

protected:
	CFrameLogMap		m_stLogMap;
};


#define	CREATE_FRAMELOGENGINE_INSTANCE	CSingleton<CFrameLogEngine>::CreateInstance
#define	g_FrameLogEngine				CSingleton<CFrameLogEngine>::GetInstance()
#define	DESTROY_FRAMELOGENGINE_INSTANCE	CSingleton<CFrameLogEngine>::DestroyInstance

#define WRITE_NETIO_LOG					g_FrameLogEngine.WriteNetioLog
#define WRITE_MAIN_LOG					g_FrameLogEngine.WriteMainLog
#define WRITE_DBACCESS_LOG				g_FrameLogEngine.WriteDBAccessLog
//#define READ_LOG						g_FrameLogEngine.ReadLog

#define WRITE_USER_LOG					g_FrameLogEngine.WriteUserLog
#define READ_USER_LOG					g_FrameLogEngine.ReadUserLog


FRAME_NAMESPACE_END

#endif /* FRAME_LOGENGINE_H_ */
