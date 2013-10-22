/**
@file   xmlconf.h
@brief  读取xml配置文件类头文件

@author sjij@mail.51.com
@date Begin 2008.3
@date Dnd 2008.3
*/

#ifndef CXMLCONFIG_H_
#define CXMLCONFIG_H_
//#define TIXML_USE_STL

#include <string>
#include <iostream>
#include "tinystr.h"
#include "tinyxml.h"


 
namespace Game51 { namespace Server {

const u_int32_t NUM_INDENTS_PER_SPACE = 2;

class CXmlConfig
{
public:
	CXmlConfig();
	explicit CXmlConfig(const char* xmlFileName) 
		:m_XmlConfigFile(xmlFileName),m_RootElem(0) 
	{
		//加载配置文件
		m_XmlConfigFile.LoadFile();  
		//得到配置文件的根结点
		m_RootElem = m_XmlConfigFile.RootElement();
	}

	int Init(const char* xmlFileName);

public:
	/***
	**	@brief：得到字符串配置内容
	**  @param[in]  const std::string& blockName 区块名
	**	@param[in]  const std::string& nodeName  节点名
	**	@param[in] char* value  返回的结果
	**/
	void GetValue(const std::string& blockName, const std::string& nodeName, char *value);

	/***
	**	@brief：得到整数参数
	**  @param[in]  const std::string& blockName 区块名
	**	@param[in]  const std::string& nodeName 节点名
	**	@param[out] int& value  返回的结果
	**  @param[in]  int  defaut 默认参数
	**/	
	void GetValue(const std::string& blockName, const std::string& nodeName, int32_t& value, int defaut = -1);
    
	/***
	**	@brief：得到字符串配置内容
	**	@param[in]  const std::string& nodeName 节点名
	**	@param[in] char* value  返回的结果
	**/
	void GetValue(const std::string& nodeName, char *value);

	/***
	**	@brief：得到整数参数
	**	@param[in]  const std::string& nodeName 节点名
	**	@param[out] int& value  返回的结果
	**  @param[in]  int  defaut 默认参数
	**/	
	void GetValue(const std::string& nodeName, int32_t& value, int defaut = -1);

	//重读配置文件
	void ReLoadConfig(const char* xmlFileName);

	//列出所有选项
	void List();
private:
	void dump_to_stdout( TiXmlNode* pParent, u_int32_t indent = 0 );
	int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);
	const char *getIndentAlt( unsigned int numIndents );
	const char *getIndent( unsigned int numIndents );
private:
	TiXmlDocument m_XmlConfigFile;
	TiXmlElement* m_RootElem;
};

}}//namespace Game51 { namespace Server

#endif

