/*!
* \file SkillData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include "../config/SkillCfg.h"
#include <list>
#include <memory>
#include "role/Role.h"

class SkillData;
typedef std::shared_ptr<SkillData> SkillDataPtr;


class SkillData
{
public:
	SkillData(const NS::int32 casterId, NS::I64 uuid);
	virtual ~SkillData();

	NS::I64 getUniqueId() const { return m_id; }

	static SkillDataPtr create(const NS::int32 casterId, NS::I64 uuid);

	inline NS::int32 getCasterId() const { return m_caster; }

	inline NS::int32 getTargetId() const { return !m_targetList.empty() ? m_targetList.front() : 0; }

	void setTargetList(const NS::int32* pTargetIds, NS::int32 targetCount);
	const std::list<NS::int32>& getTargetList() const { return m_targetList; }

	inline NS::int32 getSkillIndex() const { return m_skillIndex; }
	inline NS::I64 getItemUuid() const { return m_itemUuid; }

	inline void selectItemSkill(const NS::I64 itemUuid, const NS::int32 skillIndex) { m_itemUuid = itemUuid; m_skillIndex = skillIndex; }

	inline NS::int32 getSpeed() { return m_speed; }
	inline void setSpeed(const NS::int32 speed) { m_speed = speed; }

	const SkillType* getType() { return m_type; }
	inline void setType(const SkillType* pSkillType) { m_type = pSkillType; }

	inline glm::vec2 getStartPosition() const { return m_startPosition; }
	inline void setStartPosition(const glm::vec2& v) { m_startPosition = v; }

	inline glm::vec2 getTargetPosition() const { return m_targetPosition; }
	inline void setTargetPosition(const glm::vec2& v) { m_targetPosition = v; }

	const CharacterAttributeSnapshot& getCasterAttrSnapshot() const { return m_casterAttrSnapshot; }
	NS::int32 getMonsterRare() const { return m_nMonsterRare; }
	MapObj::Category getCasterCategory() const { return m_eCategory; }
	NS::int32 getCasterGroupId() const { return m_nGroupId; }

private:
	static NS::I64 genUniqueId(const NS::int32 casterId);

	NS::int32 m_caster{0};
	CharacterAttributeSnapshot m_casterAttrSnapshot;
	MapObj::Category m_eCategory{ MapObj::Category::None };
	NS::int32 m_nMonsterRare{ 0 };	// 施法者是怪物时，怪物的品级
	NS::int32 m_nGroupId{ 0 };	// 施法者的GroupId，各个创生体跟随
//	NS::int32 m_target{0};
	NS::int32 m_speed{0};
	NS::int32 m_skillIndex{0};
	NS::I64 m_itemUuid{0};
	const SkillType* m_type{};
//	bool m_emit{};

	glm::vec2 m_startPosition {};
	glm::vec2 m_targetPosition{};

	std::list<NS::int32> m_targetList;

	NS::I64 m_id{};
};


class SkillDataMgr
{
	typedef std::unordered_map<NS::I64, SkillDataPtr> SkillDataMap;

	SkillDataMgr();

public:
	virtual ~SkillDataMgr();

	static SkillDataMgr& getInstance();

	void terminate();

	bool add(SkillDataPtr pSkillData);
	bool remove(const NS::I64 id);
	SkillDataPtr find(const NS::I64 id);

private:
	SkillDataMap m_data;
};
