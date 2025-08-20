/*!
 * \file Player.cpp
 *
 * \author zhijieli
 * \date 2016/12/05
 *
 *
 */
#include "stdafx.h"
#include "Player.h"
#include "LogUtil.h"
#include "msg/MsgGameDef.pb.h"
#include "../ItemData.h"
//#include "../AchieveData.h"
//#include "../ItemUseCountData.h"
#include "../../config/PlayerAttrCfg.h"
#include "../../config/GlobalCfg.h"
#include <RandomUtil.h>
#include "../../config/PlayerAttrNameCfg.h"
#include "Framework.h"
#include "MacroUtility.h"
//#include "../../libsandbox/source/npc/NpcPetFight.h"
//#include "../../config/FoodCfg.h"
//#include "../../config/PvpSquadCfg.h"

using namespace NS;

PlayerMgr& PlayerMgr::getInstance()
{
	static PlayerMgr mgr;
	return mgr;
}

std::shared_ptr<Player> PlayerMgr::getPlayer(bzbee::uint mapObjId)
{
	auto iter = m_otherPlayers.find(mapObjId);
	return (iter == m_otherPlayers.end()) ? nullptr : iter->second;
}

std::shared_ptr<Player> PlayerMgr::getPlayerByIggId(NS::I64 iggId)
{
	if (m_self && m_self->getIggId() == iggId)
	{
		return m_self;
	}

	for (auto&& pir : m_otherPlayers)
	{
		if (pir.second->getIggId() == iggId)
		{
			return pir.second;
		}
	}

	return nullptr;
}

void PlayerMgr::setSelf(const std::shared_ptr<Player>& r)
{
	int oldId{};
	if (m_self) { oldId = m_self->getId(); }
	m_self = r;
	if (oldId)
	{
		MapObjMgr::getInstance().removeObj(oldId);
	}
	if (m_self && m_self->getId() > 0) {
		MapObjMgr::getInstance().addObj(m_self);
	}
}

bool PlayerMgr::addPlayer(const std::shared_ptr<Player>& player)
{
	if (!player) { return false; }
	if (getPlayer(player->getId()))
	{
		LOGE("duplicate map obj id: {0}, override.", player->getId());
	}
	m_otherPlayers[player->getId()] = player;
	return MapObjMgr::getInstance().addObj(player);
}

bool PlayerMgr::removePlayer(bzbee::uint mapObjId)
{
	if (mapObjId == m_self->getId())
	{
		m_self->setId(0);
		//m_self->clearInit();
		return MapObjMgr::getInstance().removeObj(mapObjId);
	}

	return m_otherPlayers.erase(mapObjId) == 1 && MapObjMgr::getInstance().removeObj(mapObjId);
}

bool PlayerMgr::removePlayerByIggId(NS::I64 iggid)
{
	if (iggid == m_self->getIggId())
	{
		m_self->setIggId(0);
		//m_self->clearInit();
		return MapObjMgr::getInstance().removeObjByIggId(iggid);
	}

	for (auto&& pir : m_otherPlayers)
	{
		if (pir.second->getIggId() == iggid)
		{
			return m_otherPlayers.erase(pir.second->getId()) == 1 && MapObjMgr::getInstance().removeObjByIggId(iggid);
		}
	}
	return false;
}

bool PlayerMgr::isMyself(NS::I64 uuid)
{
	auto slefPlayer = getSelf();
	return (m_self && slefPlayer->getIggId() == uuid);
}

//////////////////////////////////////////////////////////////////////////
Player::Player(bzbee::int32 id) : Role{ id }
{
	m_category = MapObj::Category::Player;
	m_bChangeEquip = true;
}

void Player::onInit()
{
	calcAttr();
	Role::onInit();
}

NS::I64 Player::getMaxExp()
{
	auto playerAttr = getPlayerAttr();
	//assert(playerAttr);
	if (!playerAttr) { return 0; }
	return 	playerAttr->n_exp;
}

