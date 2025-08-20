/*!
 * \file Role.h
 *
 * \author zhijieli
 * \date 2016/12/05
 *
 * 
 */
#pragma once
#include "../MapObj.h"
#include "../MapDataMgr.h"
#include "../../common/EffectEnum.h"
#include <msg/MsgItem.pb.h>
#include "skill/PassiveSkillMgr.h"
#include <chrono>

#undef max

using namespace NS;


//////////////////////////////////////////////////////////////////////////////////////////////
typedef std::map<MsgPB::ROLE_EFFECT_TYPE, float> AttrMap;
typedef std::map<bzbee::tstring, float> ChangeAttrMap;
void recalculateByEffectSum(AttrMap& attrsReal, const AttrMap& attrsBase, const CEffectSum& effectSum);

// 角色属性值 
struct Effect
{
	MsgPB::ROLE_EFFECT_TYPE type{};
	std::vector<AttrEffectValue> ref_values{};
};

enum {
	SOUND_TPY_NONE = 0,
	SOUND_TPY_ATK,
	SOUND_TPY_HIT,
	SOUND_TPY_DEAD
};

////////////////////////////////////////////////////////////////
struct BuffType;
class Buff
{
public:
	explicit Buff(const BuffType* buffType);

	const BuffType* getType() const { return m_buffType; }
	int getIdx() const { return m_idx; }
	unsigned getRemainMs() const;
	int getHeapCounter() const { return m_heapCounter; }
	int getCasterId() const { return m_casterId; }
	const AttrMap getCasterAttr() const { return m_casterAttr; }
	const std::vector<int>& getUnActiveEffectIndex() const { return m_vecUnActiveEffectIndex; }

	void setIdx(int idx) { m_idx = idx; }
	void setAliveTime(int ms);
	void setHeapCounter(int hc) { m_heapCounter = hc; }
	void setCasterId(int id) { m_casterId = id; }
	void setCasterAttr(AttrMap roleAttr) { m_casterAttr = roleAttr; }
	void setUnActiveEffectIndex(const std::vector<int>& vecUnActiveEffectIndex) { m_vecUnActiveEffectIndex = vecUnActiveEffectIndex; }

	//buff pause/restore
	void pause();
	void restore();
	bool isPause() const;

	bool isDisplay() const;

private:
	const BuffType* m_buffType{};
	int m_idx{};
	int m_heapCounter{};
	int m_casterId{};
	std::chrono::steady_clock::time_point m_endTime;
	//buff pause/restore
	std::chrono::steady_clock::time_point m_pauseTime;
	AttrMap m_casterAttr{};
	std::vector<int> m_vecUnActiveEffectIndex; // 被抵抗的控制效果位置索引
};

////////////////////////////////////////////////////////////////
class Role;

class CharacterAttributeSnapshot
{
public:
	CharacterAttributeSnapshot() {};
	CharacterAttributeSnapshot(const std::shared_ptr<Role>& pRole);
	~CharacterAttributeSnapshot();

	float getBaseAttrByRef(ATTR_REF ref) const;
	float getCurrentAttrByRef(ATTR_REF ref) const;

private:
	AttrMap m_mapAttr;
	AttrMap m_mapBase;
};

////////////////////////////////////////////////////////////////

class ItemData;
class Role : public MapObj
{
public:
	enum Gender
	{
		None = -1,
		Male = 0,
		Female = 1
	};

	static std::string EnumGenderToString(Gender gender)
	{
		switch (gender)
		{
		case Role::None:
			break;
		case Role::Male:
		{
			return "male";
		}
		break;
		case Role::Female:
		{
			return "female";
		}
		break;
		default:
			break;
		}
		return "";
	};

	enum ModelLevel
	{
		Low = 0,
		High = 1
	};

	enum class GroupType
	{
		Npc = 0,//中立阵营，不会被攻击也不会主动攻击
		Mob,
		Player,
		Other,
	};

	enum class Camp
	{
		Neutral = 0,		// 0 : 中立 
		Friendly = 1,		// 1 : 友好
		Hostility = 2,		// 2 : 敵對
	};

public:
	using EquipMap = std::unordered_map<int, std::shared_ptr<ItemData>>;
	using BuffVec = std::vector<Buff>;

public:
	explicit Role(bzbee::int32 id) : MapObj{ id } {}
	~Role();

	virtual void onInit();
	virtual void clearInit();
	virtual void update(float dt);

