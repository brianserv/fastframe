/*
 * frame_typedef.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_TYPEDEF_H_
#define FRAME_TYPEDEF_H_

#include "common/common_typedef.h"
#include "common/common_def.h"
#include "common/common_version.h"
#include "frame_namespace.h"

FRAME_NAMESPACE_BEGIN

//
//typedef uint8_t SessionIndexLocType;					//会话ID在消息中存放位置
//enum
//{
//	enumSessionIndexLocType_Opt			=1,			//存放在opt段
//	enumSessionIndexLocType_Body			=2			//存放在MsgBody
//};
//
//typedef uint8_t MessageEventType;					//消息事件回调方式
//enum
//{
//	enumMessageEventType_Comp,						//与现有兼容模式(buf,size)
//	enumMessageEventType_New,						//新模式(IMsgBody)
//};

typedef uint8_t ConfigType;
enum {
	enmConfigType_XML,								//配置文件从xml加载
	enmConfigType_DB								//配置文件从db加载
};

typedef char SubConfigName[32];

enum
{
	enmMaxRowCountPreKey = 1024						//单个key最多可拥有的行数量
};

/*********************************************************************
* 基本类型定义
*********************************************************************/

typedef int32_t						ErrorCode;			//错误码

typedef int32_t						UIN;				//玩家UIN

typedef int32_t						ServerID;			//服务器ID
typedef int32_t						RoleID;				//角色ID

typedef int32_t						TimerIndex;			//定时器索引号
typedef TimerIndex					TimerID;			//定时器ID

typedef uint16_t					SessionType;			//会话类型
typedef int32_t						SessionIndex;		//会话索引号

typedef int32_t						PlayerIndex;

typedef uint8_t						TimerType;			//定时器类型

typedef uint16_t					ConnIndex;			//连接索引

//网络类型
typedef uint8_t							NetType;
enum
{
	enmNetType_Tel						= 0x00,			//电信
	enmNetType_Cnc						= 0x01,			//网通
	enmNetType_Count					= 0x02,
};


/*********************************************************************
* 最大数量限制定义
*********************************************************************/

enum
{
	enmMaxOptionalDataLength			= 256,
	enmMaxZoneCount						= 16,			//最大分区数量
	enmMaxConnActiveTime				= 60,			//网络连接最大活跃时间
	enmMinReconnectTime					= 1,			//重连检查周期
};

//定义默认错误码
enum
{
	enmDefault_ErrorCode  = -1,                  
};

/*********************************************************************
* 无效ID定义
*********************************************************************/

enum
{
	enmInvalidRoleID				= 0,				//无效角色ID
	enmInvalidPlayerIndex 			= -1,
	enmInvalidTimerIndex			= -1,				//无效定时器索引
	enmInvalidTimerID				= -1,				//无效定时器ID
	enmInvalidSessionIndex			= -1,				//无效的会话索引
	enmInvalidMessageID             = 0,                //无效的消息ID
	enmInvalidMessageType           = 0xFF,                //无效的消息类型
	enmInvalidEntityType            = 0,                //无效的实体类型
	enmInvalidServerID             = -1,               //无效的服务器ID
	enmInvalidSequence             = 0,                 //无效的序列号
	enmInvalidOptionLen            = -1,                //无效的附加字段长度   
	enmInvalidLevel                = -1,                //无效的等级值
	enmInvalidExpVal               = -1,                //无效的经验值
	//enmInvalidItemID               = -1,                  //无效的物品ID
	enmInvalidPriceVal             = -1,                 //无效的价格
	enmInvalidBenefitVal           = -1,                  //无效的利益值
	enmInvalidItemCount            = -1,                   //无效的物品数量
	enmInvalidTimeVal              = -1,                   //无效的时间值
	enmInvalidVersion              = 0,                    //无效的版本号
	enmInvalid179ID                = 0,                    //无效的179账号
	enmInvalidVipLevel             = 0,                    //无效的vip等级
	enmInvalidOnlineTime           = 0,                    //无效的在线时长
	enmInvalidLeftTime             = 0,                    //无效的被踢剩余时间
	enmInvalidLeftMoney            = 0,                    //无效的剩余资产
};

//重连时间以及keep alive检测时间定义
#define INVALID_RECONNECT_TIME  (-1)
#define RECONNECT_SPAN			(3)
#define KEEP_ALIVE_SPAN			(60)

enum
{
	enmMaxConnectingTime = 2,//2s
};


typedef uint8_t						Gender;				//玩家性别
enum
{
	enmGender_Unknown				= 0xff,				//未知性别
	enmGender_Female				= 0x00,				//女性
	enmGender_Male					= 0x01,				//男性
	enmGender_Unisex				= 0x02,				//男女通用
};

typedef enum tagThreadType
{
	enmThreadType_NetioThread			= 0,
	enmThreadType_MainThread			= 1,
	enmThreadType_DBAccessThread		= 2,
} ThreadType;

