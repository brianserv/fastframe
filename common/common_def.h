/*
 * common_def.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_DEF_H_
#define COMMON_DEF_H_

#include "common_typedef.h"


/*********************************************************************
* 时间日期相关常量定义
*********************************************************************/
#define US_PER_MS				((int64_t)1000)
#define US_PER_SECOND			((int64_t)(1000 * US_PER_MS))
#define US_PER_MINUTE			((int64_t)(60 * US_PER_SECOND))
#define US_PER_HOUR				((int64_t)(60 * US_PER_MINUTE))
#define US_PER_DAY				((int64_t)(24 * US_PER_HOUR))

#define MS_PER_SECOND			1000
#define SECOND_PER_MINUTE		60
#define MINUTE_PER_HOUR			60
#define HOUR_PER_DAY			24
#define DAY_PER_YEAR			365

#define SECOND_PER_HOUR			(MINUTE_PER_HOUR * SECOND_PER_MINUTE)
#define SECOND_PER_DAY			(HOUR_PER_DAY * SECOND_PER_HOUR)
#define SECOND_PER_WEEK			(SECOND_PER_DAY * 7)
#define SECOND_PER_YEAR			(DAY_PER_YEAR * SECOND_PER_DAY)


/*********************************************************************
* 字符串长度相关定义
*********************************************************************/
enum
{
	enmMaxDateTimeStringLength			= 32,	//日期时间字符串最大长度
	enmMaxFileNameLength				= 256,	//文件名最大长度,
	enmMaxConfigCount                   = 32,   //最多一次重新加载的配置文件的个数
	enmMaxConfigNameLength              = 64,   //配置文件名称的最大长度
	enmMaxFullPathLength				= 256,	//文件全路径最大长度,
	enmMaxIPAddressLength				= 20,	//IP地址最大长度
	enmMaxFieldValueLength				= 1024*10,//数据库字段值最大长度
	enmMaxLogInfoLength					= 0x40000,	//单条日志记录最大长度(256K)
	enmMaxDBUserLength					= 256,	//数据库用户名最大长度
	enmMaxDBPasswordLength				= 32,	//数据库密码最大长度
	enmMaxDBTableLength					= 32,	//数据库表名称最大长度
	enmMaxDateLength					= 9,	//日期字符串的最大长度
	enmMaxStringLength					= 4096,	//普通字符串最大长度
	enmMaxOptionLen                     = 64,   //最大附加字段长度
	enmDefaultStringLength				= 256,	//普通字符串默认长度
	enmIDCardLength						= 19,	//用户身份证号长度
	enmIDCardLength2					= 16,	//用户身份证号长度
	enmMaxLogFileNumber					= 1000,	//日志文件最大序号数量
	enmMaxIPStringLength				= 20,	//IP字符串的最大长度
	enmMaxIPCount						= 10,	//最大的IP个数
	enmMaxPortStringLength				= 64,	//IP字符串的最大长度
	enmMaxPortCount						= 10,
	enmMaxDBCacheCount					= 2,	//dbcache最大数目
	enmMaxDBNameLength					= 64,
	enmMaxDataBaseCount					= 32,	//dmz中所有server用到的数据库数量总和
	enmMaxRoleCountPerQuery				= 100, //一次查询的玩家最大数目
	enmTableCountPerDB					= 4,  //每个DB的分表数目
	enmDefaultDBFactor					= 256,
	enmDefaultDBPort					= 3306,
	enmMaxMsgIDNumber                   = 256,  //GM透传协议的最大条数
	enmMaxNoticeStringLength            = 512,  //最大通知字符串长度
	enmMaxRoomThemeLength               = 128,  //最大房间主题字符串长度
	enmMaxRoomPasswordLength            = 32,   
	enmMaxFieldCount                    = 32,   //一张表最多有多少个字段
	enmMaxAppThreadCount				= 8,	//最大的业务线程数
};


/*********************************************************************
* 线程执行周期定义
*********************************************************************/
enum
{
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
	enmThreadExecutePeriod			= 10000,
#else
	enmThreadExecutePeriod			= 10000,
#endif
};


/*********************************************************************
* 网络连接参数定义
*********************************************************************/
typedef int EConnectParamCatagory;
enum
{
	enmConnectParamCatagory_DianXin = 1, //电信
	enmConnectParamCatagory_WangTong = 2, //网通
};
enum{
	enmMaxConnectParamCatagoryCount = 2
};
typedef struct tagConnectParam
{
	char				szRemoteIP[enmMaxIPStringLength];
	char				szRemotePorts[enmMaxPortStringLength];
	bool				bSlave;
	EConnectParamCatagory				iCatagory;
	int				iPingMSec;	//负值表示本IP不可达
} ConnectParam;