int Player::getMoney(int type) const
{
	if (type == -1 || type == 0)
		return 0;

	return m_arryMoney[type];
}

int LUA(get_money)(int type)
{
	auto pSelf = PlayerMgr::getInstance().getSelf();
	return pSelf ? pSelf->getMoney(type) : 0;
}

int LUA(get_pvp_coin)()
{
	auto pSelf = PlayerMgr::getInstance().getSelf();
	return pSelf ? pSelf->getPvpCoin() : 0;
}

int LUA(get_self_lv)()
{
	auto pSelf = PlayerMgr::getInstance().getSelf();
	return pSelf ? pSelf->getLevel() : 0;
}

int LUA(get_self_gender)()
{
	auto pSelf = PlayerMgr::getInstance().getSelf();
	return pSelf ? pSelf->getGender() : 0;
}

NS::I64 LUA(get_self_iggId)()
{
	auto pSelf = PlayerMgr::getInstance().getSelf();
	return pSelf ? pSelf->getIggId() : 0;
}

tstring LUA(get_self_name)()
{
	auto pSelf = PlayerMgr::getInstance().getSelf();
	return pSelf ? pSelf->getNickname() : "";
}

int Player::CalculateMaxHp() const
{
	// 	if (!isSelf())
	// 	{
	// 		if (m_hpm > 0)
	// 			return m_hpm;
	// 	}
	auto playerAttr = getPlayerAttr();
	//assert(playerAttr);
	if (!playerAttr) { return 0; }
	auto data = static_cast<int>(getAttr(MsgPB::ROLE_EFFECT_HPM));
	return data == 0 ? playerAttr->n_HPM : data;
}

int Player::getMaxMp() const
{
	auto playerAttr = getPlayerAttr();
	//assert(playerAttr);
	if (!playerAttr) { return 0; }
	auto data = static_cast<int>(getAttr(MsgPB::ROLE_EFFECT_MPM));
	return data == 0 ? playerAttr->n_MPM : data;
}

void Player::clearServerAttr()
{
	m_attrsServer.clear();
}

void Player::setServerAttr(MsgPB::ROLE_EFFECT_TYPE type, float value)
{
	m_attrsServer[type] = value;
}

float Player::getServerAttr(MsgPB::ROLE_EFFECT_TYPE type)
{
	float value{};
	auto iter = m_attrsServer.find(type);
	if (iter != m_attrsServer.end())
	{
		value = static_cast<float>(iter->second);
	}

	if (type == MsgPB::ROLE_EFFECT_ATK_SP && value <= .001f)
	{
		value = .5f;
	}
	return value;
}

float Player::getAttr(MsgPB::ROLE_EFFECT_TYPE type) const
{
	float value{};
	auto iter = m_attrs.find(type);
	if (iter != m_attrs.end())
	{
		value = static_cast<float>(iter->second);
	}

	if (type == MsgPB::ROLE_EFFECT_ATK_SP && value <= .001f)
	{
		value = .5f;
	}

	//// For develop.
	//auto pWeapon = ItemPacketMgr::getInstance().getCurrEquipWeqpon();
	//if (pWeapon && pWeapon->getItemId() == 1300013 && type == MsgPB::ROLE_EFFECT_ATK_RANGE)
	//{
	//	return 7.0f;
	//}

	return value;
}

float Player::getBaseAttr(MsgPB::ROLE_EFFECT_TYPE type, bool isInhert) const
{
	auto&& attrs = getPlayerAttrInfo();
	auto it = attrs.find(type);
	if (it == attrs.end()) { return{}; }

	auto outValue = it->second;
	//if (m_mapType == MapDataMgr::MapType::PVP && isInhert)
	//{
	//	auto value = PlayerAttrCfg::getInstance().getInhert(type);
	//	if (value != -1)
	//	{
	//		outValue = outValue * value / 100.0f;
	//	}
	//}
	return outValue;
}

