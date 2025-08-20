
/*!
 * \file Monster.h
 *
 * \author zhijieli
 * \date 2016/12/22
 *
 * 
 */

#pragma once

#include "data/role/Role.h"
#include "../../config/MonsterCfg.h"
#include <unordered_map>
#include <memory>

class Monster : public Role
{
public:
	Monster(bzbee::int32 id, bzbee::int32 insId, bzbee::int32 defId);

	void onInit() override;

	const MonsterCfg::MonsterType* getMonsterType() const { return m_monsterType; }
	bzbee::int32 getInsId() const { return m_insId; }
	bzbee::int32 getDefId() const { return m_defId; }
	NS::I64 getMasterUuid() const { return m_npcUuid; }
	bzbee::uint8 getEventType() const { return m_eventType; }

	void setMasterUuid(NS::I64 npcUuid) { m_npcUuid = npcUuid; }

	virtual void calcAttr(bool bInherBuff = true) override;

	virtual float getBaseAttr(MsgPB::ROLE_EFFECT_TYPE type, bool isInhert) const override;

	virtual std::string GetBattleSound(int nSountTpy) override;

protected:
	bzbee::int32 m_defId{};
	bzbee::int32 m_insId{};
	bzbee::uint8 m_eventType{};		// The event from where the monster appears.
	NS::I64 m_npcUuid{};
	const MonsterCfg::MonsterType* m_monsterType{};
	bool m_isLeaving{};
};

//////////////////////////////////////////////////////////////////////////
class MonsterDataMgr
{
public:
	using MapObjId2Monster = std::unordered_map<bzbee::uint, std::shared_ptr<Monster>>;
public:
	static MonsterDataMgr& getInstance();

	std::shared_ptr<Monster> getMonster(bzbee::int32 mapObjId);

	std::shared_ptr<Monster> getMonsterByTypeId(int mobTypeId);

	bool addMonster(const std::shared_ptr<Monster>& monster);
	bool removeMonster(bzbee::int32 mapObjId);
	int getNumMonster() const { return (int)m_monsters.size(); }
	bool isHaveCreate(NS::I64 serverMapObjId);
	void Clear() { m_monsters.clear(); }
	

	const MapObjId2Monster& getAllMonsters() const { return m_monsters; }

private:
	
	MapObjId2Monster m_monsters;
};
