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


enum {
	Defence_Build_Tpy_Normal = 0,  //普通攻击,主动攻击
	Defence_Build_Tpy_Trap,    //陷阱
	Defence_Build_Tpy_AubAtk,    //生产寻路怪
};

enum {
	Defence_Build_Pos_Tpy_C = 0, //炮台
	Defence_Build_Pos_Tpy_A,     //弓弩
};

class DefenceInfoTypeCfg
{
public:
	struct DefenceInfoTypeCfgInfo
	{
		int n_id{};
		int n_Type{};
		int n_PosTpy{};
		int n_Width{};
		int n_Height{};
		int n_RotaTpy{};  //0 : -0  1:90度
		std::string s_Name{};
		int n_isContinue{};
		int n_Index{};
		int n_SkillId{};
		float f_IntertTm{};
		int n_AtkSight{};
		int n_LoseSight{};
		int n_AtkAngle{};
		int n_AtkNum{};
		int n_AI{};
		std::string s_Foder;
		std::string s_Path;
		std::string s_Mesh;
		std::string s_Desc;
	};

	typedef std::map<int, DefenceInfoTypeCfgInfo> MAP_DEFENCEINFOTYPE;

public:

public:

	DefenceInfoTypeCfg() {}
	static DefenceInfoTypeCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const DefenceInfoTypeCfgInfo* GetDefenceById(int id);

	std::string GetBpPath(int id);
	std::string GetMeshPath(int id);
	std::string GetAttackPath(int nId);
	int GetDefencePosInRect(int id , int nPosX , int nPosY);
private:
	MAP_DEFENCEINFOTYPE m_mapDefenceInfoType;

	std::string m_strPath;
};