typedef uint8_t						EntityType;			//实体类型

/*********************************************************************
* 定时器相关数据结构定义
*********************************************************************/

//定时器信息
typedef struct tagTimer
{
	TimerType				timerType;					//定时器类型
	int32_t					nTimerID;					//定时器标识
	void					*pData;						//定时器关联的数据
	int64_t					nStartTime;					//定时器开始时间
	int64_t					nCycleTime;					//定时器周期
	int64_t					nEndTime;					//定时器结束时间
	bool					bLoop;						//是否循环触发
	uint32_t				nFiredCount;				//已触发次数
	uint32_t                attachedDataA;               //附加数据
	uint32_t                attachedDataB;               //附加数据
} Timer;

/*********************************************************************
* 字符串长度定义
*********************************************************************/

enum
{
	enmMaxAccountIDLength			= 32,				//用户帐号最大长度
	enmMaxUrlLength					= 256,				//网络链接最大长度

	/**
	××Length是字符串UTF编码缓冲区的最大长度
	××UnicodeLength是指控件中的文字长度
	××RealLength是指限制字符串限制的字节数，
	一个英文字母1个字节，一个汉字两个字节
	*/
	enmMaxRoleNameUnicodeLength = 10, //角色名称Unicode形式的最大长度
	enmMaxRoleNameRealLength = 12,  //角色名称的字节限制

	enmMaxRoleTitleLength			= 32,				//角色名称最大长度
	enmMaxRoleTitleUnicodeLength = 10, //角色名称Unicode形式的最大长度
	enmMaxRoleTitleRealLength = 12,  //角色名称的字节限制

	enmMaxGroupNameLength			= 32,				//公会名称最大长度
	enmMaxGroupNameUnicodeLength			= 10,				//公会名称Unicode形式最大长度
	enmMaxGroupNameRealLength = 20, //公会名称字节限制

	enmMaxFamilyNameLength			= 32,				//家族名称最大长度
	enmMaxFamilyTitleLength			= 32,				//家族头衔最大长度
	enmMaxFamilyPasswordLength		= 32,				//家族二次密码最大长度
	enmMaxCustomRankLength			= 512,				//家族职位列表
	enmMaxFamilyNameUnicodeLength			= 10,				//家族名称Unicode形式最大长度
	enmMaxFamilyNameRealLength = 20, //家族名称字节限制
	enmMaxFamilyIconFontLength 		= 16,				//家族族徽文字的最大长度

	enmMaxReservedStringLength		= 32,				//保留字符串最大长度

	enmMaxStageNameLength			= 64,				//舞台名称最大长度
	enmMaxStageNameUnicodeLength			= 10,				//舞台名称Unicode形式最大长度
	enmMaxStageNameRealLength = 20, //舞台名称字节限制

	enmMaxStagePasswordLength		= 32, 				//舞台密码最大长度
	enmMaxPasswordRealLength		= 6, 				//舞台密码最大长度

	enmMaxRoomNameLength			= 128,				//房间名称最大长度
	enmMaxRoomNameUnicodeLength = 10,       //房间名称Unicode形式最大长度

	enmMaxRoleNameLength			= 32,				//角色名称最大长度
	enmMaxDescriptionLength			= 256,				//描述信息最大长度

	enmMaxDescriptionUnicodeLength			= 64,				//描述信息Unicode形式最大长度

	enmMaxFamilyNoticeLength			= 256,				//家族公告最大长度
	enmMaxFamilyNoticeUnicodeLength			= 64,				//家族公告Unicode形式最大长度

	enmMaxGoodsNameLength			= 256,				//商品名字最大长度
	enmMaxGoodsNameUnicodeLength			= 64,				//商品名字Unicode形式最大长度

	enmMaxIconNameLength			= 256,				//商品图标名字最大长度
	enmMaxIconNameUnicodeLength			= 64,				//商品图标名字Unicode形式最大长度

	enmMaxItemNameLength			= 32,				//物品名称最大长度
	enmMaxItemNameUnicodeLength			= 10,				//物品名称Unicode形式最大长度

	enmMaxMusicNameLength			= 32,				//歌曲名称最大长度
	enmMaxMusicNameUnicodeLength			= 10,				//歌曲名称Unicode形式最大长度

	enmMaxSingerNameLength			= 32,				//歌手名称最大长度
	enmMaxSingerNameUnicodeLength			= 10,				//歌手名称Unicode形式最大长度

	enmMaxLanguageLength			= 32,				//语言最大长度
	enmMaxLanguageUnicodeLength			= 10,				//语言Unicode形式最大长度

	enmMaxTerritoryLength			= 32,				//地区最大长度
	enmMaxTerritoryUnicodeLength			= 10,				//地区Unicode形式最大长度

