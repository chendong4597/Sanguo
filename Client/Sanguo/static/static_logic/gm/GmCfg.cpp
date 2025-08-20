/*!
 * \file Gm.cpp
 *
 * \author ChenBK;
 * \date 2017/06/12
 *
 *
 */
#include "stdafx.h"
#include "GmCfg.h"
#include "tinyxml2.h"
#include "XmlUtil.h"
#include "Cache.h"
#include <LogUtil.h>

using namespace NS;

size_t GmCfg::GM_HISTORY_MAX = 100;
const bzbee::tstring GmCfg::GmHistoryPath = "GmHistory.xml";

GmCfg::GmCfg()
{
}

GmCfg::~GmCfg()
{
}


GmCfg& GmCfg::getInstance()
{
	static GmCfg s_gmCfg;
	return s_gmCfg;
}


bool GmCfg::initialize()
{
	if (!readCfg()) return false;
	//readGmHistory();

	return true;
}

void GmCfg::terminate()
{
	//this->flushGmHistory();
	m_gmItemList.clear();
	m_gmHistoryList.clear();
}

bool GmCfg::readCfg()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Gm.cfg")) return false;

	tstring gmName;
	tstring fixedHistory;
	tinyxml2::XMLElement* pXmlRoot = doc.RootElement();

	for (tinyxml2::XMLElement* xmlItem = pXmlRoot->FirstChildElement("Record"); xmlItem != nullptr; xmlItem = xmlItem->NextSiblingElement("Record"))
	{
		gmName.clear();
		if (!xmlItem) { return false; }

		GmItem item;
		bzbee::XmlUtil::getElement(gmName, xmlItem, "GmName");
		StringUtil::trim(gmName, gmName);
		StringUtil::tolower(item.m_gmName, fmt::format("@{0}", gmName));
		bzbee::XmlUtil::getElement(item.m_gmLvel, xmlItem, "GmLev");
		bzbee::XmlUtil::getElement(item.m_gmNote, xmlItem, "GmNote");
		bzbee::XmlUtil::getElement(item.m_gmDefaultPara, xmlItem, "DefaultPara");
		bzbee::XmlUtil::getElement(item.m_gmType, xmlItem, "GmType");

		if (!bzbee::FileSystem::isFileExist(GmHistoryPath, SubFileSystem::FS_EXTERNAL))
		{
			fixedHistory.clear();
			bzbee::XmlUtil::getAttribute(fixedHistory, xmlItem, "FixedHistory");
			if (std::strcmp(fixedHistory.c_str(), "true") == 0)
			{
				m_gmHistoryList.push_back(fmt::format("@{0} {1}", gmName, item.m_gmDefaultPara));
			}
		}

		m_gmItemList.push_back(item);
	}

	std::sort(m_gmItemList.begin(), m_gmItemList.end(), [](GmItem a, GmItem b) { return b.m_gmName > a.m_gmName; });

	return true;
}

bool GmCfg::readGmHistory()
{
	bzbee::tstring filePath = GmHistoryPath;

    tinyxml2::XMLDocument xmlDoc;
    if (!XmlUtil::loadXmlFile(xmlDoc, filePath, SubFileSystem::FS_EXTERNAL))
	{
        LOGE("load xml failed {0}", filePath);
        bzbee::FileSystem::deleteFile(filePath, SubFileSystem::FS_EXTERNAL);
        return false;
    }

    tinyxml2::XMLElement* pXmlRoot = xmlDoc.RootElement();
    for (tinyxml2::XMLElement* xmlItem = pXmlRoot->FirstChildElement("Record"); xmlItem != nullptr; xmlItem = xmlItem->NextSiblingElement("Record"))
    {
        if (!xmlItem) { return false; }
        m_gmHistoryList.push_back(xmlItem->GetText());
    }

    return true;
}

const GmCfg::GmItem* GmCfg::getGmItemById(size_t idx)
{
	if (idx >= m_gmItemList.size()) return nullptr;

	return &m_gmItemList.at(idx);
}

const GmCfg::GmItem* GmCfg::getGmItemByName(const tstring& gmName)
{
	for (auto& item : m_gmItemList)
	{
		if (item.m_gmName == gmName) return &item;
	}

	return nullptr;
}

GmCfg::GmType GmCfg::getGmType(const tstring& gmName)
{
	if (gmName.empty()) return GmType::None;

	for (auto item : m_gmItemList)
	{
		if (item.m_gmName == gmName) return item.m_gmType;
	}

	return GmType::None;
}

bool GmCfg::addGmHistory(const tstring& gmline)
{
	if (gmline.empty()) return false;

	GmHistoryList::iterator findIt = std::find(m_gmHistoryList.begin(), m_gmHistoryList.end(), gmline);

	if (findIt != m_gmHistoryList.end())
	{
		m_gmHistoryList.erase(findIt);
	}


	if (m_gmHistoryList.size() >= GM_HISTORY_MAX)
	{
		m_gmHistoryList.pop_front();
	}

	m_gmHistoryList.push_back(gmline);

	return true;
}


GmCfg::GmHistoryList GmCfg::getGmHistory(size_t getNum /*= 10*/)
{
	GmHistoryList returnList;
//	size_t gmHLsize = m_gmHistoryList.size();
	GmHistoryList::iterator endIt = m_gmHistoryList.end();
	size_t count = 0;

	while (endIt != m_gmHistoryList.begin() && count < getNum)
	{
		returnList.push_back(*--endIt);
		++count;
	}

	return returnList;
}


void GmCfg::flushGmHistory()
{
	if (!m_gmHistoryList.size()) return;

	tinyxml2::XMLDocument xmlDoc;

	tinyxml2::XMLElement* pXmlRoot = xmlDoc.NewElement("Root");
	xmlDoc.LinkEndChild(pXmlRoot);

	for (auto it : m_gmHistoryList)
	{
		tinyxml2::XMLElement* pXmlElement = xmlDoc.NewElement("Record");
		pXmlRoot->LinkEndChild(pXmlElement);

		tinyxml2::XMLText* pXmlText = xmlDoc.NewText(it.c_str());
		pXmlElement->LinkEndChild(pXmlText);
	}

    if (!XmlUtil::saveXmlFile(xmlDoc, GmHistoryPath, SubFileSystem::FS_EXTERNAL))
    {
        LOGE("save xml failed {0}", GmHistoryPath);
    }
}
