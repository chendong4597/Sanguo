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
	Weapon_AniTpy_Normal = 0,  //普通攻击
	Weapon_AniTpy_Continue,    //持续攻击
};

enum {
	Ani_Continue_Idx1 = 1,    //连续攻击动画1
	Ani_Continue_Idx2 = 2,    //连续攻击动画2
	Ani_Continue_Num = 3,
};

//技能基础类型
enum SkillEffectTpy {
	SkillEffectTpy_Arrow = 0,  //箭类型  包括炮弹  法球等效果
	SkillEffectTpy_Cannon,     //炮弹   --暂时用箭能解决，看下后期有没有用
	SkillEffectTpy_Darts,      //回旋镖
	SkillEffectTpy_Laser,       //激光
	SkillEffectTpy_MagicRange,  //范围法术
	SkillEffectTpy_Gas,       //喷气攻击
	SkillEffectTpy_Emitter,   //技能发射器
	SkillEffectTpy_Multi,   //多目标攻击
	SkillEffectTpy_Trigger,   //触发器，一般触发BUFF
	SkillEffectTpy_Multi_Arrow,   //多目标远程攻击
};

//技能释放方式  0：点击  1：选择位置释放  2：选择方向释放
enum SkillEffectHitTpy {
	SkillEffectHitTpy_None, //点击
	SkillEffectHitTpy_ScrollHit, //选择位置释放
	SkillEffectHitTpy_ArrowHit, //选择方向释放
};

//技能弹道类型
enum SkillCurveTpy {
	SkillCurveTpy_None = 0, //直线
	SSkillCurveTpy_Bending, //弯曲,主要指回旋镖
	SSkillCurveTpy_RE_Bending, //反弯曲,主要指回旋镖
	SSkillCurveTpy_Circle, //圆形
	SSkillCurveTpy_RE_Circle, //圆形
};

//技能运行方向控制
enum SkillDirectTpy {
	SkillDirectTpy_None = 0, //不变
	SkillDirectTpy_TraceTarget, //一直朝向目标
	SkillDirectTpy_AsRate, //根据Z曲率来
	SkillDirectTpy_AttachAtker, //附着在攻击者身上
};

enum SkillDamageTpy {
	SkillDamageTpy_None = 0,  //无
	SkillDamageTpy_CircleRange,  //圆形范围
	SkillDamageTpy_Rectangle_Range,  //矩形范围
	SkillDamageTpy_Effect_CircleRange,  //特效位置圆形范围
	SkillDamageTpy_Single,	//击中后单体伤害
	SkillDamageTpy_Fly_Range, //飞行过程中范围伤害
	SkillDamageTpy_Hit_Range, //击中目标后范围伤害
	SkillDamageTpy_Distance_Range, //攻击者正前方位置的伤害
	SkillDamageTpy_Sector, //攻击者正前方扇形位置
	SkillDamageTpy_To_Self, //仅对自己使用
};

class SkillTypeCfg
{
public:
	struct SkillTypeInfo
	{
		int n_id{};
		int n_SkillTpy{};
		std::string s_Name{};
		std::string s_icon{};
		std::string s_AniIdx;
		std::vector<int> n_vecAniIdx;
		float f_CoolTime{};
		int n_AttackStopMove{};
		int n_AniTpy{};
		float f_Mana{};
		float f_Hp{};
		int n_Sten{};
		int n_Anger{};
		int n_AtkType{};
		int n_ScrollHit{};
		int n_nCastDis{};  //施法距离
		int n_atk{};
		int n_magic{};
		std::string s_sockName;
		int n_num{};
		int n_flash{};
		int n_curve{};
		int n_trace{};//技能运行方向控制
		int n_AttackTarget{}; //攻击时是否朝向目标
		int n_needTarget{};//是否需要选中目标
		int n_BomrangHei{};//抛物线高度
		int n_speed{};//速度
		float f_rate{};
		int n_length{};  //移动距离
		float f_maxTm{};//最长存在时间
		float f_interTm{}; //间隔时间
		int   m_nNxtMaxNum{};//子技能最多数量
		int n_buffId{};
		std::string s_demoPath;
		float f_continueInterTm{};
		int n_hitdead{};
		int n_frontSkill{};
		int n_nxtSkill{};
		float f_dieTm{};	//延时消失
		std::vector<int> m_vecNxtSkills;
		std::string s_hitEffect;
		std::string s_beginEff;
		std::string s_meshPath;
		std::string s_sound;
		std::string s_desc;

		//伤害
		int n_DamageTpy{};			//直接代码计算伤害请求
		int n_BpDamageTpy{};		//由蓝图发送伤害请求
		int n_DamageParam1{};
		int n_DamageParam2{};
		int n_DamageParam3{};
		int n_DamageParam4{};
	};

	typedef std::map<int, SkillTypeInfo> MAP_SKILLTYPE;


public:

	SkillTypeCfg() {}
	static SkillTypeCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const SkillTypeInfo* GetSkillType(int id);
	std::string GetSkillPath() { return m_strPath; }
	std::string GetHorseSkillIcon(int nSkillId);
	std::string GetSkillIcon(int nSkillId);
private:
	MAP_SKILLTYPE m_mapSkillType;
	std::string m_strPath;
	std::string m_strHorseIconPath;
	std::string m_strIconPath;
};
