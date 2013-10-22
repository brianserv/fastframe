/*
 * frame_baseconfig.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#include "frame_errordef.h"
#include "common/common_api.h"
#include "frame_baseconfig.h"

FRAME_NAMESPACE_BEGIN


#define TAGNAME_FILESIZE			"filesize"
#define TAGNAME_COMMON				"common"
#define TAGNAME_CONFIG				"config"

#define ATTRIBUTE_LOGLEVEL			"loglevel"
#define ATTRIBUTE_LOGFILESIZE		"log"
#define ATTRIBUTE_ZONEID			"zoneid"
#define ATTRIBUTE_TYPE				"type"
#define ATTRIBUTE_SERVERTYPE		"servertype"
#define ATTRIBUTE_SERVERID			"serverid"
#define ATTRIBUTE_NAME				"name"
#define ATTRIBUTE_NETTYPE			"nettype"
#define ATTRIBUTE_APPTHREADCOUNT	"appthreadcount"

#define ATTRIBUTE_NET				"net"
#define ATTRIBUTE_LISTEN			"listen"
#define ATTRIBUTE_ADDRESS			"address"
#define ATTRIBUTE_PORT				"port"
#define ATTRIBUTE_MAXCONNECTNUM		"max_connect_num"
#define ATTRIBUTE_DESC				"desc"

#define ATTRIBUTE_PEERS				"peers"
#define ATTRIBUTE_PEER				"peer"
#define ATTRIBUTE_PEERADDRESS		"peeraddress"
#define ATTRIBUTE_PEERPORT			"peerport"
#define ATTRIBUTE_SERVERTYPE		"servertype"
#define ATTRIBUTE_SERVERID			"serverid"

CFrameBaseConfig::CFrameBaseConfig()
{
	m_logLevel = 0;
	m_nEntityType = 0;
	m_nServerID = enmInvalidServerID;
	m_nNetType = 0;
	m_nAppThreadCount = 0;
}

CFrameBaseConfig::~CFrameBaseConfig()
{
}

int32_t CFrameBaseConfig::Initialize(const char* szFileName/* = NULL*/, const int32_t type/*=0*/)
{
	if(strlen(m_szConfigFile) <= 0)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Load %s file failed!\n", m_szConfigFile);
		return E_LIBF_LOADCONFIGFILE;
	}

	TiXmlDocument doc(m_szConfigFile);
	if (!doc.LoadFile(TIXML_ENCODING_UTF8))
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Load %s file failed!\n", m_szConfigFile);
		return E_LIBF_LOADCONFIGFILE;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Load %s file success!\n", m_szConfigFile);

	//获取根节点
	TiXmlElement *pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Root node cannot be found in %s file!\n", m_szConfigFile);
		return E_LIBF_LOADCONFIGFILE;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Get root node success!\n");

	int32_t ret = LoadCommon(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadLogFileSize(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	return S_OK;
}

int32_t CFrameBaseConfig::Reload(const char* szFileName/* = NULL*/, const int32_t type/*=0*/)
{
	return Initialize();
}

int32_t CFrameBaseConfig::Uninitialize()
{
	return S_OK;
}

int32_t CFrameBaseConfig::LoadCommon(TiXmlElement *pRoot)
{
	//获取common节点
	TiXmlElement *pCommon = pRoot->FirstChildElement(TAGNAME_COMMON);
	if (NULL == pCommon)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Common node cannot be found in %s file!\n", m_szConfigFile);
		return E_LIBF_CONFIGNOKEY;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Get common node success!\n");

	const char* pszValue = NULL;

	//获取日志等级
	pszValue = pCommon->Attribute(ATTRIBUTE_LOGLEVEL);
	if (NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "Log level cannot be found, default value will be used!\n");
		m_logLevel = enmLogLevel_Notice;
	}
	else if (strcmp(pszValue, "debug") == 0)
	{
		m_logLevel = enmLogLevel_Debug;
	}
	else if (strcmp(pszValue, "info") == 0)
	{
		m_logLevel = enmLogLevel_Info;
	}
	else if (strcmp(pszValue, "notice") == 0)
	{
		m_logLevel = enmLogLevel_Notice;
	}
	else if (strcmp(pszValue, "warning") == 0)
	{
		m_logLevel = enmLogLevel_Warning;
	}
	else if (strcmp(pszValue, "error") == 0)
	{
		m_logLevel = enmLogLevel_Warning;
	}
	else if (strcmp(pszValue, "critical") == 0)
	{
		m_logLevel = enmLogLevel_Critical;
	}

	pszValue = pCommon->Attribute(ATTRIBUTE_ZONEID, &m_nZoneID);
	if (NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Get zone id failed!\n");
		m_nZoneID = 0;
	}
	int32_t type = 0;
	pszValue = pCommon->Attribute(ATTRIBUTE_SERVERTYPE, &type);
	if (NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Get server type failed!\n");
		return E_LIBF_CONFIGNOKEY;
	}
	m_nEntityType = (EntityType)type;

	pszValue = pCommon->Attribute(ATTRIBUTE_SERVERID,&m_nServerID);
	if (NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Get server id failed!\n");
		return E_LIBF_CONFIGNOKEY;
	}

	int32_t nNetType = 0;
	pszValue = pCommon->Attribute(ATTRIBUTE_NETTYPE, &nNetType);
	if (NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "It's not found nettype node, you sure continue?\n");
	}
	else
	{
		m_nNetType = nNetType;
	}

	int32_t nAppThreadCount = 0;
	pszValue = pCommon->Attribute(ATTRIBUTE_APPTHREADCOUNT, &nAppThreadCount);
	if(NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "It's not found appthreadcount node, we'll use default value{%d}\n", nAppThreadCount);
		m_nAppThreadCount = 1;
	}
	else
	{
		m_nAppThreadCount = nAppThreadCount;
	}

	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Get common info success!(m_logLevel=%d, m_nZoneID=%d, m_nEntityType=%d,m_nServerID=%d)\n",
		m_logLevel, m_nZoneID, m_nEntityType,m_nServerID);
	return S_OK;
}

int32_t	CFrameBaseConfig::LoadLogFileSize(TiXmlElement * pRoot)
{
	//获取日志文件节点
	TiXmlElement *pLog = pRoot->FirstChildElement(TAGNAME_FILESIZE);
	if (NULL == pLog)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "File-size node can not be found in %s file!\n", m_szConfigFile);
		return E_LIBF_CONFIGNOKEY;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Get files-ize node success!\n");

	const char* pszValue = NULL;
	pszValue = pLog->Attribute(ATTRIBUTE_LOGFILESIZE, &m_nLogFileSize);
	if (NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Get log file size failed!\n");
		return E_LIBF_CONFIGNOKEY;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Get log file size success!(m_nLogFileSize=%d)\n", m_nLogFileSize);
	return S_OK;
}

FRAME_NAMESPACE_END
