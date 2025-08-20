/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "WalkNpcCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




WalkNpcCfg& WalkNpcCfg::getInstance()
{
	static WalkNpcCfg s_WalkNpcCfg;
	return s_WalkNpcCfg;
}

bool WalkNpcCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/WalkNpcCfg.bin")) return false;

	return parseConfig(doc.RootElement());
}

void WalkNpcCfg::terminate()
{
	m_mapWalkNpc.clear();
}

bool WalkNpcCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
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
			NS::XmlUtil::getAttribute(s_Path, pAttrXml, "Value");
		}
		else if (strKey == "n_MinTarget") {
			NS::XmlUtil::getAttribute(n_MinTarget, pAttrXml, "Value");
		}
		else if (strKey == "n_MaxTarget") {
			NS::XmlUtil::getAttribute(n_MaxTarget, pAttrXml, "Value");
		}
		else if (strKey == "n_MinStdayTm") {
			NS::XmlUtil::getAttribute(n_MinStdayTm, pAttrXml, "Value");
		}
		else if (strKey == "n_MaxStdayTm") {
			NS::XmlUtil::getAttribute(n_MaxStdayTm, pAttrXml, "Value");
		}
		else if (strKey == "n_MidX") {
			NS::XmlUtil::getAttribute(n_MidX, pAttrXml, "Value");
		}
		else if (strKey == "n_PosZ") {
			NS::XmlUtil::getAttribute(n_PosZ, pAttrXml, "Value");
		}
		else if (strKey == "s_MidY") {
			NS::XmlUtil::getAttribute(s_MidY, pAttrXml, "Value");
		}
		else if (strKey == "s_Pos") {
			NS::XmlUtil::getAttribute(s_Pos, pAttrXml, "Value");
		}
		else if (strKey == "n_MaxWalkNum") {
			NS::XmlUtil::getAttribute(n_MaxWalkNum, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	NS::StringUtil::splitString(m_vecMidYs, s_MidY, "|");
	std::vector<std::string> vecStr;
	NS::StringUtil::splitString(vecStr, s_Pos, "|");
	for (auto str : vecStr)
	{
		std::vector<int> vec;
		NS::StringUtil::splitString(vec, str, ",");
		if (vec.size() == 2)
		{
			glm::vec3 vecPos;
			vecPos.x = vec[0];
			vecPos.y = vec[1];
			vecPos.z = n_PosZ;
			m_vecPos.push_back(vecPos);
		}
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		WalkNpcCfgInfo info;

		NS::XmlUtil::getAttribute(info.n_ID, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_Type, xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.s_Name, xmlItem, "s_Name");
		NS::XmlUtil::getAttribute(info.n_MonsterTpy, xmlItem, "n_MonsterTpy");
		NS::XmlUtil::getAttribute(info.n_rate, xmlItem, "n_rate");
		NS::XmlUtil::getAttribute(info.n_speed, xmlItem, "n_speed");
		NS::XmlUtil::getAttribute(info.s_Foder, xmlItem, "s_Foder");
		NS::XmlUtil::getAttribute(info.s_path, xmlItem, "s_path");
		NS::XmlUtil::getAttribute(info.s_desc, xmlItem, "s_desc");
		m_mapWalkNpc[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const WalkNpcCfg::WalkNpcCfgInfo* WalkNpcCfg::GetWalkNpcType(int id)
{
	if (m_mapWalkNpc.find(id) != m_mapWalkNpc.end())
	{
		return &(m_mapWalkNpc[id]);
	}
	return nullptr;
}


void WalkNpcCfg::GetStartAndEndPos(glm::vec3& startPos, glm::vec3& endPos, std::vector<glm::vec3>& midPos)
{
	int nIdx = std::rand() % m_vecPos.size();
	startPos = m_vecPos[nIdx];
	while (true)
	{
		int nextIdx = std::rand() % m_vecPos.size();
		if (nextIdx != nIdx)
		{
			endPos = m_vecPos[nextIdx];
			break;
		}
	}
	int startY = GetNearstY(startPos.y);
	int endY = GetNearstY(endPos.y);
	if (startY == endY)
	{
		{
			glm::vec3 pos;
			pos.x = startPos.x;
			pos.y = startY;
			pos.z = n_PosZ;
			midPos.push_back(pos);
		}
		{
			glm::vec3 pos;
			pos.x = endPos.x;
			pos.y = startY;
			pos.z = n_PosZ;
			midPos.push_back(pos);
		}
	}
	else {
		{
			glm::vec3 pos;
			pos.x = startPos.x;
			pos.y = startY;
			pos.z = n_PosZ;
			midPos.push_back(pos);
		}
		{
			glm::vec3 pos;
			pos.x = n_MidX;
			pos.y = startY;
			pos.z = n_PosZ;
			midPos.push_back(pos);
		}
		{
			glm::vec3 pos;
			pos.x = n_MidX;
			pos.y = endY;
			pos.z = n_PosZ;
			midPos.push_back(pos);
		}
		{
			glm::vec3 pos;
			pos.x = endPos.x;
			pos.y = endY;
			pos.z = n_PosZ;
			midPos.push_back(pos);
		}
	}
}

int WalkNpcCfg::GetNearstY(int nY)
{
	int maxDis = 100000;
	int toY = 0;
	for (auto y : m_vecMidYs)
	{
		if (std::abs(nY - y) < maxDis)
		{
			maxDis = std::abs(nY - y);
			toY = y;
		}
	}
	return toY;
}

int WalkNpcCfg::GetRandNpcId()
{
	std::vector<int> vecIds;
	for (auto npc : m_mapWalkNpc)
	{
		vecIds.push_back(npc.first);
	}
	if (vecIds.size() <= 0) {
		return 0;
	}
	int nIndex = std::rand() % vecIds.size();
	return vecIds[nIndex];
}
std::string WalkNpcCfg::GetBPWalkNpcPath(int id)
{
	auto&& pCfg = GetWalkNpcType(id);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("Blueprint'{0}/{1}/{2}.{3}'", s_Path , pCfg->s_Foder, pCfg->s_path, pCfg->s_path);
}


