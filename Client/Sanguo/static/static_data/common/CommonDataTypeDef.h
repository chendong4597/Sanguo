#pragma once

#include <map>

#define SIGNED_MAX (0x7fffffff)

//家俱的最大尺寸是10x10
#define MAX_BUILD_SZ (10)

enum PLANT_INTER_ERR_T{
	PLANT_INTER_ERR_NONE,
	PLANT_INTER_ERR_FAIL_GROW,
};

#define RANDMOM_MAX (1000000)

enum ENUM_DROP_PROCESS_DATA_TYPE
{
	ENUM_DROP_PROCESS_DATA_TYPE_NONE = 0,
	ENUM_DROP_PROCESS_DATA_TYPE_PROCESS = 1,
	ENUM_DROP_PROCESS_DATA_TYPE_ITEM = 2,
	ENUM_DROP_PROCESS_DATA_TYPE_RULE = 3,
};

enum ENUM_DROP_PROCESS_TYPE
{
	ENUM_DROP_PROCESS_TYPE_NONE = 0,
	ENUM_DROP_PROCESS_TYPE_RANDOM = 1, // 随机概率
	ENUM_DROP_PROCESS_TYPE_PROPERTY = 2, // 权重抽去其一
	ENUM_DROP_PROCESS_TYPE_ALL = 3, // 全部的命中
	ENUM_DROP_PROCESS_TYPE_RULEID = 4, // 
	ENUM_DROP_PROCESS_TYPE_ALL_SORT = 5,	// 全部的命中(根据权重随机而得的顺序输出)
	ENUM_DROP_PROCESS_TYPE_PROPERTY_ND = 6,	// 权重,正态分布
};

enum ENUM_DROP_TYPE
{
	ENUM_DROP_TYPE_PRIVATE = 1,		// 私人掉落
	ENUM_DROP_TYPE_PUBLIC = 2,		// 公共掉落
	ENUM_DROP_TYPE_EVERYONE = 3,	// 人手一份
};

enum BUILD_STICK_TYPE
{
	BUILD_STICK_TYPE_NONE = 0, //无附着点,悬空式
	BUILD_STICK_TYPE_ADJACENCY_POINT_BOTTOM = 1, //底邻接点式附着(放在地板上的桌子)
	BUILD_STICK_TYPE_PLACE_POINT = 2, //物件所有点附着(如果放在背景墙上的火把,壁画)
	BUILD_STICK_TYPE_ADJACENCY_POINT_LEFT = 3, //左接点式附着(墙上的火把)
	BUILD_STICK_TYPE_ADJACENCY_POINT_RIGHT = 4, //右接点式附着(墙上的火把)
	BUILD_STICK_TYPE_ADJACENCY_POINT_UP = 5, //顶邻接点式附着(吊灯)
};

enum EN_PLANT_GROW_TERRIAN_TYPE
{
	EN_PLANT_GROW_TERRIAN_TYPE_BLOCK = 1, //长在地形块上
	EN_PLANT_GROW_TERRIAN_TYPE_WATER = 2, //长在水里
};

enum EN_DROP_PROCESS_TYPE
{
	EN_DROP_PROCESS_TYPE_CLIENT = 1, //单人模式下掉落由客户端处理
	EN_DROP_PROCESS_TYPE_SERVER = 2, //单人模式下掉落由服务端处理
};

enum ENUM_BUILD_FUNCTION_TYPE
{
	ENUM_BUILD_FUNCTION_TYPE_NORMAL = 1,	//普通家俱
	ENUM_BUILD_FUNCTION_TYPE_TOWER = 2,		//塔防家俱
	ENUM_BUILD_FUNCTION_TYPE_NPCBADGE = 3,  //npc徽章家俱
};

// NPC招募狀態通知類型
enum EM_PROTECTOR_EVENT_TYPE
{
	EM_PROTECTOR_EVENT_TYPE_SCAN_ROOM = -1,			// 房間條件檢查通知(檢查對象有變更時)
	EM_PROTECTOR_EVENT_TYPE_INIT = 0,				// 初始(未達入住條件)
	EM_PROTECTOR_EVENT_TYPE_WAIT_JOIN = 1,			// 準備入住(達入住條件)
	//EM_PROTECTOR_EVENT_TYPE_WAIT_LEAVL = 2,			// 準備離開(未達入住條件時)
	EM_PROTECTOR_EVENT_TYPE_UPGRADE = 3,			// 徽章升級成功
};

