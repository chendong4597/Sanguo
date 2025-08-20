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
#include <vector>
#include <string>
#include <unordered_map>
#include "tinyxml2.h"


class NpcFuncDetail {
public:
	enum {
		NpcFunc_Ins = 1,  // 副本
		NpcFunc_WareHouse = 2,  //仓库
		NpcFunc_Weapon_Shop = 3,		//武器商店
		NpcFunc_Seed_Shop = 4,		//种子商店
		NpcFunc_Equip_Shop = 5,		//装备商店
		NpcFunc_Hero_Shop = 6,		//英雄商店
		NpcFunc_Media_Shop = 7,		//药品商店
		NpcFunc_Food_Shop = 8,		//食物商店
		NpcFunc_Ext_Shop = 9,		//杂货铺
		NpcFunc_Lottery = 10,		//祈福
		NpcFunc_NpcManger = 11,		//居民管理
		NpcFunc_NpcDoor = 12,		//开门官
		NpcFunc_NpcIllu = 13,		//图鉴 师爷
		NpcFunc_Fort = 14,			//军营
		NpcFunc_GenIns = 15,		//武将副本
	};

	int n_ID{};
	std::string s_Name{};
};

class FuncNpcType {
public:
	int n_ID{};
	std::string s_icon;
	std::string s_Name;
	std::string s_say;
	std::string s_HudIcon;
	std::vector<int> vecFunc;
};


class NpcTypeCfg
{
public:
	enum {
		NpcType_None = 0,
		NpcType_Func = 1,  //功能npc
		NpcType_Solider = 2,  //士兵
		NpcType_Genels = 3,  //将领
	};

	enum {
		NpcSubType_None = 0,
		NpcSubType_Infantry = 1,  //盾枪兵
		NpcSubType_Cavalry = 2,  //刀骑兵
		NpcSubType_Archer = 3,  //弓步兵
		NpcSubType_Blade = 4,  //大刀兵
		NpcSubType_Magic = 5,  //法术兵
		NpcSubType_MAX
	};

	enum {
		NpcFuncSubType_None = 0,  //居民
		NpcFuncSubType_Craft = 1,  //锻造
		NpcFuncSubType_Shop = 2,  //商店
		NpcFuncSubType_Plant = 3,  //种植 或 
		NpcFuncSubType_Farm = 4,  // 养殖
		NpcFuncSubType_Mine = 5,  //矿
		NpcFuncSubType_Worker = 6, //工匠等
		NpcFuncSubType_Trans = 7, //运输工
		NpcFuncSubType_Busi = 8, //商人
		NpcFuncSubType_Logging = 9, //伐木工
		NpcFuncSubType_MAX
	};

	struct GenelTypeInfo
	{
		int n_id{};
		std::string s_GenName;
		std::string s_HalfIcon;
		std::string s_BP_Foder;
		std::string s_BP_Path;
		std::string s_hit_path;
		std::string s_dead_path;
		std::vector<std::string> vecAniMotoPaths;
		std::vector<std::string> vecTxtures;
	};

	struct NpcTypeCfgInfo
	{
		int n_id{};
		int n_Type{};
		int n_SubType{};
		int n_Weapon1{};
		int n_Weapon2{};
		int n_Quality{ 0 };
		std::string s_icon;
		std::string s_HudIcon;
		std::string s_halfHead;
		std::string s_BP_Foder;
		std::string s_BP_Path;
		std::string s_name;
		std::string s_TpyName;
		std::string s_hit_path;
		std::string s_dead_path;
		std::string s_desc{};
		std::vector<std::string> vecAniMotoPaths;
		std::vector<int> m_vecUseTpys;
		std::vector<std::string> vecSays;
	};

	typedef std::map<int, NpcTypeCfgInfo> MAP_NPCTYPE;
	typedef std::map<int, GenelTypeInfo> MAP_GENELTYPE;
	typedef std::map<int, FuncNpcType> MAP_NPCFUNCTPY;
	typedef std::map<int, NpcFuncDetail> MAP_NPCFUNCDETAIL;

public:

	NpcTypeCfg() {}
	static NpcTypeCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	//npc
	const NpcTypeCfgInfo* GetNpcType(int id);
	std::string GetBpNpcPath(int nId);
	std::string GetAttackPath(int nId, int nTempId, bool bSecAni);
	std::string GetRandSay(int nId);
	const MAP_NPCTYPE& GetMapNpcs() { return m_mapNpcType; }

	//武将
	bool parseGenelConfig(tinyxml2::XMLElement* pXmlRoot);
	const GenelTypeInfo* GetGenelType(int id);
	std::string GetBpGenelPath(int nId);
	std::string GetBpHeadPath(int nId);
	std::string GetBpHalfHeadPath(int nId);
	std::string GetBpMatTexture(int nId, std::string strTxt);
	std::string GetBpGenProText(int nId);

	//武将外形
	std::string GetBpGenHalfHeadPath(int nId);

	//功能类型
	bool parseNpcFuncDetailConfig(tinyxml2::XMLElement* pXmlRoot);
	const NpcFuncDetail* GetNpcFuncDetail(int id);

	//普通功能NPC
	bool parseNpcFuncConfig(tinyxml2::XMLElement* pXmlRoot);
	const FuncNpcType* GetNpcFunc(int id);
	std::string GetFuncHeadPath(int nId);
private:
	MAP_NPCTYPE m_mapNpcType;
	std::string m_strPath;

	MAP_GENELTYPE m_mapGenelType;
	std::string m_strGenelPath;

	MAP_NPCFUNCDETAIL m_mapFuncDetail;
	MAP_NPCFUNCTPY	m_mapNpcFunc;
};
