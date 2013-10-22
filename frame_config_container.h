/*
 * frame_config_container.h
 *
 *  Created on: 2011-11-26
 *      Author: jimm
 */

#ifndef FRAME_CONFIGCONTAINER_H_
#define FRAME_CONFIGCONTAINER_H_

#include "common/common_singleton.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "common/common_errordef.h"
#include "lightframe_impl.h"


FRAME_NAMESPACE_BEGIN


template<typename T>
class CFrameConfigContainer : public IConfigCenter
{
public:
	CFrameConfigContainer()
	{
		m_nLoadIndex = 0;
		m_nUseIndex = 1;
	}
	virtual ~CFrameConfigContainer()
	{

	}

public:
	//初始化配置管理对象
	virtual int32_t Initialize(const char* szFileName = NULL, const int32_t type=enmConfigType_XML)
	{
		int32_t ret = S_OK;
		if (NULL == szFileName)
		{
			ret = m_arrConfig[m_nLoadIndex].Initialize();
		}
		else
		{
			ret = m_arrConfig[m_nLoadIndex].Initialize(szFileName, type);
		}
		if (0 > ret)
		{
			return ret;
		}

		int32_t nIndex = m_nUseIndex;
		m_nUseIndex = m_nLoadIndex;
		m_nLoadIndex = nIndex;

		return ret;
	}
	//销毁配置管理对象
	virtual int32_t Uninitialize()
	{
		return S_OK;
	}

	virtual int32_t Reload(const char* szFileName = NULL, const int32_t type=0)
	{
		return Initialize(szFileName, type);
	}

	virtual int32_t Resume()
	{
		return S_OK;
	}

	//获取配置管理对象
	T& GetConfigObject()
	{
		return m_arrConfig[m_nUseIndex];
	}

//protected:
//	T& GetLoadConfig()
//	{
//		return m_arrConfig[m_nLoadIndex];
//	}
//	T& GetUseConfig()
//	{
//		return m_arrConfig[m_nUseIndex];
//	}


protected:
	T						m_arrConfig[2];

	int32_t					m_nLoadIndex;
	int32_t					m_nUseIndex;
};

/************************************************************************************************

example:

class CXXXConfig
{
public:
	CXXXConfig(){}
	virtual ~CXXXConfig(){}

public:
	//初始化配置管理对象
	int32_t Initialize(const char* szLogPrefix, const char* szFileName = "./config/xxx.xml")
	{
		//TODO:
		return S_OK;
	}
	//销毁配置管理对象
	int32_t Uninitialize()
	{
		//TODO:
		return S_OK;
	}

public:
	//获取配置信息
	XXXConfigInfo& GetXXXConifgInfo();

protected:
	char	m_szLogPrefix[enmMaxFileNameLength];

};

typedef CFrameConfigContainer<CXXXConfig>	XXXConfigContainer;

#define INITIALIZE_XXXCONFIG	CSingleton<XXXConfigContainer>::GetInstance().Initialize
#define GET_XXXCONFIG			CSingleton<XXXConfigContainer>::GetInstance().GetConfigObject

************************************************************************************************/


FRAME_NAMESPACE_END

#endif /* FRAME_CONFIGCONTAINER_H_ */
