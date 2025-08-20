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


class RoleCreateCfg
{
public:
	enum Sex_Tpy
	{
		Sex_Tpy_Man = 0,              //男
		Sex_Tpy_Woman = 1,              //女
	};

	enum Body_Tpy   //身体分成3快
	{
		Body_Tpy_head = 0,  //头
		Body_Tpy_main,   //身体
		Body_Tpy_leg   //脚
	};


	enum ICON_TYPE {
		ICON_TYPE_Hair_Style = 0,
		ICON_TYPE_Hair_Color,
		ICON_TYPE_Skin_Color
	};

	struct RoleCreateCfgCfgInfo
	{
		int n_id{};
		int n_sex{};
		int n_type{};
		int n_nIdx{};
		std::string s_icon_path;
		std::string s_mesh_path;
	};

	typedef std::map<int, RoleCreateCfgCfgInfo> MAP_ROLECREATE;

public:

	RoleCreateCfg() {}
	static RoleCreateCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const RoleCreateCfgCfgInfo* GetRoleInfo(int nSex , int nType , int nIndex);
	std::string GetGameIconPath(std::string strIcon);
	std::string GetSkeletalMesh(int nSex, std::string strMesh);
	std::string GetHairMaterial(int nSex, int nStyle);
	std::string GetHairColorTexture(int nSex, int nStyle , int nIndex);
	std::string GetSkinMaterial(int nSex, int nBodyTpy);
	std::string GetSknColorTexture(int nSex, int nIndex);
	std::string GetMainSkeletalMesh(int nSex);
	std::string GetBPAniPath(int nSex);

private:
	MAP_ROLECREATE m_mapRoleInfo;
	std::string m_sIconPath;
	std::string m_sManHairMeshPath;
	std::string m_sWomanHairMeshPath;
	std::string m_sManSkinM;
	std::string m_sWomanSkinM;
	std::string m_sManSkeletalMeshPath;
	std::string m_sWomanSkeletalMeshPath;
	std::string m_sManBPAniPath;
	std::string m_sWomanBPAniPath;
};
