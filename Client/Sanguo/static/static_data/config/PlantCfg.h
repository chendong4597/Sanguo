/*!
 * \file PlantCfg.h
 *
 * \author laizhisheng
 * \date 2016/12/20 18:03
 *
 */

#pragma once

#include "BaseTypeDefine.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <map>
#include <tuple>
#include "tinyxml2.h"

class PlantCfg
{
public:
	enum GrowType
	{
		TREE = 1,
		ANIMAL = 2,
		MINE = 3,
	};

	struct AnimalCfg
	{
		bzbee::uint n_ID;
		std::string s_AnimFoder;
		std::string s_AnimPath;
		std::string s_SoundEff;
		std::string s_SoundEff1;
	};
	typedef std::unordered_map<bzbee::uint/*typeId*/, AnimalCfg> AnimalMap;

	struct PlantType
	{
		bzbee::uint m_typeId;
		bzbee::tstring s_name;
		bzbee::tstring s_name_untrans;
		bzbee::tstring s_note;
		bzbee::tstring s_note_untrans;
		int n_rate_level{};
		int m_growType{};
		int n_exp{};
		int n_Prosperous{}; //增加繁荣度
	};
	typedef std::unordered_map<bzbee::uint/*typeId*/, PlantType> PlantTypeMap;

	

	struct PlantTemplate
	{
		struct animals {
			int nId;
			int nNum;
		};

		bzbee::uint n_plant_type{};
		int m_stage{};
		int n_dropid{};
		int n_next_stage_time{};
		NS::tstring s_Foder{};
		NS::tstring s_Path{};
		std::vector<animals> vecAnimals;
	};
	typedef std::unordered_map<bzbee::uint, PlantTemplate> PlantTemplateMap;

	struct PlantPos
	{
		bzbee::uint n_ID;
		bzbee::uint n_flat;  //肥沃度
		bzbee::uint n_Lv; //开放等级
	};
	typedef std::unordered_map<bzbee::uint/*typeId*/, PlantPos> PlantPosMap;

public:
	static PlantCfg& getInstance();

	bool initializeCfg();
	void terminate();

	const PlantType* findPlantType(bzbee::uint id) const;
	const PlantTemplate* findPlantTemplate(bzbee::uint typeId, int stage);
	const PlantPos* findPlantPos(int nIdx);
	const AnimalCfg* findAnimalTemplate(bzbee::uint id);
	int findGrowMaxPlantTemplate(bzbee::uint templateId);
	int GetDropId(bzbee::uint templateId);

	const PlantTypeMap& getPlantType() { return m_plantTypeMap; }
	const PlantTemplateMap& getPlantTemplateMap() { return m_plantTemplateMap; }

	bool reloadMultiLanguage();

	std::string GetBpPath(int nId, int nStage);
	std::string GetBpAnimal(int nId);
	std::string GetBpMontagePath(int nId, int nIdx);

private:
	bool parsePlantConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parsePlantTempConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parsePlantPosConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseAnimalConfig(tinyxml2::XMLElement* pXmlRoot);
private:
	PlantTypeMap m_plantTypeMap;
	PlantPosMap m_plantPosMap;
	PlantTemplateMap m_plantTemplateMap;
	AnimalMap   m_animalMap;

	std::string m_strTreePath;
	std::string m_strAnimalPath;
	std::string m_strMinePath;
};
