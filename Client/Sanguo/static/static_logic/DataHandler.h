#pragma once
#include "BaseTypeDefine.h"
#include "msg/MsgPlayer.pb.h"
#include "msg/MsgMapServer.pb.h"
#include "msg/MsgHouse.pb.h"
#include "msg/MsgInteraction.pb.h"
#include "msg/MsgSkill.pb.h"
#include "msg/MsgTeam.pb.h"
#include "msg/MsgDungeon.pb.h"
#include "msg/MsgLogin.pb.h"

#define DECLARE_D_MYEVENT_POST(evt) virtual bool postEvent(const evt&) = 0
#define POST_DATA_EVENT(evt) GameAppExtern::getDataHandler().postEvent(evt);

class DataEvent
{

public:
	virtual ~DataEvent(void) = default;
};

//登陆事件
class HandlerLoginStateEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown,
		loginLs,			//
		loginGs,				//
		loaddata,				//
	};
public:
	HandlerLoginStateEvent() {};
	virtual ~HandlerLoginStateEvent() {};

public:
	EventType m_type;
};

//切换地图
class HandlerTransterEvent : public DataEvent
{
public:
	enum class EventTranster
	{
		EventTranster_Login = 0,
		EventTranster_CreateRole,
		EventTranster_Loading,
		EventTranster_PrivateMap,
		EventTranster_DungeonMap,
		EventTranster_World,
	};
public:
	HandlerTransterEvent() {};
	virtual ~HandlerTransterEvent() {};

public:
	EventTranster m_type;
};

//物品
class HandlerItemEvent : public DataEvent
{
public:
	enum class EventItem
	{
		EventItem_Null = 0,
		EventItem_update,
		EventItem_equip,
		EventItem_del,
	};
public:
	HandlerItemEvent() {};
	virtual ~HandlerItemEvent() {};

public:
	NS::I64 m_nIggId{};
	EventItem m_type{ EventItem::EventItem_update };
};

//塔防
class HandlerTowerBuildEvent : public DataEvent
{
public:
	enum class EventTowerBuild
	{
		EventTowerBuild_Null = 0,
		EventTowerBuild_List,
		EventTowerBuild_Update,
		EventTowerBuild_Add,
		EventTowerBuild_Del,
	};
public:
	HandlerTowerBuildEvent() {};
	virtual ~HandlerTowerBuildEvent() {};

public:
	NS::I64 m_nIggId{};
	NS::I64 m_uuid{};
	int m_nPosX{};
	int m_nPosY{};
	EventTowerBuild m_type{ EventTowerBuild::EventTowerBuild_Null };
};

//地图
class HandlerEnterMapStateEvent : public DataEvent  //进入地图是否成功，成功则初始化所有物件
{
public:
	enum class EnterMapState
	{
		EnterMapState_Null = 0,
		EnterMapState_Suc,
		EnterMapState_Fail,
	};
public:
	HandlerEnterMapStateEvent() {};
	virtual ~HandlerEnterMapStateEvent() {};

public:
	EnterMapState m_type;
};

//种植
class HandlerPlantEvent : public DataEvent
{
public:
	enum class EventPlant
	{
		EventPlant_Null = 0,
		EventPlant_List,
		EventPlant_Update,
		EventPlant_Add,
		EventPlant_Del,
	};
public:
	HandlerPlantEvent() {};
	virtual ~HandlerPlantEvent() {};

public:
	NS::I64 m_nIggId{};
	NS::I64 m_uuid{};
	int m_blockId{};
	std::vector<NS::I64> m_vecUuid;
	EventPlant m_type{ EventPlant::EventPlant_Null };
};

//怪物
class HandlerMonsterEvent : public DataEvent
{
public:
	enum class EventMonster
	{
		EventMonster_Null = 0,
		EventMonster_Appearance,
		EventMonster_DisAppearance,
	};
public:
	HandlerMonsterEvent() {};
	virtual ~HandlerMonsterEvent() {};

public:
	NS::I64 m_nIggId{};
	int mapObjId{};
	NS::I64 insId{};
	int defId{};
	EventMonster m_type{ EventMonster::EventMonster_Null };
};

//Protector
class HandlerProtectorEvent : public DataEvent
{
public:
	enum class EventProtector
	{
		EventProtector_Null = 0,
		EventProtector_Update,
		EventProtector_Add,
		EventProtector_Delete,
	};
public:
	HandlerProtectorEvent() {};
	virtual ~HandlerProtectorEvent() {};

public:
	NS::I64 m_nIggId{};
	NS::I64 uuid{};
	EventProtector m_type{ EventProtector::EventProtector_Null };
};

