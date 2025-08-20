/*!
 * \file GameAppModule.h
 *
 * \author mattho
 * \date 2017/12/20
 *
 *
 */
#include "GameAppExtern.h"
#include "Framework.h"
#include "file/FileSystem.h"
#include "BaseType.h"
#include "StringUtil.h"
#include "format.h"
#include "i18n/Localization.h"

#include "data/role/Player.h"

#if defined(_WIN32)
#include <platform/win32/WindowsFileSystem.h>
#else
#include <platform/android/AndroidFileSystem.h>
#endif

#include "NetworkModule.h"
#include "handler/HandlerMgr.h"
#include "common/ConfigInitializer.h"
#include "handler/HandlerLogin.h"
#include "handler/HandlerItem.h"
#include "handler/HandlerBuilding.h"
#include "handler/HandlerTowerBuild.h"
#include "handler/HandlerPlant.h"
#include "handler/HandlerHouse.h"
#include "handler/HandlerTeam.h"
#include "handler/HandlerPublicMap.h"
#include "GameModule/GameAppModule.h"
#include "localserver/LocalServer_Battle.h"
#include "localserver/LocalServer_Buff.h"
#include "common/EncryptUtil.h"
#include "common/MsgMgr.h"
#include "msg/MsgPlayer.pb.h"
#include "data/SkillData.h"
#include "StringUtil.h"

#include <fstream>
#include <vector>
#include <iostream>

#include "stdafx.h"

NS::Framework* g_pFramework = nullptr;
DataHandler* g_pDataHandler = nullptr;
GameAppModule* g_pAppModule = nullptr;

int g_serialId = 0;
bool g_bShowVideo = true;

GameAppExtern::GameAppExtern()
{
}

GameAppExtern::~GameAppExtern()
{
}

