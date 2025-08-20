/*!
* \file ItemCfg.h
*
* \author cd
* \date 12/01/2016
*
*
*/
#pragma once

//#include <baseutil.h>
#include <map>
#include <memory>
#include <unordered_map>
//#include "BaseType.h"
#include "tinyxml2.h"



class BuildingCfg
{
public:
	enum class Direction : int
	{
		NONE = 0,
		LEFT = 1,
		RIGHT = 2
	};

	struct BuildingTemplate
	{
		int m_templateId{};
		int m_typeId{};
		//Direction m_direction{ Direction::NONE };
		//glm::vec3 m_rotation;
		//glm::vec3 m_offset{};
		//std::vector<glm::ivec2> m_offsetList{};
		////std::vector<BuildingAttachmentPointGroup> m_attachList{};
		//bool m_isAutoRemove{};
		//int m_maxOffsetX{ 0 };
		//int m_minOffsetX{ 0 };
		//int m_maxOffsetY{ 0 };
		//int m_minOffsetY{ 0 };
		//int n_fog_unlock_x = 0;
		//int n_fog_unlock_y = 0;
	};

public:
	typedef std::unordered_map<int, BuildingTemplate> BuildingTemplateMap;
	//typedef std::unordered_multimap<NS::uint, BuildingTemplate*> BuildingTemplateByTypeMap;


public:
	static BuildingCfg& getInstance();

	bool initializeCfg();
	void terminate();

	const BuildingTemplate* findBuildingTemplate(int templateId);
	const BuildingTemplate* findBuildingTemplateByTypeId(int typeId);

	//std::vector<BuildingTemplate*> findBuildingTemplateListByTypeId(int typeId);
private:
	bool parseBuildTemplateConfig(tinyxml2::XMLElement* pXmlRoot);
private:
	BuildingTemplateMap m_buildingTemplateMap;
	//BuildingTemplateByTypeMap m_buildingTemplateByTypeMap;
};
