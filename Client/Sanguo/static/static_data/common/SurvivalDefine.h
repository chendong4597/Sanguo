#pragma once

#include <vector>

//////////////////////////////////////////////////////////////////////////
#define	DEF_SURVIVAL_EVENT_MOB_LIST_ARRAY			10		// 生存事件怪物清單配置表怪物陣列數(主線怪)
#define	DEF_SURVIVAL_EVENT_CYCLE_MOB_LIST_ARRAY		3		// 生存事件怪物清單配置表怪物陣列數(循環怪)
#define	DEF_SURVIVAL_EVENT_ITEM_ARRAY				3		// 生存事件保底獎勵配置表的物品陣列數
#define	DEF_SURVIVAL_EVENT_MOB_POOL_ARRAY			7		// 生存事件刷怪池配置表怪物陣列數
#define	DEF_SURVIVAL_EVENT_NPC_ARRAY				3		// 生存事件配置表的保護NPC陣列數

#define	MAX_SURVIVAL_COUNT							500		// 系統最大局數
#define	DEF_SURVIVAL_ONTIME_ADD_MOB					1		// 定時檢查是否刷怪(s)
#define	DEF_SURVIVAL_ONTIME_FORCED_TO_END			1		// 定時檢查遊戲時間是否結束(s)
#define	DEF_SURVIVAL_ONTIME_CHANGE_WAVE				1		// 定時檢查是否換波(s) (按时间换波模式才参照)

#define	DEF_MOB_BIRTH_POINT_SCATTER_X				6		// 散點刷怪範圍，距離玩家幾格：X軸 (玩家屏幕內範圍)
#define	DEF_MOB_BIRTH_POINT_SCATTER_Y				8		// 散點刷怪範圍，距離玩家幾格：Y軸 (玩家屏幕內範圍)

#define	DEF_MOB_BIRTH_POINT_HIGHEST					6		// 左右刷怪點範圍最高點，距離水晶幾格
#define	DEF_MOB_BIRTH_POINT_LOWEST					6		// 左右刷怪點範圍最低點，距離水晶幾格

#define	DEF_SURVIVAL_RUN_EVENT_TIME					1000	// 定時執行佇列裡的事件間隔時間(ms)

#define	DEF_SURVIVAL_CHECK_CLOSE_TIMER				1000	// 定時檢查是否刪除已關閉的局(ms)

//////////////////////////////////////////////////////////////////////////
// 生存塔防模式
enum SURVIVAL_MODE
{
	SURVIVAL_MODE_NONE = 0,			// 未設定
	SURVIVAL_MODE_SINGLE = 1,		// 單人模式(策劃方設定主動或被動觸發)
	SURVIVAL_MODE_MULTIPLAYER = 2,	// 多人模式(策劃方設定僅主動觸發)
};

// 生存塔防觸發天氣
enum SURVIVAL_WEATHER
{
	SURVIVAL_WEATHER_NONE = -1,		// 不判斷
	SURVIVAL_WEATHER_SUNNY = 0,		// 晴天
	SURVIVAL_WEATHER_CLOUDY = 1,	// 陰天
	SURVIVAL_WEATHER_RAIN = 2,		// 雨天
	SURVIVAL_WEATHER_SNOWY = 3,		// 雪天
	SURVIVAL_WEATHER_SANDSTORM = 4,	// 沙塵暴
};

// 生存塔防觸發生態區
enum SURVIVAL_AREA
{
	SURVIVAL_AREA_NONE = -1,	// 不判斷
	SURVIVAL_AREA_FOREST = 0,	// 森林
	SURVIVAL_AREA_DESERT = 1,	// 沙漠
	SURVIVAL_AREA_SNOWY = 2,	// 雪原
	SURVIVAL_AREA_RUIN = 3,		// 腐地
	SURVIVAL_AREA_JUNGLE = 4,	// 叢林
	SURVIVAL_AREA_SkyIsland = 5,// 空島
	SURVIVAL_AREA_LAVA = 6,		// 熔岩
};

// 生存塔防事件是否可疊加
//enum SURVIVAL_SUPERIMPOSE
//{
//	SUPERIMPOSE_NO = 0,		// 不可疊加
//	SUPERIMPOSE_YES = 1,	// 可疊加
//};

