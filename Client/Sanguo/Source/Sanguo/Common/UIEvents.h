#pragma once
#include "GameAppExtern.h"
#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"


#define DECLARE_UI_MYEVENT(evt) DECLARE_EVENT_OneParam(UEgine, F##evt, const evt&) \
public: \
	F##evt& On##evt() { return m_evt##evt; } \
private: \
	F##evt m_evt##evt;

#define UI_REGISTER_MYEVENT(evt , fun_name) HandlerUIEvents::getInstance().On##evt().AddUObject(this, fun_name); 
#define UI_UN_REGISTER_MYEVENT(evt, widget) HandlerUIEvents::getInstance().On##evt().RemoveAll(widget); 

#define UI_DISPATCH_MYEVENT(evtname , evt) HandlerUIEvents::getInstance().On##evtname().Broadcast(evt);


//*************************************************************************************************
//
//*************************************************************************************************
class BallEvent
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	BallEvent() {};
	virtual ~BallEvent() {};

public:
	EventType m_type;
	float x;
	float y;
};

//*************************************************************************************************
//
//*************************************************************************************************
class MaterialEvent
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	MaterialEvent() {};
	virtual ~MaterialEvent() {};

public:
	EventType m_type;
};

//*************************************************************************************************
//
//*************************************************************************************************
class SightEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	SightEvent() {};
	virtual ~SightEvent() {};

public:
	EventType m_type;
	float value;
};

//*************************************************************************************************
//
//*************************************************************************************************
class AttackEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	AttackEvent() {};
	virtual ~AttackEvent() {};

public:
	EventType m_type;
};

//*************************************************************************************************
//
//*************************************************************************************************
class StartContinueAttackEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	StartContinueAttackEvent() {};
	virtual ~StartContinueAttackEvent() {};

public:
	EventType m_type;
};

//*************************************************************************************************
//
//*************************************************************************************************
class StopContinueAttackEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	StopContinueAttackEvent() {};
	virtual ~StopContinueAttackEvent() {};

public:
	EventType m_type;
};

//*************************************************************************************************
//
//*************************************************************************************************
class HoverAttackEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	HoverAttackEvent() {};
	virtual ~HoverAttackEvent() {};

public:
	EventType m_type;
	float x;
	float y;
	float z;
	int m_nHitTpy{};
};

//*************************************************************************************************
//
//*************************************************************************************************
class UseWeaponExternEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	UseWeaponExternEvent() {};
	virtual ~UseWeaponExternEvent() {};

public:
	EventType m_type;
};


//*************************************************************************************************
//
//*************************************************************************************************
class HurtEvent// : public bzbee::Event   //攻击掉血效果
{
public:
	enum class EventType
	{
		Unknown,
	};

	enum DAMAGE_TPY {
		DAMAGE_TPY_CRIT = 000001,  //暴击
		DAMAGE_TPY_OUTFLOW = 0x00002, //流失
	};
public:
	HurtEvent() {};
	virtual ~HurtEvent() {};

public:
	EventType m_type;
	int m_nDamage{};    //伤害值
	int m_nDamageTpy{}; //飙血类型
	int m_nId{};   //唯一ID
};


//*************************************************************************************************
//
//*************************************************************************************************
class DeathEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	DeathEvent() {};
	virtual ~DeathEvent() {};

public:
	EventType m_type;
	int m_nId{};
};



//*************************************************************************************************
//
//*************************************************************************************************
class BeatBackEvent// : public bzbee::Event  击退
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	BeatBackEvent() {};
	virtual ~BeatBackEvent() {};

public:
	EventType m_type;
	float m_X{};
	float m_Y{};
};


//*************************************************************************************************
//
//*************************************************************************************************
class WeaponEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	WeaponEvent() {};
	virtual ~WeaponEvent() {};

public:
	EventType m_type;
	int m_nWeaponId{};
};


//*************************************************************************************************
//
//*************************************************************************************************
class SkillStopContinueEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	SkillStopContinueEvent() {};
	virtual ~SkillStopContinueEvent() {};

public:
	EventType m_type;
	int m_nId{};
};


//*************************************************************************************************
//
//*************************************************************************************************
class BuildEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	BuildEvent() {};
	virtual ~BuildEvent() {};