const PlayerAttrCfg::PlayerAttr* Player::getPlayerAttr() const
{
	auto playerAttr = PlayerAttrCfg::getInstance().findPlayerAttr(m_level);
	if (!playerAttr)
	{
		LOGE("player attribute not found, level: {0}.", m_level);
	}

	return playerAttr;
}

const AttrMap& Player::getPlayerAttrInfo() const
{
	return PlayerAttrCfg::getInstance().findPlayerAttrInfo(m_level);
}


float Player::getWidth() const
{
	if (getIsOverrideModel())
	{
		return getOverrideWidth();
	}
	return PlayerAttrCfg::getInstance().f_PlayerWidth;
}

float Player::getHeight() const
{
	if (getIsOverrideModel())
	{
		return getOverrideHeight();
	}
	return PlayerAttrCfg::getInstance().f_PlayerHeight;
}

int Player::getJump() const
{
	return (int)getAttr(MsgPB::ROLE_EFFECT_JUMP);
}

const bzbee::tstring Player::getHurtEffectByAniName(const bzbee::tstring& aniName) const
{
	//auto&& weapon = getCurrEftWeapon();
	//if (!weapon) { return{}; }
	//auto&& armorCfg = weapon->getArmorCfg();
	//if (!armorCfg || armorCfg->m_hurtEffect.empty()) { return{}; }

	//std::size_t idx{};
	//for (; idx < armorCfg->m_atkAniEffect.size(); ++idx)
	//{
	//	if (std::get<0>(armorCfg->m_atkAniEffect[idx]) == aniName) { break; }
	//}

	//return armorCfg->m_hurtEffect[idx < armorCfg->m_hurtEffect.size() ? idx : 0];
	return "";
}

const bzbee::tstring Player::getAtkSound() const
{
	//auto&& weapon = getCurrEftWeapon();
	//if (!weapon) { return{}; }
	//auto&& armorCfg = weapon->getArmorCfg();
	//if (!armorCfg) { return{}; }
	//return armorCfg->s_AttackSound;
	return "";
}

const bzbee::tstring Player::getAtkHitSound() const
{
	//auto&& weapon = getCurrEftWeapon();
	//if (!weapon) { return{}; }
	//auto&& armorCfg = weapon->getArmorCfg();
	//if (!armorCfg) { return{}; }
	//return armorCfg->s_UnderAttackSound;
	return "";
}

float Player::getAttackPauseTime() const
{
	float defaultTime = 5;//GlobalCfg::getInstance().getSlidingPauseDuration();

	auto&& weapon = getCurrEftWeapon();
	if (!weapon) { return defaultTime; }
	auto&& armorCfg = weapon->getArmorCfg();
	if (!armorCfg) { return defaultTime; }
	return std::max(armorCfg->f_Pause_time, .0001f);
}

const std::tuple<bzbee::tstring, bzbee::tstring> Player::getAtkAniAndEffect(bool isJumping) const
{
	auto&& weapon = getCurrEftWeapon();
	if (!weapon || !weapon->getArmorCfg())
	{
		return std::make_tuple(fmt::format("melee_attack_0{0}", RandomUtil::getRandomInt(1, 3)), "");
	}
	const auto& aniEffect = isJumping ? weapon->getArmorCfg()->m_jumpAtkAniEffect : weapon->getArmorCfg()->m_atkAniEffect;
	if (aniEffect.empty()) { return{}; }
	return aniEffect[bzbee::RandomUtil::getRandomInt(0, aniEffect.size() - 1)];
}