// 生存塔防換波判斷模式
enum SURVIVAL_CHANGE_WAVE
{
	SURVIVAL_CHANGE_WAVE_NONE = 0,			// 僅一波(可疊加事件必選)
	SURVIVAL_CHANGE_WAVE_CLEAR_ENEMY = 1,	// 清除敵方模式
	SURVIVAL_CHANGE_WAVE_SCORE = 2,			// 積分換波模式
	SURVIVAL_CHANGE_WAVE_TIME = 3,			// 按時間換波模式
};

// 任務目標的顯示類型
enum SURVIVAL_TARGET_INFO_TYPE
{
	SURVIVAL_TARGET_INFO_TYPE_NONE = 0,					// 不顯示
	SURVIVAL_TARGET_INFO_TYPE_NPC_COUNT = 1,			// NPC存活數 (目前NPC數 / NPC總數)
	//SURVIVAL_TARGET_INFO_TYPE_CRYSTAL_DURABLE = 2,	// 水晶耐久度 (目前耐久度 / 最大耐久度)
};

// 生存塔防系統階段狀態
enum SURVIVAL_STATE
{
	SURVIVAL_STATE_NULL = 0,
	SURVIVAL_STATE_READY_TO_GO,		// 每波開始前倒數準備階段 (定時處理觸發結束)
	SURVIVAL_STATE_BATTLE,			// 戰鬥階段 (計數器觸發結束)
	SURVIVAL_STATE_REWARD,			// 結算給予獎勵階段 (定時處理觸發結束)
	SURVIVAL_STATE_EXIT,			// 玩家離開階段 (定時處理觸發結束)
	SURVIVAL_STATE_CLOSE,			// 關閉階段 (等待管理器刪除此局)
};

// 略過勝利條件檢查 (用途在先預先檢查某一項，符合後再檢查所有的勝利條件)
enum SURVIVAL_PASS_WIN_CONDITION
{
	SURVIVAL_PASS_WIN_CONDITION_NONE = 0,	// 檢查所有條件
	SURVIVAL_PASS_WIN_CONDITION_MOB,		// 敵方怪物是否全滅
	SURVIVAL_PASS_WIN_CONDITION_KILL_TARGET,// 击杀指定怪物
};

// 失敗條件檢查
enum SURVIVAL_CHECK_LOSE_CONDITION
{
	SURVIVAL_CHECK_LOSE_CONDITION_PLAYER = 0,	// 我方玩家被殺
	SURVIVAL_CHECK_LOSE_CONDITION_CRYSTAL,		// 我方水晶被摧毀
	SURVIVAL_CHECK_LOSE_CONDITION_NPC,			// 我方NPC被殺
};

// 死亡通知角色分類
enum SURVIVAL_DEAD_CHARACTER_TYPE
{
	SURVIVAL_DEAD_CHARACTER_TYPE_NULL = 0,
	SURVIVAL_DEAD_CHARACTER_TYPE_PLAYER,		// 玩家(我方)
	SURVIVAL_DEAD_CHARACTER_TYPE_CRYSTAL,		// 水晶(我方)
	SURVIVAL_DEAD_CHARACTER_TYPE_NPC,			// NPC(我方)
	SURVIVAL_DEAD_CHARACTER_TYPE_TOWER,			// 塔方裝置(我方)
	SURVIVAL_DEAD_CHARACTER_TYPE_MOB,			// 怪物(敵方)
};

// 刷怪點分類
enum SURVIVAL_MOB_BIRTH_POINT_TYPE
{
	SURVIVAL_MOB_BIRTH_POINT_TYPE_SCATTER = 0,	// 散點
	SURVIVAL_MOB_BIRTH_POINT_TYPE_LEFT,		// 左方刷怪點
	SURVIVAL_MOB_BIRTH_POINT_TYPE_RIGHT,		// 右方刷怪點
};

// 刷怪點類型
enum SURVIVAL_EVENT_MOB_LIST_BORN_TYPE
{
	SURVIVAL_EVENT_MOB_LIST_BORN_TYPE_POINT = 0,			// 固定點刷怪(左方、右方、散點)
	SURVIVAL_EVENT_MOB_LIST_BORN_TYPE_RECTANGLE = 1,		// 矩形範圍刷怪(可刷怪與不可刷怪範圍)
};