public:
	EventType m_type;
	int m_nId{};
	bool m_bBuild{};
};

//*************************************************************************************************
//
//*************************************************************************************************
class HeroWalkEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	HeroWalkEvent() {};
	virtual ~HeroWalkEvent() {};

public:
	EventType m_type;
	int m_nX{0};
	int m_nY{0};
	int m_nZ{0};
};


//*************************************************************************************************
//
//*************************************************************************************************
class UITouchEvent// : public bzbee::Event
{
public:
	enum class TouchEvent_Tpy
	{
		TouchEvent_Begin,
		TouchEvent_Move,
		TouchEvent_End,
	};
public:
	UITouchEvent() {};
	virtual ~UITouchEvent() {};

public:
	TouchEvent_Tpy m_type;
	float m_nX{ 0.f };
	float m_nY{ 0.f };
};


//*************************************************************************************************
//
//*************************************************************************************************
class WindowEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
		Window_NpcTalk,
		Window_NpcFuncTalk,
		Window_DefeccePlace,
		Window_CharaterInfo,
		Window_Bag,
		Window_DebugView,
		Window_ItemTips,
		Window_ItemTipsCompare,
		Window_PlantView,
		Window_DefenceInfo,
		Window_PlantsInfo,
		Window_HouseInfo,
		Window_TeamInfo,
		Window_MineDev,
		Window_SetUp,
		Window_OperaSetUp,
		Window_WorldMap,
		Window_InsMap,
		Window_InsSelect,
		Window_TroopsView,
		Window_General,
		Window_WareHouse,
		Window_Craft,
		Window_CraftConfirm,
		Window_Shop,
		Window_BuyConfirm,
		Window_NpcSelect,
		Window_ItemSelect,
		Window_HouseSelect,
		Window_Lottery,
		Window_ItemGenelSel,
		Window_NpcAward,
		Window_NpcManager,
		Window_CityInfo,
		Window_FortInfo,
		Window_CrystalInfo,
		Window_NpcInfo,
		Window_InsChestSelect,
		Window_InsDefeat,
		Window_InsGenSkills,
		Window_InsAddExp,
		Window_Notes,
		Window_Help,
		Window_Illu,
		Window_Achieve,
		Window_GenIns,
	};
public:
	WindowEvent() {};
	virtual ~WindowEvent() {};

public:
	EventType m_type;
	NS::I64 nParam{};
	NS::int32 nParam1{};
	NS::int32 nParam2{};
	NS::int32 nParam3{};
};

//*************************************************************************************************
//
//*************************************************************************************************
class MsgBoxEvent// : public bzbee::Event
{
public:
	MsgBoxEvent() {};
	virtual ~MsgBoxEvent() {};

public:
	NS::uint m_nUIType{0};
	NS::uint m_nBoxType{0};
};

//*************************************************************************************************
//
//*************************************************************************************************
class HorseEvent// : public bzbee::Event
{
public:
	HorseEvent() {};
	virtual ~HorseEvent() {};
};


//*************************************************************************************************
//
//*************************************************************************************************
class TipsCloseEvent// : public bzbee::Event  关闭tips
{
public:
	TipsCloseEvent() {};
	virtual ~TipsCloseEvent() {};
};


//*************************************************************************************************
//
//*************************************************************************************************
class ProfessEvent// : public bzbee::Event
{
public:
	ProfessEvent() {};
	virtual ~ProfessEvent() {};
};

//*************************************************************************************************
//
//*************************************************************************************************
class SkillEvent// : public bzbee::Event
{
public:
	SkillEvent() {};
	virtual ~SkillEvent() {};
public:
	int m_nSkillId{};
};

//*************************************************************************************************
//
//*************************************************************************************************
class WeaponChangeEvent// : public bzbee::Event
{
public:
	WeaponChangeEvent() {};
	virtual ~WeaponChangeEvent() {};
public:
	int nWeaponId{};
};


//*************************************************************************************************
//
//*************************************************************************************************
class WeaponPreEquipEvent// : public bzbee::Event
{
public:
	WeaponPreEquipEvent() {};
	virtual ~WeaponPreEquipEvent() {};
};

