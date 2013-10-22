/*
 * frame_configmgt.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_CONFIGMGT_H_
#define FRAME_CONFIGMGT_H_


#include "common/common_object.h"
#include "frame_logengine.h"
#include "lightframe_impl.h"
#include "frame_def.h"
#include "frame_baseconfig.h"
#include "frame_namespace.h"
#include "frame_config_container.h"

FRAME_NAMESPACE_BEGIN

typedef struct tagUsrConfigCenter
{
	int32_t			nType;
	char			szName[enmMaxConfigCenterNameLength];
	IConfigCenter*	pConfigCenter;
}UsrConfigCenter;

class CFrameConfigMgt : public CObject
{
public:
	CFrameConfigMgt();
	virtual ~CFrameConfigMgt();

	int32_t Initialize();
	int32_t Resume();
	int32_t Uinitialize();

	//添加外部ConfigCenter
	void AddUsrConfigCenter(int32_t nType, const char *szName, IConfigCenter* pConfigCenter);

	//获取外部注册的ConfigCenter
	IConfigCenter* GetUsrConfigCenter(const char *szName);

	CFrameBaseConfig &GetFrameBaseConfig()
	{
		return m_stFrameBaseConfig.GetConfigObject();
	}


	void MakeFrameBaseConfigFilePath(const char *szServerName)
	{
		m_stFrameBaseConfig.GetConfigObject().SetFrameBaseConfigFilePath(szServerName);
	}

protected:
	CFrameConfigContainer<CFrameBaseConfig>		m_stFrameBaseConfig;

	int32_t					m_nUsrConfigCenterCount;
	UsrConfigCenter			m_arrUsrConfigCenter[enmMaxConfigCenterCount];
};

#define	CREATE_FRAMECONFIGMGT_INSTANCE		CSingleton<CFrameConfigMgt>::CreateInstance
#define	g_FrameConfigMgt					CSingleton<CFrameConfigMgt>::GetInstance()
#define	DESTROY_FRAMECONFIGMGT_INSTANCE		CSingleton<CFrameConfigMgt>::DestroyInstance

FRAME_NAMESPACE_END
#endif /* FRAME_CONFIGMGT_H_ */
