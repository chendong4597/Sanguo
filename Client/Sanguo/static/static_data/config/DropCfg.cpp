/*!
* \file DropCfg.h
*
* \author jiangping huang
* \date 03/02/2017
*
*
*/
#include "stdafx.h"
#include "DropCfg.h"
#include "XmlUtil.h"
#include <StringUtil.h>

using namespace NS;
DropCfg& DropCfg::getInstance()
{
	static DropCfg s_DropCfg;
	return s_DropCfg;
}

bool DropCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/DropProcess.bin")) return false;
	if (!parseDropCfg(doc.RootElement())) return false;

    return true;
}

void DropCfg::terminate()
{
	m_dropTypeMap.clear();
}

const DropCfg::DropType* DropCfg::findDropType(int id) const
{
	auto it = m_dropTypeMap.find(id);
	if (it == m_dropTypeMap.end()) return nullptr;
	return &it->second;
}

bool DropCfg::parseDropCfg(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;


	int nMaxCell = 10;
	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "n_max_count_per_cell")
		{
			NS::XmlUtil::getAttribute(nMaxCell, pAttrXml, "Value");
		}

		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		int nId = 0;
		NS::XmlUtil::getAttribute(nId, xmlItem, "n_ID");
		if (nId == 0)
		{
			continue;
		}
		auto& dropType = m_dropTypeMap[nId];

		NS::XmlUtil::getAttribute(dropType.n_process_type, xmlItem, "n_process_type");
		NS::XmlUtil::getAttribute(dropType.n_process_data, xmlItem, "n_process_data");

		for (int i = 0; i < nMaxCell; i++)
		{
			std::string str = fmt::format("s_data_param{0}", i + 1);
			bzbee::tstring strParam;
			NS::XmlUtil::getAttribute(strParam, xmlItem, str.c_str());
			if (!strParam.empty()) {
				dropType.tv_param.push_back(strParam);
			}
		}
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

void DropCfg::getRewardVecById(int dropId, std::vector<std::tuple<int, int>>& outRewards)
{
	if (auto&& dropInfo = findDropType(dropId))
	{
		parseDropInfo(dropInfo->tv_param, outRewards);
	}
}

void DropCfg::parseDropInfo(const std::vector<bzbee::tstring>& source, std::vector<std::tuple<int, int>>& target)
{
	for (size_t idx = 0; idx < source.size(); ++idx)
	{
		if (source[idx].empty())
		{
			continue;
		}

		std::vector<NS::tstring> v1;
		NS::StringUtil::splitString(v1, source[idx], "[");

		std::vector<NS::tstring> v2;
		NS::StringUtil::splitString(v2, v1[1], ",");
		if (v2.size() < 2)
		{
			continue;
		}

		int itemId = -1;
		NS::StringUtil::str2int(itemId, v2[0].c_str());

		if (itemId < 0)
		{
			continue;
		}

		int num = -1;
		NS::StringUtil::str2int(num, v2[1].c_str());

		target.push_back(std::make_tuple(itemId, num));
	}
}