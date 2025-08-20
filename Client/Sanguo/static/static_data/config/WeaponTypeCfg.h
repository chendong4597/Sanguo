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
#include <string>
#include "tinyxml2.h"


 // 武器类型

// 1:剑 2 : 双手剑 3 : 刀 4 : 斧子 5 : 大刀 6 : 爪子 7 : 回旋镖 8 : 法杖 9 : 法球 10 : 法书 11 : 枪 12 : 大炮 13 : 弓 
// 14 : 大锤子 15 : 炸弹 16 : 古代枪 17 : 盾牌 18 : 箭匣 19 : 马
enum WEAPON_USE_TYPE
{
	WEAPON_USE_TYPE_NONE = 0,		// 
	WEAPON_USE_TYPE_SWORD,		// 剑 
	WEAPON_USE_TYPE_WEINSWORD,		// 双手剑 
	WEAPON_USE_TYPE_KNIFE,		// 刀
	WEAPON_USE_TYPE_AXE,		// 斧子
	WEAPON_USE_TYPE_BLADE,		// 大刀
	WEAPON_USE_TYPE_CLAW,		// 爪子
	WEAPON_USE_TYPE_DARTS,		// 回旋镖
	WEAPON_USE_TYPE_STAFF,		// 法杖
	WEAPON_USE_TYPE_BALL,		// 法球
	WEAPON_USE_TYPE_BOOK,		// 法书
	WEAPON_USE_TYPE_GUN,		// 枪 十字弓
	WEAPON_USE_TYPE_CANNON,		// 大炮
	WEAPON_USE_TYPE_BOW,		// 弓
	WEAPON_USE_TYPE_HAMMER,		// 大锤子
	WEAPON_USE_TYPE_BOMB,		// 炸弹
	WEAPON_USE_TYPE_SPEAR,		// 古代枪
	WEAPON_USE_TYPE_SHIELD,		// 盾牌
	WEAPON_USE_TYPE_QUIVER,		// 箭匣
	WEAPON_USE_TYPE_HORSE,		// 马
};

class WeaponTypeCfg
{
	struct WeaponTypeCfgInfo
	{
		int n_id{};
		int n_Type{};
		int n_Flip{};
		std::string s_SocketName1;
		std::string s_SocketName2;
		std::string s_DemoPath;
		std::string s_desc{};
		int n_Combo;
		int n_HitHurt;
		std::vector<std::string> vecAniMotoPaths;
		std::vector<std::string> vecAniSoundPaths;
		std::string s_Foder;
		std::string s_AniS1;
		std::string s_AniE1;
		std::string s_AniS2;
		std::string s_AniE2;
		std::string s_Death;
		std::string s_Hit;
	};

	typedef std::map<int, WeaponTypeCfgInfo> MAP_WEAPONTYPE;

public:

	WeaponTypeCfg() {}
	static WeaponTypeCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const WeaponTypeCfgInfo* GetWeaponType(int id);

	std::string GetProfessPath(int nProfessIdx);

	std::string GetAniPath(int nPrefessIdx , int weaponTpy, int comboIdx);
	std::string GetSoundPath(int nPrefessIdx, int weaponTpy, int comboIdx);
	std::string GetContinueAniPath(int nPrefessIdx, int weaponTpy, int nIdx);
	std::string GetDeathPath(int nPrefessIdx, int weaponTpy);

private:
	MAP_WEAPONTYPE m_mapWeaponType;
	std::string m_strWarriorPath;
	std::string m_strMushaPath;
	std::string m_strMagePath;
	std::string m_strBravoPath;
};