// 生存事件类型
enum SURVIVAL_EVENT_TYPE
{
	SURVIVAL_EVENT_TYPE_NULL = 0,
	SRUVIVAL_EVENT_TYPE_BLOOD_MOON = 1,
	SURVIVAL_EVENT_TYPE_ZOMBIE = 2,
	SURVIVAL_EVENT_TYPE_GOBLIN = 3,
	SURVIVAL_EVENT_TYPE_PENGUIN = 4,
	SURVIVAL_EVENT_TYPE_OTHERWORLD = 5,
	SURVIVAL_EVENT_TYPE_ANT = 6,

	SURVIVAL_EVENT_TYPE_BLOOD_MOON_ENDLESS = 8,
	SURVIVAL_EVENT_TYPE_OTHERWORLD_ENDLESS = 9,
};

//////////////////////////////////////////////////////////////////////////
// 共用資訊
struct S_SurvivalEventTemplate_Info
{
	int n_ReadyTime;			// 每波開始前倒數準備階段時間(秒)
	int n_RewardTime;			// 結算給予獎勵階段時間(秒)
	int n_ExitTime;			// 玩家離開副本階段時間(ms)
};

// 勝利條件(組合)，全符合立即獲勝
struct S_SurvivaWinCondition
{
	union
	{
		int Flag;					// 配置填表值為以下各複選的值加總
		struct
		{
			bool bMpn : 1;			// 1:敵方怪物全滅
			bool bKillTarget : 1;	// 2:击杀指定怪物
			// 4:
		};
	};
};

// 失敗條件(組合)，任一符合立即失敗
struct S_SurvivaLoseCondition
{
	union
	{
		int Flag;					// 配置填表值為以下各複選的值加總
		struct
		{
			bool bTimeOver : 1;		// 1:時間結束判斷為敗，反之若未勾選判斷為勝
			bool bCrystal : 1;		// 2:我方水晶被摧毀，則判斷為敗
			bool bPlayer : 1;		// 4:我方玩家團滅，則判斷為敗
			bool bNpc : 1;			// 8:我方NPC全滅，則判斷為敗 (NPC尚未實作)
		};
	};
};

//  使用道具略過被動觸發條件(組合)
struct S_SkipPassiveTrigger
{
	union
	{
		int Flag;					// 配置填表值為以下各複選的值加總
		struct
		{
			bool bLevel : 1;			// 1:角色等級
			bool bAlternateMoment : 1;	// 2:日月交替時刻
			bool bMoonPhase : 1;		// 4:月相
			bool bWeather : 1;			// 8:天氣
		};
	};
};

struct S_SurvivalEventMobListTemplate_Mob
{
	int n_PoolID = 0;		// 怪物池ID
	int n_Count = 0;		// 每刷怪回合刷怪次數
	int n_Interval = 0;		// 每刷怪回合間隔時間(秒)

	bool Empty() const
	{
		if (this->n_PoolID <= 0 || this->n_Count <= 0)
			return true;
		return false;
	}
};

struct S_SurvivalEventMobPoolTemplate_Mob
{
	int n_MobID = 0;		// 怪物ID
	int n_percent = 0;	// 概率(%)
	int n_Score = 0;		// 殺死獲得積分

	bool Empty()
	{
		if (n_MobID <= 0 || n_percent <= 0)
			return true;
		return false;
	}
};

struct S_SurvivalEventGuaranteeRewardsTemplate_Item
{
	int n_ItemID = 0;
	int n_ItemCount = 0;
};

struct S_SurvivalEventNpcTemplate_Npc
{
	int n_MobID = 0;			// Mob配置ID
	int n_BirthPointX = 0;	// 出生點與地圖安全點的X軸格數差
	int n_BirthPointY = 0;	// 出生點與地圖安全點的Y軸格數差
	int n_ItemID = 0;			// 道具配置ID
	int n_ItemCount = 0;		// 道具數量
};

struct SCacheEvent
{
	int nEventID{ 0 };
	long long tCreateTime{ 0 };
	long long tElapsedTime{ 0 };
	int nWave{ 0 };
	int nState{ SURVIVAL_STATE_READY_TO_GO };
	int nKillEnemyScore{ 0 };
	int nTotalMobCount{ 0 };
	int nGameResult{ 0 };
	int nRound{ 0 };
	SCacheEvent() {}
	SCacheEvent(int nID, long long nTime)
		:nEventID(nID), tCreateTime(nTime) {}
};
using VecCacheEvent = std::vector<SCacheEvent>;
