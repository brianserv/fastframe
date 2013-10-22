#include "tinyxml.h"
#include "tinystr.h"
#include "xmlconf.h"
#include <iostream>
#include <string>

const char* XML_FILE_NAME = "./cs.xml";
const char* XML_FILE_NAME_ATTRIBUTE = "./cs1.xml";

//#define TIXML_USE_STL   //如果makefile不加，则加上定义，用stl
using namespace Game51::Server;
using namespace std;

int main()
{
	u_int32_t nDebug;
	u_int32_t nBufSize;
	char pErrLog[PATH_MAX];
	char pSysLog[PATH_MAX];
	u_int32_t nPort;
	char ShMFile[PATH_MAX];

	u_int32_t ShMSize;
	memset(pErrLog, 0, PATH_MAX);
	memset(pSysLog, 0, PATH_MAX);
	memset(ShMFile, 0, PATH_MAX);

	CXmlConfig conf;
	if(-1 == conf.Init(XML_FILE_NAME))
	{
		std::cerr << "conf init error." << std::endl;
		exit(-1);
	}

	conf.GetValue("DebugLevel", nDebug, 0);
	conf.GetValue("Log", "BufferSize", nBufSize, 8192);
	conf.GetValue("Log", "ErrLog", pErrLog);
	conf.GetValue("Log", "SysLog", pSysLog);
	conf.GetValue("Server", "Port", nPort);
	conf.GetValue("ShareMemory", "ShMFile", ShMFile);
	conf.GetValue("ShareMemory", "ShmSize", ShMSize, 671088640);

	std::cout << "system configuration." << std::endl;
	std::cout<<"DebugLevel:"<<nDebug<<std::endl;
	std::cout<<"BufferSize:"<<nBufSize<<std::endl;
	std::cout<<"ErrLog:"<<pErrLog<<std::endl;
	std::cout<<"SysLog:"<<pErrLog<<std::endl;
	std::cout<<"Server->Port:"<<nPort<<std::endl;
	std::cout<<"ShareMemory->ShMFile:"<<ShMFile<<std::endl;
	std::cout<<"ShareMemory->ShmSize:"<<ShMSize<<std::endl;

	std::cout<<"------------------------------------------------"<<std::endl;
	conf.List();

	memset(pErrLog, 0, PATH_MAX);
	memset(pSysLog, 0, PATH_MAX);
	memset(ShMFile, 0, PATH_MAX);
	CXmlConfig conf2(XML_FILE_NAME_ATTRIBUTE);
	conf2.GetValue("Log", "BufferSize", nBufSize, 8192);
	conf2.GetValue("Log", "ErrLog", pErrLog);
	conf2.GetValue("Log", "SysLog", pSysLog);
	conf2.GetValue("Server", "Port", nPort);
	conf2.GetValue("ShareMemory", "ShMFile", ShMFile);
	conf2.GetValue("ShareMemory", "ShMSize", ShMSize, 671088640);
	
	std::cout<<"ErrLog:"<<pErrLog<<std::endl;
	std::cout<<"SysLog:"<<pErrLog<<std::endl;
	std::cout<<"ShareMemory->ShMFile:"<<ShMFile<<std::endl;
	std::cout<<"BufferSize:"<<nBufSize<<std::endl;
	std::cout<<"ErrLog:"<<pErrLog<<std::endl;
	std::cout<<"SysLog:"<<pErrLog<<std::endl;
	std::cout<<"Server->Port:"<<nPort<<std::endl;
	std::cout<<"ShareMemory->ShMFile:"<<ShMFile<<std::endl;
	std::cout<<"ShareMemory->ShMSize:"<<ShMSize<<std::endl;

	return 0;
}

