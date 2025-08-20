/*!
 * \file Cache.cpp
 *
 * \author laizhisheng
 * \date 2016/12/02 13:33
 *
 */

#include "Cache.h"
#include "StringUtil.h"
#include "SystemUtil.h"
#include "FileUtil.h"
#include "LogUtil.h"
#include "XmlUtil.h"
#include "RandomUtil.h"

NS_BEGIN

static const tstring s_globalUserDefault = "GlobalUserDefault.xml";

Cache& Cache::getInstance()
{
	static Cache cache;
	return cache;
}

bool Cache::initialize()
{
	if (!XmlUtil::loadXmlFile(m_xmlGlobalDoc, s_globalUserDefault, SubFileSystem::FS_EXTERNAL))
	{
		LOGE("load xml failed {0}", s_globalUserDefault);
		FileSystem::deleteFile(s_globalUserDefault, SubFileSystem::FS_EXTERNAL);

		tinyxml2::XMLElement* pXmlGlobalElement = m_xmlGlobalDoc.NewElement("userDefaultRoot");
		m_xmlGlobalDoc.LinkEndChild(pXmlGlobalElement);
		flushGlobal();
	}

	return true;
}

bool Cache::SetIPAndIggId(tstring strIP, I64 iggId)
{
	m_loginServer = strIP;
	m_iggId = iggId;
	m_filePath = fmt::format("UserDefault_{0}@{1}.xml", strIP, iggId);

	if (!XmlUtil::loadXmlFile(m_xmlDoc, m_filePath, SubFileSystem::FS_EXTERNAL))
	{
		LOGE("load user xml failed {0}", m_filePath);
		FileSystem::deleteFile(m_filePath, SubFileSystem::FS_EXTERNAL);

		tinyxml2::XMLElement* pXmlElement = m_xmlDoc.NewElement("userDefaultRoot");
		m_xmlDoc.LinkEndChild(pXmlElement);
		flush();
	}

	return true;
}

void Cache::terminate()
{
	flush();
}

int Cache::getInt(const tstring& key, int defaultValue)
{
	int value = defaultValue;
	tstring strValue = getString(key, EMPTY_STRING);
	if (strValue != EMPTY_STRING)
	{
		StringUtil::stringToType(value, strValue.c_str());
	}

	return value;
}

void Cache::setInt(const tstring& key, int value)
{
	tstring strValue;
	StringUtil::typeToString(strValue, value);
	setString(key, strValue);
}

I64 Cache::getInt64(const tstring& key, I64 defaultValue)
{
	I64 value = defaultValue;
	tstring strValue = getString(key, EMPTY_STRING);
	if (strValue != EMPTY_STRING)
	{
		StringUtil::stringToType(value, strValue.c_str());
	}

	return value;
}

void Cache::setInt64(const tstring& key, I64 value)
{
	tstring strValue;
	StringUtil::typeToString(strValue, value);
	setString(key, strValue);
}

tstring Cache::getString(const tstring& key, const tstring& defaultValue)
{
	tinyxml2::XMLElement* pXmlRoot = m_xmlDoc.RootElement();
	if (!pXmlRoot) { return defaultValue; }
	tinyxml2::XMLElement* pXmlElement = pXmlRoot->FirstChildElement(key.c_str());
	if (!pXmlElement) return defaultValue;
    if (pXmlElement->NoChildren()) return EMPTY_STRING;
	return pXmlElement->GetText();
}

void Cache::setString(const tstring& key, const tstring& value)
{
	tinyxml2::XMLElement* pXmlRoot = m_xmlDoc.RootElement();
	if (!pXmlRoot) { return; }
	tinyxml2::XMLElement* pXmlElement = pXmlRoot->FirstChildElement(key.c_str());
	if (!pXmlElement)
	{
		pXmlElement = m_xmlDoc.NewElement(key.c_str());
		pXmlRoot->LinkEndChild(pXmlElement);
	}
	pXmlElement->DeleteChildren();

	tinyxml2::XMLText* pXmlText = m_xmlDoc.NewText(value.c_str());
	pXmlElement->LinkEndChild(pXmlText);
}

