/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "DefenceInfoTypeCfg.h"
#include "DefencePosTpyCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"

struct DPOINT
{
	int x{};
	int y{};
};;

bool PtInPolygon(DPOINT p, DPOINT ptPolygon[], int nCount)
{

	int nCross = 0;

	for (int i = 0; i < nCount; i++)

	{

		DPOINT p1 = ptPolygon[i];

		DPOINT p2 = ptPolygon[(i + 1) % nCount];

		// 求解 y=p.y 与 p1p2 的交点

		if (p1.y == p2.y) // p1p2 与 y=p0.y平行

			continue;

		if (p.y < std::min(p1.y, p2.y)) // 交点在p1p2延长线上

			continue;

		if (p.y >= std::max(p1.y, p2.y)) // 交点在p1p2延长线上

			continue;

		// 求交点的 X 坐标 --------------------------------------------------------------

		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;

		if (x > p.x)

			nCross++; // 只统计单边交点

	}

	// 单边交点为偶数，点在多边形之外 ---

	return (nCross % 2 == 1);
}


DefenceInfoTypeCfg& DefenceInfoTypeCfg::getInstance()
{
	static DefenceInfoTypeCfg s_DefenceInfoTypeCfg;
	return s_DefenceInfoTypeCfg;
}

bool DefenceInfoTypeCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/DefenceInfoType.bin")) return false;

	return parseConfig(doc.RootElement());
}

void DefenceInfoTypeCfg::terminate()
{
	m_mapDefenceInfoType.clear();
}

bool DefenceInfoTypeCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
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
			NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		DefenceInfoTypeCfgInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_Type , xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.n_PosTpy, xmlItem, "n_PosTpy");
		NS::XmlUtil::getAttribute(info.n_Width, xmlItem, "n_Width");
		NS::XmlUtil::getAttribute(info.n_Height, xmlItem, "n_Height");
		NS::XmlUtil::getAttribute(info.n_RotaTpy, xmlItem, "n_RotaTpy");
		NS::XmlUtil::getAttribute(info.s_Name, xmlItem, "s_Name");
		NS::XmlUtil::getAttribute(info.n_isContinue , xmlItem , "n_isContinue");
		NS::XmlUtil::getAttribute(info.n_Index, xmlItem, "n_Index");
		NS::XmlUtil::getAttribute(info.n_SkillId, xmlItem, "n_SkillId");
		NS::XmlUtil::getAttribute(info.f_IntertTm, xmlItem, "f_IntertTm");
		NS::XmlUtil::getAttribute(info.n_AtkSight, xmlItem, "n_AtkSight");
		NS::XmlUtil::getAttribute(info.n_LoseSight, xmlItem, "n_LoseSight");
		NS::XmlUtil::getAttribute(info.n_AtkAngle, xmlItem, "n_AtkAngle");
		NS::XmlUtil::getAttribute(info.n_AtkNum, xmlItem, "n_AtkNum");
		NS::XmlUtil::getAttribute(info.n_AI, xmlItem, "n_AI");
		NS::XmlUtil::getAttribute(info.s_Foder, xmlItem, "s_Foder");
		NS::XmlUtil::getAttribute(info.s_Path, xmlItem, "s_Path");
		NS::XmlUtil::getAttribute(info.s_Mesh, xmlItem, "s_Mesh");
		NS::XmlUtil::getAttribute(info.s_Desc, xmlItem, "s_Desc");

		xmlItem = xmlItem->NextSiblingElement("Record");
		m_mapDefenceInfoType[info.n_id] = info;
	}
	return true;
}

const DefenceInfoTypeCfg::DefenceInfoTypeCfgInfo* DefenceInfoTypeCfg::GetDefenceById(int id)
{
	if (m_mapDefenceInfoType.find(id) != m_mapDefenceInfoType.end())
	{
		return &(m_mapDefenceInfoType[id]);
	}
	return nullptr;
}

std::string DefenceInfoTypeCfg::GetBpPath(int nId)
{
	auto&& pCfg = GetDefenceById(nId);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("Blueprint'{0}/{1}/{2}.{3}_C'", m_strPath,pCfg->s_Foder, pCfg->s_Path,pCfg->s_Path);
}

std::string DefenceInfoTypeCfg::GetAttackPath(int nId)
{
	auto&& pCfg = GetDefenceById(nId);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("AnimMontage'{0}/{1}/attack_{2:02}_Montage.attack_{3:02}_Montage'", m_strPath, pCfg->s_Foder , 1 , 1);
}

std::string DefenceInfoTypeCfg::GetMeshPath(int nId)
{
	auto&& pCfg = GetDefenceById(nId);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("SkeletalMesh'{0}/{1}/{2}.{3}'", m_strPath, pCfg->s_Foder, pCfg->s_Mesh, pCfg->s_Mesh);
}


int DefenceInfoTypeCfg::GetDefencePosInRect(int nId , int nPosX, int nPosY)
{
	auto&& pDef = GetDefenceById(nId);
	if (!pDef)
	{
		return 0;
	}
	auto&& defPoss = DefencePosTpyCfg::getInstance().GetDefencePos();
	for (auto info : defPoss)
	{
		if (info.second.n_Type == Defence_Build_PosTpy_B && (info.second.n_PosTpy & pDef->n_PosTpy))
		{
			DPOINT pot;
			pot.x = nPosX;
			pot.y = nPosY;
			DPOINT pots[8];
			int nIdx = 0;
			for (int i = 0 ; i < 8 && i < info.second.vecHeroRangeInfo.size() ; i++)
			{
				pots[i].x = info.second.vecHeroRangeInfo[i].X;
				pots[i].y = info.second.vecHeroRangeInfo[i].Y;
				nIdx++;
			}
			if (PtInPolygon(pot , pots , nIdx))
			{
				return info.first;
			}
		}
	}
	return 0;
}


