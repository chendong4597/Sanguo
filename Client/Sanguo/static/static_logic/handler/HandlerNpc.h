/*!
 * \file HandlerNpc.h
 * \date 02-23-2017 16:06:41
 *
 * \author yuechuanxu
 */
#pragma once
#include "BaseTypeDefine.h"
#include <MsgHandler.h>
#include <memory>
#include "data/NpcProtectorData.h"
#include "common/Timer.h"


namespace MsgPB
{
	class MsgNpcFeaturesResultS2C;
	class MsgNpcAppearance;
	class MsgNpcDestroyG2C;
	class MsgProtectorSyncS2C;
	class MsgProtectorUpdateS2C;
	class MsgProtectorRemoveS2C;
	class MsgProtectorGetStatusResultS2C;
	class MsgProtectorPatrolResultS2C;
	class MsgMobAppearance;
	class MsgMobInteractiveS2C;
	class MsgProtectorRentResultS2C;
	class MsgProtectorReviveC2S;
	class MsgProtectorReviveResultS2C;
	class MsgBuildingRoomCondition_S2C;
}


class Npc;
class CheckAllConditionRet;
class CheckAllCondInfo;
class HandlerBuildingEvent;

class HandlerNpc : public NS::MsgHandler
{
	RTTI_DEF(HandlerNpc);
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:
	HandlerNpc();
	virtual ~HandlerNpc();
	static HandlerNpc& getInstance();
    static bool isInitialized();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

	bool requestNpcProtectorClaimRentAndGift(NS::I64 nUuid);
	bool requestNpcProtectorStatusResult(NS::I64 nUuid);
	bool requestNpcProtectorPatrolResult(NS::I64 nUuid);

	bool requestNpcFeatures(int npcId, NS::I64 roomPos);
	bool requestNpcBadgeUpgrade(int npcId, NS::I64 roomPos);
	bool requestNpcRentsAndGifts();

	bool onNpcInteracted(NS::int32 npcMapObjId,bool isLocalCreate);
	bool requestMobInteractiveC2S(NS::int32 npcMapObjId, NS::int32 npcActiveEffectId);

	bool requestNpcRevive(NS::I64 nUuid);

	bool addNpc(int mapObjId, NS::I64 insId, int defId, const glm::vec3& pos, bool createWithLocal = false,bool isBadgeNpc = false);

	bool dragonBallListReq(int interactedMapObjId);
	bool dragonBallTrigReq(NS::int32 id);

	void onNpcBadgeDataSyncS2cEvent();

	bool npcEquipReq(NS::I64 badgeUuuid,NS::I64 itemUuuid,int equipPos);
	bool npcBathEquipRequest(NS::I64 badgeUuid, std::vector<std::tuple<NS::I64, NS::int32>> equipments);
	bool npcUseAttrReq(NS::I64 badgeUuuid, NS::I64 itemUuuid , int count,int curNum , int maxNum);

	void reviveNpc(int npcDefId);

	void setNpcFeedPress(bool bPress) { m_bPressFeed = bPress; }
	bool reviveNpcReq(NS::I64 badgeUuid);
	void setIsDonotCreate(NS::int32 iDonotCreate) { m_iDonotCreate = iDonotCreate; }

	NS::int32 getDonotCreate() { return m_iDonotCreate; }

	void setUnFollowNpc();

	void setRebirthTips(bool bTips) { m_bRebirthTips = bTips; }
	bool getRebirthTips() { return m_bRebirthTips; }

