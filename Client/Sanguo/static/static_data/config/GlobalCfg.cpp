/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "GlobalCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"
#include "../common/Heartbeat.h"




GlobalCfg& GlobalCfg::getInstance()
{
	static GlobalCfg s_GlobalCfg;
	return s_GlobalCfg;
}

bool GlobalCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/global_config.bin")) return false;

	return parseConfig(doc.RootElement());
}

void GlobalCfg::terminate()
{
}

bool GlobalCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto xmlSight = pXmlRoot->FirstChildElement("Sight");
	if (xmlSight)
	{
		NS::XmlUtil::getAttribute(m_stuInfoSight.nMin , xmlSight , "min");
		NS::XmlUtil::getAttribute(m_stuInfoSight.nMax , xmlSight , "max");
	}

	auto xmlTalkSight = pXmlRoot->FirstChildElement("NpcSign");
	if (xmlTalkSight)
	{
		NS::XmlUtil::getAttribute(m_stuTalkSight.nArmLen, xmlTalkSight, "armLen");
		NS::XmlUtil::getAttribute(m_stuTalkSight.nPitch, xmlTalkSight, "Pitch");
	}

	auto xmlSelfSight = pXmlRoot->FirstChildElement("SelfSign");
	if (xmlSelfSight)
	{
		NS::XmlUtil::getAttribute(m_stuSelfSight.nArmLen, xmlSelfSight, "armLen");
		NS::XmlUtil::getAttribute(m_stuSelfSight.nRoll, xmlSelfSight, "Roll");
		NS::XmlUtil::getAttribute(m_stuSelfSight.nPitch, xmlSelfSight, "Pitch");
		NS::XmlUtil::getAttribute(m_stuSelfSight.nYaw, xmlSelfSight, "Yaw");
	}

	auto xmlCullDis = pXmlRoot->FirstChildElement("CullDis");
	if (xmlCullDis)
	{
		int nCount = 0;
		NS::XmlUtil::getAttribute(nCount , xmlCullDis, "count");
		for (int i = 0 ; i < nCount ; i++)
		{
			CullDistance info;
			std::string strSize = fmt::format("size{0}", i + 1);
			std::string strDis = fmt::format("distance{0}", i + 1);
			NS::XmlUtil::getAttribute(info.nSize, xmlCullDis, strSize.c_str());
			NS::XmlUtil::getAttribute(info.nDistance, xmlCullDis, strDis.c_str());
			m_vecCullDistance.push_back(info);
		}
	}

	auto xmlBaseCfg = pXmlRoot->FirstChildElement("BaseCfg");
	if (xmlBaseCfg)
	{
		int nServerTpy = 0;
		NS::XmlUtil::getAttribute(nServerTpy, xmlBaseCfg, "IsServer");
		m_bServer = nServerTpy == 1;
	}

	auto xmlCfg = pXmlRoot->FirstChildElement("IpConfig");
	if (xmlCfg)
	{
		NS::XmlUtil::getAttribute(strLoginIp, xmlCfg, "IP");
		NS::XmlUtil::getAttribute(nLoginPort, xmlCfg, "Port");
	}

	xmlCfg = pXmlRoot->FirstChildElement("SightLeft");
	if (xmlCfg)
	{
		TouchSightInfo info;
		NS::XmlUtil::getAttribute(info.nArmLen, xmlCfg, "armLen");
		NS::XmlUtil::getAttribute(info.nRoll, xmlCfg, "Roll");
		NS::XmlUtil::getAttribute(info.nPitch, xmlCfg, "Pitch");
		NS::XmlUtil::getAttribute(info.nYaw, xmlCfg, "Yaw");
		m_mapSightInfo[0] = info;
	}

	xmlCfg = pXmlRoot->FirstChildElement("SightRight");
	if (xmlCfg)
	{
		TouchSightInfo info;
		NS::XmlUtil::getAttribute(info.nArmLen, xmlCfg, "armLen");
		NS::XmlUtil::getAttribute(info.nRoll, xmlCfg, "Roll");
		NS::XmlUtil::getAttribute(info.nPitch, xmlCfg, "Pitch");
		NS::XmlUtil::getAttribute(info.nYaw, xmlCfg, "Yaw");
		m_mapSightInfo[1] = info;
	}

	xmlCfg = pXmlRoot->FirstChildElement("SightBack");
	if (xmlCfg)
	{
		TouchSightInfo info;
		NS::XmlUtil::getAttribute(info.nArmLen, xmlCfg, "armLen");
		NS::XmlUtil::getAttribute(info.nRoll, xmlCfg, "Roll");
		NS::XmlUtil::getAttribute(info.nPitch, xmlCfg, "Pitch");
		NS::XmlUtil::getAttribute(info.nYaw, xmlCfg, "Yaw");
		m_mapSightInfo[2] = info;
	}

	xmlCfg = pXmlRoot->FirstChildElement("SightBefore");
	if (xmlCfg)
	{
		TouchSightInfo info;
		NS::XmlUtil::getAttribute(info.nArmLen, xmlCfg, "armLen");
		NS::XmlUtil::getAttribute(info.nRoll, xmlCfg, "Roll");
		NS::XmlUtil::getAttribute(info.nPitch, xmlCfg, "Pitch");
		NS::XmlUtil::getAttribute(info.nYaw, xmlCfg, "Yaw");
		m_mapSightInfo[3] = info;
	}

	xmlCfg = pXmlRoot->FirstChildElement("SightAll");
	if (xmlCfg)
	{
		TouchSightInfo info;
		NS::XmlUtil::getAttribute(info.nArmLen, xmlCfg, "armLen");
		NS::XmlUtil::getAttribute(info.nRoll, xmlCfg, "Roll");
		NS::XmlUtil::getAttribute(info.nPitch, xmlCfg, "Pitch");
		NS::XmlUtil::getAttribute(info.nYaw, xmlCfg, "Yaw");
		m_mapSightInfo[4] = info;
	}

	xmlCfg = pXmlRoot->FirstChildElement("LoggingGrow");
	if (xmlCfg)
	{
		NS::XmlUtil::getAttribute(m_LoggingInfo.nMinTm, xmlCfg, "MinTm");
		NS::XmlUtil::getAttribute(m_LoggingInfo.nMaxTm, xmlCfg, "MaxTm");
	}

	xmlCfg = pXmlRoot->FirstChildElement("InitSpeed");
	if (xmlCfg)
	{
		NS::XmlUtil::getAttribute(m_nInitSpeed, xmlCfg, "PlayerSpeed");
	}
	
	xmlCfg = pXmlRoot->FirstChildElement("StrenthSyn");
	if (xmlCfg)
	{
		NS::XmlUtil::getAttribute(m_nStrenthDec, xmlCfg, "Dec");
		NS::XmlUtil::getAttribute(m_nStrenthInc, xmlCfg, "Inc");
	}

	xmlCfg = pXmlRoot->FirstChildElement("BreathSyn");
	if (xmlCfg)
	{
		NS::XmlUtil::getAttribute(m_nBreathInc, xmlCfg, "Inc");
	}
	
	xmlCfg = pXmlRoot->FirstChildElement("HeroRevive");
	if (xmlCfg) {
		NS::XmlUtil::getAttribute(m_fHeroReviveTm, xmlCfg, "time");
	}

	xmlCfg = pXmlRoot->FirstChildElement("HelpLimit");
	if (xmlCfg) {
		NS::XmlUtil::getAttribute(m_helpLvLimit, xmlCfg, "helplv");
	}

	return true;
}

const TouchSightInfo* GlobalCfg::GetSightInfo(int nTpy, bool bRight)
{
	if (bRight) {  //从右方向攻击，改下方向位
		if (nTpy == 0) {
			nTpy = 1;
		} else if (nTpy == 1) {
			nTpy = 0;
		} else if (nTpy == 2) {
			nTpy = 3;
		} else if (nTpy == 3) {
			nTpy = 2;
		}
	}
	if (m_mapSightInfo.find(nTpy) == m_mapSightInfo.end())
	{
		return nullptr;
	}
	return &(m_mapSightInfo[nTpy]);
}


