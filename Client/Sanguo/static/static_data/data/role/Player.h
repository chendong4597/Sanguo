/*!
float getAttackPauseTime() const;
 * \file Player.h
 *
 * \author zhijieli
 * \date 2016/12/05
 *
 *
 */
#pragma once
#include "Role.h"
#include "../../config/PlayerAttrCfg.h"

class ItemData;
class Player : public Role
{
public:
	struct PlayerModelData
	{
		int m_race{ 0 };
		int m_gender{ -2 };
		int m_skincolor{ 0 };
		int m_hair{ 0 };
		int m_haircolor{ 0 };
		int m_face{ 0 };
		int m_facecolor{ 0 };
		int m_underwear{ 0 };
		int m_underwearcolor{ 0 };
		int m_ownmapnum{ 0 };
	};

public:
	explicit Player(bzbee::int32 id);

	void onInit() override;

	NS::I64 getIggId() const override { return m_iggId; }
	void clearServerAttr();
	void setServerAttr(MsgPB::ROLE_EFFECT_TYPE type, float value);
	void setAttr(MsgPB::ROLE_EFFECT_TYPE type, float value);
	void calPvPSegAttr();
	void clearAttrs();
	float getServerAttr(MsgPB::ROLE_EFFECT_TYPE type);
	float getAttr(MsgPB::ROLE_EFFECT_TYPE type) const override;
	float getBaseAttr(MsgPB::ROLE_EFFECT_TYPE type, bool isInhert) const override;
	float getWidth() const override;
	float getHeight() const override;
	int getJump() const override;
	const std::tuple<bzbee::tstring/*animation*/, bzbee::tstring/*effect*/> getAtkAniAndEffect(bool isJumping) const override;
	const bzbee::tstring getAtkEffectByAniName(const bzbee::tstring& aniName) const override;
	const bzbee::tstring getHurtEffectByAniName(const bzbee::tstring& aniName) const override;
	const bzbee::tstring getAtkSound() const;
	const bzbee::tstring getAtkHitSound() const;
	float getAttackPauseTime() const;
	int getMaxMp() const override;
	NS::I64 getMaxExp();
	int getMaxFood() const { return 10000; }
	int GetMaxBreath() const { return 100; }
	int getMoney(int type) const;
	NS::I64 getExp() const { return m_exp; }
	int getFood() const { return m_food; }
	int getBreath() const { return m_breath; }
	int getMaxAnger() const { return GetMaxBreath(); }
	int getAnger() const { return GetMaxBreath() - m_breath; }
	PlayerModelData& getPlayerModelData() { return m_PlayerModelData; }
	const NS::tstring& getNickname() const { return m_nickname; }
	int getPvpSegment() const { return m_pvpsegment; }
	int getPvpScore() { return m_pvpscore; }	//pvp总积分
	int getPvpKillScore() { return m_pvpkillscore; }	//pvp总杀人积分
	int getPvpRankScore() { return m_pvprankscore; }	//pvp总排行积分
	int getPvpTime() { return m_pvptime; } // pvp累积时间
	int getPvpCoin() { return m_pvpCoin; } // pvp总货币
	int getEndlessLev() { return m_endlessLev; } // 无尽模式等级
	NS::I64 getEndlessExp() { return m_endlessExp; } // 无尽模式经验
	int getPrivateLev() { return m_privateLev; } // 私人模式等级
	int getCurInsMapId() { return m_curInsMapId; }  //当前副本ID
	NS::I64 getLottyTime() { return m_nLottyTime; }	//抽奖时间

	void setLevel(int lev) override;
	bool getChangeEquipState() const { return m_bChangeEquip; }
	float getCatch() const;
	float getAddAttrValue(const Effect& tDataInfo);	// 即将弃用.
	void setDisableJumpTime(bool bYes);
	float getShortJumpTime() const;
	float getLongJumpTime() const;
	float getInitialVerticalVelocity() const;
	float getAccelerationUp() const;
	float getHpUiCheck() const override;
	void getChangePlayerAttr(int newLevel, ChangeAttrMap &changeAttrs);
	std::map<int, std::chrono::steady_clock::time_point>& getEnemies() { return m_enemies; }

	bool isSelf() const;
	bool isSelfCompId() const;
	bool isEnemy(int mapObjId) const;

	//	void setMaxHp(int hpm) { m_hpm = hpm; }
	int CalculateMaxHp() const override;
	void SetCacheHp(int nHp) { m_nCacheHp = nHp; }
	int GetCacheHp() { return m_nCacheHp; }
	void SetCacheMaxHp(int nMaxHp) { m_nCacheMaxHp = nMaxHp; }
	int GetCacheMaxHp() { return m_nCacheMaxHp; }

	void setIggId(NS::I64 iggId) override { m_iggId = iggId; }
	void setExp(NS::I64 exp) { m_exp = exp; }
	void setFood(int food) { m_food = food; }
	void setBreath(int breath) { m_breath = breath; }
	void setMaxMp(int mpm) { m_mpm = mpm; }
	void setNickname(const NS::tstring& nickname) { m_nickname = nickname; }