//NPC
class HandlerNpcEvent : public DataEvent
{
public:
	enum class EventNpc
	{
		EventNpc_Null = 0,
		EventNpc_Appearance,
		EventNpc_DisAppearance,
		EventNpc_Equip,
	};
public:
	HandlerNpcEvent() {};
	virtual ~HandlerNpcEvent() {};

public:
	NS::I64 m_nIggId{};
	NS::I64 uuid{};
	int mapObjId{};
	NS::I64 insId{};
	int defId{};
	EventNpc m_type{ EventNpc::EventNpc_Null };
};

//角色
class HandlerRoleEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		UpdateRoleInfo,
		UpdateRoleProperty,
		UpMoneyInfo,
		UpdateEquipInfo,
		createRole,
		UpdateHP,
		Player_state,//玩家走动或者停止的时候，发送
		RoleDie,
		PlayerModifyAvator, // 玩家模型更好(发型、肤色、发色等)
		PlayerLvUp,
	};

public:
	HandlerRoleEvent() {};
	virtual ~HandlerRoleEvent() {};

public:
	EventType m_type{ EventType::Unknown };

	NS::I64 m_uuid{ 0 };
	int m_packetType{ 0 };
	int m_uiPos{ 0 };
	bool m_bHideHUD{ false };
};

//战斗
class HandlerBattleEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		BattleDamage,
	};

	struct UnderAttack
	{
		NS::int32 mapobjectid{}; //攻击对象
		NS::int32 damage{};
		NS::int32 criticl{}; //是否暴击
		NS::int32 currhp{};
		NS::int32 targetSkill{}; //目标技能 比如格挡
		NS::int32 durability{}; //护盾抵消的伤害
	};

public:
	HandlerBattleEvent() {};
	virtual ~HandlerBattleEvent() {};

public:
	EventType m_type{ EventType::Unknown };

	NS::I64 iggid{};
	NS::int32 serialid{};
	NS::I64 idHP{};
	std::vector<UnderAttack> targetInfo;
};

//房子
class HandlerHouseEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		SynHouse,
		UpdateHouse,
		DeleteHouse
	};

public:
	HandlerHouseEvent() {};
	virtual ~HandlerHouseEvent() {};

public:
	EventType m_type{ EventType::Unknown };

	int m_nHouseId{};
};

//锻造
class HandlerCraftEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		Update,
		Craft_Suc,
		Get_Suc,
		Del_Suc,
	};

public:
	HandlerCraftEvent() {};
	virtual ~HandlerCraftEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	NS::I64 m_nProtectorUUID{};
};

//获得物品提示
class HandlerStoreEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
	};

public:
	HandlerStoreEvent() {};
	virtual ~HandlerStoreEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	NS::I64 m_uuid{ 0 };
};


//提示
class HandlerTipsEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		Evt_Coin,
		Evt_Cash,
		Evt_Exp,
		Evt_ItemTips,
		Evt_Material,
	};

public:
	HandlerTipsEvent() {};
	virtual ~HandlerTipsEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	std::string m_strtips;
};

//获得物品提示
class HandlerItemGetEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
	};

public:
	HandlerItemGetEvent() {};
	virtual ~HandlerItemGetEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	NS::I64 m_itemUuid{ 0 };
	int m_itemNum{ 0 };
};

//断线通知
class HandlerNetEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		NET_BREAK,
		NET_FAILED,
	};

public:
	HandlerNetEvent() {};
	virtual ~HandlerNetEvent() {};

public:
	EventType m_type{ EventType::Unknown };
};

//npc复活，武将复活
class HandlerNpcAliveEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
	};

public:
	HandlerNpcAliveEvent() {};
	virtual ~HandlerNpcAliveEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	NS::I64 m_uuid{ 0 };
};

//hero出现在地图，主要是设置 ObjectId
class HandlerHeroAppearanceEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
	};

public:
	HandlerHeroAppearanceEvent() {};
	virtual ~HandlerHeroAppearanceEvent() {};

public:
	EventType m_type{ EventType::Unknown };
};

class HandlerHeroAttrChangeEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		Event_HP_ADD,
		Event_HP_VALUE,
	};

public:
	HandlerHeroAttrChangeEvent() {};
	virtual ~HandlerHeroAttrChangeEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	int nValue{ 0 };
};

class HandlerRoleBuffEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		EventType_Buff_Add,
		EventType_Buff_Update,
		EventType_Buff_Del
	};

public:
	HandlerRoleBuffEvent() {};
	virtual ~HandlerRoleBuffEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	NS::int32 objId{ 0 };
	NS::int32 buffId{ 0 };
};

class HandlerRoleStatusEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
	};

public:
	HandlerRoleStatusEvent() {};
	virtual ~HandlerRoleStatusEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	NS::int32 objId{ 0 };
};

class HandlerAchieveEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		AddAchieve,
		SyncAchieve,
		UpdateAchieve,
		DelAchieve,
		AccomplishAchieve,
	};

public:
	HandlerAchieveEvent() {};
	virtual ~HandlerAchieveEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	NS::I64 m_detailId{ 0 };
};

