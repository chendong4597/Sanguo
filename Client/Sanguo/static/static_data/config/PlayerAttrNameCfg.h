/*!
 * \file PlayerAttrNameCfg.h
 *
 * \author jiangping huang
 * \date 03/02/2017
 *
 * 
 */
#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include "tinyxml2.h"

class PlayerAttrNameCfg
{
public:
	class PlayerAttrNameType
	{
		typedef std::vector<std::string> ParamVector;
	public:
		int n_Type{};
		int n_Class;
		std::string m_Name;
		std::string m_Name_untrans;
		std::string m_Pic;
		std::string m_PicAchieve;
	};

	class AtterPower
	{
	public:
		int id{};
		int value{};
		int power{};
	};

	typedef std::unordered_map<int, PlayerAttrNameType> PlayerAttrNameMap;
	typedef std::unordered_map<int, AtterPower> AtterPowerMap;

public:
	static PlayerAttrNameCfg& getInstance();

	bool initialize();
	void terminate();

	const PlayerAttrNameType* findPlayerAttrNameType(int id) const;
	const PlayerAttrNameMap& getPlayerAttrNameMap() const { return m_PlayerAttrNameMap; };
	const PlayerAttrNameCfg::AtterPower* getPowerCfg(int id);

	bool reloadMultiLanguage();

private:
	bool parsePlayerAttrNameConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parsePowerCalAttrConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	PlayerAttrNameMap m_PlayerAttrNameMap;
	AtterPowerMap m_atterPowerMap{};

};
