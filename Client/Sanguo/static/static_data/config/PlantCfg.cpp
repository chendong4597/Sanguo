/*!
 * \file PlantCfg.cpp
 *
 * \author laizhisheng
 * \date 2016/12/20 18:03
 *
 */
#include "stdafx.h"
#include "PlantCfg.h"
#include "XmlUtil.h"
#include <StringUtil.h>
#include <FileUtil.h>

using namespace NS;
PlantCfg& PlantCfg::getInstance()
{
	static PlantCfg cfg;
	return cfg;
}

bool PlantCfg::initializeCfg()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/PlantCfg.bin")) return false;
	parsePlantConfig(doc.RootElement());
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/PlantTemplateCfg.bin")) return false;
	parsePlantTempConfig(doc.RootElement());
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/PlantPosCfg.bin")) return false;
	parsePlantPosConfig(doc.RootElement());
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/AnimalTemplateCfg.bin")) return false;
	parseAnimalConfig(doc.RootElement());

	return true;
}

void PlantCfg::terminate()
{
	m_plantTypeMap.clear();
	m_plantTemplateMap.clear();
}

bool PlantCfg::reloadMultiLanguage()
{
	for (auto& item : m_plantTypeMap)
	{
		item.second.s_name = _LC(item.second.s_name_untrans);
	}

	return true;
}

bool PlantCfg::parsePlantConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		PlantType info;
		NS::XmlUtil::getAttribute(info.m_typeId, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.s_name, xmlItem, "s_name");
		NS::XmlUtil::getAttribute(info.n_rate_level, xmlItem, "n_rate_level");
		NS::XmlUtil::getAttribute(info.n_exp, xmlItem, "n_exp");
		NS::XmlUtil::getAttribute(info.m_growType, xmlItem, "n_grow_type");
		NS::XmlUtil::getAttribute(info.n_Prosperous, xmlItem, "n_Prosperous");
		m_plantTypeMap[info.m_typeId] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

bool PlantCfg::parsePlantTempConfig(tinyxml2::XMLElement* pXmlRoot)
{

	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "s_Path")
		{
			NS::XmlUtil::getAttribute(m_strTreePath, pAttrXml, "Value");
		}
		else if (strKey == "s_PathAni")
		{
			NS::XmlUtil::getAttribute(m_strAnimalPath, pAttrXml, "Value");
		}
		else if (strKey == "s_PathMine")
		{
			NS::XmlUtil::getAttribute(m_strMinePath, pAttrXml, "Value");
		}
		
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		PlantTemplate info;
		int nID = 0;
		NS::XmlUtil::getAttribute(nID, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.n_plant_type, xmlItem, "n_plant_type");
		NS::XmlUtil::getAttribute(info.m_stage, xmlItem, "n_stage");
		NS::XmlUtil::getAttribute(info.n_dropid, xmlItem, "n_dropid");
		NS::XmlUtil::getAttribute(info.n_next_stage_time, xmlItem, "n_next_stage_time");
		{
			std::string strPath;
			NS::XmlUtil::getAttribute(strPath, xmlItem, "s_Path");
			std::vector<std::string> vecStr;
			NS::StringUtil::splitString(vecStr, strPath, "/");
			if (vecStr.size() == 2) {
				info.s_Foder = vecStr[0];
				info.s_Path = vecStr[1];
			} else {
				info.s_Path = strPath;
			}
		}
		{
			std::string strAnimalPath;
			NS::XmlUtil::getAttribute(strAnimalPath, xmlItem, "s_Animals");
			std::vector<std::string> vecStr;
			NS::StringUtil::splitString(vecStr, strAnimalPath, ";");
			for (int i = 0 ; i < vecStr.size(); i++)
			{
				std::vector<std::string> vecStr1;
				NS::StringUtil::splitString(vecStr1, vecStr[i], ",");
				if (vecStr1.size() == 2)
				{
					PlantTemplate::animals animal;
					animal.nId = atoi(vecStr1[0].c_str());
					animal.nNum = atoi(vecStr1[1].c_str());
					info.vecAnimals.push_back(animal);
				}
			}
		}
		
		m_plantTemplateMap[nID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

bool PlantCfg::parsePlantPosConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		PlantPos info;
		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.n_flat, xmlItem, "n_flat");
		NS::XmlUtil::getAttribute(info.n_Lv, xmlItem, "n_Lv");
		m_plantPosMap[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}


