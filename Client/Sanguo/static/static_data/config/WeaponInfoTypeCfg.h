/*!
 * \file GlobalCfg.h
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#pragma once
#include "WeaponTypeCfg.h"
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"


class WeaponInfoTypeCfg
{
public:
	struct WeaponInfoTypeCfgInfo
	{
		int n_id{};
		int n_Type{};
		int n_isCombo{};
		int n_Flip{};
		int n_PreSkillId{};//前置技能ID(时间比较靠前)
		int n_SkillId{};
		int n_ComboSkill{};//最后一个连招产生技能
		int n_exSkillId{};//状态技能ID
		int n_HurtRangeTpy{};// 攻击范围类型 攻击范围类型 0：无 1：圆形  2：扇形  3：矩形
		int n_RangeParam1{};  //小距离
		int n_RangeParam2{};  //大距离
		int n_RangeAngle{};	//扇形角度
		int n_Speed{};		//基础速度，坐骑才有
		int n_runStrenth{};		//跑动时减少体力消耗（主要是坐骑）
		int n_restStrenth{};		//休息时增加体力回复
		int n_normalStrenth{};		//普通攻击消耗体力
		std::string s_Path;
		std::string s_MainPath;
		std::string s_atkPath;
		std::string s_RunSound;
		std::string s_IdleSound;
		std::string s_Desc;
		std::vector<std::string> m_vecTextures;
	};

	typedef std::map<int, WeaponInfoTypeCfgInfo> MAP_WEAPONINFOTYPE;


public:

	WeaponInfoTypeCfg() {}
	static WeaponInfoTypeCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const WeaponInfoTypeCfgInfo* GetWeaponById(int id);
	int GetNextWeaponId(int id);
	std::string GetWeaponTexture(int nId);
	std::string GetBPWeaponPath(int id);
	std::string GetSkeshWeaponPath(int id);
	std::string GetAnimHorse(int nId);
	std::string GetAnimSound(int nId, bool bRun);
	std::string GetTexture(int nWeaponId, int nIdx);

	std::string GetAttackAniPath(int nWeaponId);
private:
	MAP_WEAPONINFOTYPE m_mapWeaponInfoType;
	std::string m_strPath;
	std::string m_strSoundBase;
};