//*************************************************************************************************
//英雄注册完成
//*************************************************************************************************
class HeroInitEvent {
public:
	HeroInitEvent() {};
	virtual ~HeroInitEvent() {};
};

//*************************************************************************************************
//
//*************************************************************************************************
class HorseChangeEvent// : public bzbee::Event
{
public:
	HorseChangeEvent() {};
	virtual ~HorseChangeEvent() {};
public:
	int m_nSkill{};
};

//*************************************************************************************************
//
//*************************************************************************************************
class HorseSkillEvent// : public bzbee::Event
{
public:
	HorseSkillEvent() {};
	virtual ~HorseSkillEvent() {};
};


//*************************************************************************************************
//
//*************************************************************************************************
class HouseEvent// : public bzbee::Event
{
public:
	HouseEvent() {};
	virtual ~HouseEvent() {};
};

//*************************************************************************************************
//
//*************************************************************************************************
class NavigationEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	NavigationEvent() {};
	virtual ~NavigationEvent() {};

public:
	EventType m_type;
};


//*************************************************************************************************
//
//*************************************************************************************************
class CityEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
		EventType_CityDoor,
	};
public:
	CityEvent() {};
	virtual ~CityEvent() {};

public:
	EventType m_type;
	int		  m_param{ 0 };
};

//*************************************************************************************************
//
//*************************************************************************************************
class NpcDispearEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	NpcDispearEvent() {};
	virtual ~NpcDispearEvent() {};

public:
	EventType m_type;
	NS::I64	  m_uuid{ 0 };
};


//*************************************************************************************************
//
//*************************************************************************************************
class SkillCoolTimeEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	SkillCoolTimeEvent() {};
	virtual ~SkillCoolTimeEvent() {};
public:
	EventType m_type;
};

//*************************************************************************************************
//
//*************************************************************************************************
class UILoadintEvent// : public bzbee::Event
{
public:
	enum class LoadingType
	{
		Unknown,
		Pre_Load,
		Nxt_Load
	};
public:
	UILoadintEvent() {};
	virtual ~UILoadintEvent() {};

public:
	int m_loadstate;
	LoadingType m_type;
};

//*************************************************************************************************
//
//*************************************************************************************************
class UILayoutEvent// : public bzbee::Event
{
public:
	UILayoutEvent() {};
	virtual ~UILayoutEvent() {};

public:
	int m_nBang{0};
};

//*************************************************************************************************
//
//*************************************************************************************************
class GeneralsEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
		Gen_Add,
		Gen_Dead,
	};
public:
	GeneralsEvent() {};
	virtual ~GeneralsEvent() {};

public:
	EventType m_type;
	int objId{};
};

//*************************************************************************************************
//
//*************************************************************************************************
class InsEndEvent// : public bzbee::Event  副本通关
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	InsEndEvent() {};
	virtual ~InsEndEvent() {};

public:
	EventType m_type;
};


//*************************************************************************************************
//
//*************************************************************************************************
class GeneralsSkillEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
		Gen_INF,  //步兵
		Gen_CAV,  //骑兵
		Gen_ARC,  //弓兵
		Gen_BLA,  //大刀兵
		Gen_MAG,  //法术兵
		Gen_INF_C,  //步兵取消
		Gen_StartBuff,  //开局全力冲击
	};
public:
	GeneralsSkillEvent() {};
	virtual ~GeneralsSkillEvent() {};

public:
	EventType m_type;
};

//*************************************************************************************************
//
//*************************************************************************************************
class UItemClickEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	UItemClickEvent() {};
	virtual ~UItemClickEvent() {};

public:
	EventType m_type;
	NS::I64 m_uuid{ 0 };
};

//*************************************************************************************************
//
//*************************************************************************************************
class UIHurtShowEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	UIHurtShowEvent() {};
	virtual ~UIHurtShowEvent() {};

public:
	EventType m_type;
	bool bHideGen{ false };
	bool bHideMonster{ false };
	bool bHideMonsterHud{ false };
	bool bHideSoliderWeapon{ false };
};

//*************************************************************************************************
//
//*************************************************************************************************
class UITapLoginEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
		EventType_Login,
		EventType_Video
	};
public:
	UITapLoginEvent() {};
	virtual ~UITapLoginEvent() {};

