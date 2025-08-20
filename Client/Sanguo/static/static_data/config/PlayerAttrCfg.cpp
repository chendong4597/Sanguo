/*!
 * \file PlayerAttrCfg.cpp
 *
 * \author zhijieli
 * \date 2017/02/09
 *
 * 
 */
#include "stdafx.h"
#include "PlayerAttrCfg.h"
#include "StringUtil.h"
#include "msg/MsgGameDef.pb.h"
#include <XmlUtil.h>

PlayerAttrCfg& PlayerAttrCfg::getInstance()
{
	static PlayerAttrCfg s_PlayerAttrCfg;
	return s_PlayerAttrCfg;
}

bool PlayerAttrCfg::initialize()
{
	//if (!readAttachAttCfg()) return false;
	//if (!parseBareMoldBinary()) return false;
	//if (!parseAttrInhertBinary()) return false;
	//if (!parseEndlessAttrInhertBinary()) return false;

	//bool res =  parsePlayerAttrBinary();
	//bool res1 = parseEndlessPlayerAttrBinary();

	//return res1 && res;
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/PlayerAttr.bin")) return false;
	if (!parsePlayerAttrConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/AttachAttrName.cfg")) return false;
	if (!parseAttachAttCfg(doc.RootElement())) return false;

	return true;
}

void PlayerAttrCfg::terminate()
{
	m_playerAttrs.clear();
	m_mapAttrInfo.clear();
}

const PlayerAttrCfg::PlayerAttr* PlayerAttrCfg::findPlayerAttr(int id) const
{
	auto it = m_playerAttrs.find(id);
	return (it == m_playerAttrs.end()) ? nullptr : &it->second;
}

const AttrMap& PlayerAttrCfg::findPlayerAttrInfo(int nLev)
{
	const PlayerAttr* playerAttr = findPlayerAttr(nLev);
	if (playerAttr)
	{
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPM] = static_cast<float>(playerAttr->n_HPM);
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MPM] = static_cast<float>(playerAttr->n_MPM);
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_AD] = static_cast<float>(playerAttr->n_AD);
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_AP] = static_cast<float>(playerAttr->n_AP);
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ARM] = static_cast<float>(playerAttr->n_ARM);
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MARM] = static_cast<float>(playerAttr->n_MARM);
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MV_SP] = f_Move / 100.0f;
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CHASE] = f_Run / 100.0f;
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPR] = static_cast<float>(playerAttr->n_HPR);
		m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MPR] = static_cast<float>(playerAttr->n_MPR);
	}
	return m_mapAttrInfo;
}

void PlayerAttrCfg::getPlayerAttrInfo(int nLev, AttrMap &attrInfo)
{
	const PlayerAttr* playerAttr = findPlayerAttr(nLev);
	if (playerAttr)
	{
		attrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPM] = static_cast<float>(playerAttr->n_HPM);
		attrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MPM] = static_cast<float>(playerAttr->n_MPM);
		attrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_AD] = static_cast<float>(playerAttr->n_AD);
		attrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_AP] = static_cast<float>(playerAttr->n_AP);
		attrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ARM] = static_cast<float>(playerAttr->n_ARM);
		attrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MARM] = static_cast<float>(playerAttr->n_MARM);
		attrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPR] = static_cast<float>(playerAttr->n_HPR);
		attrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MPR] = static_cast<float>(playerAttr->n_MPR);
	}
}