const bzbee::tstring Player::getAtkEffectByAniName(const bzbee::tstring& aniName) const
{
	auto&& weapon = getCurrEftWeapon();
	if (!weapon) { return{}; }
	auto&& armorCfg = weapon->getArmorCfg();
	if (!armorCfg || (armorCfg->m_atkAniEffect.empty() && armorCfg->m_jumpAtkAniEffect.empty())) { return{}; }

	// look up in attack effect
	for (const auto& aniEffect : armorCfg->m_atkAniEffect)
	{
		if (std::get<0>(aniEffect) == aniName)
		{
			return std::get<1>(aniEffect);
		}
	}

	// look up in jump attack effect
	for (const auto& aniEffect : armorCfg->m_jumpAtkAniEffect)
	{
		if (std::get<0>(aniEffect) == aniName)
		{
			return std::get<1>(aniEffect);
		}
	}

	return{};
}

void Player::setChangeEquipState(bool bChange)
{
	m_bChangeEquip = bChange;
}

void Player::setLevel(int lev)
{
	Role::setLevel(lev);
}

float Player::getCatch() const
{
	return PlayerAttrCfg::getInstance().n_Catch;
}

void Player::setPlayerModelData(PlayerModelData tData)
{
	m_PlayerModelData = tData;
}

void Player::setPos(const glm::vec3& pos)
{
	Role::setPos(pos);


	//if (MapDataMgr::getInstance().isPlayerSetAway() && isSelf())
	//{
	//	m_pos.y = -1000;
	//	//m_pos.z = -1000000;
	//}
}

//void Player::setOrientation(const glm::vec3& o)
//{
//	//
//}

float Player::getAddAttrValue(const Effect& tDataInfo)
{
	float nValue{ 0 };
	//auto&& mMapAttrInfo= getPlayerAttrInfo();
	for (auto&& attrVal : tDataInfo.ref_values)
	{
		if (attrVal.attrref == 1) //固定值;
		{
			if (tDataInfo.type == MsgPB::ROLE_EFFECT_MV_SP || tDataInfo.type == MsgPB::ROLE_EFFECT_CHASE)
			{
				nValue += (float)attrVal.value / 100.0f;
			}
			else
			{
				nValue += attrVal.value;
			}
		}
		else
		{
			nValue += attrVal.value / 1000.0f;
			//int nType = (int)floor(attrVal.attrref / 100.0f);
			//int nkey = attrVal.attrref % 100;
			//if (nType == 1) //读配置表计算属性;
			//{
			//	auto iter = mMapAttrInfo.find(MsgPB::ROLE_EFFECT_TYPE(nkey));
			//	if (iter != mMapAttrInfo.end())
			//	{
			//		nValue += attrVal.value * iter->second / 1000.0f;
			//	}
			//}
			//else //以自身属性为基础计算;
			//{
			//	auto iter = m_attrs.find(MsgPB::ROLE_EFFECT_TYPE(nkey));
			//	if (iter != m_attrs.end())
			//	{
			//		nValue += attrVal.value * iter->second / 1000.0f;
			//	}
			//}
		}
	}
	return nValue;
}

void Player::setDisableJumpTime(bool bYes)
{
	m_bDisableJumpTime = bYes;
}

float Player::getShortJumpTime() const
{
	return m_bDisableJumpTime ?
		PlayerAttrCfg::getInstance().f_LongJumpTime
		: PlayerAttrCfg::getInstance().f_ShortJumpTime;
}

float Player::getLongJumpTime() const
{
	return PlayerAttrCfg::getInstance().f_LongJumpTime;
}

float Player::getInitialVerticalVelocity() const
{
	return getAttr(MsgPB::ROLE_EFFECT_JUMP_INIT_SPEED);
}

float Player::getAccelerationUp() const
{
	return PlayerAttrCfg::getInstance().f_AccelerationUp;
}

void Player::clearAttrs()
{
	m_attrs.clear();
}

void Player::setAttr(MsgPB::ROLE_EFFECT_TYPE type, float value)
{
	m_attrs[type] = value;
}

