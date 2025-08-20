#include "stdafx.h"
#include "format.h"
#include "ItemData.h"
#include "data/role/Player.h"
#include "common/Heartbeat.h"
#include "../config/ArmorCfg.h"
#include "../config/PlantCfg.h"
#include "../config/BuildingCfg.h"
#include "../config/MedicamentCfg.h"

using namespace NS;
using namespace MsgPB;

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp)
{
	return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
		|| std::abs(x - y) < std::numeric_limits<T>::min();
}

ItemData::ItemData()
{

}

ItemData::~ItemData()
{

}

bool ItemData::initialize()
{
	return true;
}


void ItemData::reset()
{
	m_uuid = 0;
	m_itemId = 0;
	m_pos = 0;
	m_count = 0;
}

bool ItemData::initWithItemId(int itemId, bool bReset, bool bSetAttr)
{
	if (bReset)
	{
		this->reset();
	}
	setItemId(itemId);
	m_pItem = ItemCfg::getInstance().getItemById(itemId);
	if (!m_pItem)
	{
		return false;
	}
	m_pArmorType = ArmorCfg::getInstance().findArmorType(itemId);
	setSubType(static_cast<MsgPB::ITEM_SUBTYPE>(m_pItem->n_sub_type));
	m_pWeaponCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(itemId);
	
	return true;
}

void ItemData::setSubType(MsgPB::ITEM_SUBTYPE nType)
{
	m_subType = nType;
	switch (m_subType)
	{
	case MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_BLOCKMINE:
		break;
	case MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_FURNITURE:
		//m_pBuildingType = BuildingCfg::getInstance().findBuildingType(getItemId());
		break;
	case MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_PLANT_SEED:
		m_pPlantSeed = PlantCfg::getInstance().findPlantType(getItemId());
		break;
	case MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_MEDICAMENT:
		//m_pMedicamentType = MedicamentCfg::getInstance().findMedicament(getItemId());
		break;
	case  MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_ARMOR:
		m_pArmorType = ArmorCfg::getInstance().findArmorType(getItemId());
		if (m_pArmorType)
		{
			m_vAllSkill.assign(m_pArmorType->m_vSkill.begin(), m_pArmorType->m_vSkill.end());
			for (auto&& pPassiveSkill : m_pArmorType->m_vPassiveSkill)
			{
				m_vAllSkill.push_back(pPassiveSkill);
			}
		}
		initAttrEffectVec();
		break;
	default:
		break;
	}
}

void ItemData::initAttrEffectVec()
{
	auto&& armorCfg = getArmorCfg();
	if (armorCfg)
	{
		cleanAttrEffect(AttrType::ATTR_BASE);
		for (auto&& eff : armorCfg->m_vecEffectAttr)
		{
			if (eff.n_effect_type == 0)
			{
				continue;
			}
			Effect effect;
			effect.type = static_cast<ROLE_EFFECT_TYPE>(eff.n_effect_type);
			for (auto cfgAttr : eff.vecArmorAtta)
			{
				if (cfgAttr.n_effect_value == 0)
				{
					continue;
				}

				AttrEffectValue attrVal;
				attrVal.attrref = static_cast<ATTR_REF>(cfgAttr.n_effect_attrref);
				attrVal.value = cfgAttr.n_effect_value;

				effect.ref_values.push_back(attrVal);
			}

			m_mapAttrVec[AttrType::ATTR_BASE].push_back(effect);
		}
	}
}

std::string ItemData::GetAssertIcon()
{
	if (!m_pItem)
	{
		return "";
	}
	return fmt::format("/Game/UI/Assert/{0}/{1}.{2}", m_pItem->s_foder , m_pItem->s_icon , m_pItem->s_icon);
}


const AttrVec& ItemData::getAttrEffectVec(AttrType attrtype)
{
	auto&& AttrVecData = m_mapAttrVec.find(attrtype);
	if (AttrVecData == m_mapAttrVec.end())
	{
		return M_EMPTY_ATTR;
	}
	return 	AttrVecData->second;
}