bool PlayerAttrCfg::parsePlayerAttrConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "n_AR")
		{
			NS::XmlUtil::getAttribute(n_AR, pAttrXml, "Value");
		}
		else if (strKey == "n_Breath") {
			NS::XmlUtil::getAttribute(n_Breath, pAttrXml, "Value");
		}
		else if (strKey == "f_PlayerHeight") {
			NS::XmlUtil::getAttribute(f_PlayerHeight, pAttrXml, "Value");
		}
		else if (strKey == "f_PlayerWidth") {
			NS::XmlUtil::getAttribute(f_PlayerWidth, pAttrXml, "Value");
		}
		else if (strKey == "f_AtkRange") {
			NS::XmlUtil::getAttribute(f_AtkRange, pAttrXml, "Value");
		}
		else if (strKey == "f_AtkSpeed") {
			NS::XmlUtil::getAttribute(f_AtkSpeed, pAttrXml, "Value");
		}
		else if (strKey == "n_MR_Fire") {
			NS::XmlUtil::getAttribute(n_MR_Fire, pAttrXml, "Value");
		}
		else if (strKey == "n_MR_Forst") {
			NS::XmlUtil::getAttribute(n_MR_Forst, pAttrXml, "Value");
		}
		else if (strKey == "n_MR_Thunder") {
			NS::XmlUtil::getAttribute(n_MR_Thunder, pAttrXml, "Value");
		}
		else if (strKey == "n_MR_Earth") {
			NS::XmlUtil::getAttribute(n_MR_Earth, pAttrXml, "Value");
		}
		else if (strKey == "n_MR_Poison") {
			NS::XmlUtil::getAttribute(n_MR_Poison, pAttrXml, "Value");
		}
		else if (strKey == "n_MR_Light") {
			NS::XmlUtil::getAttribute(n_MR_Light, pAttrXml, "Value");
		}
		else if (strKey == "n_MR_Dark") {
			NS::XmlUtil::getAttribute(n_MR_Dark, pAttrXml, "Value");
		}
		else if (strKey == "f_RES") {
			NS::XmlUtil::getAttribute(f_RES, pAttrXml, "Value");
		}
		else if (strKey == "f_MRES") {
			NS::XmlUtil::getAttribute(f_MRES, pAttrXml, "Value");
		}
		else if (strKey == "f_CRI") {
			NS::XmlUtil::getAttribute(f_CRI, pAttrXml, "Value");
		}
		else if (strKey == "f_MCRI") {
			NS::XmlUtil::getAttribute(f_MCRI, pAttrXml, "Value");
		}
		else if (strKey == "f_ARP") {
			NS::XmlUtil::getAttribute(f_ARP, pAttrXml, "Value");
		}
		else if (strKey == "f_MRP") {
			NS::XmlUtil::getAttribute(f_MRP, pAttrXml, "Value");
		}
		else if (strKey == "f_CRID") {
			NS::XmlUtil::getAttribute(f_CRID, pAttrXml, "Value");
		}
		else if (strKey == "f_MCRID") {
			NS::XmlUtil::getAttribute(f_MCRID, pAttrXml, "Value");
		}
		else if (strKey == "n_CC_Hit") {
			NS::XmlUtil::getAttribute(n_CC_Hit, pAttrXml, "Value");
		}
		else if (strKey == "n_CC_Res") {
			NS::XmlUtil::getAttribute(n_CC_Res, pAttrXml, "Value");
		}
		else if (strKey == "f_Move") {
			NS::XmlUtil::getAttribute(f_Move, pAttrXml, "Value");
		}
		else if (strKey == "f_Run") {
			NS::XmlUtil::getAttribute(f_Run, pAttrXml, "Value");
		}
		else if (strKey == "n_DeadCoins") {
			NS::XmlUtil::getAttribute(n_DeadCoins, pAttrXml, "Value");
		}
		else if (strKey == "n_DeadExp") {
			NS::XmlUtil::getAttribute(n_DeadExp, pAttrXml, "Value");
		}
		else if (strKey == "n_DeadDurability") {
			NS::XmlUtil::getAttribute(n_DeadDurability, pAttrXml, "Value");
		}
		else if (strKey == "n_Jump") {
			NS::XmlUtil::getAttribute(n_Jump, pAttrXml, "Value");
		}
		else if (strKey == "f_Swim") {
			NS::XmlUtil::getAttribute(f_Swim, pAttrXml, "Value");
		}
		else if (strKey == "f_ShortJumpTime") {
			NS::XmlUtil::getAttribute(f_ShortJumpTime, pAttrXml, "Value");
		}
		else if (strKey == "f_LongJumpTime") {
			NS::XmlUtil::getAttribute(f_LongJumpTime, pAttrXml, "Value");
		}
		else if (strKey == "f_InitialVerticalVelocity") {
			NS::XmlUtil::getAttribute(f_InitialVerticalVelocity, pAttrXml, "Value");
		}
		else if (strKey == "f_AccelerationUp") {
			NS::XmlUtil::getAttribute(f_AccelerationUp, pAttrXml, "Value");
		}
		else if (strKey == "n_Dig") {
			NS::XmlUtil::getAttribute(n_Dig, pAttrXml, "Value");
		}
		else if (strKey == "n_Log") {
			NS::XmlUtil::getAttribute(n_Log, pAttrXml, "Value");
		}
		else if (strKey == "n_Fish") {
			NS::XmlUtil::getAttribute(n_Fish, pAttrXml, "Value");
		}
		else if (strKey == "n_Catch") {
			NS::XmlUtil::getAttribute(n_Catch, pAttrXml, "Value");
		}
		else if (strKey == "f_HpUICheck") {
			NS::XmlUtil::getAttribute(f_HpUICheck, pAttrXml, "Value");
		}
		else if (strKey == "n_fog_view") {
			NS::XmlUtil::getAttribute(n_fog_view, pAttrXml, "Value");
		}
		else if (strKey == "n_HPRS") {
			NS::XmlUtil::getAttribute(n_HPRS, pAttrXml, "Value");
		}
		else if (strKey == "n_MPRS") {
			NS::XmlUtil::getAttribute(n_MPRS, pAttrXml, "Value");
		}
		else if (strKey == "n_ResourceAtkDmg") {
			NS::XmlUtil::getAttribute(n_ResourceAtkDmg, pAttrXml, "Value");
		}
		else if (strKey == "n_fog_empty_view") {
			NS::XmlUtil::getAttribute(n_fog_empty_view, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");

	}

	m_playerAttrs.clear();
	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		PlayerAttr playerAttr{};

		NS::XmlUtil::getAttribute(playerAttr.n_ID, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(playerAttr.n_exp, xmlItem, "n_exp");// = proto.n_exp();
		NS::XmlUtil::getAttribute(playerAttr.n_HPM, xmlItem, "n_HPM");// = proto.n_hpm();
		NS::XmlUtil::getAttribute(playerAttr.n_HPR, xmlItem, "n_HPR");// = proto.n_hpr();
		NS::XmlUtil::getAttribute(playerAttr.n_MPM, xmlItem, "n_MPM");// = proto.n_mpm();
		NS::XmlUtil::getAttribute(playerAttr.n_MPR, xmlItem, "n_MPR");// = proto.n_mpr();
		NS::XmlUtil::getAttribute(playerAttr.n_AD, xmlItem, "n_AD");// = proto.n_ad();
		NS::XmlUtil::getAttribute(playerAttr.n_AP, xmlItem, "n_AP");// = proto.n_ap();
		NS::XmlUtil::getAttribute(playerAttr.n_ARM, xmlItem, "n_ARM");// = proto.n_arm();
		NS::XmlUtil::getAttribute(playerAttr.n_MARM, xmlItem, "n_MARM");// = proto.n_marm();
		NS::XmlUtil::getAttribute(playerAttr.n_MARM, xmlItem, "n_MARM");// = proto.n_marm();
		NS::XmlUtil::getAttribute(playerAttr.n_Speed, xmlItem, "n_Speed");
		NS::XmlUtil::getAttribute(playerAttr.n_MaxHuman, xmlItem, "n_MaxHuman");

		if (playerAttr.n_ID > m_maxLevel)
		{
			m_maxLevel = playerAttr.n_ID;
		}
		m_playerAttrs[playerAttr.n_ID] = playerAttr;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}


	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPM] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MPM] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_AD] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_AP] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ARM] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MARM] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_AR] = static_cast<float>(n_AR);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_FIRE] = static_cast<float>(n_MR_Fire);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_FORST] = static_cast<float>(n_MR_Forst);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_THUNDER] = static_cast<float>(n_MR_Thunder);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_EARTH] = static_cast<float>(n_MR_Earth);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_POSION] = static_cast<float>(n_MR_Poison);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_LIGHT] = static_cast<float>(n_MR_Light);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_DARK] = static_cast<float>(n_MR_Dark);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ARP] = 0.0f;	 
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MRP] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CRI] = f_CRI;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MCRI] = f_MCRI;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CRID] = f_CRID;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MCRID] = f_MCRID;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_RES] = f_RES;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MRES] = f_MRES;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CC_HIT] = static_cast<float>(n_CC_Hit); 
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CC_RES] = static_cast<float>(n_CC_Res);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ATK_SP] = f_AtkSpeed;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MV_SP] = f_Move / 100.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HB] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPR] = 0.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPRS] = (float)n_HPRS;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MPR] = f_MRP;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MPRS] = (float)n_MPRS;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_GATHER_ATK] = (float)n_ResourceAtkDmg;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_JUMP] = static_cast<float>(n_Jump);
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_SWIM] = f_Swim;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ATK_RANGE] = f_AtkRange;

	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CHASE] = f_Run / 100.0f;
	m_mapAttrInfo[MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_JUMP_INIT_SPEED] = f_InitialVerticalVelocity;

	return true;
}



