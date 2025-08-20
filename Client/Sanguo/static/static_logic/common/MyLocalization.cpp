/*!
 * \file SandboxConfigInitializer.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 05/10/2016
 *
 *
 */
#include "MyLocalization.h"
#include "FileUtil.h"
#include "XmlUtil.h"

#if defined(_WIN32)
#include <platform/win32/WindowsFileSystem.h>
#endif

MyLocalization& MyLocalization::getInstance()
{
	static MyLocalization s_MyLocals;
	return s_MyLocals;
}

bool MyLocalization::initialize(const char* pszfilePath)
{
	if (bInit)
	{
		return true;
	}
#if defined(_WIN32)
	std::string projectPath = pszfilePath;
	NS::tstring fileDir = NS::FileUtil::getFileDirectory(projectPath);
	NS::FileSystem::addSubFileSystem(new NS::WindowsFileSystem(fileDir, NS::SubFileSystem::FP_READ_WRITE, NS::SubFileSystem::FS_INTERNAL, nullptr));

	std::string strXmlData;
	std::string filePath = "../../ThirdConfig/localization/UE4/ui.cpp";
	if (!NS::XmlUtil::loadXmlFile(m_xmlGlobalDoc, filePath, 0))
	{
		tinyxml2::XMLElement* pXmlGlobalElement = m_xmlGlobalDoc.NewElement("userDefaultRoot");
		m_xmlGlobalDoc.LinkEndChild(pXmlGlobalElement);
		flushGlobal();
	}
	bInit = true;
#endif
	return true;
}

void MyLocalization::flushGlobal()
{
	std::string filePath = "../../ThirdConfig/localization/UE4/ui.cpp";
	NS::XmlUtil::saveXmlFile(m_xmlGlobalDoc, filePath, 0, true);
}

void MyLocalization::setString(const std::string& key, const std::string& value)
{
	std::string strKey = key;
	NS::StringUtil::replaceString(strKey, "/" , "");
	NS::StringUtil::replaceString(strKey, ".", "");
	for (int i = 0 ; i < 20 ; i++)
	{
		std::string str = fmt::format("_{0}WidgetTree_", i);
		NS::StringUtil::replaceString(strKey, str.c_str(), "_WidgetTree_");
	}
	std::string strValue = fmt::format("_LC(\"{0}\")", value.c_str());
	tinyxml2::XMLElement* pXmlRoot = m_xmlGlobalDoc.RootElement();
	if (!pXmlRoot) return;
	tinyxml2::XMLElement* pXmlElement = pXmlRoot->FirstChildElement(strKey.c_str());
	if (!pXmlElement)
	{
		pXmlElement = m_xmlGlobalDoc.NewElement(strKey.c_str());
		pXmlRoot->LinkEndChild(pXmlElement);
	}
	pXmlElement->DeleteChildren();

	tinyxml2::XMLText* pXmlText = m_xmlGlobalDoc.NewText(strValue.c_str());
	pXmlElement->LinkEndChild(pXmlText);
	flushGlobal();
}