// build place range limit
enum EN_BUILD_PLACERANGE_LIMIT_TYPE
{
	EN_BUILD_PLACERANGE_LIMIT_TYPE_NONE          = 0, //无限制
	EN_BUILD_PLACERANGE_LIMIT_TYPE_CRYSTAL_RANGE = 1, //水晶范围
};

enum BUILD_DIRECTION_TYPE
{
	BUILD_DIRECTION_TYPE_NONE = 0, //无朝向(壁画之类 BUILD_STICK_TYPE_PLACE_POINT)
	BUILD_DIRECTION_TYPE_UPLEFT = 1, //正放向左 BUILD_STICK_TYPE_ADJACENCY_POINT
	BUILD_DIRECTION_TYPE_UPRIGHT = 2, //正放向右 BUILD_STICK_TYPE_ADJACENCY_POINT
	BUILD_DIRECTION_TYPE_BOTTOMLEFT = 3, //倒立向左 BUILD_STICK_TYPE_ADJACENCY_POINT
	BUILD_DIRECTION_TYPE_BOTTOMRIGHT = 4, //倒立向右 BUILD_STICK_TYPE_ADJACENCY_POINT
	BUILD_DIRECTION_TYPE_OUT_UP = 5, //正放朝外(附着在背景墙上的火把 BUILD_STICK_TYPE_PLACE_POINT)
	BUILD_DIRECTION_TYPE_OUT_DOWN = 6, //倒放朝外(附着在背景墙上的火把 BUILD_STICK_TYPE_PLACE_POINT)
};

enum BUILD_TYPE_IN_ROOM
{
	BUILD_TYPE_IN_ROOM_NONE,  //无 
	BUILD_TYPE_IN_ROOM_LIGHT, //照明
};

enum BUILD_N_TYPEID // BuildTypeCfg.xlsx n_TypeId
{
	BUILD_N_TYPEID_WALL         = 1, //墙壁
	BUILD_N_TYPEID_BACK_GROUP   = 2, //背景墙
	BUILD_N_TYPEID_TORCH        = 3, //火把
	BUILD_N_TYPEID_LADDER_STEP  = 4, //阶梯
	BUILD_N_TYPEID_LADDER_CLIMB = 5, //爬梯
	BUILD_N_TYPEID_PLATFORM     = 6, //平台
	BUILD_N_TYPEID_ROPE         = 7, //绳子
	BUILD_N_TYPEID_DOOR         = 8, //门
	BUILD_N_TYPEID_BUILD_HANG   = 9, //悬吊类家具
	BUILD_N_TYPEID_BUILD_SIDE   = 10, //侧挂类家具
	BUILD_N_TYPEID_BUILD_MURAL  = 11, //壁画类家具
	BUILD_N_TYPEID_BUILD_PLACE  = 12, //放置类家具
	BUILD_N_TYPEID_BUILD_GRASS  = 13, //草丛
};

enum TOWERBUILD_SUMMON_MOB_MODE
{
	TOWERBUILD_SUMMON_MOB_MODE_NORMAL,  //正常召唤
	TOWERBUILD_SUMMON_MOB_MODE_CONTROL, //受控召唤
	TOWERBUILD_SUMMON_MOB_MODE_AUTO,    //自动召怪
};

enum BUILD_N_SUBTYPEID // BuildTypeCfg.xlsx n_SubTypeId
{
	//1	床
	//2	衣柜
	//3	桌子
	//4	椅子
	//5	营火
	//6	货柜
	//7	流理台
	//8	写字板
	//9	墓碑
	//10	栅栏
	//11	沙发
	//12	立灯
	//13	澡盆
	//14	落地钟
	//15	长椅
	//16	书柜
	//17	土偶
	//18	鱼缸
	//19	武器架
	//20	防具架
	//21	点唱机
	//22	洗手台
	//23	壁炉
	//24	盆栽
	//25	水族箱
	//26	地毯块
	//27	乐器
	//28	壁镜
	//29	假人
	//30	游戏机台
	//31	工作台
	//32	城堡屋顶
	//33	模型基础块
	BUILD_N_SUBTYPEID_TREASURE_CHEST = 34, //宝箱
};



#define GEN_XY(__x_, __y_) ( ((((NS::I64)__x_)  << 24) & 0x0ffffff000000) | (__y_ & 0x0ffffff))


struct DropRet
{
	DropRet() :
		m_dataid(-1)
		, m_iCount(0)
		, m_iSplit(0)
	{}

	int m_dataid;
	int m_iCount;
	int m_iSplit;
};

struct DropInfo
{
	int m_dataid;
	int m_iMaxCount; // 可能的最大掉落个数
	int m_iRate;     // 百万分之一 
};

