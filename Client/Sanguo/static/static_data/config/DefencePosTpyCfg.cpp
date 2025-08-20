/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "DefencePosTpyCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




DefencePosTpyCfg& DefencePosTpyCfg::getInstance()
{
	static DefencePosTpyCfg s_DefencePosTpyCfg;
	return s_DefencePosTpyCfg;
}

bool DefencePosTpyCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/DefencePosBuild.bin")) return false;

	return parseConfig(doc.RootElement());
}

void DefencePosTpyCfg::terminate()
{
	m_mapDefencePosInfo.clear();
}

bool DefencePosTpyCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "n_BlockWidth")
		{
			NS::XmlUtil::getAttribute(m_nBlockWidth, pAttrXml, "Value");
		}
		else if (strKey == "n_BlockHeight") {
			NS::XmlUtil::getAttribute(m_nBlockHeight, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		DefencePosTpyInfo info;

		NS::XmlUtil::getAttribute(info.n_ID, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_Type, xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.n_PosTpy, xmlItem, "n_PosTpy");
		NS::XmlUtil::getAttribute(info.n_CameraLen, xmlItem , "n_CameraLen");
		NS::XmlUtil::getAttribute(info.n_AnglesR, xmlItem, "n_AnglesR");
		NS::XmlUtil::getAttribute(info.n_AnglesY, xmlItem, "n_AnglesY");
		NS::XmlUtil::getAttribute(info.n_BeginX, xmlItem, "n_BeginX");
		NS::XmlUtil::getAttribute(info.n_BeginY, xmlItem, "n_BeginY");
		NS::XmlUtil::getAttribute(info.n_BeginZ, xmlItem, "n_BeginZ");
		NS::XmlUtil::getAttribute(info.n_Width, xmlItem, "n_Width");
		NS::XmlUtil::getAttribute(info.n_Height, xmlItem, "n_Height");
		NS::XmlUtil::getAttribute(info.s_BPTag, xmlItem, "s_BPTag");
		{
			std::string strInfo;
			NS::XmlUtil::getAttribute(strInfo, xmlItem, "s_Range");
			std::vector<std::string> vecString;
			NS::StringUtil::splitString(vecString, strInfo, "|");
			if (strInfo.size() >= 3)
			{
				for (auto sInfo : vecString)
				{
					std::vector<int> vecInt;
					NS::StringUtil::splitString(vecInt, sInfo, ",");
					if (vecInt.size() == 2)
					{
						DefencePosInfo in;
						in.X = vecInt[0];
						in.Y = vecInt[1];
						info.vecRangeInfo.push_back(in);
					}
				}
			}
		}
		{
			std::string strInfo;
			NS::XmlUtil::getAttribute(strInfo, xmlItem, "s_HeroRange");
			std::vector<std::string> vecString;
			NS::StringUtil::splitString(vecString, strInfo, "|");
			if (strInfo.size() >= 3)
			{
				for (auto sInfo : vecString)
				{
					std::vector<int> vecInt;
					NS::StringUtil::splitString(vecInt, sInfo, ",");
					if (vecInt.size() == 2)
					{
						DefencePosInfo in;
						in.X = vecInt[0];
						in.Y = vecInt[1];
						info.vecHeroRangeInfo.push_back(in);
					}
				}
			}
		}

		xmlItem = xmlItem->NextSiblingElement("Record");
		m_mapDefencePosInfo[info.n_ID] = info;
	}
	return true;
}

const DefencePosTpyCfg::DefencePosTpyInfo* DefencePosTpyCfg::GetDefencePosById(int id)
{
	if (m_mapDefencePosInfo.find(id) != m_mapDefencePosInfo.end())
	{
		return &(m_mapDefencePosInfo[id]);
	}
	return nullptr;
}


void DefencePosTpyCfg::GetDefenceJustPos(int nId, int& nPosX, int& nPosY, int nWidth, int nHeight)
{
	auto&& pCfg = GetDefencePosById(nId);
	if (!pCfg)
	{
		return;
	}
	int nIdxX = (nPosX - pCfg->n_BeginX) / m_nBlockWidth;
	int nIdxY = (nPosY - pCfg->n_BeginY) / m_nBlockHeight;
	nPosX = pCfg->n_BeginX + nIdxX * m_nBlockWidth - (nWidth / 2)*m_nBlockWidth;
	nPosY = pCfg->n_BeginY + nIdxY * m_nBlockHeight - (nHeight / 2)*m_nBlockHeight;
	if (pCfg->n_PosTpy == Defence_Build_PosTpy_LandWall)
	{
		nPosY = pCfg->n_BeginY;
	}
}

void DefencePosTpyCfg::GetDefenceJustPosWall(int nId, int& nPosX, int& nPosY, int nWidth, int nHeight)
{
	auto&& pCfg = GetDefencePosById(nId);
	if (!pCfg)
	{
		return;
	}
	int nIdxX = (nPosX - pCfg->n_BeginX) / m_nBlockWidth;
	nPosX = pCfg->n_BeginX + nIdxX * m_nBlockWidth - (nWidth / 2)*m_nBlockWidth;
	nPosY = pCfg->n_BeginY;
}