bool PlayerAttrCfg::parseAttachAttCfg(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	tinyxml2::XMLElement* pXmlItem_Table = pXmlRoot->FirstChildElement("Table");

	while (pXmlItem_Table)
	{
		bzbee::tstring tableName = bzbee::EMPTY_STRING;
		bzbee::XmlUtil::getAttribute(tableName, pXmlItem_Table, "Name");
		if (tableName == "AttachAttrName")
		{
			for (tinyxml2::XMLElement* xmlItem = pXmlItem_Table->FirstChildElement("Record"); xmlItem != nullptr; xmlItem = xmlItem->NextSiblingElement("Record"))
			{
				AttachInfo item;
				bzbee::StringList vAttachId;
				bzbee::StringList vAttachNode;
				bzbee::StringList vPrvotNode;
				if (!bzbee::XmlUtil::getAttribute(item.m_attachPos, xmlItem, "attach_pos")) continue;
				if (!bzbee::XmlUtil::getAttribute(item.m_attachName, xmlItem, "attach_name")) continue;
				if (!bzbee::XmlUtil::getAttribute(vAttachId, xmlItem, "attach_id")) continue;
				if (!bzbee::XmlUtil::getAttribute(vAttachNode, xmlItem, "attach_node")) continue;
				bzbee::XmlUtil::getAttribute(item.m_vHideEquip, xmlItem, "hide_equip");
				bzbee::XmlUtil::getAttribute(item.m_vHideNode, xmlItem, "hide_node");
				bzbee::XmlUtil::getAttribute(item.m_vDisablePos, xmlItem, "disable_pos");
				bzbee::XmlUtil::getAttribute(item.m_weight, xmlItem, "weight");
				bzbee::XmlUtil::getAttribute(vPrvotNode, xmlItem, "pivot_node");
				bzbee::XmlUtil::getAttribute(item.m_animPath, xmlItem, "anim_path");

				item.m_mainAttachId = vAttachId.size() ? vAttachId[0] : bzbee::EMPTY_STRING;
				item.m_subAttachId = vAttachId.size() > 1 ? vAttachId[1] : bzbee::EMPTY_STRING;
				item.m_mainAttachNode = vAttachNode.size() ? vAttachNode[0] : bzbee::EMPTY_STRING;
				item.m_subAttachNode = vAttachNode.size() > 1 ? vAttachNode[1] : bzbee::EMPTY_STRING;
				item.m_mainPivotNode = vPrvotNode.size() ? vPrvotNode[0] : bzbee::EMPTY_STRING;
				item.m_subPivotNode = vPrvotNode.size() > 1 ? vPrvotNode[1] : bzbee::EMPTY_STRING;
				item.m_attachName_untrans = item.m_attachName;
				item.m_attachName = _LC(item.m_attachName);
				if (bzbee::XmlUtil::getAttribute(item.m_vRotation, xmlItem, "attach_rotation"))
				{
					item.m_vRotation.x = glm::radians(item.m_vRotation.x);
					item.m_vRotation.y = glm::radians(item.m_vRotation.y);
					item.m_vRotation.z = glm::radians(item.m_vRotation.z);
				}
				else
				{
					item.m_vRotation.x = 0.0f;
					item.m_vRotation.y = glm::half_pi<float>();
					item.m_vRotation.z = 0.0f;
				}
				m_attachInfoMap.push_back(item);
			}
			//			std::sort(m_attachInfoMap.begin(), m_attachInfoMap.end(), std::less<AttachInfo>());
		}
		else if (tableName == "NodeAttrName")
		{
			for (tinyxml2::XMLElement* xmlItem = pXmlItem_Table->FirstChildElement("Record"); xmlItem != nullptr; xmlItem = xmlItem->NextSiblingElement("Record"))
			{
				std::shared_ptr<NodeInfo> item = std::make_shared<NodeInfo>();

				if (!bzbee::XmlUtil::getAttribute(item->m_nodePos, xmlItem, "node_pos")) continue;
				if (!bzbee::XmlUtil::getAttribute(item->m_nodeName, xmlItem, "node_name")) continue;
				if (!bzbee::XmlUtil::getAttribute(item->m_male_nodeId, xmlItem, "node_id_male")) continue;
				if (!bzbee::XmlUtil::getAttribute(item->m_female_nodeId, xmlItem, "node_id_female")) continue;
				item->m_nodeName_untrans = item->m_nodeName;
				item->m_nodeName = _LC(item->m_nodeName);
				m_nodeInfoMap.emplace(item->m_nodePos, item);
			}
		}
		else if (tableName == "Skin")
		{
			for (tinyxml2::XMLElement* xmlItem = pXmlItem_Table->FirstChildElement("Record"); xmlItem != nullptr; xmlItem = xmlItem->NextSiblingElement("Record"))
			{
				bzbee::XmlUtil::getAttribute(m_nMaxSkinId, xmlItem, "skin_id");
				continue;
			}
		}
		pXmlItem_Table = pXmlItem_Table->NextSiblingElement("Table");
	}

	return true;
}


