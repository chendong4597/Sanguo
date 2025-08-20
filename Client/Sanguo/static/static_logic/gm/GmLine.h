/*!
 * \file GmLine.h
 *
 * \author ChenBK;
 * \date 2017/06/16
 *
 * 
 */

#pragma once

#include "BaseTypeDefine.h"
//#include "StringUtil.h"
#include <functional>
//#include "msg/MsgPvpSquad.pb.h"

class GmLineBase
{ 
public:
	virtual bool run(const NS::tstring& gmline) { return false; };
	virtual bool runQuery(const NS::tstring& text) { return false; };
	
};

class GmlineS : public GmLineBase
{
public:
	virtual bool run(const NS::tstring& trimGmline) override;

private:

};

class GmlineC : public GmLineBase
{
public:
	GmlineC();
	virtual ~GmlineC();
	virtual bool run(const NS::tstring& trimGmline) override;
	virtual bool runQuery(const NS::tstring& text) override;

private:
	//template<typename T> bool getGmPara(T& valueOut, int idx)
	//{
	//	if (idx < 0 || idx >= static_cast<int>(m_gmParaList.size())) return false;

	//	return NS::StringUtil::stringToType(valueOut, m_gmParaList[idx]);
	//}

	//NS::uint requestGmParaCount(NS::uint count);

	//Exec Gmline --------------------------------
	//bool setForbidChangeZoom();
	//bool showid();
	//bool hideid();
	//bool createNetWorkLog();
	//bool sendTestLogToSrv();
	//bool openScoreReq();
	//bool openLocalPushReq();
	//bool watchAdmobReq();
	//bool getFriendList();
	//bool registerAddressBook();
	//bool addPlayer	();
	//bool showequip();
	//bool gameobject();
	//bool switchskin();
	//bool shownode();
	//bool showdialog();
	//bool daynightadd();
	//bool daynight();
	//bool daynightopen();
	//bool undergroundLightOpen();
	//bool testnews();
	//bool send();
	//bool testbossnews();
	//bool testwave();
	//bool clearBag();
	//bool turnOnBgm();
	//bool turnOffBgm();
	//bool turnOnUiBatching();
	//bool turnOffUiBatching();
	//bool turnOffFpsLimit();
	//bool turnOnFpsLimit();
	//bool parallelUpateAnimation();
	//bool stat();
	//bool playSound();
	//bool soundSetState();
	//bool soundSetSwitch();
 //   bool clientSwitchMap();
	//bool addDummyMob();
	//bool addDummyPlayer();
	//bool addOneDummyPlayer(int mapObjId, const glm::vec3& pos);
	//bool addLocalItem();
	//bool showmap();
	//bool unlockAllFog();
	//bool towerpvp();
	//bool TowerpvpRevenge();
	//bool towergettomb();
	//bool towerpvpstart();
	//bool towerpvpend();
	//bool droptest();
	//bool checkroom();
	//bool towerPvPRevenge();

	//bool showAirBrick();
	//bool changeShipGunType();
 //   bool setShipPosition();

 //   //bool goNpcPetFight(const NS::StringList& params);
	//bool setCameraPos();
 //   bool setCameraParam();

	//bool changeShipCrystalStatus();

	//bool uploadLogs();

	//bool cancelUploadLog();

	//bool NotifyNpcDakEvent();
	//// 商队到来
	//bool NpcCaravanArrival();
	//// 商队离开
	//bool NpcCaravanLeave();
	//// 商队逗留
	//bool NpcCaravanStay();
	//// 商队被打劫(杀人越货)
	//bool RobNpcCaravan();
	//// 以物易物
	//bool GiftExchange();
	//// 驿站创建协助塔防Mob
	//bool NpcDakCreateDefenseMob();
	//// NPC大悬赏
	//bool NpcPetFight();

	////水晶加护
	//bool CrystalBuff();
	//bool showroom();
	//bool showallroom();

	////飞船升级 
	//bool airshipupdate();

	//bool showplantpos();
	//bool drawplantgrid();