	int getLevel() const { return m_level; }
	bool hasAttr(MsgPB::ROLE_EFFECT_TYPE type) const;
	int getMp() const { return m_mp; }
	//virtual const std::tuple<glm::vec2, glm::vec2> getAABB() const;
	const BuffVec& getBuffs() const { return m_vecBuff; }
	void setBuffs(const BuffVec& buff);
	float getBuffAtkSpeed() const { return m_buffAtkSpeed; }
	float getBaseAttrByRef(ATTR_REF ref) const;
	float getCurrentAttrByRef(ATTR_REF ref) const;
	float getMoveAniTimeScale(bool isRunning) const;
	bool  IsHasBuffEffectId(int nBuffEffId);
	unsigned getCtrlState() const;

	// Temp
	void setMaxHp(int nMaxHp) { m_nMaxHp = nMaxHp; }
	virtual int getMaxHp() const { return m_nMaxHp; }

	virtual int getMaxMp() const { return 0; }
	virtual int getTotalPower();
	virtual float getAttr(MsgPB::ROLE_EFFECT_TYPE type) const;
	virtual AttrMap getAttrMap() const { return m_attrs; }
	virtual float getBaseAttr(MsgPB::ROLE_EFFECT_TYPE type, bool isInhert = true) const { return 0.f; }
	virtual float getZOffset() const { return 0.f; }
	virtual int getJump() const { return 0; }
	virtual const std::tuple<bzbee::tstring/*animation*/, bzbee::tstring/*effect*/> getAtkAniAndEffect(bool isJumping) const { return{}; }
	virtual const bzbee::tstring getAtkEffectByAniName(const bzbee::tstring& aniName) const { return{}; }
	virtual const bzbee::tstring getHurtEffectByAniName(const bzbee::tstring& aniName) const { return{}; }
	virtual const bzbee::tstring getAtkSoundByAniName(const bzbee::tstring& aniName) const { return{}; }
	virtual bool canFly() const { return false; }
	virtual bool IsIgnoreFlyElevate() const { return true; }
	virtual bool canThroughWall() const { return false; }
	virtual bool isCanAtAir() { return false; };
	virtual void calcAttr(bool bInherBuff = true) { return; }
	virtual int getGender() const { return Gender::None; }
	static void filterAttrShow(AttrMap& attr);

	auto getDeadTime() const { return m_deadTime; }

	virtual void setPos(const glm::vec3& pos) override;
	virtual void setOrientation(const glm::vec3& o);
	virtual const glm::vec3& getOrientation() { return m_orientation; };
	virtual void setLevel(int lev) { m_level = lev; }

 	void setHp(int hp, bool setDisplay = true);
// 	bool checkAndResetRefreshFlatter() 
// 	{ 
// 		if (m_refreshFlatter) 
// 		{ 
// 			m_refreshFlatter = false; 
// 			return true; 
// 		} 
// 		return false; 
// 	}
// 	void setDisplayHp(int hp);
//	virtual int getMaxHp() const = 0;
	int getHp() const { return m_hp; }
//	int getDisplayHp() const { return m_displayHp; }
	//virtual int getMaxHp() const = 0;
	virtual int CalculateMaxHp() const { return 0; }
	void setMp(int mp) { m_mp = mp; }

	bool isInitialized() const { return m_initialized; }
	void setInitialzed() { m_initialized = true; }
	virtual bool isAlive() const { return !m_isDead; }
	bool isDead() const { return m_isDead; }
	void setDead(bool isDead);
	const std::tuple<bool, bzbee::tstring> canRevive(bool freeRecurrection) const;
	virtual float getHpUiCheck() const { return 1.0f; };
	virtual float getSpeakHeight() const { return 1.0f; };

	void setGroupId(NS::int32 groupId) { m_groupId = groupId; }
	NS::int32 getGroupId() const { return m_groupId; }
	virtual NS::int32 getMobRare() const { return 1; }

	bool isGroupMatch(NS::int32 groupId);

	void setNaked3DModel(const bzbee::tstring& file){ m_naked3DModel = file; }
	const bzbee::tstring& getNaked3DModel() const { return m_naked3DModel; }
	void setNaked3DHighModel(const bzbee::tstring& file) { m_naked3DHighModel = file; }
	const bzbee::tstring& getNaked3DHighModel() const { return m_naked3DHighModel; }
	void addBuff(const Buff& buff);
	Buff* findBuffByIdx(int idx);
	void removeBuff(int idx);
	void clearBuff();
	void addReduceNegativeEffectOnBuffBegin(const Buff& buff);
	void removeReduceNegativeEffectOnBuffEnd(const Buff& buff);
	void addReduceNegativeEffectByRate(int32 buffDataID, int32 effect, int32 rate);
	void removeReduceNegativeEffectByRate(int32 buffDataID);
	int getReduceNegativeEffectRate(int32 effectType);
	Buff* findBuffById(int buffId);
	bool isInvincible(Category nCategoryAttacker); //是否是无敌的