class HandlerSoundEvent : public DataEvent
{
public:
	enum class EventType
	{
		Unknown = 0,
		Evt_Equip,
		Evt_UnEquip,
		Evt_AddCoin,
		Evt_AddCash,
		Evt_Achieve,
		Evt_ItemMan
	};

public:
	HandlerSoundEvent() {};
	virtual ~HandlerSoundEvent() {};

public:
	EventType m_type{ EventType::Unknown };
};


//*********************************************************************************************************************************************************
//
//*********************************************************************************************************************************************************
using namespace MsgPB;

class DataHandler
{
	//static DataHandler& getInstance() = 0;

public:
	//锻造
	DECLARE_D_MYEVENT_POST(HandlerCraftEvent);

	DECLARE_D_MYEVENT_POST(HandlerLoginStateEvent);
	DECLARE_D_MYEVENT_POST(HandlerTransterEvent);
	DECLARE_D_MYEVENT_POST(HandlerItemEvent);
	DECLARE_D_MYEVENT_POST(HandlerTowerBuildEvent);
	DECLARE_D_MYEVENT_POST(HandlerPlantEvent);
	DECLARE_D_MYEVENT_POST(HandlerEnterMapStateEvent);
	DECLARE_D_MYEVENT_POST(HandlerMonsterEvent);
	DECLARE_D_MYEVENT_POST(HandlerRoleEvent);
	DECLARE_D_MYEVENT_POST(HandlerBattleEvent);
	DECLARE_D_MYEVENT_POST(HandlerTipsEvent);
	DECLARE_D_MYEVENT_POST(HandlerItemGetEvent);
	DECLARE_D_MYEVENT_POST(HandlerNpcAliveEvent);
	DECLARE_D_MYEVENT_POST(HandlerHeroAppearanceEvent);
	DECLARE_D_MYEVENT_POST(HandlerHeroAttrChangeEvent);
	DECLARE_D_MYEVENT_POST(HandlerRoleBuffEvent);
	DECLARE_D_MYEVENT_POST(HandlerRoleStatusEvent);
	DECLARE_D_MYEVENT_POST(HandlerAchieveEvent);
	DECLARE_D_MYEVENT_POST(HandlerSoundEvent);

	//断线
	DECLARE_D_MYEVENT_POST(HandlerNetEvent);

	//房子
	DECLARE_D_MYEVENT_POST(HandlerHouseEvent);

	//NPC
	DECLARE_D_MYEVENT_POST(HandlerProtectorEvent);
	DECLARE_D_MYEVENT_POST(HandlerNpcEvent);

	//商店
	DECLARE_D_MYEVENT_POST(HandlerStoreEvent);

	//servermsg
	DECLARE_D_MYEVENT_POST(MsgJoinMapG2C);
	DECLARE_D_MYEVENT_POST(MsgItemMaterialS2C);
	DECLARE_D_MYEVENT_POST(MsgPlayerInfoGS2C);
	DECLARE_D_MYEVENT_POST(MsgItemUseResultS2C);
	DECLARE_D_MYEVENT_POST(MsgTeamDungeonReadyToGo_S2C);
	DECLARE_D_MYEVENT_POST(MsgPlatformAccountCheckLS2C);
	DECLARE_D_MYEVENT_POST(MsgRegisterAccountLS2C);
	DECLARE_D_MYEVENT_POST(MsgPlayerADNVideoAwardS2C);
	DECLARE_D_MYEVENT_POST(MsgDeleteAccountS2C);

	DECLARE_D_MYEVENT_POST(MsgDungeonBossChest_MS2C);
	DECLARE_D_MYEVENT_POST(MsgDungeonBossOpenChestRs_MS2C);
	DECLARE_D_MYEVENT_POST(MsgDungeonNextMob_S2C);

	DECLARE_D_MYEVENT_POST(MsgPlayerSetAvatorResultS2C);

	//local msg
	DECLARE_D_MYEVENT_POST(MsgMapObjectUnderAttackG2C);
	DECLARE_D_MYEVENT_POST(MsgSkillEffectDotDmgMS2C);
	DECLARE_D_MYEVENT_POST(MsgPlayerNormalAttackResS2C);
	DECLARE_D_MYEVENT_POST(MsgSkillEffectDmgMS2C);
	DECLARE_D_MYEVENT_POST(MsgCharacterDeadS2C);
	DECLARE_D_MYEVENT_POST(MsgInteractionInWorldS2C);
	DECLARE_D_MYEVENT_POST(MsgInteractionWalkS2C);
	DECLARE_D_MYEVENT_POST(MsgInteractionS2C);
	DECLARE_D_MYEVENT_POST(MsgMagicEffectAppenceS2C);
	DECLARE_D_MYEVENT_POST(MsgInteractionTurnS2C);
	DECLARE_D_MYEVENT_POST(MsgSkillEffectRestoreMS2C);
};