/*********************************************************************
* 获取上一个月（相对于传进来的月份）
*********************************************************************/
#define GET_PREV_MONTH(month)	(((month) % 100) == 1) ? ((month) - 89) : ((month) - 1)



/*********************************************************************
* 日志相关类型定义
*********************************************************************/

typedef uint8_t						LogLevel;			//日志级别
enum
{
	enmLogLevel_Debug				= 0x00,				//调试信息
	enmLogLevel_Info				= 0x01,				//一般日志信息
	enmLogLevel_Notice				= 0x02,				//通知
	enmLogLevel_Warning				= 0x03,				//警告
	enmLogLevel_Error				= 0x04,				//错误
	enmLogLevel_Critical			= 0x05,				//严重错误
};

#define DECLARE_LOGLEVELLIST	\
	const char * arrLogLevel[] = {\
	"[Debug]   ",\
	"[Info]    ",\
	"[Notice]  ",\
	"[Warning] ",\
	"[Error]   ",\
	"[Critical]"}

//内存分配大小定义
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
enum
{
	enmMaxCSQueueSize				= 0x02000000/16,			//32M
	enmMaxCommandQueueSize			= 0x02000000/16,			//32M
	enmMaxReturnQueueSize			= 0x02000000/16,			//32M
	enmMaxLogQueueSize				= 0x02000000/16,			//32M

	enmMaxPlayerCount				= 0x00010000/16,			//64K
	enmMaxTimerCount				= 0x00010000/16,			//64K
	enmMaxSessionCount				= 0x00010000/16,			//64K
};
#else
enum
{
	enmMaxCSQueueSize				= 0x02000000,			//16M
	enmMaxCommandQueueSize			= 0x02000000,			//16M
	enmMaxReturnQueueSize			= 0x02000000,			//16M
	enmMaxLogQueueSize				= 0x02000000,			//16M

	enmMaxPlayerCount				= 0x00010000,			//32K
	enmMaxTimerCount				= 0x00010000,			//32K
	enmMaxSessionCount				= 0x00010000,			//32K
};
#endif


enum
{
	enmDBServiceGroupCount			= 1,					//数据库业务组数量
	enmDBIOThreadCountPerGroup		= 4,					//每组数据库读写线程数量
};


//桶大小定义
enum
{
	enmPlayerBucketSize				= (enmMaxPlayerCount * 256),		//所有玩家列表桶大小
	enmTimerBucketSize				= (enmMaxTimerCount * 256),			//所有定时器列表桶大小
	enmSessionBucketSize				= (enmMaxSessionCount * 256),		//所有会话列表桶大小
	enmMaxCommandSize					= 1024 * 256,		//256k
};

//会话类型定义
enum
{
	enmSessionType_None				= 0x0000,		//无效类型
	enmSessionType_CreateRole		= 0x0001,		//创建角色会话
	enmSessionType_DeleteRole		= 0x0002,		//删除角色会话
	enmSessionType_UpdateBaseInfo	= 0x0003,		//更新角色基本信息会话
	enmSessionType_UpdateAsset		= 0x0004,		//更新角色资产会话
	enmSessionType_UpdateAvatar		= 0x0005,		//更新角色形象会话
	enmSessionType_UpdateIDCard		= 0x0006,		//更新角色身份证信息会话
	enmSessionType_UpdateLoginInfo	= 0x0007,		//更新角色防沉迷信息会话
	enmSessionType_UpdatePlayerDBState	= 0x0008,	//更新角色状态会话
	enmSessionType_UpdatePunish		= 0x0009,		//更新角色处罚信息会话
	enmSessionType_UpdateLover		= 0x000A,		//更新角色情侣信息会话
	enmSessionType_UpdateAchievement	= 0x000B,		//更新角色成就数据会话
	enmSessionType_UpdateGroup		= 0x000C,		//更新角色公会信息会话
	enmSessionType_GetRoleInfo		= 0x0011,		//获取角色数据会话
	enmSessionType_GetRoleGeneral	= 0x0012,		//获取角色概要信息会话
	enmSessionType_GetRolePublicInfo	= 0x0013,		//获取角色概要信息会话
	enmSessionType_UpdateAvatarItem	= 0x0014,		//更新avatar物品信息
	enmSessionType_UpdateRoleRank	= 0x0015,		//更新角色公会等级
};


/*********************************************************************
* 日志相关类型定义
*********************************************************************/

typedef uint8_t						LogSource;		//日志
enum
{
	enmLogSource_Netio		= 0x00,			//通信线程
	enmLogSource_Main				= 0x01,			//主业务线程
	enmLogSource_DBAccess			= 0x02,			//数据库访问线程
};


#endif /* COMMON_DEF_H_ */