	bzbee::tstring getAnimationPath(int eftPos, ModelLevel modelLv=Low);
	bzbee::tstring getAnimationPath(const Role::EquipMap& equipMap, int eftPos, ModelLevel modelLv = Low);
	const bzbee::tstring& getCurrNakedModel(ModelLevel modelLv = Low) const { return (modelLv == Low) ? s_currNakedModel : s_currNakedHighModel; }
	const bzbee::tstring& getCurrNakedAttachId() const { return s_currNakedAttachId; }
	const bzbee::tstring& getTrailModel() const { return s_trailModel; }
	const bzbee::IntegerList& getHideEquip() const { return m_vHideEquip; }
	const bzbee::IntegerList& getHideNode(Role::Gender gender) const { return gender == Role::Gender::Female ? m_vFemaleHideNode : m_vHideNode; }
	int getCurrSelectPos() const;
	bool setCurrSelectPos(int pos);
	int getCurrWeaponPos();
	bool isEquipTwoHandWeapon() { return m_isEquipTwoHandWeapon; }

	
	bool equipPvPWeapon(int itemId);
	const EquipMap& getPvPEquipMap(){ return m_pvpEquipMap; }
	void setMapType(MapDataMgr::MapType mapType) { m_mapType = mapType; };
	MapDataMgr::MapType getMapType() { return m_mapType; };

	void clearWeaponDataOfPvP() { m_pvpEquipMap.clear(); };
	bool isCanEquipArm(int itemId);
	
	void cacheEquipMap() { m_cacheEquipMap = m_equipMap; m_equipMap.clear(); }
	void restoreEquipMap() { m_equipMap = m_cacheEquipMap; m_cacheEquipMap.clear(); }
	const EquipMap& getcacheEquipMap() { return m_cacheEquipMap; }

	virtual const std::tuple<bzbee::tstring/*animation*/, bzbee::tstring/*effect*/> getAtkAniByIndex(bool isJumping, int index) const { return {}; };
	int getAtkAniCount(bool isJumping);
	

	void getLastEquipCompare(ChangeAttrMap& chgMap);
	void getLastBuffCompare(ChangeAttrMap& chgMap);
	
	bool isDisableOpenDoor() { return m_isDisableOpenDoor; };
	void setIsDisableOpenDoor(bool isDisable) { m_isDisableOpenDoor = isDisable; };
	bool isCanOpenDoor() { return m_isCanOpenDoor; };
	void setIsCanOpenDoor(bool isCan) { m_isCanOpenDoor = isCan; };
	void setAcceSpeed(bool isAcceSpeed) { m_isAcceSpeed = isAcceSpeed; };
	bool isAcceSpeed() { return m_isAcceSpeed; };
	void SetMoveSpeedRatio(float fMoveSpeedRatio);
	float GetMoveSpeedRatio();

	// 角色模型重载，用于变形效果
	void startOverrideModel(float overrideWidth, float overrideHeight, int targetMonsterId);
	void endOverrideModel();
	bool getIsOverrideModel() const { return m_isOverrideModel; }
	float getOverrideWidth() const { return m_overrideWidth; }
	float getOverrideHeight() const { return m_overrideHeight; }

	NS::int32 getWaterBreathDeep() const { return m_waterBreathDeep; }
	void setWaterBreathDeep(NS::int32 deep) { m_waterBreathDeep = deep; }

	virtual bzbee::tstring GetShowIDTag();
	void sumEffectBuf(CEffectSum& effectSum, bool bLastBuff = false);


	// 角色装备相关
	virtual const std::shared_ptr<ItemData> getCurrCosWeapon() const;
	virtual const std::shared_ptr<ItemData> getCurrNorWeapon() const;
	virtual const std::shared_ptr<ItemData> getCurrEftWeapon() const;
	virtual const std::shared_ptr<ItemData> getCurrFarmTool() const;
	virtual const std::shared_ptr<ItemData> getEquipByPos(MsgPB::EQUIP_PACKET_POS ePos) const;
	virtual const std::shared_ptr<ItemData> getEquipIncludingHideByPos(MsgPB::EQUIP_PACKET_POS ePos) const;
	const std::shared_ptr<ItemData> getEquipmentByItemUuid(NS::I64 nItemUuid);

	bool removeEquipmentByPosIndex(NS::int32 pos, std::shared_ptr<ItemData>& pRemoveEquipment);		// 移除某位置的装备
	bool removeEquipmentByItemUuid(NS::I64 itemUuid, std::shared_ptr<ItemData>& pRemoveEquipment);	// 移除某ID的装备
	bool switchEquipment(NS::int32 pos, std::shared_ptr<ItemData> pNewEquipment, std::shared_ptr<ItemData>& pOldEquipment); // 替换添加装备
	bool clearAllEquipment();