void Player::calPvPSegAttr()
{
	// 小队PVP 敌方玩家数据以服务端下发数据为准;
	for (auto&& iter = m_attrs.begin(); iter != m_attrs.end(); ++iter)
	{
		MsgPB::ROLE_EFFECT_TYPE eType = iter->first;
		float value = iter->second;
		// 服务器下发的 附加属性多 *1000;
		if (eType == MsgPB::ROLE_EFFECT_ATK_RANGE ||
			eType == MsgPB::ROLE_EFFECT_GATHER_SP ||
			eType == MsgPB::ROLE_EFFECT_ATK_SP)
		{
			value = value / 1000;
		}
		// 服务器下发的 附加属性多 *100;
		if (eType == MsgPB::ROLE_EFFECT_MV_SP ||
			eType == MsgPB::ROLE_EFFECT_CHASE)
		{
			value = value / 100;
		}
		m_attrs[eType] = value;
	}
	if (m_attrs.find(MsgPB::ROLE_EFFECT_MV_SP) == m_attrs.end())
	{
		m_attrs[MsgPB::ROLE_EFFECT_MV_SP] = PlayerAttrCfg::getInstance().f_Move / 100;
	}
	if (m_attrs.find(MsgPB::ROLE_EFFECT_CHASE) == m_attrs.end())
	{
		m_attrs[MsgPB::ROLE_EFFECT_CHASE] = PlayerAttrCfg::getInstance().f_Run / 100;
	}
	if (m_attrs[MsgPB::ROLE_EFFECT_MV_SP] > m_attrs[MsgPB::ROLE_EFFECT_CHASE])
	{
		// 正常情况追击速度不能比移动速度慢
		//m_attrs[MsgPB::ROLE_EFFECT_CHASE] = m_attrs[MsgPB::ROLE_EFFECT_MV_SP] > (PlayerAttrCfg::getInstance().f_Run / 100) ? m_attrs[MsgPB::ROLE_EFFECT_MV_SP] : (PlayerAttrCfg::getInstance().f_Run / 100);
	}
	if (m_attrs.find(MsgPB::ROLE_EFFECT_JUMP_INIT_SPEED) == m_attrs.end() || m_attrs[MsgPB::ROLE_EFFECT_JUMP_INIT_SPEED] < PlayerAttrCfg::getInstance().f_InitialVerticalVelocity)
	{
		//m_attrs[MsgPB::ROLE_EFFECT_JUMP_INIT_SPEED] = PlayerAttrCfg::getInstance().f_InitialVerticalVelocity;
	}
	if (getMp() > getAttr(MsgPB::ROLE_EFFECT_MPM) && isInitialized())
	{
		setMp((int)getAttr(MsgPB::ROLE_EFFECT_MPM));
		setMaxMp((int)getAttr(MsgPB::ROLE_EFFECT_MPM));
	}
}

void Player::calcAttr(bool bInherBuff)
{
	// 清空 当前属性, 增加属性.
	m_attrs.clear();
	m_effectSum.CleanUp();
	// 装备 增加属性
	for (auto iteMap = m_equipMap.begin(); iteMap != m_equipMap.end(); iteMap++)
		iteMap->second->sumEffect(m_effectSum);
	// Buff 增加属性
	if (bInherBuff)
		sumEffectBuf(m_effectSum);
	if (isSelfCompId())
	{
		// 成就 增加属性
		//AchieveData::getInstance().sumEffect(m_effectSum);
		// 属性精华 增加属性
		//ItemUseCountData::getInstance().sumEffect(m_effectSum);
		//PvpSquadCfg::getInstance().sumEffect(m_effectSum);
	}
	// 计算 当前属性
	AttrMap baseAttr = getPlayerAttrInfo();
	recalculateByEffectSum(m_attrs, baseAttr, m_effectSum);

	// 属性值变更, 触发成就。
	if (m_calcAttrTriggerAchieveCallback)
		m_calcAttrTriggerAchieveCallback();

	// ### ChenBK CLV-1380
	if (/*getHp() > getAttr(MsgPB::ROLE_EFFECT_HPM) &&*/ isInitialized())
	{
		//setHp((int)getAttr(MsgPB::ROLE_EFFECT_HPM));
		//setMaxHp((int)getAttr(MsgPB::ROLE_EFFECT_HPM));
	}
	setMaxHp(Player::CalculateMaxHp());
	setMaxMp((int)getAttr(MsgPB::ROLE_EFFECT_MPM));
	if (!isInitialized()) {
		if (getMp() < 0) {
			setMp(0);
		}

		if (GetCacheHp() > 0) {
			setHp(GetCacheHp());
			SetCacheHp(0);
		}

		if (getHp() > getMaxHp()) {
			setHp(getMaxHp());
		}
	}
	setInitialzed();
}


