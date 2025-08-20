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


class WeaponAniEffectCfg
{
	struct WeaponAniEffectInfo
	{
		int n_ID{};
		int n_WeaponTpy{};
		int n_ComboIdx{};
		int n_AttachTpy{};
		std::string s_SockName;
		std::string s_AssertPath;
		float f_DisplayTm{0.f};
		std::string s_HitAssertPath;
	};

	typedef std::map<int, WeaponAniEffectInfo> MAP_WEAPONANIEFFECT;


public:

	WeaponAniEffectCfg() {}
	static WeaponAniEffectCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const WeaponAniEffectInfo* GetWeaponAniEffect(int weaponTpy , int comboIdx);

private:
	MAP_WEAPONANIEFFECT m_mapWeaponAniEffect;
	
};
