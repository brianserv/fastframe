/*
 * frame_logengine.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */


#include <stdarg.h>
#include <memory.h>
#include "common/common_datetime.h"
#include "frame_errordef.h"
#include "frame_logengine.h"
#include "frame_configmgt.h"
#include "frame_logqueue.h"

FRAME_NAMESPACE_BEGIN

CFrameLogEngine::CFrameLogEngine()
{
}

CFrameLogEngine::~CFrameLogEngine()
{

}

//写通信线程日志
int32_t CFrameLogEngine::WriteNetioLog(LogLevel logLevel, const char* szFormat, ...)
{
	//判断等级是否足够
	if (logLevel < g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel())
	{
		return E_LIBF_LOGLEVEL;
	}

	int32_t ret = S_OK;

	//获取日志信息
	va_list ap;
	va_start(ap, szFormat);
	ret = WriteLog(enmThreadType_NetioThread, logLevel, enmLogSource_Netio, 0, 0, szFormat, ap);
	va_end(ap);

	return ret;
}


int32_t CFrameLogEngine::WriteMainLog(LogLevel logLevel, const char* szFormat, ...)
{
	//判断等级是否足够
	if (logLevel < g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel())
	{
		return E_LIBF_LOGLEVEL;
	}

	int32_t ret = S_OK;

	//获取日志信息
	va_list ap;
	va_start(ap, szFormat);
	ret = WriteLog(enmThreadType_MainThread, logLevel, enmLogSource_Main, 0, 0, szFormat, ap);
	va_end(ap);

	return ret;
}

//写主业务线程日志
int32_t CFrameLogEngine::WriteMainLog(LogLevel logLevel, const char *szFunc, int32_t nLineNO, const char *szMessage)
{
	//判断等级是否足够
	if (logLevel < g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel())
	{
		return E_LIBF_LOGLEVEL;
	}

	DECLARE_LOGLEVELLIST;

	char szLog[enmMaxLogInfoLength];
	szLog[0] = '\0';
	CDateTime dt = CDateTime::CurrentDateTime();

	char szDate[enmMaxDateTimeStringLength];
	szDate[0] = 0;
	sprintf(szDate, "%04d%02d%02d", dt.Year(), dt.Month(), dt.Day());

	//生成日志信息
	sprintf(szLog, "%04d-%02d-%02d %02d:%02d:%02d %s [%s:%d] %s", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second(), arrLogLevel[logLevel], szFunc, nLineNO, szMessage);

	return WriteLog(enmThreadType_MainThread, szLog, enmLogSource_Main, 0, 0, szDate);
}

//写数据库访问线程日志
int32_t CFrameLogEngine::WriteDBAccessLog(LogLevel logLevel, const char* szFormat, ...)
{
	//判断等级是否足够
	if (logLevel < g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel())
	{
		return E_LIBF_LOGLEVEL;
	}

	int32_t ret = S_OK;

	//获取日志信息
	va_list ap;
	va_start(ap, szFormat);
	ret = WriteLog(enmThreadType_DBAccessThread, logLevel, enmLogSource_DBAccess, 0, 0, szFormat, ap);
	va_end(ap);

	return ret;
}