bool GameAppExtern::initialize(const char* pszfilePath, const char* savePath, const char* basePath)
{
	NS::FileSystem::cleanup();
#if defined(_WIN32)
	std::string filePath = pszfilePath;
	NS::tstring fileDir = NS::FileUtil::getFileDirectory(filePath);
	NS::tstring fileName = NS::FileUtil::getFileNameWithoutExtension(filePath);
	NS::FileSystem::addSubFileSystem(new NS::WindowsFileSystem(fileDir, NS::SubFileSystem::FP_READABLE, NS::SubFileSystem::FS_ASSET, nullptr));
	// setup internal/external storage
	NS::FileSystem::addSubFileSystem(new NS::WindowsFileSystem(NS::FileUtil::combinePath(filePath + "../x64/", "internalStorage"), NS::SubFileSystem::FP_READ_WRITE, NS::SubFileSystem::FS_INTERNAL, nullptr));
	NS::FileSystem::addSubFileSystem(new NS::WindowsFileSystem(NS::FileUtil::combinePath(filePath + "../x64/", "externalStorage"), NS::SubFileSystem::FP_READ_WRITE, NS::SubFileSystem::FS_EXTERNAL, nullptr));
	//NS::LogUtil::openLogFile(fileName + ".log");
#else
	std::string filePath = pszfilePath;
	NS::tstring fileDir = NS::FileUtil::getFileDirectory(filePath);
	NS::tstring fileName = NS::FileUtil::getFileNameWithoutExtension(filePath);
	NS::tstring fileAssert = fmt::format("{0}Content/", basePath);
	NS::tstring fileInteral = fmt::format("{0}", savePath);
	NS::tstring fileSave = fmt::format("{0}Saved/", basePath);
	LOGI("GameAppExtern::initialize assert fileAssert = {0}", fileAssert);
	LOGI("GameAppExtern::initialize inter fileSave = {0}", fileSave);
	LOGI("GameAppExtern::initialize inter fileInteral = {0}", fileInteral);
	NS::FileSystem::addSubFileSystem(new NS::AndroidFileSystem(fileAssert, NS::SubFileSystem::FP_READABLE, NS::SubFileSystem::FS_ASSET, nullptr));
	NS::FileSystem::addSubFileSystem(new NS::AndroidFileSystem(fileInteral, NS::SubFileSystem::FP_READ_WRITE, NS::SubFileSystem::FS_INTERNAL, nullptr));
	NS::FileSystem::addSubFileSystem(new NS::AndroidFileSystem(fileSave, NS::SubFileSystem::FP_READ_WRITE, NS::SubFileSystem::FS_EXTERNAL, nullptr));
	NS::LogUtil::openLogFile(fileName + ".log");
#endif

	NS::XmlUtil::setDataDecoder(std::bind(EncryptUtil::DecodeData, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
	
	NS::Component::static_initialize();

	///* initialize framework */
	g_pFramework = new NS::Framework();
	if (!g_pFramework->initialize(nullptr))
	{
		g_pFramework->terminate();
		SAFE_DELETE(g_pFramework);
		return false;
	}

	g_pAppModule = new GameAppModule();
	/* initialize app */
	if (!g_pFramework->addComponent(g_pAppModule))
	{
		g_pFramework->terminate();
		SAFE_DELETE(g_pFramework);
		return false;
	}
	//g_pAppModule->initialize(nullptr);
	return true;
}

bool ReadFileToBufferToBinary(const std::string& file_path, std::string& bufferOut)
{
	std::ifstream infile(file_path.c_str(), std::ios::binary | std::ios::ate);
	if (!infile.is_open())
	{
		//std::cout << "Open file [" << file_path << "] failed.";
		//buffer.clear();
		return false;
	}


	if (!infile.is_open()) {
		//std::cerr << "un open" << std::endl;
		return false;
	}

	// 获取文件大小
	std::streamsize size = infile.tellg();
	infile.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);

	// 读取文件内容到缓冲区
	if (infile.read(buffer.data(), size)) {
		//std::cout << "read suc " << size << " bytes" << std::endl;
	}

	std::string str(buffer.begin(), buffer.end());
	bufferOut = str;

	infile.close();
	return true;
}

bool WriteFileToBufferToBinary(const std::string& file_path, std::string& bufferOut)
{
	std::ofstream outFile(file_path.c_str(), std::ios::binary);

	if (!outFile) {
		return false;
	}

	// 将整个字符串内容以二进制形式写入文件
	outFile.write(bufferOut.data(), bufferOut.size());

	// 检查是否写入成功
	if (!outFile) {
		return false;
	}

	// 关闭文件
	outFile.close();

	return true;
}


bool GameAppExtern::GameEncryptFile(const std::string& pathIn, std::string& pathOut, std::string& fileContentOut, std::string& strFileOut)
{
	// Encrypt file name
	pathOut = EncryptUtil::getEncryptedFilePath(pathIn);

	// Encrypt file content
	if (!ReadFileToBufferToBinary(pathIn, fileContentOut)) {
		return false;
	}

	
	fileContentOut = EncryptUtil::EncryptString(fileContentOut.c_str());

	WriteFileToBufferToBinary(strFileOut, fileContentOut);

	return true;
}

void GameAppExtern::terminate()
{
	if (g_pFramework)
	{
		g_pFramework->removeFromParent();
		g_pFramework = nullptr;
	}
	
	SAFE_DELETE(g_pFramework);
}

void GameAppExtern::update()
{
	if (g_pFramework) {
		g_pFramework->step();
		LocalServer_Buff::getInstance().update(g_pFramework->getRealFrameTime());
	}
}

bool GameAppExtern::Start()
{
	if (g_pAppModule)
	{
		g_pAppModule->StartModule();
	}
	return true;
}

bool GameAppExtern::ShutDown()
{
	if (g_pAppModule)
	{
		g_pAppModule->StopModule();
	}
	NetworkModule::getInstance().disconnect();
	return true;
}


void GameAppExtern::setDataHandler(DataHandler* dataHandler)
{
	g_pDataHandler = dataHandler;
}

DataHandler& GameAppExtern::getDataHandler()
{
	return *g_pDataHandler;
}

void GameAppExtern::ShowToastTips(const std::string& strTips, int nEvtTpy)
{
	if (!g_pDataHandler) {
		return;
	}
	HandlerTipsEvent evt;
	evt.m_type = static_cast<HandlerTipsEvent::EventType>(nEvtTpy);
	evt.m_strtips = strTips;
	g_pDataHandler->postEvent(evt);
}


bool GameAppExtern::sendMsg(NS::uint16 msgId, const NS::Msg& msg)
{
	return MsgMgr::getInstance().sendMessage(msgId, &msg);
}

bool GameAppExtern::sendDirectMsg(NS::uint16 msgId, const ::google::protobuf::Message& msg)
{
	return MsgMgr::getInstance().sendMessageDirect(msgId, &msg);
}

void GameAppExtern::Login(int nId)
{
	HandlerLogin::getInstance().loginToLS(nId);
}

void GameAppExtern::LoginAccount(std::string strAccount, std::string strPwd, int platform, int client_ver)
{
	HandlerLogin::getInstance().checkPlatformAccount(strAccount, strPwd, platform, client_ver);
}

void GameAppExtern::RegisterAccount(std::string strAccount, std::string strPwd, int platform)
{
	HandlerLogin::getInstance().registerAccount(strAccount, strPwd, platform);
}


bool GameAppExtern::requestItemEquip(NS::I64 srcUuid, int equipPacketPos, int bodyPos, NS::I64 clientUseUuid, bool bAutoFindAmuletPos)
{
	return HandlerItem::getInstance().requestItemEquip(srcUuid, equipPacketPos, bodyPos, clientUseUuid, bAutoFindAmuletPos);
}

bool GameAppExtern::requestSyncPacket()
{
	return HandlerItem::getInstance().requestSyncItemPacketPos();
}

bool GameAppExtern::requestBuildPlace(int X, int Y, int tempMapId, NS::I64 uuid)
{
	HandlerTowerBuild::getInstance().requestPlace(glm::ivec2(X , Y) , tempMapId , uuid);
	return true;
}

bool GameAppExtern::requestRemoveBuild(NS::I64 uuid)
{
	HandlerTowerBuild::getInstance().requestRemove(uuid);
	return true;
}

void GameAppExtern::requestEnterMap()
{
	HandlerLogin::getInstance().requesetMapDone();
}


bool GameAppExtern::requestPlant(int X, int Y, int tempItemId, NS::I64 uuid)
{
	HandlerPlant::getInstance().requestPlant(glm::ivec2(X, Y), tempItemId, 0);
	return true;
}

bool GameAppExtern::requestCutPlant(NS::I64 uuid)
{
	HandlerPlant::getInstance().requestCut(uuid);
	return true;
}

bool GameAppExtern::requestManagerPlant(int X, int Y, NS::I64 protector_id)
{
	return HandlerPlant::getInstance().requestMangerPlant(X, Y, protector_id);
}

bool GameAppExtern::requestUpgradeHouse(NS::int32 houseId, NS::int32 houseTpy)
{
	HandlerHouse::getInstance().requestUpgrade(houseId, houseTpy);
	return true;
}

bool GameAppExtern::requestHomeHouse(NS::int32 nHouseId, NS::I64 uuid, bool bCheckin)
{
	return HandlerHouse::getInstance().requestHome(nHouseId, uuid, bCheckin);
}

bool GameAppExtern::IsServer()
{
	return MsgMgr::getInstance().IsServerMode();
}

void GameAppExtern::SetClientMode(bool bClientMode)
{
	MsgMgr::getInstance().SetClientMode(bClientMode);
}

bool GameAppExtern::IsClientMode()
{
	return MsgMgr::getInstance().IsClientMode();
}



//*********************************************************************************************************************************
//
//*********************************************************************************************************************************
bool GameAppExtern::requestDungeonTeam(int nDungeonId)
{
	HandlerTeam::getInstance().createTeamByDungeonId(nDungeonId);
	return true;
}

void GameAppExtern::joinPublicMap()
{
	HandlerPublicMap::getInstance().requestJoinToPubllic();
}

void GameAppExtern::requestTroopUp(const MsgPB::MsgTroopsC2S& msg)
{
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void GameAppExtern::requestNpcEquip(const MsgPB::MsgProtectorEquipReqC2S& msg)
{
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void GameAppExtern::requestCraft(const MsgPB::MsgCraftC2G& msg)
{
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void GameAppExtern::requestCraftGet(const MsgPB::MsgCraftGetC2G& msg)
{
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void GameAppExtern::requestCraftSpeed(const MsgPB::MsgCraftCompleteC2G& msg)
{
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY nBaseTpy, bool bInc, 
	int nValue, MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON reson, bool bLocalSyn)
{
	if (nValue == 0) {
		return;
	}
	auto&& self = PlayerMgr::getInstance().getSelf();
	if (!self) {
		return;
	}

	if (bLocalSyn)
	{
		//预先先赋值
		switch (nBaseTpy) {
		case MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_HP:
			if (bInc) {
				self->setHp(std::min(self->getHp() + nValue, self->getMaxHp()));
			}
			else {
				self->setHp(std::max(self->getHp() - nValue, 0));
			}
			break;
		case MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_MP:
			if (bInc) {
				self->setMp(std::min(self->getMp() + nValue, self->getMaxMp()));
			}
			else {
				self->setMp(std::max(self->getMp() - nValue, 0));
			}
			break;
		case MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_STRENTH:
			if (bInc) {
				self->setFood(std::min(self->getFood() + nValue, self->getMaxFood()));
			}
			else {
				self->setFood(std::max(self->getFood() - nValue, 0));
			}
			break;
		case MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_ANGER:
			if (bInc) {
				self->setBreath(std::min(self->getBreath() + nValue, self->GetMaxBreath()));
			}
			else {
				self->setBreath(std::max(self->getBreath() - nValue, 0));
			}
			break;
		default:
			break;
		}
	}
	
	MsgPlayerSynBaseC2S msg;
	msg.set_basetpy(nBaseTpy);
	msg.set_binc(bInc);
	msg.set_nvalue(nValue);
	msg.set_reason(reson);
	MsgMgr::getInstance().sendMessageDirect(msg);
}


//*********************************************************************************************************************************
//local
//*********************************************************************************************************************************
bool GameAppExtern::c2cMsgPlayerNormalAttackC2S(NS::I64 iggId, std::vector<int> targetMapObjIds, int serialId, NS::I64 n64Hp, bool isFinalRow)
{
	MsgPlayerNormalAttackC2S msg;
	msg.set_iggid(iggId);
	for (const auto& id : targetMapObjIds)
	{
		msg.add_target_obj(id);
	}
	msg.set_serialid(serialId);
	msg.set_idhp(n64Hp);
	msg.set_isfinalblow(isFinalRow);
	NS::MsgMgr::getInstance().sendMessage(MsgPB::_MSG_PLAYER_NORMAL_ATTACK_C2S, &msg);
	return true;
}

//因为是技能触发，因此暂时不实现
bool GameAppExtern::c2cMsgPlayerSkillAttackC2S(NS::I64 iggId, std::vector<int> targetMapObjIds, int serialId, NS::I64 n64Hp, bool isFinalRow)
{
	return true;
}

//**************************************************************************
//local
//**************************************************************************

bool GameAppExtern::c2cNormalAttack(NS::int32 caster_id, NS::int32 target_id)
{
	LocalServer_Battle::getInstance().fight(caster_id, target_id);
	return true;
}

bool GameAppExtern::c2cNormalAttackTargets(NS::int32 caster_id, std::vector<NS::int32> targets)
{
	for (int i = 0; i < targets.size(); i++)
	{
		LocalServer_Battle::getInstance().fight(caster_id, targets[i]);
	}
	return true;
}

bool GameAppExtern::c2cSkillAttack(NS::int32 skillId, NS::int32 caster_id, NS::int32 target_id)
{
	auto pSkillType = SkillCfg::getInstance().findSkillType(skillId);
	if (!pSkillType)
	{
		return false;
	}
	SkillDataPtr pSkillData = SkillData::create(caster_id, 0);
	pSkillData->setType(pSkillType);
	SkillDataMgr::getInstance().add(pSkillData);
	LocalServer_Battle::getInstance().skillDamage(pSkillData->getUniqueId(), skillId, caster_id, target_id);
	return true;
}

bool GameAppExtern::c2cSkillAttackTargets(NS::int32 skillId, NS::int32 caster_id, std::vector<NS::int32> targets)
{
	auto pSkillType = SkillCfg::getInstance().findSkillType(skillId);
	if (!pSkillType)
	{
		return false;
	}
	for (int i = 0; i < targets.size(); i++)
	{
		SkillDataPtr pSkillData = SkillData::create(caster_id, 0);
		pSkillData->setType(pSkillType);
		//pSkillData->setTargetPosition(targetPosition);
		//pSkillData->setTargetList(pTargetIds, targetCount);
		SkillDataMgr::getInstance().add(pSkillData);

		LocalServer_Battle::getInstance().skillDamage(pSkillData->getUniqueId(), skillId, caster_id, targets[i]);
	}
	return true;
}

bool GameAppExtern::c2cNormalDelBuff(NS::int32 buffid, NS::int32 target_id)
{
	if (buffid == 0)
	{
		LocalServer_Buff::getInstance().delAllBuff(target_id, true);
	}
	else {
		LocalServer_Buff::getInstance().removeBuff(buffid, target_id);
	}
	
	return true;
}

std::string GameAppExtern::LC_String(const NS::tstring& key)
{
	return _LC(key);
}

bool GameAppExtern::SplitVecString(std::vector<std::string>& vecStr, const std::string& strIn, const std::string& strSplit)
{
	NS::StringUtil::splitString<std::vector<std::string>>(vecStr, strIn, strSplit);
	return true;
}

bool GameAppExtern::IsCanShowVideo()
{
	return g_bShowVideo;
}