	////屏幕debug信息
	//bool showdebuginfo();

	//bool airshipGunUpgrade();

	//bool checkAllBuildings();

	//bool NpcBadgeRoomCheckUpdateCounter();
	//bool SetHideCropInEvent();
	//bool SetHideBuildingInEvent();

	//bool BeginPatrolAttack();
	//bool EndPatrolAttack();

	//// LUA_GM  --------------------------------
	//bool LUA_GM();

	////Exec Gm Script --------------------------------
	//bool rungmscript();

	//// LocalServer GM --------------------------------
	//bool levelup();
	//bool sethp();
	//bool setfood();
	//bool addexp();
	//bool addcash();
	//bool addcoin();
	//bool addticket();
	//bool addMoney(int type);
	//bool addmob();

	//bool fastNpcBadge();

	//bool AddTowerPVPMob();
	//// 停止自动生怪
	//bool DisableAutobirthMob();
	//// 开启自动生怪
	//bool EnableAutobirthMob();
	//// 清空所有怪
	//bool ClearAllMob();
	//// 清空所有NPC
	//bool ClearAllNpc();
	////停止自动生怪，清空所有怪物，清空所有NPC
	//bool ClearMobAndNpc();
	//// 暂停AI
	//bool SetAllAIEnable();
	//bool setPlayerModel();
	//bool setachieveevent();
	//bool teleport();
	//bool goprivatemap();
	//bool addBuff();
	//bool addTargetBuff();
	//bool setmp();
	//bool survivaltrigger();
	//bool fullHpMp();
	//bool setplantgrowtime();
	//bool settbdebug();
	//bool tranlan();
	////fog
	//bool showfog();
	//bool openLC();
	//bool reqAgreementStatus();
	//bool reqAgreementList();
	//bool agreeAgreement();
	//bool afevent();
	//bool fbevent();
	//bool ratingGame();
	//bool invalidAccount();
	//bool reloadServerCfg();

	//bool tornadoplay();
	//bool togglePlainLightmap();
	//bool clientquitmap();

	//// 小队PVP开始战斗
	//bool SquadPvpGoFight();
	//// 小队PVP离开
	//bool SquadPvpQuitMap();
	//// 小队PVP测试
	//bool SquadPvpCheck();
	//// Server Query --------------------------------
	//bool iggQueryServerInfo();

	//bool showUpgradeNote();

	//bool scaleRT();
	//bool uploadlogfile();

	//bool createDummyPlayer();

	//bool skipNewbie();
	//bool getAdventureLog();

	//bool jumpAchieve();

	//bool fieldAdventrue();


	//// NPC委托 挂机 公共地图 进入地图
	//bool HangupCommonEnterMap();
	//// NPC委托 挂机 公共地图 退出地图
	//bool HangupCommonQuitMap();
	//// NPC委托 挂机 公共地图 切换挂机类型
	//bool HangupCommonSwitchHangupType();

	//// 为方便测试而添加供GM使用的
	//bool SetFieldAdventureDisplayTestMonsterDefId();
	//// 为方便测试而添加供GM使用的
	//bool SetFieldAdventureDisplayTestTaskId();

	////将上次朝贡的时间往前移
	//bool moveTributeLastAwardTime();

private:
	NS::tstring m_gmline;
	NS::StringList m_gmParaList;

	typedef std::function<bool()> GM;
	std::map<NS::tstring, GM> m_gmMap;

	//MsgPB::MsgPvpSquadData m_snapshot;
	//MsgPB::MsgPvpSquadData m_snapshotEnemy;
	std::vector<int> m_vecFieldAdventrueNpcDefID;
};


class Gmline : public GmLineBase
{
public:
	virtual ~Gmline() {}
	static Gmline& getInstance();

	virtual bool run(const NS::tstring& gmline) override;
	virtual bool runQuery(const NS::tstring& text) override;
	void runLua(const NS::tstring& lua);


private:
	GmlineS m_gmlineS;
	GmlineC m_gmlineC;
};