int32_t CFrameLogEngine::WriteBaseLog(LogLevel logLevel, const char* szFormat, ...)
{
	va_list ap;
	va_start(ap, szFormat);
	if (NULL == szFormat)
	{
		return S_OK;
	}

	DECLARE_LOGLEVELLIST;

	//生成文件名
	char szFileName[enmMaxFileNameLength] = { 0 };

#ifndef WIN32
	CDateTime dt = CDateTime::CurrentDateTime();
	sprintf(szFileName, "%s_%04d%02d%02d.log", "./log/baselog", dt.Year(), dt.Month(), dt.Day());
#else
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	sprintf(szFileName, "%s_%04d%02d%02d.log", szFilePrefix, st.wYear, st.wMonth, st.wDay);
#endif

	FILE *pf = fopen(szFileName, "a+");
	if (NULL == pf)
	{
		return S_OK;
	}

#ifndef WIN32
	fprintf(pf, "%04d-%02d-%02d %02d:%02d:%02d %s ", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second(), arrLogLevel[logLevel]);
#else
	fprintf(pf, "%04d-%02d-%02d %02d:%02d:%02d %s ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, arrLogLevel[logLevel]);
#endif

	vfprintf(pf, szFormat, ap);

	fclose(pf);
	va_end(ap);

	return S_OK;
}

//读取日志
int32_t CFrameLogEngine::ReadLog(ThreadType threadType, char* szLog, uint32_t nLogSize, LogSource& logSource, int32_t& id1, int32_t& id2, char* szDate)
{
	return g_FrameLogQueue.Pop(threadType, szLog, nLogSize, logSource, id1, id2, szDate);
}

//写日志
int32_t CFrameLogEngine::WriteLog(ThreadType threadType, LogLevel logLevel, LogSource logSource, int32_t id1, int32_t id2, const char* szFormat, va_list vaList)
{
	//判断等级是否足够
	if (logLevel < g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel())
	{
		return E_LIBF_LOGLEVEL;
	}

	DECLARE_LOGLEVELLIST;

	uint32_t len = 0;
	char szLog[enmMaxLogInfoLength];
	szLog[0] = '\0';
	CDateTime dt = CDateTime::CurrentDateTime();

	char szDate[enmMaxDateTimeStringLength];
	szDate[0] = 0;
	sprintf(szDate, "%04d%02d%02d", dt.Year(), dt.Month(), dt.Day());

	//获取日志信息
	sprintf(szLog, "%04d-%02d-%02d %02d:%02d:%02d %s ", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second(), arrLogLevel[logLevel]);
	len = (uint32_t)strlen(szLog);
	vsprintf(szLog + len, szFormat, vaList);

	return WriteLog(threadType, szLog, logSource, id1, id2, szDate);
}

//写日志
int32_t CFrameLogEngine::WriteLog(ThreadType threadType, const char* szLog, LogSource logSource, int32_t id1, int32_t id2, const char* szDate)
{
	int32_t nRet = S_OK;
//#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
//	//日志直接写文件
//	CFrameBll::GetLogThread()->WriteToLogFile(logSource, id1, id2, szDate, szLog);
//#else
	//将日志添加到队列
	nRet = g_FrameLogQueue.Push(threadType, szLog, logSource, id1, id2, szDate);
//#endif
	return nRet;
}

//写日志
void CFrameLogEngine::WriteLog(const char* szFilePrefix, const LogLevel logLevel, const char* szFormat, ...)
{
#ifdef WIN32
	return;
#endif

	if ((NULL == szFilePrefix) || (NULL == szFormat))
	{
		return;
	}

	DECLARE_LOGLEVELLIST;

	//生成文件名
	char szFileName[enmMaxFileNameLength] = { 0 };

#ifndef WIN32
	CDateTime dt = CDateTime::CurrentDateTime();
	sprintf(szFileName, "%s_%04d%02d%02d.log", szFilePrefix, dt.Year(), dt.Month(), dt.Day());
#else
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	sprintf(szFileName, "%s_%04d%02d%02d.log", szFilePrefix, st.wYear, st.wMonth, st.wDay);
#endif

	FILE *pf = fopen(szFileName, "a+");
	if (NULL == pf)
	{
		return;
	}

#ifndef WIN32
	fprintf(pf, "%04d-%02d-%02d %02d:%02d:%02d %s ", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second(), arrLogLevel[logLevel]);
#else
	fprintf(pf, "%04d-%02d-%02d %02d:%02d:%02d %s ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, arrLogLevel[logLevel]);
#endif

	va_list ap;
	va_start(ap, szFormat);
	vfprintf(pf, szFormat, ap);
	va_end(ap);

	fclose(pf);
}

CFrameLog *CFrameLogEngine::GetFrameLog(const char *szPrefixName)
{
	if(m_stLogMap.count(szPrefixName) > 0)
	{
		return m_stLogMap[szPrefixName];
	}

	return NULL;
}

//写日志
int32_t CFrameLogEngine::WriteUserLog(LogLevel logLevel, const char *szName, const char *szFunc, int32_t nLineNO, const char *szMessage)
{
	//判断等级是否足够
	if (logLevel < g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel())
	{
		return E_LIBF_LOGLEVEL;
	}

	DECLARE_LOGLEVELLIST;

	char szLog[enmMaxLogInfoLength];
	szLog[0] = '\0';
	CDateTime dt = CDateTime::CurrentDateTime();

	char szDate[enmMaxDateTimeStringLength];
	szDate[0] = 0;
	sprintf(szDate, "%04d%02d%02d", dt.Year(), dt.Month(), dt.Day());

	//生成日志信息
	sprintf(szLog, "%04d-%02d-%02d %02d:%02d:%02d %s [%s:%d] %s", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second(), arrLogLevel[logLevel], szFunc, nLineNO, szMessage);

	CFrameLog *pFrameLog = GetFrameLog(szName);
	if(pFrameLog == NULL)
	{
		return E_NULLPOINTER;
	}

	return pFrameLog->Push(szLog, szName, 0, 0, szDate);
}


//读取日志
int32_t CFrameLogEngine::ReadUserLog(const char *szName, char* szLog, uint32_t nLogSize, int32_t& nID1, int32_t& nID2, char* szDate)
{
	CFrameLog *pFrameLog = GetFrameLog(szName);
	if(pFrameLog == NULL)
	{
		return E_NULLPOINTER;
	}

	char szPrefixName[enmMaxFileNameLength];
	return pFrameLog->Pop(szLog, nLogSize, szPrefixName, nID1, nID2, szDate);
}

int32_t CFrameLogEngine::GetAllUserLogObj(char szPrefixName[][enmMaxFileNameLength], int32_t &nObjCount)
{
	nObjCount = 0;

	CFrameLogMap::iterator it;
	for(it = m_stLogMap.begin(); it != m_stLogMap.end(); ++it)
	{
		strcpy(szPrefixName[nObjCount], it->first.c_str());
		++nObjCount;
	}

	return nObjCount;
}

int32_t CFrameLogEngine::RegistLog(const char *szPrefixName)
{
	if(m_stLogMap.count(szPrefixName) > 0)
	{
		return S_OK;
	}

	CFrameLog *pLog = new CFrameLog();
	if(pLog == NULL)
	{
		return -1;
	}

	int32_t nRet = pLog->Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	m_stLogMap[string(szPrefixName)] = pLog;

	return S_OK;
}

FRAME_NAMESPACE_END