	void setEquipMap(const EquipMap& equipMap);		// 初始化角色的装备map
	void setEquipMap(int pos, std::shared_ptr<ItemData> itemData);			// 更新某位置的装备
	const EquipMap& getEquipMap() const;	// 获取角色的全部装备
	void getEquipMap(const EquipMap &equipMapIn, EquipMap &equipMapOut);	// 获取模型穿戴装备
	void refreshEquip(const EquipMap &mapEquip);							// 更新换装后角色的模型，(恐龙/卡牌/厨师套装 会替换角色裸模)


	//bool isEquipItem() { return m_isEquipItem; };
	//void setIsCalChangeAttr(bool isCal) { m_isCalChangeAttr = isCal; };
	void getChangeEquipAttr(ChangeAttrMap &attrMap) { attrMap = m_changeEuipAttrs; }
	void getNewEuipName(bzbee::tstring &name) { name = m_newEuipName; };

	//
	PassiveSkillMgr* getPassiveSkillMgr() { return m_PassiveSkillMgr; }

	//
	virtual std::string GetBattleSound(int nSountTpy) { return ""; }
protected:
	//void calChangeEquipAttr(const EquipMap& equipMap);
	//bool isEquip(NS::I64 id, const EquipMap &equipMap);

	bool isEquip(EquipMap &mapEquip, int equipPos, int equipFishionPos);

protected:
	int m_level{ 0};
	int m_hp{0};
//	int m_displayHp{0};

	int m_mp{0};
	int m_nMaxHp{0};
	int m_totalPower{0};
	EquipMap m_lastEquipMap{}; // 用来比较装备属性变更.
	EquipMap m_equipMap{};
	EquipMap m_pvpEquipMap{};
	EquipMap m_cacheEquipMap{};
	MapDataMgr::MapType m_mapType;
	
	bool m_isCanOpenDoor{ false };	
	bool m_isDisableOpenDoor{ false };
//	bool m_refreshFlatter{ false };

	AttrMap m_attrs;				// 属性
	CEffectSum m_effectSum;			// 增加的属性(装备+Buf+成就).
	AttrMap m_attrsServer;				// 服务端下发的属性
	
	std::chrono::steady_clock::time_point m_deadTime;
	bool m_isAcceSpeed{ false };
	float m_fMoveSpeedRatio{ 1.0f };
	glm::vec3 m_orientation{1, 0, 0}; //朝向

	bool allowedNaked{ false };

	//bool m_isCalChangeAttr{ true };
	//bool m_isEquipItem{ false };
	bzbee::tstring m_newEuipName;

private:
	bool m_isDead{ false };
	bool m_initialized{};

	bzbee::tstring m_naked3DModel{ "models/players/ingame/body/man/man_naked_01/man_naked_01.model" };
	bzbee::tstring m_naked3DHighModel{ "models/players/lobby/body/man/man_naked_01/man_naked_01.model" };

	BuffVec m_vecBuff;
	BuffVec m_lastVecBuff;	// 用来比较Buff属性变更.
	// 记录下降低负面效应的buff信息
	// buff data id -> ROLE_EFFECT_TYPE/BUFF_EFFECT_TYPE -> rate
	std::unordered_map<int32, std::unordered_map<int32, int32>> m_reduceNegativeEffectByRateBuffs;
	float m_buffAtkSpeed{};

	NS::int32 m_waterBreathDeep{ 0 };// 用来记录角色是否能够水下呼吸

	bzbee::tstring s_currNakedModel;
	bzbee::tstring s_currNakedHighModel;
	bzbee::tstring s_currNakedAttachId;
	bzbee::IntegerList m_vHideEquip;
	bzbee::IntegerList m_vHideNode;
	bzbee::IntegerList m_vFemaleHideNode;
	bzbee::tstring s_trailModel;
	int m_currSelectPos{ -1 };
	int m_currSelectPosAtPvP{ -1 };
	bool m_isEquipTwoHandWeapon;

	NS::int32 m_groupId{ -1 };

	std::shared_ptr<ItemData> m_clothesEquipItem{};
	std::shared_ptr<ItemData> m_PantsEquipItem{};

	bool m_isOverrideModel{ false };
	int m_overrideMonsterId{ -1 };
	float m_overrideWidth{ 0.0f };
	float m_overrideHeight{ 0.0f };

	ChangeAttrMap m_changeEuipAttrs;

	PassiveSkillMgr* m_PassiveSkillMgr{ nullptr };
};
