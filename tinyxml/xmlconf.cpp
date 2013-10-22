/**
@file   xmlconf.cpp
@brief  读取xml配置文件类的实现

@author sjij@mail.51.com
@date Begin 2008.3
@date Dnd 2008.3
*/ 

#include "xmlconf.h"   
#include <iostream>   

namespace Game51 { namespace Server {

CXmlConfig::CXmlConfig()
{
	m_RootElem = 0;
}

int CXmlConfig::Init(const char* xmlFileName)
{
	m_XmlConfigFile = xmlFileName;
	//加载配置文件
	m_XmlConfigFile.LoadFile();  
	//得到配置文件的根结点
	m_RootElem = m_XmlConfigFile.RootElement();
	if(m_RootElem)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/***
**	@brief：得到字符串配置内容
**	@param[in]  const std::string& blockName 
**	@param[in]  const std::string& nodeName 节点名
**	@param[in] char* value  返回的结果
**/
void CXmlConfig::GetValue(const std::string& blockName, const std::string& nodeName, char *value)
{
	if(!m_RootElem)
	{   
		std::cerr << "read root error!" << std::endl;
		exit(-1);
	}

	//TiXmlElement* NodeElem;
	TiXmlElement* pElem = m_RootElem->FirstChildElement(blockName.c_str());
	if(pElem->FirstChildElement(nodeName.c_str()))                          //如果是节点
	{
		strcpy(value, pElem->FirstChildElement(nodeName.c_str())->GetText());
	}else if(pElem->Attribute(nodeName.c_str()))                            //如果是属性
	{	
		strcpy(value, pElem->Attribute(nodeName.c_str()));
	}else 
	{
		std::cerr << "node:"<<nodeName<<" is null!" << std::endl;
		exit(-1); 
	}
}


/***
**	@brief：得到整数参数
**  @param[in]  const std::string& blockName 区块名
**	@param[in]  const std::string& nodeName 节点名或属性名
**	@param[out] int& value  返回的结果
**  @param[in]  int  defaut 默认参数
**/
void CXmlConfig::GetValue(const std::string& blockName, const std::string& nodeName, int32_t& value, int defaut)
{
	if(!m_RootElem)
	{   
		std::cerr << "read root error!" << std::endl;
		exit(-1);
	}

	TiXmlElement* pElem = m_RootElem->FirstChildElement(blockName.c_str());
	if(pElem->FirstChildElement(nodeName.c_str()))                          //如果是节点
	{
		value = atoi(pElem->FirstChildElement(nodeName.c_str())->GetText());
	}else if(pElem->Attribute(nodeName.c_str()))                            //如果是属性
	{	
		value = atoi(pElem->Attribute(nodeName.c_str()));
	}else 
	{
		if(defaut == -1)
		{
			std::cerr << "node:"<<nodeName<<" is null!" << std::endl;
			exit(-1);
		}
		value = defaut;
		std::cerr << nodeName << " use default value[" << defaut << "]." << std::endl;
	}
}

/***
**	@brief：得到字符串配置内容，只有一层的结构
**  @param[in]  const std::string& blockName 区块名
**	@param[in]  const std::string& nodeName 节点名
**	@param[in]  char* value  返回的结果
**/
void CXmlConfig::GetValue(const std::string& nodeName, char *value)
{
	if(!m_RootElem)
	{   
		std::cerr << "read root error!" << std::endl;
		exit(-1);
	}

	//TiXmlElement* NodeElem;
	TiXmlElement* pElem = m_RootElem->FirstChildElement(nodeName.c_str());
	if(pElem)
	{
		strcpy(value, pElem->GetText());
	}else
	{
		std::cerr << "node:"<<nodeName<<" is null!" << std::endl;
		exit(-1); 
	}
}


/***
**	@brief：得到整数参数
**	@param[in]  const std::string& nodeName 节点名
**	@param[out] int& value  返回的结果
**  @param[in]  int  defaut 默认参数
**/
void CXmlConfig::GetValue(const std::string& nodeName, int32_t& value, int defaut)
{
	if(!m_RootElem)
	{   
		std::cerr << "read root error!" << std::endl;
		exit(-1);
	}

	TiXmlElement* pElem = m_RootElem->FirstChildElement(nodeName.c_str());
	if(pElem)
	{
		value = atoi(pElem->GetText());
	}
	else
	{
		if(defaut == -1)
		{
			std::cerr << "node:"<<nodeName<<" is null!" << std::endl;
			exit(-1);
		}
		value = defaut;
		std::cerr << nodeName << " use default value[" << defaut << "]." << std::endl;
	}
}

/*
@brief 重读配置文件
@param[in] const char* xmlFileName
*/
void CXmlConfig::ReLoadConfig(const char* xmlFileName)
{
	m_XmlConfigFile = xmlFileName;
	m_RootElem = 0;
	//加载配置文件
	m_XmlConfigFile.LoadFile();  
	//得到配置文件的根结点
	m_RootElem=m_XmlConfigFile.RootElement();
	return;
}

/*
@brief 显示配置文件内容
*/
void CXmlConfig::List()
{
	dump_to_stdout(&m_XmlConfigFile);
	return;
}

void CXmlConfig::dump_to_stdout( TiXmlNode* pParent, u_int32_t indent)
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;

	switch ( t )
	{
	case TiXmlNode::DOCUMENT:
		printf( "Document" );
		break;

	case TiXmlNode::ELEMENT:
		printf( "Element [%s]", pParent->Value() );
		num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
		switch(num)
		{
		case 0: printf( " (No attributes)"); break;
		case 1: printf( "%s1 attribute", getIndentAlt(indent)); break;
		default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
		}
		break;

	case TiXmlNode::COMMENT:
		printf( "Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}

	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		dump_to_stdout( pChild, indent+1 );
	}
}

// ———————————————————————-
// STDOUT输出和缩进实用函数
// ———————————————————————-
const char *CXmlConfig::getIndent( unsigned int numIndents )
{
	static const char * pINDENT=" + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

// 与getIndent相同，但最后没有“+”
const char *CXmlConfig::getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT=" ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

int CXmlConfig::dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	printf("\n");
	while (pAttrib)
	{
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS) printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i; 
}


}}//namespace Game51 { namespace Server