public:
	EventType m_type;
	std::string strUuid;
	std::string strToken;
};

//*************************************************************************************************
//
//*************************************************************************************************
class UIGuidEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		EventType_Achieve,
		EventType_House,
		EventType_Npc,
		EventType_Gen,
	};
public:
	UIGuidEvent() {};
	virtual ~UIGuidEvent() {};

public:
	EventType m_type{ EventType::EventType_Achieve };
	NS::I64 nDetailId{ 0 };
	int nIdx{ 0 };
};

//*************************************************************************************************
//
//*************************************************************************************************
class UIGenChangeEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	UIGenChangeEvent() {};
	virtual ~UIGenChangeEvent() {};

public:
	EventType m_type;
	NS::I64 uuid{ 0 };
	int nObjectId{ 0 };
};

//*************************************************************************************************
//
//*************************************************************************************************
class UIIosAgreementEvent// : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
	};
public:
	UIIosAgreementEvent() {};
	virtual ~UIIosAgreementEvent() {};

public:
	EventType m_type;
};



//*************************************************************************************************
//
//*************************************************************************************************
class HandlerUIEvents// : public NS::MsgHandler
{
public:
	HandlerUIEvents();
	virtual ~HandlerUIEvents();

	static HandlerUIEvents& getInstance();

public:
	DECLARE_UI_MYEVENT(BallEvent);
	DECLARE_UI_MYEVENT(SightEvent);
	DECLARE_UI_MYEVENT(AttackEvent);
	DECLARE_UI_MYEVENT(StartContinueAttackEvent);
	DECLARE_UI_MYEVENT(StopContinueAttackEvent);
	DECLARE_UI_MYEVENT(HoverAttackEvent);
	DECLARE_UI_MYEVENT(UseWeaponExternEvent);
	DECLARE_UI_MYEVENT(HurtEvent);
	DECLARE_UI_MYEVENT(DeathEvent);
	DECLARE_UI_MYEVENT(WeaponEvent);
	DECLARE_UI_MYEVENT(WeaponPreEquipEvent);
	DECLARE_UI_MYEVENT(SkillStopContinueEvent);
	DECLARE_UI_MYEVENT(BuildEvent);
	DECLARE_UI_MYEVENT(WindowEvent);
	DECLARE_UI_MYEVENT(HeroWalkEvent);
	DECLARE_UI_MYEVENT(MsgBoxEvent);
	DECLARE_UI_MYEVENT(HorseEvent);
	DECLARE_UI_MYEVENT(ProfessEvent);
	DECLARE_UI_MYEVENT(SkillEvent);
	DECLARE_UI_MYEVENT(WeaponChangeEvent);
	DECLARE_UI_MYEVENT(HorseChangeEvent);
	DECLARE_UI_MYEVENT(HorseSkillEvent);
	DECLARE_UI_MYEVENT(HouseEvent);
	DECLARE_UI_MYEVENT(NavigationEvent);
	DECLARE_UI_MYEVENT(BeatBackEvent);
	DECLARE_UI_MYEVENT(CityEvent);
	DECLARE_UI_MYEVENT(SkillCoolTimeEvent);
	DECLARE_UI_MYEVENT(UITouchEvent);
	DECLARE_UI_MYEVENT(HeroInitEvent);
	DECLARE_UI_MYEVENT(MaterialEvent);
	DECLARE_UI_MYEVENT(NpcDispearEvent);
	DECLARE_UI_MYEVENT(UILoadintEvent);
	DECLARE_UI_MYEVENT(TipsCloseEvent);
	DECLARE_UI_MYEVENT(GeneralsEvent);
	DECLARE_UI_MYEVENT(InsEndEvent);
	DECLARE_UI_MYEVENT(GeneralsSkillEvent);
	DECLARE_UI_MYEVENT(UILayoutEvent);
	DECLARE_UI_MYEVENT(UItemClickEvent);
	DECLARE_UI_MYEVENT(UIHurtShowEvent);
	DECLARE_UI_MYEVENT(UITapLoginEvent);
	DECLARE_UI_MYEVENT(UIGuidEvent);
	DECLARE_UI_MYEVENT(UIGenChangeEvent);
	DECLARE_UI_MYEVENT(UIIosAgreementEvent);
};