bool Player::isSelf() const
{
	return this == PlayerMgr::getInstance().getSelf().get();
}

bool Player::isSelfCompId() const
{
	if (auto&& pSelf = PlayerMgr::getInstance().getSelf())
	{
		return getId() == pSelf->getId();
	}

	return false;
}

void Player::getChangePlayerAttr(int newLevel, ChangeAttrMap &changeAttrs)
{
	AttrMap oldAttrs;
	PlayerAttrCfg::getInstance().getPlayerAttrInfo(m_level, oldAttrs);
	AttrMap newAttrs;
	PlayerAttrCfg::getInstance().getPlayerAttrInfo(newLevel, newAttrs);

	for (int i = MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPM; i <= MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MARM; ++i)
	{
		float oldAttrValue = 0;
		auto oldIterAttr = oldAttrs.find((MsgPB::ROLE_EFFECT_TYPE)i);
		if (oldIterAttr != oldAttrs.end())
		{
			oldAttrValue = oldIterAttr->second;
		}

		float newaAttrValue = 0;
		auto newIterAttr = newAttrs.find((MsgPB::ROLE_EFFECT_TYPE)i);
		if (newIterAttr != newAttrs.end())
		{
			newaAttrValue = newIterAttr->second;
		}

		float diffValue = newaAttrValue - oldAttrValue;
		const PlayerAttrNameCfg::PlayerAttrNameType* pAttrNameType = PlayerAttrNameCfg::getInstance().findPlayerAttrNameType((MsgPB::ROLE_EFFECT_TYPE)i);
		if (!pAttrNameType)
		{
			return;
		}

		if (pAttrNameType->n_Class != 1)
		{
			diffValue = diffValue / 1000 * 100;
		}

		if (diffValue != 0)
		{
			changeAttrs[pAttrNameType->m_Name] = diffValue;
		}
	}

}

std::string Player::GetHeadTxt()
{
	if (getGender() == 1)
	{
		return "Texture2D'/Game/UI/Assert/lobby/female.female'";
	}
	return "Texture2D'/Game/UI/Assert/lobby/male.male'";
}

void Player::addEnemy(int mapObjId)
{
	m_enemies.emplace(mapObjId, std::chrono::steady_clock::now());
}

void Player::resetExtraAttrByPotionInEndlessMode()
{
	m_extraAttrByPotionInEndlessMode.clear();
}

void Player::addExtraAttrByPotionInEndlessMode(MsgPB::ROLE_EFFECT_TYPE type, int value)
{
	m_extraAttrByPotionInEndlessMode[type] = (float)value;
	calcAttr();
}

bool Player::isEnemy(int mapObjId) const
{
	return m_enemies.find(mapObjId) != m_enemies.end();
}

float Player::getHpUiCheck() const
{
	return PlayerAttrCfg::getInstance().f_HpUICheck;
}

bool Player::getIsHungryLimit() const
{
	return false;//getFood() <= FoodCfg::getInstance().getFoodLow();
}