void ItemData::addAttrEffect(AttrType attrtype, const MsgPB::AttrEffect& attrEffect)
{
	Effect effect;
	effect.type = attrEffect.type();
	for (auto&& val : attrEffect.values())
	{
		AttrEffectValue attrVal;
		attrVal.attrref = static_cast<ATTR_REF>(val.key());
		attrVal.value = val.val();
		effect.ref_values.push_back(attrVal);
	}
	m_mapAttrVec[attrtype].push_back(effect);
}

void ItemData::cleanAttrEffect(AttrType attrtype)
{
	if (attrtype == AttrType::ATTR_All)
	{
		m_mapAttrVec.clear();
	}
	auto&& AttrVecData = m_mapAttrVec.find(attrtype);
	if (AttrVecData != m_mapAttrVec.end())
	{
		AttrVecData->second.clear();
	}
}

bool ItemData::IsInCooldown()
{
	int nLess = ItemPacketMgr::getInstance().GetCooldown(getItemId());
	if (nLess <= 0)
	{
		return false;
	}
	return true;
}

void ItemData::sumEffect(CEffectSum& effectSum, bool bATK_SP)
{
	for (int i = 0; i <= int(AttrType::ATTR_STATICVAL); i++)
	{
		auto && vecAttrIter = m_mapAttrVec.find(AttrType(i));
		if (vecAttrIter != m_mapAttrVec.end())
		{
			for (auto&& effect : vecAttrIter->second)
			{
				if (!bATK_SP && (effect.type == MsgPB::ROLE_EFFECT_ATK_SP || effect.type == MsgPB::ROLE_EFFECT_ATK_SP_RATE))
				{
					continue;
				}
				effectSum.AddEffect(effect.type, effect.ref_values, 0);
			}
		}
	}
}

float ItemData::getAttrValue(Effect& tDataInfo)
{
	float nValue{ 0 };
	for (auto&& attrVal : tDataInfo.ref_values)
	{
		if (attrVal.attrref == 1) //固定值;
		{
			if ((MsgPB::ROLE_EFFECT_TYPE)attrVal.attrref == MsgPB::ROLE_EFFECT_MV_SP || (MsgPB::ROLE_EFFECT_TYPE)attrVal.attrref == MsgPB::ROLE_EFFECT_CHASE)
			{
				nValue += (float)attrVal.value / 100.0f;
			}
			else
			{
				nValue += attrVal.value;
			}
		}
		else  //千分比;
		{
			nValue += attrVal.value / 1000.0f;
		}
	}
	return nValue;
}

