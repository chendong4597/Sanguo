// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIEvents.h"
#include "GameAppExtern.h"
#include "Delegates/DelegateCombinations.h"
#include "RootEventObject.generated.h"

#define DECLARE_MYEVENT_POST(evt_name) DECLARE_EVENT_OneParam(UEgine, F##evt_name, const evt_name&) \
public: \
	virtual bool postEvent(const evt_name& evt) override { m_evt##evt_name.Broadcast(evt); return true;} \
	F##evt_name& On##evt_name() { return m_evt##evt_name; } \
private: \
	F##evt_name m_evt##evt_name;


#define RG_REGISTER_MYEVENT(evt , fun_name) UReichGameInstance::getRootObject()->On##evt().AddUObject(this, fun_name); 
#define UNRG_REGISTER_MYEVENT(evt , dele) UReichGameInstance::getRootObject()->On##evt().RemoveAll(dele); 
/**
 *
 */
UCLASS()
class SANGUO_API URootEventObject : public UObject, public DataHandler
{
	GENERATED_BODY()
	//DataHandler& URootEventObject::getInstance() override;

public:
	void InitializeEvents();
protected:
	void PreLoadedMapEvent(const FString& strWord);
	void LoadedMapEvent(UWorld* world);

public:
	DECLARE_MYEVENT_POST(HandlerCraftEvent);
	DECLARE_MYEVENT_POST(HandlerLoginStateEvent);
	DECLARE_MYEVENT_POST(HandlerTransterEvent);
	DECLARE_MYEVENT_POST(HandlerItemEvent);
	DECLARE_MYEVENT_POST(HandlerTowerBuildEvent);
	DECLARE_MYEVENT_POST(HandlerPlantEvent);
	DECLARE_MYEVENT_POST(HandlerEnterMapStateEvent);
	DECLARE_MYEVENT_POST(HandlerMonsterEvent);
	DECLARE_MYEVENT_POST(HandlerBattleEvent);
	DECLARE_MYEVENT_POST(HandlerHouseEvent);
	DECLARE_MYEVENT_POST(HandlerRoleEvent);
	DECLARE_MYEVENT_POST(HandlerNpcEvent);
	DECLARE_MYEVENT_POST(HandlerProtectorEvent);
	DECLARE_MYEVENT_POST(HandlerTipsEvent);
	DECLARE_MYEVENT_POST(HandlerItemGetEvent);
	DECLARE_MYEVENT_POST(HandlerStoreEvent);
	DECLARE_MYEVENT_POST(HandlerNpcAliveEvent);
	DECLARE_MYEVENT_POST(HandlerHeroAppearanceEvent);
	DECLARE_MYEVENT_POST(HandlerHeroAttrChangeEvent);
	DECLARE_MYEVENT_POST(HandlerRoleBuffEvent);
	DECLARE_MYEVENT_POST(HandlerRoleStatusEvent);
	DECLARE_MYEVENT_POST(HandlerAchieveEvent);
	DECLARE_MYEVENT_POST(HandlerSoundEvent);

	//¶ÏÏß
	DECLARE_MYEVENT_POST(HandlerNetEvent);

	//server data
	DECLARE_MYEVENT_POST(MsgJoinMapG2C);
	DECLARE_MYEVENT_POST(MsgItemMaterialS2C);
	DECLARE_MYEVENT_POST(MsgPlayerInfoGS2C);
	DECLARE_MYEVENT_POST(MsgItemUseResultS2C);
	DECLARE_MYEVENT_POST(MsgTeamDungeonReadyToGo_S2C);

	DECLARE_MYEVENT_POST(MsgDungeonBossChest_MS2C);
	DECLARE_MYEVENT_POST(MsgDungeonBossOpenChestRs_MS2C);
	DECLARE_MYEVENT_POST(MsgDungeonNextMob_S2C);
	DECLARE_MYEVENT_POST(MsgPlatformAccountCheckLS2C);
	DECLARE_MYEVENT_POST(MsgRegisterAccountLS2C);
	DECLARE_MYEVENT_POST(MsgPlayerADNVideoAwardS2C);
	DECLARE_MYEVENT_POST(MsgDeleteAccountS2C);

	DECLARE_MYEVENT_POST(MsgPlayerSetAvatorResultS2C);

	//local data
	DECLARE_MYEVENT_POST(MsgMapObjectUnderAttackG2C);
	DECLARE_MYEVENT_POST(MsgSkillEffectDotDmgMS2C);
	DECLARE_MYEVENT_POST(MsgPlayerNormalAttackResS2C);
	DECLARE_MYEVENT_POST(MsgSkillEffectDmgMS2C);
	DECLARE_MYEVENT_POST(MsgCharacterDeadS2C);
	DECLARE_MYEVENT_POST(MsgInteractionInWorldS2C);
	DECLARE_MYEVENT_POST(MsgInteractionWalkS2C);
	DECLARE_MYEVENT_POST(MsgInteractionS2C);
	DECLARE_MYEVENT_POST(MsgMagicEffectAppenceS2C);
	DECLARE_MYEVENT_POST(MsgInteractionTurnS2C);
	DECLARE_MYEVENT_POST(MsgSkillEffectRestoreMS2C);
};