void Player::calPvPSegPower(int pvpSegLevel)
{
	// 清空 当前属性, 增加属性;
	CEffectSum effectSum{};
	AttrMap attrs{};
	effectSum.CleanUp();
	// 装备 增加属性;
	for (auto iteMap = m_equipMap.begin(); iteMap != m_equipMap.end(); iteMap++)
		iteMap->second->sumEffect(effectSum);
	/*if (pvpSegLevel != -1)
	{
		PvpSquadCfg::getInstance().sumEffect(effectSum, pvpSegLevel);
	}*/
	// 计算 当前属性;
	AttrMap baseAttr = getPlayerAttrInfo();
	recalculateByEffectSum(attrs, baseAttr, effectSum);

	float power{ 0 };
	for (int beginPos = 1; beginPos <= (int)MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ATTR_STATIC_MAX; ++beginPos)
	{
		const PlayerAttrNameCfg::PlayerAttrNameType* pAttrNameType = PlayerAttrNameCfg::getInstance().findPlayerAttrNameType(beginPos);
		if (!pAttrNameType)
			continue;
		float attrValue{};
		auto iter = attrs.find((ROLE_EFFECT_TYPE)pAttrNameType->n_Type);
		if (iter != attrs.end())
		{
			attrValue = static_cast<float>(iter->second);
		}
		if (pAttrNameType->n_Type == MsgPB::ROLE_EFFECT_ATK_SP && attrValue <= .001f)
		{
			attrValue = .5f;
		}
		auto powerCfg = PlayerAttrNameCfg::getInstance().getPowerCfg(pAttrNameType->n_Type);
		if (powerCfg)
		{
			power += (attrValue*powerCfg->power / powerCfg->value);
		}
	}
	m_totalPower = (int)power;
}

void Player::calPower()
{
	float power{ 0 };
	for (int beginPos = 1; beginPos <= (int)MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_ATTR_STATIC_MAX; ++beginPos)
	{
		const PlayerAttrNameCfg::PlayerAttrNameType* pAttrNameType = PlayerAttrNameCfg::getInstance().findPlayerAttrNameType(beginPos);
		if (!pAttrNameType)
			continue;
		int attrValue = (int)getAttr(MsgPB::ROLE_EFFECT_TYPE(pAttrNameType->n_Type));
		auto powerCfg = PlayerAttrNameCfg::getInstance().getPowerCfg(pAttrNameType->n_Type);
		if (powerCfg)
		{
			power += (attrValue*powerCfg->power / powerCfg->value);
		}
		LOGI("player attr={0},value={1}", pAttrNameType->n_Type, attrValue);
	}
	m_totalPower = (int)power;
}

void PlayerMgr::setOtherPlayerEnterMapType(MapDataMgr::MapType atMapType)
{
	for (auto it = m_otherPlayers.begin(); it != m_otherPlayers.end(); it++)
	{
		const std::shared_ptr<Player> player = it->second;
		if (!player) continue;
		player->setMapType(atMapType);
	}
}

NS::I64 PlayerMgr::getSelfIggId()
{
	if (m_self) return m_self->getIggId();
	return 0;
}

NS::I64 PlayerMgr::getSelfId()
{
	if (m_self) return m_self->getId();
	return 0;
}

PlayerMgr::MapObjId2Player PlayerMgr::getAllPlayers()
{
	MapObjId2Player allPlayers = m_otherPlayers;
	allPlayers[m_self->getId()] = m_self;

	return allPlayers;
}

void PlayerMgr::ClearOtherPlayer()
{
	for (auto& v : m_otherPlayers)
	{
		MapObjMgr::getInstance().removeObj(v.first);
	}
	m_otherPlayers.clear();
}

void PlayerMgr::removeAllPlayerData(void)
{
	if (m_self)
	{
		MapObjMgr::getInstance().removeObj(m_self->getId());
		m_self = nullptr;
	}

	ClearOtherPlayer();
}