struct TowerBuildLevelInfo
{
	int m_iLevel = 1;
};
typedef std::map<int/* build type */, TowerBuildLevelInfo> MAP_TOWERBUILD_LEVEL_INFO;

#define WORLD_COORD_SCALE (100)

#ifdef BUILD_COMMON_SRV
// -------------------- for srv include --------------------

#include "Types.h"

#include "MsgBuild.pb.h"
#include "MsgDig.pb.h"
#include "MsgSkill.pb.h"
#include "MsgPacketDrop.pb.h"
#include "MsgPlant.pb.h"
#include "MsgBuildTower.pb.h"
#include "MsgItem.pb.h"
#include "MsgFog.pb.h"

#include "CommonDefine.h"

#else



// -------------------- for client include --------------------

#include "BaseTypeDefine.h"
//#include "RenderTypes.h"

#include <msg/MsgBuild.pb.h>
//#include <msg/MsgDig.pb.h>
#include <msg/MsgSkill.pb.h>
#include <msg/MsgPacketDrop.pb.h>
#include <msg/MsgPlant.pb.h>
#include <msg/MsgBuildTower.pb.h>
#include <msg/MsgItem.pb.h>
//#include <msg/MsgFog.pb.h>

using NS::uint8;
using NS::int32;
using NS::uint32;
using NS::I64;
using NS::UI64;
//using NS::Vector2;

#define MAX_BUILD_SZ (10)

//enum BUILD_STICK_TYPE
//{
//	BUILD_STICK_TYPE_NONE = 0, //无附着点,悬空式
//	BUILD_STICK_TYPE_ADJACENCY_POINT_BOTTOM = 1, //底邻接点式附着(放在地板上的桌子)
//	BUILD_STICK_TYPE_PLACE_POINT = 2, //物件所有点附着(如果放在背景墙上的火把,壁画)
//	BUILD_STICK_TYPE_ADJACENCY_POINT_LEFT = 3, //左接点式附着(墙上的火把)
//	BUILD_STICK_TYPE_ADJACENCY_POINT_RIGHT = 4, //右接点式附着(墙上的火把)
//	BUILD_STICK_TYPE_ADJACENCY_POINT_UP = 5, //顶邻接点式附着(吊灯)
//};
//
//
//enum EN_DROP_PROCESS_TYPE
//{
//	EN_DROP_PROCESS_TYPE_CLIENT = 1, //单人模式下掉落由客户端处理
//	EN_DROP_PROCESS_TYPE_SERVER = 2, //单人模式下掉落由服务端处理
//};
//
//
//enum EN_PLANT_GROW_TERRIAN_TYPE
//{
//	EN_PLANT_GROW_TERRIAN_TYPE_BLOCK = 1, //长在地形块上
//	EN_PLANT_GROW_TERRIAN_TYPE_WATER = 2, //长在水里
//};

// 地圖類型
enum EN_MAP_TYPE
{
	MAP_TYPE_NONE = 0,	// 未初始化
	MAP_TYPE_PRIVATE = 1,	// 私人有地图
	MAP_TYPE_PUBLIC = 2,	// 公共地图
	MAP_TYPE_DUNGEON = 3,	// 副本地圖
	MAP_TYPE_PVP = 4,	// PVP地图
	MAP_TYPE_PARTY = 5,	// Party地圖
	MAP_TYPE_DUNGEON_ENDLESS = 6, // 无尽副本(策划案里为单人模式)

	MAP_TYPE_MAX,
};

class CIntPoint
{
public:
	CIntPoint();
	CIntPoint(int32 inLeft, int32 inTop);
	//CIntPoint(const Vector2& v);
	CIntPoint(const char* str);

	void reset(int32 inLeft = 0, int32 inTop = 0);
	void ReadFromString(const char* str);
	//Vector2 toVector2() const;

	bool operator == (const CIntPoint& point) const;
	bool operator != (const CIntPoint& point) const;
	CIntPoint& operator =(const CIntPoint& point);
	CIntPoint operator +(const CIntPoint& point) const;
	CIntPoint operator -(const CIntPoint& point) const;
	CIntPoint& operator +=(const CIntPoint& point);
	CIntPoint& operator -=(const CIntPoint& point);
	CIntPoint operator -() const;
	bool operator < (const CIntPoint& r)const;

	static const CIntPoint ZERO;
	//Vector2 toVector2() const
	//{
	//	return Vector2(static_cast<float>(left), static_cast<float>(top));
	//}
public:
	union
	{
		int32 left;
		int32 x{ 0 };
	};

	union
	{
		int32 top;
		int32 y{ 0 };
	};
};