bool PlantCfg::parseAnimalConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		AnimalCfg info;
		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.s_AnimFoder, xmlItem, "s_AnimFoder");
		NS::XmlUtil::getAttribute(info.s_AnimPath, xmlItem, "s_AnimPath");
		NS::XmlUtil::getAttribute(info.s_SoundEff, xmlItem, "s_SoundEff");
		NS::XmlUtil::getAttribute(info.s_SoundEff1, xmlItem, "s_SoundEff1");
		m_animalMap[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const PlantCfg::PlantType* PlantCfg::findPlantType(bzbee::uint id) const
{
	auto iter = m_plantTypeMap.find(id);
	return (iter == m_plantTypeMap.end()) ? nullptr : &iter->second;
}

const PlantCfg::PlantTemplate* PlantCfg::findPlantTemplate(bzbee::uint typeId, int stage)
{
	for (auto&& it = m_plantTemplateMap.begin(); it != m_plantTemplateMap.end(); it++)
	{
		if (it->second.n_plant_type == typeId && it->second.m_stage == stage)
		{
			return &it->second;
		}
	}
	return nullptr;
}

const PlantCfg::PlantPos* PlantCfg::findPlantPos(int nIdx)
{
	auto iter = m_plantPosMap.find(nIdx + 1);
	return (iter == m_plantPosMap.end()) ? nullptr : &iter->second;
}


std::string PlantCfg::GetBpPath(int nId, int nStage)
{
	auto&& pCfg = findPlantTemplate(nId, nStage);
	if (!pCfg)
	{
		return "";
	}
	auto&& pTpyCfg = findPlantType(nId);
	if (!pTpyCfg)
	{
		return "";
	}
	std::string strFoder = "";
	if (pTpyCfg->m_growType == GrowType::TREE) {
		strFoder = m_strTreePath;
	}
	else if (pTpyCfg->m_growType == GrowType::ANIMAL) {
		strFoder = m_strAnimalPath;
	} else if (pTpyCfg->m_growType == GrowType::MINE) {
		strFoder = m_strMinePath;
	}
	return fmt::format("Blueprint'{0}/{1}/{2}.{3}_C'", strFoder, pCfg->s_Foder, pCfg->s_Path, pCfg->s_Path);
}


std::string PlantCfg::GetBpAnimal(int nId)
{
	auto&& iter = m_animalMap.find(nId);
	if (iter == m_animalMap.end())
	{
		return "";
	}
	return fmt::format("Blueprint'{0}/{1}/{2}.{3}_C'", m_strAnimalPath, iter->second.s_AnimFoder,iter->second.s_AnimPath, iter->second.s_AnimPath);
}


std::string PlantCfg::GetBpMontagePath(int nId, int nIdx)
{
	auto&& iter = m_animalMap.find(nId);
	if (iter == m_animalMap.end())
	{
		return "";
	}
	return fmt::format("AnimMontage'{0}/{1}/play{2}_Montage.play{3}_Montage'", m_strAnimalPath, iter->second.s_AnimFoder, nIdx + 1, nIdx + 1);
}

const PlantCfg::AnimalCfg* PlantCfg::findAnimalTemplate(bzbee::uint nId)
{
	auto&& iter = m_animalMap.find(nId);
	if (iter != m_animalMap.end())
	{
		return &(iter->second);
	}
	return nullptr;
}



int PlantCfg::findGrowMaxPlantTemplate(bzbee::uint templateId)
{
	int nSpeed = 0;
	for (auto&& it = m_plantTemplateMap.begin(); it != m_plantTemplateMap.end(); it++)
	{
		if (it->second.n_plant_type == templateId)
		{
			nSpeed = std::max(nSpeed, it->second.n_next_stage_time);
		}
	}
	return nSpeed;
}

int PlantCfg::GetDropId(bzbee::uint templateId)
{
	for (auto&& it = m_plantTemplateMap.begin(); it != m_plantTemplateMap.end(); it++)
	{
		if (it->second.n_plant_type == templateId && it->second.n_next_stage_time == 0)
		{
			return it->second.n_dropid;
		}
	}
	return 0;
}


