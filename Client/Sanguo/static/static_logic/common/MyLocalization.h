/*!
 * \file SandboxConfigInitializer.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 05/10/2016
 *
 * 
 */
#pragma once 
#include "BaseTypeDefine.h"
#include <map>
#include "tinyxml2.h"


class MyLocalization
{
public:
	static MyLocalization& getInstance();

	void setString(const std::string& key, const std::string& value);
	void flushGlobal();
	bool initialize(const char* pszfilePath);
private:
	std::map<std::string, std::string> m_strLocals;

	tinyxml2::XMLDocument m_xmlGlobalDoc;
	bool bInit{ false };
};
