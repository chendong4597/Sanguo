/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "NpcTypeCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




NpcTypeCfg& NpcTypeCfg::getInstance()
{
	static NpcTypeCfg s_NpcTypeCfg;
	return s_NpcTypeCfg;
}

bool NpcTypeCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/NpcType.bin")) return false;
	if (!parseConfig(doc.RootElement())) {
		return false;
	}

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/GenelTemplate.bin")) return false;
	if (!parseGenelConfig(doc.RootElement())) {
		return false;
	}

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/FuncDetailType.bin")) return false;
	if (!parseNpcFuncDetailConfig(doc.RootElement())) {
		return false;
	}

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/FuncNpcType.bin")) return false;
	if (!parseNpcFuncConfig(doc.RootElement())) {
		return false;
	}

	return true;
}

void NpcTypeCfg::terminate()
{
	m_mapNpcType.clear();
}

bool NpcTypeCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	int nSayNum = 4;
	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "n_SayNum")
		{
			NS::XmlUtil::getAttribute(nSayNum, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		NpcTypeCfgInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.s_icon, xmlItem, "s_icon");
		NS::XmlUtil::getAttribute(info.s_halfHead, xmlItem, "s_halfHead");
		NS::XmlUtil::getAttribute(info.s_HudIcon, xmlItem, "s_HudIcon");
		NS::XmlUtil::getAttribute(info.s_name, xmlItem, "s_Name");
		NS::XmlUtil::getAttribute(info.s_TpyName, xmlItem, "s_type_name");
		NS::XmlUtil::getAttribute(info.n_Type , xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.n_SubType, xmlItem, "n_SubType");
		NS::XmlUtil::getAttribute(info.n_Quality, xmlItem, "n_Quality");
		NS::XmlUtil::getAttribute(info.n_Weapon1, xmlItem, "n_Weapon1");
		NS::XmlUtil::getAttribute(info.n_Weapon2, xmlItem, "n_Weapon2");
		NS::XmlUtil::getAttribute(info.s_BP_Foder, xmlItem, "s_BP_Foder");
		NS::XmlUtil::getAttribute(info.s_BP_Path, xmlItem, "s_BP_Path");
		for (int i = 0 ; i < 8 ; i++)
		{
			std::string str = fmt::format("s_Attack_{0:02d}" , i + 1);
			std::string strInfo;
			NS::XmlUtil::getAttribute(strInfo , xmlItem, str.c_str());
			info.vecAniMotoPaths.push_back(strInfo);
		}

		NS::XmlUtil::getAttribute(info.s_dead_path, xmlItem, "s_dead_path");
		NS::XmlUtil::getAttribute(info.s_hit_path, xmlItem, "s_hit_path");
		NS::XmlUtil::getAttribute(info.s_desc, xmlItem, "s_desc");
		std::string s_useHead;
		NS::XmlUtil::getAttribute(s_useHead, xmlItem, "s_useHead");
		NS::StringUtil::splitString(info.m_vecUseTpys, s_useHead, "|");
		for (int i = 0 ; i < nSayNum; i++)
		{
			std::string str = fmt::format("s_say{0}", i + 1);
			std::string strSay;
			NS::XmlUtil::getAttribute(strSay, xmlItem, str.c_str());
			if (!strSay.empty()) {
				info.vecSays.push_back(strSay);
			}
		}

		m_mapNpcType[info.n_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const NpcTypeCfg::NpcTypeCfgInfo* NpcTypeCfg::GetNpcType(int id)
{
	if (m_mapNpcType.find(id) != m_mapNpcType.end())
	{
		return &(m_mapNpcType[id]);
	}
	return nullptr;
}

std::string NpcTypeCfg::GetRandSay(int nId)
{
	const NpcTypeCfg::NpcTypeCfgInfo* pNpcTpy = GetNpcType(nId);
	if (pNpcTpy && pNpcTpy->vecSays.size() > 0) {
		int nIdx = rand() % pNpcTpy->vecSays.size();
		return pNpcTpy->vecSays[nIdx];
	}
	return "";
}


std::string NpcTypeCfg::GetBpNpcPath(int nId)
{
	auto&& genelCfg = GetGenelType(nId);
	if (!genelCfg)
	{
		return "";
	}
	return fmt::format("Blueprint'{0}/{1}/{2}.{3}_C'", m_strPath, genelCfg->s_BP_Foder, genelCfg->s_BP_Path, genelCfg->s_BP_Path);
}

std::string NpcTypeCfg::GetAttackPath(int nId, int nTempId, bool bSecAni)
{
	auto&& npcCfg = GetNpcType(nId);
	if (!npcCfg || npcCfg->n_SubType == NpcTypeCfg::NpcSubType_None)
	{
		return "";
	}
	auto&& genelCfg = GetGenelType(nTempId);
	if (!genelCfg)
	{
		return "";
	}
	std::string strAtk = "infantry_attack_01_Montage";
	if (npcCfg->n_SubType == NpcTypeCfg::NpcSubType_Archer)
	{
		strAtk = "archer_attack_01_Montage";
	}else if (npcCfg->n_SubType == NpcTypeCfg::NpcSubType_Cavalry)
	{
		if (bSecAni) {
			strAtk = "cavalry_attack_02_Montage";
		}
		else {
			strAtk = "cavalry_attack_01_Montage";
		}
	}
	else if (npcCfg->n_SubType == NpcTypeCfg::NpcSubType_Blade)
	{
		if (bSecAni) {
			strAtk = "blade_attack_02_Montage";
		}
		else {
			strAtk = "blade_attack_01_Montage";
		}
	}
	else if (npcCfg->n_SubType == NpcTypeCfg::NpcSubType_Magic)
	{
		if (bSecAni) {
			strAtk = "magic_attack_02_Montage";
		}
		else {
			strAtk = "magic_attack_01_Montage";
		}
	}
	return fmt::format("AnimMontage'{0}/{1}/{2}.{3}'", m_strGenelPath, genelCfg->s_BP_Foder, strAtk, strAtk);
}


/*******************************************************************************************************************
*******************************************************************************************************************/

bool NpcTypeCfg::parseGenelConfig(tinyxml2::XMLElement* pXmlRoot)
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
			NS::XmlUtil::getAttribute(m_strGenelPath, pAttrXml, "Value");
		}else if (strKey == "s_NpcPath")
		{
			NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		GenelTypeInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.s_GenName, xmlItem, "s_name");
		NS::XmlUtil::getAttribute(info.s_HalfIcon, xmlItem, "s_HalfIcon");
		NS::XmlUtil::getAttribute(info.s_BP_Foder, xmlItem, "s_BP_Foder");
		NS::XmlUtil::getAttribute(info.s_BP_Path, xmlItem, "s_BP_Path");
		for (int i = 0; i < 8; i++)
		{
			std::string str = fmt::format("s_Attack_{0:02d}", i + 1);
			std::string strInfo;
			NS::XmlUtil::getAttribute(strInfo, xmlItem, str.c_str());
			info.vecAniMotoPaths.push_back(strInfo);
		}

		for (int i = 0; i < 4; i++)
		{
			std::string str = fmt::format("s_Txt{0}", i + 1);
			std::string strInfo;
			NS::XmlUtil::getAttribute(strInfo, xmlItem, str.c_str());
			if (!strInfo.empty())
			{
				info.vecTxtures.push_back(strInfo);
			}
		}

		NS::XmlUtil::getAttribute(info.s_dead_path, xmlItem, "s_dead_path");
		NS::XmlUtil::getAttribute(info.s_hit_path, xmlItem, "s_hit_path");
		m_mapGenelType[info.n_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const NpcTypeCfg::GenelTypeInfo* NpcTypeCfg::GetGenelType(int id)
{
	if (m_mapGenelType.find(id) != m_mapGenelType.end())
	{
		return &(m_mapGenelType[id]);
	}
	return nullptr;
}

std::string NpcTypeCfg::GetBpGenelPath(int nId)
{
	auto&& genelCfg = GetGenelType(nId);
	if (!genelCfg)
	{
		return "";
	}
	return fmt::format("Blueprint'{0}/{1}/{2}.{3}_C'", m_strGenelPath, genelCfg->s_BP_Foder, genelCfg->s_BP_Path, genelCfg->s_BP_Path);
}

std::string NpcTypeCfg::GetBpMatTexture(int nId, std::string strTxt)
{
	auto&& genelCfg = GetGenelType(nId);
	if (!genelCfg)
	{
		return "";
	}
	return fmt::format("Texture2D'{0}/{1}/{2}.{3}'", m_strGenelPath, genelCfg->s_BP_Foder, strTxt, strTxt);
}

std::string NpcTypeCfg::GetBpHeadPath(int nId)
{
	auto&& genelCfg = GetNpcType(nId);
	if (!genelCfg)
	{
		return "";
	}
	if (genelCfg->n_Type == NpcTypeCfg::NpcType_Func) {
		return fmt::format("Texture2D'/Game/UI/Assert/npc_head/{0}.{1}'", genelCfg->s_icon, genelCfg->s_icon);
	}
	else {
		return fmt::format("Texture2D'/Game/UI/Assert/Warrior/{0}.{1}'", genelCfg->s_icon, genelCfg->s_icon);
	}
}

std::string NpcTypeCfg::GetBpGenProText(int nId)
{
	auto&& genelCfg = GetNpcType(nId);
	if (!genelCfg)
	{
		return "";
	}
	
	return fmt::format("Texture2D'/Game/UI/Assert/common/bin_{0}.bin_{1}'", genelCfg->n_SubType, genelCfg->n_SubType);
}

std::string NpcTypeCfg::GetBpHalfHeadPath(int nId)
{
	auto&& genelCfg = GetNpcType(nId);
	if (!genelCfg)
	{
		return "";
	}
	if (genelCfg->n_Type == NpcTypeCfg::NpcType_Func) {
		return fmt::format("Texture2D'/Game/UI/Assert/half_head/{0}.{1}'", genelCfg->s_halfHead, genelCfg->s_halfHead);
	}
	else {
		return fmt::format("Texture2D'/Game/UI/Assert/Warrior/{0}.{1}'", genelCfg->s_halfHead, genelCfg->s_halfHead);
	}
}

std::string NpcTypeCfg::GetBpGenHalfHeadPath(int nId)
{
	auto&& genelCfg = GetGenelType(nId);
	if (!genelCfg)
	{
		return "";
	}
	return fmt::format("Texture2D'/Game/UI/Assert/GenHalfHead/{0}.{1}'", genelCfg->s_HalfIcon, genelCfg->s_HalfIcon);
}

std::string  NpcTypeCfg::GetFuncHeadPath(int nId)
{
	auto&& funcCfg = GetNpcFunc(nId);
	if (!funcCfg)
	{
		return "";
	}
	return fmt::format("Texture2D'/Game/UI/Assert/npc_head/{0}.{1}'", funcCfg->s_icon, funcCfg->s_icon);
}


//功能类型
bool NpcTypeCfg::parseNpcFuncDetailConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		NpcFuncDetail info;
		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.s_Name, xmlItem, "s_Name");
		m_mapFuncDetail[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const NpcFuncDetail* NpcTypeCfg::GetNpcFuncDetail(int id)
{
	if (m_mapFuncDetail.find(id) != m_mapFuncDetail.end())
	{
		return &(m_mapFuncDetail[id]);
	}
	return nullptr;
}

//普通功能NPC
bool NpcTypeCfg::parseNpcFuncConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		FuncNpcType info;
		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.s_icon, xmlItem, "s_icon");
		NS::XmlUtil::getAttribute(info.s_Name, xmlItem, "s_Name");
		NS::XmlUtil::getAttribute(info.s_say, xmlItem, "s_say");
		NS::XmlUtil::getAttribute(info.s_HudIcon, xmlItem, "s_HudIcon");
		std::string strFunc;
		NS::XmlUtil::getAttribute(strFunc, xmlItem, "s_func");
		NS::StringUtil::splitString(info.vecFunc, strFunc, ",");
		m_mapNpcFunc[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const FuncNpcType* NpcTypeCfg::GetNpcFunc(int id)
{
	if (m_mapNpcFunc.find(id) != m_mapNpcFunc.end())
	{
		return &(m_mapNpcFunc[id]);
	}
	return nullptr;
}


