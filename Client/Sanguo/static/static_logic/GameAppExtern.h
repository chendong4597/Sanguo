/*!
 * \file GameAppModule.h
 *
 * \author mattho
 * \date 2017/12/20
 *
 *
 */
#pragma once
#include "MsgDecoder.h"
#include "DataHandler.h"
#include "BaseTypeDefine.h"


#include "msg/MsgTroops.pb.h"
#include "msg/MsgCraft.pb.h"
#include "msg/MsgPlayer.pb.h"

class GameAppExtern
{
public:
	static bool IsServer();
	static void SetClientMode(bool bClientMode);
	static bool IsClientMode();
public:
	GameAppExtern();
	virtual ~GameAppExtern();

	static bool initialize(const char* filePath , const char* savePath , const char* basePath);
	static bool Start();
	static bool ShutDown();
	static void terminate();
	static void update();

	static void setDataHandler(DataHandler* dataHandler);
	static DataHandler& getDataHandler();

	static bool sendMsg(NS::uint16 msgId, const NS::Msg& msg);
	static void ShowToastTips(const std::string& strTips, int nEvtTpy = 0);
	static bool sendDirectMsg(NS::uint16 msgId, const ::google::protobuf::Message& msg);
public:
	static void LoginAccount(std::string strAccount, std::string strPwd, int platform, int client_ver);
	static void RegisterAccount(std::string strAccount, std::string strPwd, int platform);
	static void Login(int nId);
	static void requestEnterMap();

	//物品
	static bool requestItemEquip(NS::I64 srcUuid, int equipPacketPos = 0, int bodyPos = 0, NS::I64 clientUseUuid = 0, bool bAutoFindAmuletPos = true);
	static bool requestBuildPlace(int X , int Y , int tempMapId , NS::I64 uuid);
	static bool requestRemoveBuild(NS::I64 uuid);
	static bool requestSyncPacket();  //整理背包

	//种植
	static bool requestPlant(int X, int Y, int tempItemId, NS::I64 uuid);
	static bool requestCutPlant(NS::I64 uuid);
	static bool requestManagerPlant(int X, int Y, NS::I64 protector_id);

	//house
	static bool requestUpgradeHouse(NS::int32 houseId, NS::int32 houseTpy);
	static bool requestHomeHouse(NS::int32 nHouseId, NS::I64 uuid, bool bCheckin);

	//dungeon
	static bool requestDungeonTeam(int nDungeonId);

	//public map
	static void joinPublicMap();

	// z阵法
	static void requestTroopUp(const MsgPB::MsgTroopsC2S& msg);
	static void requestNpcEquip(const MsgPB::MsgProtectorEquipReqC2S& msg);

	//锻造
	static void requestCraft(const MsgPB::MsgCraftC2G& msg);
	static void requestCraftGet(const MsgPB::MsgCraftGetC2G& msg);
	static void requestCraftSpeed(const MsgPB::MsgCraftCompleteC2G& msg);

	//同步基础属性
	static void requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY nBaseTpy, bool bInc, int nValue, 
		MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON reson = MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON_MSG_SYN_BASE_TPY_REASON_UPDATE,
		bool bLocalSyn = true);

public:
	static bool GameEncryptFile(const std::string& pathIn, std::string& pathOut, std::string& fileContentOut, std::string& strFileOut);

//local server 本地服务器
public:
	//主角攻击 要走服务器
	static bool c2cMsgPlayerNormalAttackC2S(NS::I64 iggId, std::vector<int> targetMapObjIds, int serialId, NS::I64 n64Hp, bool isFinalRow);
	static bool c2cMsgPlayerSkillAttackC2S(NS::I64 iggId, std::vector<int> targetMapObjIds, int serialId, NS::I64 n64Hp, bool isFinalRow);
	
	//普通攻击
	static bool c2cNormalAttack(NS::int32 caster_id, NS::int32 target_id);
	static bool c2cNormalAttackTargets(NS::int32 caster_id, std::vector<NS::int32> targets);
	static bool c2cSkillAttack(NS::int32 skillId, NS::int32 caster_id, NS::int32 target_id);
	static bool c2cSkillAttackTargets(NS::int32 skillId, NS::int32 caster_id, std::vector<NS::int32> targets);

	//清除BUFF
	static bool c2cNormalDelBuff(NS::int32 buffid, NS::int32 target_id);
	static bool SplitVecString(std::vector<std::string>& vecStr, const std::string& strIn, const std::string& strSplit);
	//static 
public:
	static std::string  LC_String(const NS::tstring& key);
	static bool IsCanShowVideo();
};

#define __LC(x) GameAppExtern::LC_String(x)

