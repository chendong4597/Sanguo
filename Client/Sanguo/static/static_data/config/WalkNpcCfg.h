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
#include "glm/detail/type_vec.hpp"


class WalkNpcCfg
{
	struct WalkNpcCfgInfo
	{
		int n_ID{};
		int n_Type{};
		std::string s_Name;
		int n_MonsterTpy{};
		int n_rate{};
		int n_speed{};
		std::string s_Foder{};
		std::string s_path{};
		std::string s_desc{};
	};

	typedef std::map<int, WalkNpcCfgInfo> MAP_WALKNPCTYPE;

public:

	WalkNpcCfg() {}
	static WalkNpcCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const WalkNpcCfgInfo* GetWalkNpcType(int id);

	void GetStartAndEndPos(glm::vec3& startPos , glm::vec3& endPos , std::vector<glm::vec3>& midPos);
	int GetNearstY(int nY);
	int GetRandNpcId();
	std::string GetBPWalkNpcPath(int id);
	int GetMaxWalkNum() { return n_MaxWalkNum; }

private:
	MAP_WALKNPCTYPE m_mapWalkNpc;
	std::string s_Path;
	int n_MinTarget;
	int n_MaxTarget;
	int n_MinStdayTm;
	int n_MaxStdayTm;
	int n_MidX;
	int n_PosZ;
	std::string s_MidY;
	std::string s_Pos;
	std::vector<int> m_vecMidYs;
	std::vector<glm::vec3> m_vecPos;
	int n_MaxWalkNum{};
};