const bzbee::tstring& PlayerAttrCfg::getAttachId(int nPos)
{
	for (const auto& it : m_attachInfoMap)
	{
		if (it.m_attachPos == nPos)
			return it.m_mainAttachId;
	}

	return bzbee::EMPTY_STRING;
}



const bzbee::tstring& PlayerAttrCfg::getSubAttachId(int nPos)
{
	for (const auto& it : m_attachInfoMap)
	{
		if (it.m_attachPos == nPos)
			return it.m_subAttachId;
	}

	return bzbee::EMPTY_STRING;
}

bzbee::StringList PlayerAttrCfg::getAttachIds(const bzbee::IntegerList& ids, bool unique /* = false */)
{
	bzbee::StringList vStr;
	for (const auto& it : ids)
	{
		bzbee::tstring id = getAttachId(it);
		if (id.empty()) continue;
		if (!unique || std::find(vStr.begin(), vStr.end(), id) == vStr.end())
			vStr.push_back(id);
	}

	for (const auto& it : ids)
	{
		bzbee::tstring id = getSubAttachId(it);
		if (id.empty()) continue;
		if (!unique || std::find(vStr.begin(), vStr.end(), id) == vStr.end())
			vStr.push_back(id);
	}

	return vStr;
}

//************************************
// FullName:  bzbee::ModelInstance::getAttachPos
// Access:    public static 
// Returns:   int
// Parameter: const tstring & attachId
// Desc:	  获取指定绑定ID的绑点nPos
//************************************
int PlayerAttrCfg::getAttachPos(const bzbee::tstring& attachId)
{
	for (const auto& it : m_attachInfoMap)
	{
		if (it.m_mainAttachId == attachId)
			return it.m_attachPos;
	}
	return 0;
}