bool Cache::getData(const tstring& key, BufferData& dataOut)
{
	tstring strData = getString(key, EMPTY_STRING);
	if (strData.empty()) return false;

	BufferData decodeData;
	if (!StringUtil::base64Decode(decodeData, strData.c_str())) return false;

	if (!StringUtil::uncompressData(dataOut, &decodeData[0], decodeData.size())) return false;

	return true;
}

bool Cache::setData(const tstring& key, const BufferData& data)
{
	BufferData compressedData;
	if (!StringUtil::compressData(compressedData, data.data(), data.size())) return false;

	tstring strBase64;
	if (!StringUtil::base64Encode(strBase64, &compressedData[0], compressedData.size())) return false;

	setString(key, strBase64);
	return true;
}

void Cache::setLoginServers(std::vector<LoginServerAddress>& vec)
{
	m_loginServerArray.swap(vec);
}

Cache::LoginServerAddress Cache::chooseLoginServer(void) const
{
	if (m_loginServerArray.empty())
		return { "35.197.25.150", 38201 };
	else
		return m_loginServerArray[RandomUtil::getRandomInt(0, m_loginServerArray.size() - 1)];
}

void Cache::flush()
{
	XmlUtil::saveXmlFile(m_xmlDoc, m_filePath, SubFileSystem::FS_EXTERNAL, true);
}

void Cache::removeCache()
{
    FileSystem::deleteFile(m_filePath, SubFileSystem::FS_EXTERNAL);
    m_xmlDoc.Clear();
	tinyxml2::XMLElement* pXmlElement = m_xmlDoc.NewElement("userDefaultRoot");
	m_xmlDoc.LinkEndChild(pXmlElement);
}

I64 Cache::getGlobalInt64(const tstring& key, I64 defaultValue)
{
	I64 value = defaultValue;
	tstring strValue = getGlobalString(key, EMPTY_STRING);
	if (strValue != EMPTY_STRING)
	{
		StringUtil::stringToType(value, strValue);
	}

	return value;
}

void Cache::setGlobalInt64(const tstring& key, I64 value)
{
	tstring strValue;
	StringUtil::typeToString(strValue, value);
	setGlobalString(key, strValue);
}

tstring Cache::getGlobalString(const tstring& key, const tstring& defaultValue)
{
	tinyxml2::XMLElement* pXmlRoot = m_xmlGlobalDoc.RootElement();
	if (!pXmlRoot) { return defaultValue; }
	tinyxml2::XMLElement* pXmlElement = pXmlRoot->FirstChildElement(key.c_str());
	if (!pXmlElement) return defaultValue;
    if (pXmlElement->NoChildren()) return EMPTY_STRING;
	return pXmlElement->GetText();
}

void Cache::setGlobalString(const tstring& key, const tstring& value)
{
	tinyxml2::XMLElement* pXmlRoot = m_xmlGlobalDoc.RootElement();
	if (!pXmlRoot) return;
	tinyxml2::XMLElement* pXmlElement = pXmlRoot->FirstChildElement(key.c_str());
	if (!pXmlElement)
	{
		pXmlElement = m_xmlGlobalDoc.NewElement(key.c_str());
		pXmlRoot->LinkEndChild(pXmlElement);
	}
	pXmlElement->DeleteChildren();

	tinyxml2::XMLText* pXmlText = m_xmlGlobalDoc.NewText(value.c_str());
	pXmlElement->LinkEndChild(pXmlText);
}

void Cache::flushGlobal()
{
	XmlUtil::saveXmlFile(m_xmlGlobalDoc, s_globalUserDefault, SubFileSystem::FS_EXTERNAL, true);
}

const tstring& Cache::getUserBehaviourLogPath()
{
    static const tstring s_tag = "SandBox_Logs";
    return s_tag;
}

NS_END