struct area_bound_t
{
	CIntPoint m_min;
	CIntPoint m_max;

	void addArea(const CIntPoint& a)
	{
		if (this->m_min.x > a.x)
			this->m_min.x = a.x;
		if (this->m_min.y > a.y)
			this->m_min.y = a.y;

		if (this->m_max.x < a.x)
			this->m_max.x = a.x;
		if (this->m_max.y < a.y)
			this->m_max.y = a.y;
	}


	void addArea(const int32 x, const int32 y)
	{
		if (this->m_min.x > x)
			this->m_min.x = x;
		if (this->m_min.y > y)
			this->m_min.y = y;

		if (this->m_max.x < x)
			this->m_max.x = x;
		if (this->m_max.y < y)
			this->m_max.y = y;
	}
};


class CIntRect
{
public:
	CIntRect();
	CIntRect(int32 inLeft, int32 inTop, int32 inRight, int32 inBottom);

	void reset(int32 inLeft = 0, int32 inTop = 0, int32 inRight = 0, int32 inBottom = 0);

	int32 GetWidth() const { return (right > left) ? right - left : left - right; }
	int32 GetHeight() const { return (top > bottom) ? top - bottom : bottom - top; }
	int32 GetArea() const { return GetWidth() * GetHeight(); }

	bool operator == (const CIntRect& rect) const;
	CIntRect& operator =(const CIntRect& rect);

	static const CIntRect ZERO;

public:
	int32 left{ 0 };
	int32 top{ 0 };
	int32 right{ 0 };
	int32 bottom{ 0 };
};


class CIntSize
{
public:
	CIntSize();
	CIntSize(int32 inWidth, int32 inHeight);

	void reset(int32 inWidth = 0, int32 inHeight = 0);
	//void ReadFromString(const char* str);

	bool operator == (const CIntSize& size) const;
	CIntSize& operator =(const CIntSize& size);

	static const CIntSize ZERO;

public:
	int32 width{ 0 };
	int32 height{ 0 };
};

#endif

struct RoomRequiredCommon
{
	// 未達住宿需求檢查結果(需通知客戶端)
	MsgPB::PROTECTOR_LESS_THAN_REQUIRED	emRequired;	// NPC未達住宿需求類型
	std::vector<int32> m_vRoomParams;				// index_0 - [傢俱需求:非0為家具1不足][空間需求:非0為空間太小] [背景牆需求:背景墙占%] 
	// index_1 - [傢俱需求:非0為家具2不足][空間需求:非0為空間太大]  
	// index_2 - [傢俱需求:非0為家具3不足]
	// index_3 - [傢俱需求:非0為家具4不足]
	// index_4 - [傢俱需求:非0為家具5不足]
	int32	FlagCount;								// 房間內徽章數
	bool	bHasDoor;								// 邊緣是否有門 
	int32	Comfort;								// 總舒適度
	int32	QualityGrade[MsgPB::QUALITY_GRADE_FOR_DB_SETTING_ROOM_FURNITURE_ARRAYSIZE];		// 各品質傢俱數量
	bool m_bBuildTypeIdOK = true;

	RoomRequiredCommon()
	{
		Init();
	}

	void Init()
	{
		emRequired = MsgPB::PROTECTOR_LESS_THAN_REQUIRED::PROTECTOR_LESS_THAN_REQUIRED_NONE;
		m_vRoomParams.clear();
		FlagCount = 0;
		bHasDoor = false;
		m_bBuildTypeIdOK = false;
		Comfort = 0;
		for (int32 i = 0; i < sizeof(QualityGrade) / sizeof(QualityGrade[0]); ++i)
			QualityGrade[i] = 0;
	}

	// 累加指定品質傢俱數量
	void AddQualityGradeCount(MsgPB::QUALITY_GRADE emType)
	{
		if (emType < 0 || emType >= sizeof(QualityGrade) / sizeof(QualityGrade[0]))
			return;
		QualityGrade[emType] += 1;
	}

	// 檢查傢俱舒適度品質數量是否符合條件
	bool CheckComfortAndQualityGrade(MsgPB::QUALITY_GRADE emType, int32 CheckCount, int32 CheckComfort)
	{
		if (Comfort < CheckComfort)
			return false;
		if (emType < 0 || emType >= sizeof(QualityGrade) / sizeof(QualityGrade[0]))
			return false;
		if (QualityGrade[emType] < CheckCount)
			return false;
		return true;
	}
};

struct MapTemplateStaticInfo
{
	int32 m_StaticTemplateID;
};
typedef std::map<CIntPoint, MapTemplateStaticInfo> MAP_STATIC_POS;