const PlayerAttrCfg::AttachInfo* PlayerAttrCfg::getAttach(int nPos)
{
	for (const auto& it : m_attachInfoMap)
	{
		if (it.m_attachPos == nPos)
			return &it;
	}
	return nullptr;
}

const PlayerAttrCfg::AttachInfo* PlayerAttrCfg::getAttachById(const bzbee::tstring& attachId)
{
	for (const auto& it : m_attachInfoMap)
	{
		if (!attachId.empty() && (it.m_mainAttachId == attachId || it.m_subAttachId == attachId))
			return &it;
	}
	return nullptr;
}

const bzbee::tstring& PlayerAttrCfg::getNodeId(int nPos, int gender)
{
	NodeInfoMap::const_iterator it = m_nodeInfoMap.find(nPos);
	return it == m_nodeInfoMap.end() ? bzbee::EMPTY_STRING : gender == 0 ? it->second->m_male_nodeId : it->second->m_female_nodeId;
}

bzbee::StringList PlayerAttrCfg::getNodeIds(const bzbee::IntegerList& ids, const int gender, bool unique /* = false */)
{
	bzbee::StringList vStr;
	for (const auto& it : ids)
	{
		bzbee::tstring id = getNodeId(it, gender);
		if (id.empty()) continue;
		if (!unique || std::find(vStr.begin(), vStr.end(), id) == vStr.end())
			vStr.push_back(id);
	}
	return vStr;
}

int PlayerAttrCfg::getNodePos(const bzbee::tstring& strNodeId)
{
	for (const auto& item : m_nodeInfoMap)
	{
		if (item.second->m_male_nodeId == strNodeId || item.second->m_female_nodeId == strNodeId)
			return item.first;
	}
	return 0;
}


bool PlayerAttrCfg::reloadMultiLanguage()
{
	return true;
}