	void setPvpSegment(int pvpsegment) { m_pvpsegment = pvpsegment; }
	void setPvpScore(int value) { m_pvpscore = value; }	//pvp总积分
	void setPvpKillScore(int value) { m_pvpkillscore = value; }	//pvp总杀人积分
	void setPvpRankScore(int value) { m_pvprankscore = value; }	//pvp总排行积分
	void setPvpTime(int value) { m_pvptime = value; } // pvp累积时间
	void setPvpCoin(int value) { m_pvpCoin = value; } // pvp总货币
	void setEndlessLev(int value) { m_endlessLev = value; } // 无尽模式等级
	void setEndlessExp(NS::I64 value) { m_endlessExp = value; } // 无尽模式经验
	void setPrivateLev(int value) { m_privateLev = value; } // 私人模式等级
	void setCurInsMapId(int nMapId) { m_curInsMapId = nMapId; }  //当前副本ID
	void setLottyTime(NS::I64 nLotttTm) { m_nLottyTime = nLotttTm; }	//抽奖时间

	void setChangeEquipState(bool bChange);
	void setPlayerModelData(PlayerModelData tData);

	virtual void setPos(const glm::vec3& pos) override;
	//virtual void setOrientation(const glm::vec3& o);

	void calcAttr(bool bInherBuff = true) override;
	void setCalcAttrTriggerAchieveCallback(std::function<void()> fun) { m_calcAttrTriggerAchieveCallback = fun; };
	int getGender() const override { return m_PlayerModelData.m_gender; }
	std::string GetHeadTxt();
	void resetDeadTime() { m_deadTime = {}; }
	void addEnemy(int mapObjId);

	void setMoney(int type, int value) { m_arryMoney[type] = value; };

	void resetExtraAttrByPotionInEndlessMode();
	void addExtraAttrByPotionInEndlessMode(MsgPB::ROLE_EFFECT_TYPE type, int value);

	bool getIsHungryLimit() const;

	const std::tuple<bzbee::tstring/*animation*/, bzbee::tstring/*effect*/> getAtkAniByIndex(bool isJumping, int index) const override;

	const AttrMap& getPlayerAttrInfo() const;
	void calPower();
	void calPvPSegPower(int pvpSegLevel = -1);
	int getPower() { return m_totalPower; };
	NS::tstring getHeadIcon();
	NS::tstring getHalfIcon();
	void resetAttr();

	virtual std::string GetBattleSound(int nSountTpy) override;

public:
	int CalcConsumeHp(float fBaseHp);
	int CalcConsumeMp(float fBaseMp);

public:
	NS::I64 GetADNVideoNextTime() { return m_n64ADNNxtTime; }
	void SetADNVideoNextTime(NS::I64 nextTime) { m_n64ADNNxtTime = nextTime; }

private:
	const PlayerAttrCfg::PlayerAttr* getPlayerAttr() const;

private:
	int m_nCacheHp{ 0 };
	int m_nCacheMaxHp{ 0 };
	NS::I64 m_exp{ 0 };
	int m_food{ 0 };
	int m_breath{ 0 };
	//	int m_hpm{ 0 };
	int m_mpm{ 0 };
	int m_pvpsegment{ 1 };
	int m_pvpscore{ 0 };	//pvp总积分
	int m_pvpkillscore{ 0 };	//pvp总杀人积分
	int m_pvprankscore{ 0 };	//pvp总排行积分
	int m_pvptime{ 0 }; // pvp累积时间
	int m_pvpCoin{ 0 }; // pvp累积时间
	int m_curInsMapId{ 0 }; //当前副本ID

	int m_endlessLev{ 1 }; // 无尽模式等级
	int m_privateLev{ 1 }; // 私人模式等级
	NS::I64 m_endlessExp{ 0 }; // 无尽模式经验

	NS::I64 m_nLottyTime{ 0 };	//抽奖时间
	NS::I64 m_n64ADNNxtTime{ 0 };  //剩余播放激励视频时间

	bzbee::tstring m_nickname{};
	PlayerModelData m_PlayerModelData{};
	// NS::I64 m_iggId{};
	int m_totalPower{};

	bool m_bDisableJumpTime{ false };	// 禁用JumpTime
	bool m_bChangeEquip{ true };
	std::map<int, std::chrono::steady_clock::time_point> m_enemies;

	//货币(可能有无数种，所以采用数组，杖举当作下标，这样客户端不要改代码，当新增货币类型时)
	int m_arryMoney[101]{};

	//无尽中使用秘药的属性加成
	AttrMap m_extraAttrByPotionInEndlessMode{};
	std::function<void()> m_calcAttrTriggerAchieveCallback{ nullptr };
};

//////////////////////////////////////////////////////////////////////////
class PlayerMgr
{
public:
	using MapObjId2Player = std::unordered_map<bzbee::uint, std::shared_ptr<Player>>;
public:
	static PlayerMgr& getInstance();

	const std::shared_ptr<Player>& getSelf() const { return m_self; }
	std::shared_ptr<Player> getPlayer(bzbee::uint mapObjId);

	NS::I64 getSelfIggId();
	NS::I64 getSelfId();

	std::shared_ptr<Player> getPlayerByIggId(NS::I64 iggId);

	void setSelf(const std::shared_ptr<Player>& r);
	bool addPlayer(const std::shared_ptr<Player>& player);
	bool removePlayer(bzbee::uint mapObjId);
	bool removePlayerByIggId(NS::I64 iggid);
	const MapObjId2Player& getOtherPlayer() const { return m_otherPlayers; }
	bool isMyself(NS::I64 uuid);
	void setOtherPlayerEnterMapType(MapDataMgr::MapType atMapType);

	MapObjId2Player getAllPlayers();

	void ClearOtherPlayer();
	void removeAllPlayerData(void);

private:
	std::shared_ptr<Player> m_self;

	MapObjId2Player m_otherPlayers;
};