const std::tuple<bzbee::tstring, bzbee::tstring> Player::getAtkAniByIndex(bool isJumping, int index) const
{
	if (index < 0) { return {}; }
	if (getIsOverrideModel()) {
		return std::make_tuple(fmt::format("melee_attack_0{0}", index % 2 + 1), "");
	}
	auto&& weapon = getCurrEftWeapon();
	if (!weapon || !weapon->getArmorCfg())
	{
		if (isJumping)
			return std::make_tuple("empty_jump_attack", "");
		return std::make_tuple(fmt::format("melee_attack_0{0}", index + 1), "");
	}
	const auto& aniEffect = isJumping ? weapon->getArmorCfg()->m_jumpAtkAniEffect : weapon->getArmorCfg()->m_atkAniEffect;
	if (aniEffect.empty()) { return{}; }
	if (isJumping)
	{
		return aniEffect[bzbee::RandomUtil::getRandomInt(0, aniEffect.size() - 1)];
	}
	else if (static_cast<size_t>(index) < aniEffect.size())
	{
		return aniEffect[index];
	}

	return {};
}

NS::tstring Player::getHeadIcon()
{
	int nGender = getGender();
	if (nGender == Gender::Male)
	{
		return "common.ps_male_head";
	}
	else if (nGender == Gender::Female)
	{
		return "common.ps_female_head";
	}
	// 错误——无性别信息
	return "common.ps_common_naver_cafe";
}

NS::tstring Player::getHalfIcon()
{
	int nGender = getGender();
	if (nGender == Role::Gender::Female)
	{
		return "npc.ps_img_message_player_female";
	}
	else
	{
		return "npc.ps_img_message_player_male";
	}
	// 错误——无性别信息
	return "common.ps_common_naver_cafe";
}


void Player::resetAttr()
{
	//setHp(getMaxHp());
}

int Player::CalcConsumeHp(float fBaseHp)
{
	auto playerAttr1 = PlayerAttrCfg::getInstance().findPlayerAttr(1);
	if (!playerAttr1 || playerAttr1->n_HPM <= 0) {
		return -1;
	}

	auto playerAttrCur = PlayerAttrCfg::getInstance().findPlayerAttr(getLevel());
	if (!playerAttrCur) {
		return -1;
	}
	return std::floor(fBaseHp * playerAttrCur->n_HPM / playerAttr1->n_HPM);
}

int Player::CalcConsumeMp(float fBaseMp)
{
	auto playerAttr1 = PlayerAttrCfg::getInstance().findPlayerAttr(1);
	if (!playerAttr1 || playerAttr1->n_MPM <= 0) {
		return -1;
	}

	auto playerAttrCur = PlayerAttrCfg::getInstance().findPlayerAttr(getLevel());
	if (!playerAttrCur) {
		return -1;
	}
	return std::floor(fBaseMp * playerAttrCur->n_MPM / playerAttr1->n_MPM);
}

std::string Player::GetBattleSound(int nSountTpy)
{
	if (nSountTpy == SOUND_TPY_ATK) {
		int nIdx = std::rand() % 5;
		if (nIdx > 1) return "";
		if (getGender() == 0) {
			return fmt::format("SoundWave'/Game/Sound/player/male/atk{0}.atk{1}'", nIdx + 1, nIdx + 1);
		}
		else {
			return fmt::format("SoundWave'/Game/Sound/player/female/atk{0}.atk{1}'", nIdx + 1, nIdx + 1);
		}
	} else if (nSountTpy == SOUND_TPY_HIT) {
		int nIdx = std::rand() % 10;
		if (nIdx > 2) return "";
		if (getGender() == 0) {
			return fmt::format("SoundWave'/Game/Sound/player/male/hit{0}.hit{1}'", nIdx + 1, nIdx + 1);
		}
		else {
			return fmt::format("SoundWave'/Game/Sound/player/female/hit{0}.hit{1}'", nIdx + 1, nIdx + 1);
		}
	}
	else if (nSountTpy == SOUND_TPY_DEAD) {
		if (getGender() == 0) {
			return "SoundWave'/Game/Sound/player/male/dead.dead'";
		}
		else {
			return "SoundWave'/Game/Sound/player/female/dead.dead'";
		}
	}
	return "";
}


