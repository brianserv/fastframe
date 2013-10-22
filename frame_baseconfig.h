/*
 * frame_baseconfig.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_BASECONFIG_H_
#define FRAME_BASECONFIG_H_

#include <memory.h>
#include "tinyxml/tinyxml.h"
#include "common/common_singleton.h"
#include "common/common_tcpsocket.h"
#include "common/common_def.h"
#include "frame_namespace.h"
#include "frame_errordef.h"
#include "frame_logengine.h"
#include "frame_def.h"
#include "lightframe_impl.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define  enmMaxDBCount 10

FRAME_NAMESPACE_BEGIN

#ifndef MAX_PEERNODE_COUNT
#define MAX_PEERNODE_COUNT 32
#endif

#ifndef MAX_NETCONFIG_COUNT
#define MAX_NETCONFIG_COUNT 32
#endif


class CFrameBaseConfig : public IServerConfig_Impl , public IConfigCenter
{
public:
	CFrameBaseConfig();
	virtual ~CFrameBaseConfig();

	void SetFrameBaseConfigFilePath(const char* szServerName)
	{
		strcpy(m_szServerName, szServerName);

		memset(m_szConfigFile, 0, sizeof(m_szConfigFile));
		sprintf(m_szConfigFile, "./config/%s.xml", szServerName);
	}

	char *GetFrameBaseConfigFilePath()
	{
		return m_szConfigFile;
	}

	virtual int32_t Initialize(const char* szFileName = NULL, const int32_t type=0);
	virtual int32_t Reload(const char* szFileName = NULL, const int32_t type=0);
	virtual int32_t Uninitialize();

	virtual int32_t LoadCommon(TiXmlElement *pRoot);
	virtual int32_t LoadLogFileSize(TiXmlElement * pRoot);
public:
	const char* GetServerName()
	{
		return m_szServerName;
	}

	//获取Server配置
	int32_t GetZoneID()
	{
		return m_nZoneID;
	}

	//Server类型
	EntityType GetServerType()
	{
		return m_nEntityType;
	}

	//ServerID
	ServerID GetServerID()
	{
		return m_nServerID;
	}

	NetType GetNetType()
	{
		return m_nNetType;
	}

	//日志等级
	LogLevel GetLogLevel()
	{
		return m_logLevel;
	}

	//日志文件大小限制
	int32_t GetLogFileSize()
	{
		return m_nLogFileSize;
	}

	int32_t GetAppThreadCount()
	{
		return m_nAppThreadCount;
	}
protected:
	char					m_szConfigFile[enmMaxFileNameLength];
	char					m_szServerName[enmMaxFileNameLength];
	LogLevel				m_logLevel;
	int32_t					m_nZoneID;
	EntityType				m_nEntityType;
	ServerID				m_nServerID;
	NetType					m_nNetType;
	int32_t					m_nLogFileSize;
	int32_t					m_nAppThreadCount;
};

FRAME_NAMESPACE_END


#endif /* FRAME_BASECONFIG_H_ */
