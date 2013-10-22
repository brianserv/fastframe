/*
 * frame_configmgt.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */


#include "frame_namespace.h"
#include "frame_errordef.h"
#include "frame_configmgt.h"

FRAME_NAMESPACE_BEGIN

CFrameConfigMgt::CFrameConfigMgt()
{
	m_nUsrConfigCenterCount = 0;
	memset(m_arrUsrConfigCenter, 0, sizeof(m_arrUsrConfigCenter));
}

CFrameConfigMgt::~CFrameConfigMgt()
{
}

int32_t CFrameConfigMgt::Initialize()
{
	int32_t nRet = S_OK;
	nRet = m_stFrameBaseConfig.GetConfigObject().Initialize();
	if(nRet < 0)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Error When Initialize FrameBaseConfig {%s}{ret=0x%x}\n", m_stFrameBaseConfig.GetConfigObject().GetFrameBaseConfigFilePath(), nRet);
		return nRet;
	}

	for(int32_t i = 0; i < m_nUsrConfigCenterCount; i++)
	{
		IConfigCenter * pConfigCenter = m_arrUsrConfigCenter[i].pConfigCenter;
		const char *szFilename = NULL;
		if(strlen(m_arrUsrConfigCenter[i].szName) > 0)
		{
			szFilename = m_arrUsrConfigCenter[i].szName;
		}
		const int32_t type = m_arrUsrConfigCenter[i].nType;
		nRet = pConfigCenter->Initialize(szFilename,type);

		if(0 > nRet)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Error When Initialize UserConfigCenter {%s}{ret=0x%x}\n", szFilename, nRet);
			return nRet;
		}
	}
	return S_OK;
}

int32_t CFrameConfigMgt::Resume()
{
	int32_t nRet = S_OK;
	nRet = m_stFrameBaseConfig.GetConfigObject().Reload();
	if(nRet < 0)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Error When Initialize FrameBaseConfig {%s}{ret=0x%x}\n", m_stFrameBaseConfig.GetConfigObject().GetFrameBaseConfigFilePath(), nRet);
		return nRet;
	}

	for(int32_t i = 0; i < m_nUsrConfigCenterCount; i++)
	{
		IConfigCenter * pConfigCenter = m_arrUsrConfigCenter[i].pConfigCenter;

		const char *szFilename = NULL;
		if(strlen(m_arrUsrConfigCenter[i].szName) > 0)
		{
			szFilename = m_arrUsrConfigCenter[i].szName;
		}
		const int32_t type = m_arrUsrConfigCenter[i].nType;
		nRet = pConfigCenter->Reload(szFilename,type);
	}
	return S_OK;
}

int32_t CFrameConfigMgt::Uinitialize()
{
	int32_t nRet = S_OK;
	for(int32_t i = 0; i < m_nUsrConfigCenterCount; i++)
	{
		IConfigCenter * pConfigCenter = m_arrUsrConfigCenter[i].pConfigCenter;
		nRet = pConfigCenter->Uninitialize();
	}
	return S_OK;
}


void CFrameConfigMgt::AddUsrConfigCenter(int32_t nType, const char *szName, IConfigCenter* pConfigCenter)
{
	m_arrUsrConfigCenter[m_nUsrConfigCenterCount].nType = nType;
	strcat(m_arrUsrConfigCenter[m_nUsrConfigCenterCount].szName, szName);
	m_arrUsrConfigCenter[m_nUsrConfigCenterCount].pConfigCenter = pConfigCenter;
	m_nUsrConfigCenterCount++;
}

//获取外部注册的ConfigCenter
IConfigCenter* CFrameConfigMgt::GetUsrConfigCenter(const char *szName)
{
	for(int32_t i = 0; i < m_nUsrConfigCenterCount; i++)
	{
		IConfigCenter * pConfigCenter = m_arrUsrConfigCenter[i].pConfigCenter;
		if(strcmp(szName, m_arrUsrConfigCenter[i].szName) == 0)
		{
			return pConfigCenter;
		}
	}
	return NULL;
}


FRAME_NAMESPACE_END