	void handlerReliveNpc(int badgeId);
public:
	void onMsgNpcProtectorAppearance(const MsgPB::MsgMobAppearance& msg);
	void onMsgUpdateNpcProtectorData(const MsgPB::MsgProtectorUpdateS2C& msg);
	void onBuildingChangeEvt(const HandlerBuildingEvent &evt);
	//void showPlaceBadgeErrorTip(CheckConditionRet &rc);
	void scanAllBadgeOfAtMap();
	void setFollowNpc(int npcDefId);

private:
	void onMsgNpcAppearance(const MsgPB::MsgNpcAppearance& msg);
	void onMsgNpcDestroy(const MsgPB::MsgNpcDestroyG2C& msg);
	void onMsgNpcFeaturesRes(const MsgPB::MsgNpcFeaturesResultS2C& msg);
	void onMsgNpcProtectorClaimRentAndGiftRes(const MsgPB::MsgProtectorRentResultS2C& msg);
	void onMsgNpcRentsAndGiftsRes(const MsgPB::MsgProtectorMultiRentS2C& msg);
	void onMsgGetNpcProtectorStatusResult(const MsgPB::MsgProtectorGetStatusResultS2C& msg);
	void onMsgProtectorPatrolResult(const MsgPB::MsgProtectorPatrolResultS2C& msg);
	void onMsgSyncNpcProtectorData(const MsgPB::MsgProtectorSyncS2C& msg);
	void onMsgRemoveNpcProtectorData(const MsgPB::MsgProtectorRemoveS2C& msg);
	void onMsgMobInteractiveS2C(const MsgPB::MsgMobInteractiveS2C& msg);
	void onMsgProtectorReviveResultS2C(const MsgPB::MsgProtectorReviveResultS2C& msg);
	void onMsgBuildingRoomConditionS2C(const MsgPB::MsgBuildingRoomCondition_S2C& msg);
	//void showRentAndGiftDialog(UiMsgBoxCommonMessage::StyleParam param);
	void mapRoomServerTipInfo2UiViewTipInfo(CheckAllConditionRet &serverInfo, CheckAllCondInfo &viewInfo);
	//void onMsgDragonBallLisResS2C(const MsgPB::MsgDragonBallLisResS2C& msg);
	void mapRoomServerMsgInfo2UiviewTipInfo(const MsgPB::MsgBuildingRoomCondition_S2C& msg, CheckAllCondInfo &viewInfo);
	//void onMsgDragonBallTrigResS2C(const MsgPB::MsgDragonBallTrigResS2C& msg);
	//void onMsgDragonBallTrigUIResS2C(const MsgPB::MsgDragonBallTrigUIResS2C& msg);
	//void onNpcBadgeStateChagedEvent(const NpcBadgeStateChangedS2CEvent& evt);
	void onMsgProtectorEquipResS2C(const MsgPB::MsgProtectorEquipResS2C& msg);
	void onMsgProtectorUseEssenceResS2C(const MsgPB::MsgProtectorUseEssenceResS2C& msg);
	void onMsgProtectorRebornAtOnceS2C(const MsgPB::MsgProtectorRebornAtOnceS2C& msg);
	//void onNpcBadgeReciveEvent(const NpcBadgeReciveEvent& evt);
	//void onNpcBadgeEnableEvent(const NpcBadgeEnableFuncEvent& evt);
	void onTriggerAchieveEvent(const Npc* npc);
	int findAmuletPos();

	void onRebirthTimer(const TimerEvent& evt, int level);

private:
	int n_ndragonBallNpcMapObjId{};
	int m_curUseNum;
	int m_maxUseNum;
	int m_badgeUuid{ 0 }; // 穿装备的npc对应的徽章uuids
	int m_npcId{ 0 }; // 使用精华的npc id
	int m_itemId{ 0 }; // 使用的精华id
	bool m_bPressFeed{ false };
	NS::int32 m_iDonotCreate{ -1 };

	bool m_bRebirthTips{ false };

	NS::I64 m_totalTm{ 0 };
	NS::uint m_rebirthTimerId{ 0 };
};

class HandlerNpcAtterEvent : public NS::Event
{
public:
	enum class EventType
	{
		sccu,
		equip,
	};

public:
	HandlerNpcAtterEvent() {};
	virtual ~HandlerNpcAtterEvent() {};

public:
	EventType m_type{ EventType::sccu };
	NS::I64 uuid{};
};

class HandlerNpcDeathEvent : public bzbee::Event
{
public:
	int m_npcId{};
	int m_cdTime{};
};

class HandlerNpcReliveEvent : public bzbee::Event
{
public:
	int m_npcId{};
};


