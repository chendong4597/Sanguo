/*!
 * \file MedicamentCfg.cpp
 *
 * \author ChenBK;
 * \date 2017/08/09
 *
 * 
 */
#include "stdafx.h"
#include "MedicamentCfg.h"
#include "../data/ItemData.h"
#include "XmlUtil.h"
#include <StringUtil.h>
#include "msg/MsgGameDef.pb.h"
#include "Cache.h"
#include "GlobalCfg.h"
#include <StringUtil.h>

using namespace NS;

MedicamentCfg& MedicamentCfg::getInstance()
{
	static MedicamentCfg s_medicamentCfg;
	return s_medicamentCfg;
}

bool MedicamentCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/MedicamentCfg.bin")) return false;

	return parseMedicamentXml(doc.RootElement());
    return true;
}

void MedicamentCfg::terminate()
{
	m_medicamentTypeMap.clear();
}

const MedicamentCfg::MedicamentType* MedicamentCfg::findMedicament(bzbee::uint32 id) const
{
	auto it = m_medicamentTypeMap.find(id);
	return (it == m_medicamentTypeMap.end()) ? nullptr : &it->second;
}

bool MedicamentCfg::parseMedicamentXml(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		MedicamentType info;

		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.s_name, xmlItem, "s_name");
		NS::XmlUtil::getAttribute(info.s_note, xmlItem, "s_note");
		NS::XmlUtil::getAttribute(info.n_effect_type, xmlItem, "n_effect_type");
		NS::XmlUtil::getAttribute(info.n_val, xmlItem, "n_val");
		NS::XmlUtil::getAttribute(info.n_medicament_type, xmlItem, "n_medicament_type");

		m_medicamentTypeMap[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

