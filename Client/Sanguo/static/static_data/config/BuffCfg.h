/*!
 * \file BuffCfg.h
 *
 * \author zhijieli
 * \date 2017/06/13
 *
 * 
 */
#pragma once
#include <unordered_map>
#include <memory>
#include <tuple>
#include <array>
#include "../data/role/Role.h"

#define HAVE_LONG_LONG
////////////////////////////////////////////////////////////////
struct BuffType
{
	enum BUFF_EFFECT_TYPE
	{
		CLI_BUFF_EFFECT_NULL = 1000,
		CLI_BUFF_EFFECT_REAL_DOT = 1001,	// 真實傷害Dot
		CLI_BUFF_EFFECT_FIRE_DOT = 1002,	// 火傷害Dot
		CLI_BUFF_EFFECT_FORST_DOT = 1003, // 冰傷害Dot
		CLI_BUFF_EFFECT_THUNDER_DOT = 1004,	// 雷傷害Dot
		CLI_BUFF_EFFECT_EARTH_DOT = 1005,	// 土傷害Dot
		CLI_BUFF_EFFECT_POSION_DOT = 1006,	// 毒傷害Dot
		CLI_BUFF_EFFECT_LIGHT_DOT = 1007,	// 光傷害Dot
		CLI_BUFF_EFFECT_OXY_DOT = 1008,	// 扣氧气Dot
		CLI_BUFF_EFFECT_WEAPON_DOT = 1009,	// 物理傷害Dot
		CLI_BUFF_EFFECT_DARK_DOT = 1010,	// 暗傷害Dot
		CLI_BUFF_EFFECT_HP_RESTORE = 1011,	// 回恢HP
		CLI_BUFF_EFFECT_MP_RESTORE = 1012,	// 回恢MP
		CLI_BUFF_EFFECT_ROLE_STATE = 1013, // 角色狀態改變
		CLI_BUFF_EFFECT_MAX,
	};

	enum : NS::int32
	{
		BUFF_INF = 3000000,
		BUFF_CAV = 3000001,
		BUFF_ARC = 3000002,
		BUFF_BLA = 3000003,
		BUFF_MAG = 3000004,
	};

	enum BUFF_LOGOUT_SAVE : int
	{
		NO_SAVE = 0,			//下线不保存
		SAVE_AND_PAUSE,			//下线保存并且暂停计时
		SAVE_AND_CONTINUE,		//下线保存并且继续计时
	};

	enum BUFF_DEL_TYPE : int
	{
		NONE = 0,					// 无
		BY_TIME = 1,				// 持续时间消失
		BY_HURT = 1 << 1,			// 受伤消失
		BY_ATTACK = 1 << 2,			// 发起攻击消失
		BY_MAP_SWITCH = 1 << 3,		// 地图切换消失
	};

	// buff特效表现类型
	enum BUFF_SHOW_TYPE : NS::uint32
	{
		COMMON = 0,		// 普通Buff表现类
		MARKING = 1,	// 印记Buff表现类
	};

	NS::uint32 n_ID;
	NS::tstring s_name;
    NS::tstring s_name_untrans;
	NS::tstring s_buffdes;					// 描述
    NS::tstring s_buffdes_untrans;
	NS::tstring s_effectres;				// 特效
	NS::uint32 n_effectplaytype;			// 特效表现类型
	NS::tstring s_bufficon;					// 图标
	NS::tstring s_buff_color;
	NS::StringList s_effectattachment;		// 挂点
	NS::uint32 n_alive_time;
	NS::tstring s_action;					// 动作
	NS::tstring s_effect_soundKey;

	NS::uint32 n_groupid;
	NS::uint32 n_interval_time;
	NS::uint32 n_max_heap;
	NS::uint32 n_auto_heap;
	NS::uint32 n_group_rank;
	NS::int32 n_definition;               //buff类型定位

	NS::uint32 n_BuffEffID;

	enum { EFFECT_NUM = 8 };
	std::array<Effect, EFFECT_NUM> effects;

	enum { FadeIn = 1, FadeOut = 2 };
	enum { USE_ALIVE_TIME = 1, };
	unsigned n_buff_del;
	bool n_buff_display;
	bool n_lock_buff_display;
	NS::uint8 n_effect_solution;
	bool is_debuff;
	bool n_buff_effect_orientation{};
	glm::vec4 ava_clr;
	glm::vec4 ava_frame_color{ 1.f };
	//int n_save{};

	BUFF_LOGOUT_SAVE n_logout_save{};
    float m_value[5];

public:
	bool isNeedSaveBuff() const { return n_logout_save != BUFF_LOGOUT_SAVE::NO_SAVE; }
};

////////////////////////////////////////////////////////////////
class BuffCfg
{
public:
	using BuffTypeMap = std::unordered_map<NS::uint32, BuffType>;

	static BuffCfg& getInstance();

	bool initializeCfg();
	void terminate();
    void reloadMultiLanguage();

	const BuffType* findBuffType(NS::uint32 id) const;

private:
	bool parseBuffConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	BuffTypeMap m_buffTypeMap;
};
