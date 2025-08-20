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


//技能播放位置
enum {
	SkillFrontTarget_Self = 0,  //玩家
	SkillFrontTarget_Target,    //目标身上
	SkillFrontTarget_Position   //位置
};



class SkillFrontEffectCfg
{
public:
	struct SkillFrontEffectInfo
	{
		int n_id{};
		int n_Type{};
		std::string s_SockName{};
		std::string s_DemoPath{};
		float f_tm{};
		std::string s_desc{};
	};

	typedef std::map<int, SkillFrontEffectInfo> MAP_SKILLFONTTYPE;

public:

	SkillFrontEffectCfg() {}
	static SkillFrontEffectCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const SkillFrontEffectInfo* GetSkillFrontType(int id);
	std::string GetSkillPath() { return m_strPath; }
private:
	MAP_SKILLFONTTYPE m_mapSkillType;
	std::string m_strPath;
};