bool ItemData::IsHeroUnFullAttr()
{
	const std::shared_ptr<Player>& pPlayerData = PlayerMgr::getInstance().getSelf();
	if (!pPlayerData)
	{
		return false;
	}
	if (!m_pItem) {
		return false;
	}
	ItemCfg::ITEM_USE_BY_SYSTEM useTpy = m_pItem->GetUseBySystem();
	switch (useTpy) {
	case ItemCfg::ITEM_USE_BY_SYSTEM_FOOD:
	{
		return true;
	}
	break;
	case ItemCfg::ITEM_USE_BY_SYSTEM_MEDICAMENT:
	{
		auto&& pMediaCfg = MedicamentCfg::getInstance().findMedicament(getItemId());
		if (!pMediaCfg) {
			return false;
		}
		switch (pMediaCfg->n_effect_type) {
		case MedicamentCfg::EN_MEDICAMENT_TYPE_HP:
		case MedicamentCfg::EN_MEDICAMENT_TYPE_HP_PER:
			if (pPlayerData->getHp() >= pPlayerData->getMaxHp()) {
				return false;
			}
			break;
		case MedicamentCfg::EN_MEDICAMENT_TYPE_MP:
		case MedicamentCfg::EN_MEDICAMENT_TYPE_MP_PER:
			if (pPlayerData->getMp() >= pPlayerData->getMaxMp()) {
				return false;
			}
			break;
		case MedicamentCfg::EN_MEDICAMENT_TYPE_HPMP:
		case MedicamentCfg::EN_MEDICAMENT_TYPE_HPMP_PER:
			if (pPlayerData->getMp() >= pPlayerData->getMaxMp()
				&& pPlayerData->getHp() >= pPlayerData->getMaxHp()) {
				return false;
			}
			break;
		case MedicamentCfg::EN_MEDICAMENT_TYPE_BREATH:
			if (pPlayerData->getAnger() >= pPlayerData->getMaxAnger()) {
				return false;
			}
			break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
	return true;
}


//------------------------------------------------------------------------------------------------
// ItemPacketMgr
//------------------------------------------------------------------------------------------------

ItemPacketMgr::ItemPacketMgr()
{

}

ItemPacketMgr::~ItemPacketMgr()
{

}


ItemPacketMgr& ItemPacketMgr::getInstance()
{
	static ItemPacketMgr s_itemPacketMgr;
	return s_itemPacketMgr;
}

bool ItemPacketMgr::addItem(std::shared_ptr<ItemData> itemData)
{
	if (itemData == nullptr) return false;

	// add ItemData
	m_itemDataMap[itemData->m_uuid] = itemData;

	return true;
}

bool ItemPacketMgr::removeItem(NS::I64 delUuid)
{
	ItemDataMap::iterator delIt = m_itemDataMap.find(delUuid);
	if (delIt != m_itemDataMap.end())
	{
		// Del ItemData
		m_itemDataMap.erase(delIt);

		return true;
	}

	return false;
}

ItemPacketMgr::WeakItemDataKeyPosMap ItemPacketMgr::findPacketMap(MsgPB::PACKET_TYPE packetType)
{
	WeakItemDataKeyPosMap itemDataMap{};

	for (auto& it : m_itemDataMap)
	{
		if (it.second->getPacketType() != packetType)
		{
			continue;
		}
		itemDataMap.insert(std::make_pair(it.second->getPos(), it.second));
	}

	return itemDataMap;
}

std::shared_ptr<ItemData> ItemPacketMgr::findItemDataByPos(MsgPB::PACKET_TYPE packetType, int bagPos)
{

	for (auto& it : m_itemDataMap)
	{
		if (it.second.get()->m_packetType == packetType)
		{
			//			auto pos = it.second.get()->m_pos;
			if (it.second.get()->m_pos == bagPos)
			{
				return it.second;
			}
		}
	}

	return nullptr;
}

std::shared_ptr<ItemData> ItemPacketMgr::findFirstBagItemDataByEquipPos(ARMOR_EQUIP_POS equipPos)
{
	for (auto& it : m_itemDataMap)
	{
		if (it.second.get()->m_packetType == MsgPB::PACKET_TYPE::BODY) {
			if (it.second.get()->m_pArmorType && it.second.get()->m_pArmorType->m_equipPos == equipPos) {
				return it.second;
			}
		}
	}

	return nullptr;
}

int ItemPacketMgr::getPacketItemCount(MsgPB::PACKET_TYPE packetType, int nItemId)
{
	int totalCount{ 0 };

	for (auto& it : m_itemDataMap)
	{
		if (it.second.get()->m_packetType == packetType && it.second.get()->m_itemId == nItemId)
		{
			totalCount += it.second.get()->m_count;
		}
	}

	return totalCount;
}

std::shared_ptr<ItemData> ItemPacketMgr::findItemDataByUuid(NS::I64 uuid)
{
	ItemDataMap::iterator it = m_itemDataMap.find(uuid);
	if (it != m_itemDataMap.end())
	{
		return it->second;
	}

	return nullptr;
}

ItemPacketMgr::WeakItemDataKeyItemIdMutmap ItemPacketMgr::findPacketMapByItemId(MsgPB::PACKET_TYPE packetType, int itemId)
{
	WeakItemDataKeyItemIdMutmap itemDataMap{};

	for (auto& it : m_itemDataMap)
	{
		if (it.second.get()->m_packetType == packetType && it.second.get()->m_itemId == itemId)
		{
			itemDataMap.insert(std::make_pair(it.second.get()->m_itemId, it.second));
		}
	}

	return itemDataMap;
}

std::shared_ptr<ItemData> ItemPacketMgr::findFirstItemDataItemId(MsgPB::PACKET_TYPE packetType, int itemId)
{
	for (auto& it : m_itemDataMap)
	{
		if (it.second.get()->m_packetType == packetType && it.second.get()->m_itemId == itemId)
		{
			return it.second;
		}
	}
	return nullptr;
}

ItemPacketMgr::WeakItemDataKeyUuidMap ItemPacketMgr::findPacketMapByTag(MsgPB::PACKET_TYPE packetType, const bzbee::tstring& strTag, int nSubType /*= 0*/)
{
	WeakItemDataKeyUuidMap itemDataMap{};

	for (auto& it : m_itemDataMap)
	{
		if (it.second.get()->m_packetType == packetType)
		{
			const ItemCfg::Item* pItem = it.second.get()->getItemCfg();
			if (pItem)
			{
				if (ItemCfg::getInstance().checkItemTag(pItem->m_tags, strTag))
				{
					if (nSubType == 0 || it.second.get()->m_subType == nSubType)
					{
						itemDataMap.insert(std::make_pair(it.second.get()->m_uuid, it.second));
					}
				}
			}
		}
	}

	return itemDataMap;
}

ItemPacketMgr::WeakItemDataKeyPosMap ItemPacketMgr::findPacketMapByPos(MsgPB::PACKET_TYPE packetType, int bPos, int ePos)
{
	WeakItemDataKeyPosMap itemDataMap{};

	for (auto& it : m_itemDataMap)
	{
		auto pItemData = it.second.get();

		if (pItemData->m_packetType == packetType && (pItemData->m_pos >= bPos || pItemData->m_pos <= ePos))
		{
			itemDataMap.insert(std::make_pair(pItemData->m_pos, it.second));
		}
	}

	return itemDataMap;
}




bool ItemPacketMgr::addPacketInfo(const PacketInfo* info)
{
	if (!info) return false;

	m_packetInfoMap[info->m_packetType] = *info;

	PacketInfoMap::iterator it = m_packetInfoMap.find(info->m_packetType);

	if (it != m_packetInfoMap.end())
	{
		it->second = *info;
	}
	else
	{
		m_packetInfoMap.insert(std::make_pair(info->m_packetType, *info));
	}

	return true;
}


bool ItemPacketMgr::checkPacketInfoMax(MsgPB::PACKET_TYPE packetType)
{
	auto&& packetInfo = findPacketInfo(packetType);
	if (!packetInfo)
		return false;

	return packetInfo->m_packetInit >= packetInfo->m_packetMax;
}

const ItemPacketMgr::PacketInfo* ItemPacketMgr::findPacketInfo(MsgPB::PACKET_TYPE packetType)
{
	PacketInfoMap::iterator it = m_packetInfoMap.find(packetType);
	if (it != m_packetInfoMap.end()) return &it->second;

	return nullptr;
}

int ItemPacketMgr::getEquipPosByArmCfgPos(int armCfgPos)
{
	int equipPos = armCfgPos;
	if (equipPos > 6)
	{
		equipPos += 2;
	}
	return equipPos;
}

int ItemPacketMgr::getEquipPosById(int id)
{
	int pos = -1;
	auto cfg = ArmorCfg::getInstance().findArmorType(id);
	if (cfg)
		return getEquipPosByArmCfgPos(cfg->m_equipPos);
	return pos;
}


bool ItemPacketMgr::isWeaponByTempId(int id)
{
	auto cfg = ArmorCfg::getInstance().findArmorType(id);
	if (!cfg) return false;
	if (cfg->m_equipType == static_cast<int>(MsgPB::EQUIP_MELEE_WEQPON) ||
		cfg->m_equipType == static_cast<int>(MsgPB::EQUIP_RANGED_WEQPON) ||
		cfg->m_equipType == static_cast<int>(MsgPB::EQUIP_MAGIC_WEQPON) ||
		cfg->m_equipType == static_cast<int>(MsgPB::EQUIP_THROWING_WEQPON))
	{
		return true;
	}
	return false;
}

bool ItemPacketMgr::isArmByTempId(int id)
{
	auto cfg = ArmorCfg::getInstance().findArmorType(id);
	if (!cfg) return false;
	if (cfg->m_equipType == static_cast<int>(MsgPB::EQUIP_ARMOR))
		return true;
	return false;
}


ItemPacketMgr::ItemDataVec& ItemPacketMgr::getSortBag(MsgPB::PACKET_TYPE Bag, PackSortType sort)
{
	m_retSortBagVec.clear();

	for (auto& it : m_itemDataMap)
	{
		if (it.second.get()->m_packetType == Bag)
		{
			m_retSortBagVec.push_back(it.second);
		}
	}

	switch (sort) // 排序.
	{
	case PackSortType::PackSortBag:
		std::sort(m_retSortBagVec.begin(), m_retSortBagVec.end(), std::bind(&ItemPacketMgr::sortBag, this, std::placeholders::_1, std::placeholders::_2));
		break;
	case PackSortType::PackSort_None:
	default:
		break;
	}

	return m_retSortBagVec;
}


bool ItemPacketMgr::sortBag(const std::shared_ptr<ItemData> lhs, const std::shared_ptr<ItemData> rhs)
{
	return lhs->m_itemId < rhs->m_itemId;
}

void ItemPacketMgr::SetMaterial(NS::uint unItemId, NS::I64 n64Num)
{
	m_mapMaterials[unItemId] = n64Num;
	
}

NS::I64 ItemPacketMgr::GetMaterial(NS::uint unItemId)
{
	auto iter = m_mapMaterials.find(unItemId);
	if (iter != m_mapMaterials.end()) {
		return iter->second;
	}
	return 0;
}

bool ItemPacketMgr::AddNewItemTips(NS::I64 uuid)
{
	if (m_mapItemGets.find(uuid) != m_mapItemGets.end()) {
		return false;
	}
	m_mapItemGets[uuid] = true;
	return true;
}

void ItemPacketMgr::ResetCooldown(int nItemId)
{
	auto&& pItem = ItemCfg::getInstance().getItemById(nItemId);
	if (!pItem) {
		return;
	}
	int nCooldownTm = ItemCfg::getInstance().getCooldownType(pItem->n_cooldown_type);
	if (nCooldownTm > 0) {
		m_mapCooldown[pItem->n_cooldown_type] = Heartbeat::getInstance().getServerTimeInMs() + nCooldownTm;
	}
}

int ItemPacketMgr::GetCooldown(int nItemId)
{
	auto&& pItem = ItemCfg::getInstance().getItemById(nItemId);
	if (!pItem) {
		return 0;
	}
	auto iter = m_mapCooldown.find(pItem->n_cooldown_type);
	if (iter != m_mapCooldown.end() && iter->second > Heartbeat::getInstance().getServerTimeInMs()) {
		return static_cast<int>(iter->second - Heartbeat::getInstance().getServerTimeInMs());
	}
	return 0;
}

int ItemPacketMgr::getLessPackNum()
{
	int nNum = 0;
	for (auto& it : m_itemDataMap)
	{
		if (it.second.get()->m_packetType == MsgPB::PACKET_TYPE::BODY)
		{
			nNum++;
		}
	}
	if (nNum < PACK_MAX_NUM_BODY) {
		return PACK_MAX_NUM_BODY - nNum;
	}
	return 0;
}



