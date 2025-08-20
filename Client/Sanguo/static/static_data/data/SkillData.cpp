/*!
* \file ItemData.cpp
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#include "stdafx.h"
#include "SkillData.h"
#include "role/Monsters.h"

using namespace NS;



SkillData::SkillData(const int32 casterId, NS::I64 uuid)
	: m_id(uuid)
	, m_caster(casterId)
{
	if (!m_id) m_id = genUniqueId(casterId);

	// 施法者属性快照
	std::shared_ptr<MapObj> pCasterMapObj = MapObjMgr::getInstance().findObj(casterId);
	if (!pCasterMapObj) { return; }
	std::shared_ptr<Role> pCasterRoleData = std::dynamic_pointer_cast<Role>(pCasterMapObj);
	if (!pCasterRoleData) { return; }

	m_casterAttrSnapshot = CharacterAttributeSnapshot();
	m_eCategory = pCasterRoleData->getCategory();
	m_nGroupId = pCasterRoleData->getGroupId();
	m_nMonsterRare = pCasterRoleData->getMobRare();
}

SkillData::~SkillData()
{}


NS::I64 SkillData::genUniqueId(const int32 casterId)
{
	static uint32 g_counter{0};
	g_counter++;

	return ((((NS::I64)casterId) << 32) | g_counter);
}


SkillDataPtr SkillData::create(const int32 casterId, NS::I64 uuid)
{
	return std::make_shared<SkillData>(casterId, uuid);
}


void SkillData::setTargetList(const int32* pTargetIds, int32 targetCount)
{
	for (int32 i = 0u; i < targetCount; ++i)
	{
		m_targetList.push_back(pTargetIds[i]);
	}
}

///////////////////////////////////////////////////////////////////////


SkillDataMgr::SkillDataMgr()
{}


SkillDataMgr::~SkillDataMgr()
{}


SkillDataMgr& SkillDataMgr::getInstance()
{
	static SkillDataMgr mgr;
	return mgr;
}


void SkillDataMgr::terminate()
{}


bool SkillDataMgr::add(SkillDataPtr pSkillData)
{
	if (!pSkillData) return false;

	return m_data.emplace(pSkillData->getUniqueId(), pSkillData).second;
}


bool SkillDataMgr::remove(const NS::I64 id)
{
	auto iter = m_data.find(id);
	if (iter == m_data.end()) return false;
	
	m_data.erase(iter);
	return true;
}


SkillDataPtr SkillDataMgr::find(const NS::I64 id)
{
	auto iter = m_data.find(id);
	if (iter == m_data.end()) return nullptr;

	return iter->second;
}
