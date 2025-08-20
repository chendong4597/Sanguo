/*!
 * \file GlobalCfg.h
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"


enum EQUIP_SHOW_TPY {
	EQUIP_SHOW_NONE = 0,
	EQUIP_SHOW_HEAD,  //头部
	EQUIP_SHOW_BODY,  //身体
	EQUIP_SHOW_LEGS,  //脚
	EQUIP_SHOW_CAPE,  //披风
	EQUIP_SHOW_WRAP   //外套
};

class EquipTypeCfg
{
	struct EquipCfgInfo
	{
		int n_id{};
		int n_Type{};
		int n_Wrap{};
		std::string s_ManFoder;
		std::string s_WomanFoder;
		std::string s_ManHead;
		std::string s_ManBody;
		std::string s_ManLegs;
		std::string s_ManCape;
		std::string s_ManWrap;
		std::string s_WomanHead;
		std::string s_WomanBody;
		std::string s_WomanLegs;
		std::string s_WomanCape;
		std::string s_WomanWrap;
		int n_ShowHair{};
	};

	typedef std::map<int, EquipCfgInfo> MAP_EQUIPTYPE;

public:

	EquipTypeCfg() {}
	static EquipTypeCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const EquipCfgInfo* GetEquipType(int id);
	std::string GetNonePath(int nSex, bool bBody);
	std::string GetNoneDifusePath(int nSex, bool bBody);
	std::string GetEquipPath(int nSex , int nId , EQUIP_SHOW_TPY nShowTpy);
	std::string GetDifusePath(int nSex, int nId, EQUIP_SHOW_TPY nShowTpy);
	std::string GetHightstPath(int nSex, int nId, EQUIP_SHOW_TPY nShowTpy);
private:
	MAP_EQUIPTYPE m_mapEquipType;
	std::string m_strManPath;
	std::string m_strWomanPath;
	std::string m_strManBodyPath;
	std::string m_strWomanBodyPath;
	std::string m_strManLegsPath;
	std::string m_strWomanLegsPath;

	std::string m_strDifuseManBodyPath;
	std::string m_strDifuseWomanBodyPath;
	std::string m_strDifuseManLegsPath;
	std::string m_strDifuseWomanLegsPath;
};
