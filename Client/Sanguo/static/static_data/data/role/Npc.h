/*!
 * \file Npc.h
 * \date 02-23-2017 16:08:54
 *
 * \author yuechuanxu
 */
#pragma once
#include <unordered_map>
#include <memory>
#include "Monsters.h"

class Npc : public Monster
{
public:
	Npc(NS::int32 id, NS::int32 insId, NS::int32 defId);
	void setPatrol(bool bPatrol) { m_bPatrol = bPatrol; }
	bool isPatrol() {return m_bPatrol; }
	void setIsBadgeNpc(bool isBadgeNpc) { m_isBadgeNpc = isBadgeNpc; };
	bool isBattleNpc() { return m_isBattleNpc; }
	bool isBadgeNpc() { return m_isBadgeNpc; };
	void calcAttr(bool bInherBuff = true) override;
	void setCalcAttrTriggerAchieveCallback(std::function<void(const Npc* npc)> fun) { m_calcAttrTriggerAchieveCallback = fun; };

	float getBaseAttr(MsgPB::ROLE_EFFECT_TYPE type, bool isInhert) const override;
	int CalculateMaxHp() const override;
	int getMaxMp() const override;
	void setFollowPlayer(bool isFollow) { m_isFollowPlayer = isFollow; };
	bool isFollowPlayer() { return m_isFollowPlayer; };
	virtual float getAttr(MsgPB::ROLE_EFFECT_TYPE type) const override;
	void setOwnerMapObjId(NS::I64 id) {m_ownerMapObjId = id; };
	NS::I64 getOwnerMapObjId() { return m_ownerMapObjId; };
    virtual void setPos(const glm::vec3& pos) override;
	void setIsMyNpc(bool isMe) { m_isMyNpc = isMe; };
	bool getIsMyNpc() const { return m_isMyNpc; };
	void setIsAtPvpTower(bool isAt) { m_isAtPvpTower = isAt; };
	void setEnemyPlayerLevel(int level) { m_enemyPlayerLevel = level; };
	virtual const std::shared_ptr<ItemData> getCurrNorWeapon() const override;
	void setNpcProtectorItemUuid(NS::I64 nItemUuid) { m_nBadgeItemUuid = nItemUuid; }
	NS::I64 getNpcProtectorItemUuid() const { return m_nBadgeItemUuid; }
	void SetProtectorUuid(NS::I64 nProtectorUuid);
	NS::I64 GetProtectorUuid() const { return m_ProtectorUuid; }

	virtual std::string GetBattleSound(int nSountTpy) override;

private:
	//是否是巡逻NPC
	bool m_bPatrol{ false };
	bool m_isBadgeNpc{ false };
	bool m_isFollowPlayer{ false };
	bool m_isBattleNpc{ false };
	bool m_isAtPvpTower{ false };
	bool m_isMyNpc{ true };
	int m_enemyPlayerLevel{0};
	NS::I64 m_ProtectorUuid{};
	NS::I64 m_ownerMapObjId{0};
	std::vector<int> m_vecNeedCalAtterType{};
	std::function<void(const Npc* npc)> m_calcAttrTriggerAchieveCallback{ nullptr };
	NS::I64 m_nBadgeItemUuid{ -1 }; // associate with NpcProtectorData, not all npc, default -1
};


//////////////////////////////////////////////////////////////////////////


class NpcMgr
{
public:
	using MapObjId2Npc = std::unordered_map<NS::uint, std::shared_ptr<Npc>>;

public:
	static NpcMgr& getInstance();

	void Clear();

	std::shared_ptr<Npc> getNpcByDefId(int32 defId);
	std::shared_ptr<Npc> getNpc(NS::int32 mapObjId);
	std::shared_ptr<Npc> getNpcByProtectorUuid(NS::I64 protectorId);

	bool addNpc(const std::shared_ptr<Npc>& npc);
	bool removeNpc(NS::int32 mapObjId);
	int getNumNpc() const { return (int)m_npcs.size(); };

	const MapObjId2Npc& getNpcs() { return m_npcs; }

private:
	MapObjId2Npc m_npcs;
};