	enmMaxCompanyLength				= 64,				//公司最大长度
	enmMaxCompanyUnicodeLength				= 20,				//公司Unicode形式最大长度

	enmMaxSpecialNameLength			= 32,				//专辑名称最大长度
	enmMaxSpecialNameUnicodeLength			= 10,				//专辑名称Unicode形式最大长度

	enmMaxSceneNameLength			= 32,				//场景名称最大长度
	enmMaxSceneNameUnicodeLength			= 10,				//场景名称Unicode形式最大长度

	enmMaxSqlStringLength			= 32768,				//SQL语句最大长度

	enmMaxMailSubjectLength		= 256,					//邮件主题的最大长度(utf8形式)
	enmMaxMailSubjectUnicodeLength = 10,            //邮件主题Unicode形式的最大长度
	enmMaxMailSubjectRealLength = 20,					//邮件主题字节限制

	enmMaxMailContentLength			= 1024,				//邮件内容的最大长度
	enmMaxMailContentUnicodeLength = 200,        //邮件内容Unicode形式的最大长度
	enmMaxMailContentRealLength = 400,        //邮件内容字节限制

	enmMaxAddFriendInteractiveMsgLength	= 256,			//在添加好友交互中附带的消息的长度
	enmMaxAddFriendInteractiveMsgUnicodeLength = 30, //在添加好友交互中附带的消息的Unicode编码长度
	enmMaxAddFriendInteractiveMsgRealLength = 60,	//在添加好友交互中附带的消息的字节限制

	enmMaxChannelNameLength			= 32,

	enmMaxTitleNameLength			= 32,				//玩家头衔最大长度
	enmMaxTitleNameUnicodeLength = 10,		//玩家头衔Unicode编码的最大长度

	enmMaxBigspkerLength			= 512,				//大喇叭的内容长度
	enmMaxBigspkerUncodeLength = 30,             //大喇叭的内容的Unicode编码长度
	enmMaxBigspkerRealLength = 60,             //大喇叭的内容的字节限制

	enmMaxSmallSpeakerLength		= 256,				//小喇叭的内容长度
	enmMaxSmallSpeakerUnicodeLength = 30, //小喇叭的内容的Unicode形式的编码长度
	enmMaxSmallSpeakerRealLength = 60, //小喇叭的内容的字节限制

	enmMaxStyleLength				= 256,		//最大喇叭样式长度
	enmMaxStyleRealLength			= 32,		//最大喇叭样式实际长度

	enmMaxChatMessageLength			= 512,				//玩家聊天时能发的最大字节数
	enmMaxChatMessageUnicodeLength = 50,		//玩家聊天信息的Unicode形式的最大长度
	enmMaxChatMessageRealLength = 100,		//玩家聊天信息的字节限制

	enmMaxSendMessageLength			= 60*3,	//发送消息长度

	enmSettingBufferLength			= 1024, //客户端私有buffer长度

	enmMaxErrorMessageLength = 512,	//返回给客户端的错误信息长度
	enmMaxErrorMessageUnicodeLength = 128,
	enmMaxErrorMessageRealLength = 256,
	enmMaxNotifyMessageLength		= 512,
	enmMaxNotifyParamLength			= 32,

	enmMaxQuestionStringLength		= 128,
	enmMaxClueStringLength			= 256,
	enmMaxOptionStringLength		= 32,
	enmMaxMusicInfoLength			= 1024,
	enmMaxPersonalSignatureLength	= 128, //个性签名最大长度(字节)
	enmMaxLogPrefixLength			= 128,
	enmMaxLogPrefixCount			= 5,
	enmMaxActionParaLength			= 256, //ue
	enmMaxActionResultDesc			= 128,
	enmMaxEchoStringLength			= 1024,
	enmMaxShowMessageLength			= 256,
	enmMaxItemPeriodCount			= 256,//最大的计数发放，计时使用道具数量

	enmMaxDescStringLength			= 256,		//最大描述信息长度
	enmMaxUpdateAssetDescLength		= 128,	//更新资产附加描述最大长度

	enmMaxRelativesNameLength		=32,
	enmMaxRelativesGroupNameLength	=32,
	enmMaxSpeakerPostfixLength		=32,

	enmMaxNoticeLength				= 256,	//通知字符串长度
	enmPrizeItemNum					= 16,	//礼包个数
	enmNewPrizeLevelNum				= 20,	//新手礼包级别
	enmAllPrizeItemNum				= enmPrizeItemNum*enmNewPrizeLevelNum,	//每个人礼包总个数
	enmMaxItemClassCount            = 128,   //玩家最多拥有的物品的种类数
	enmMaxPunishReasonLength        = 512,   //最大惩罚原因长度
};

/*********************************************************************
* 服务器相关数据结构定义
*********************************************************************/


#define MaxOptionDataSize			(1 * 1024)

FRAME_NAMESPACE_END

#endif /* FRAME_TYPEDEF_H_ */
