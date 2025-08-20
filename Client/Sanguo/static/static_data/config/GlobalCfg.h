/*!
 * \file GlobalCfg.h
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include "tinyxml2.h"


struct SightInfo {
	int nMin;
	int nMax;
	SightInfo():nMin(0), nMax(0) {}
};

struct NpcTalkSight {
	int nArmLen;
	int nPitch;
	NpcTalkSight() :nArmLen(0), nPitch(0) {}
};

//距离体积剔除盘配置
struct CullDistance {
	int nSize{};
	int nDistance{};
};

struct SelfSight {
	int nArmLen;
	int nRoll{};
	int nPitch{};
	int nYaw{};
};

struct TouchSightInfo {
	int nArmLen;
	float nRoll{};
	float nPitch{};
	float nYaw{};
};

typedef std::map<int, TouchSightInfo> MAPSIGHTINFO;

struct LoggingCampInfo {
	int nMinTm;
	int nMaxTm;
};

class GlobalCfg
{


public:

	GlobalCfg() {}
	static GlobalCfg& getInstance();

	bool initializeCfg();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);
	void terminate();

	const SightInfo& GetSightInfo() {return m_stuInfoSight;}
	const NpcTalkSight& GetTalkSight() { return m_stuTalkSight; }
	const SelfSight& GetSelfSight() { return m_stuSelfSight; }
	const LoggingCampInfo& GetLoggingCampInfo() { return m_LoggingInfo; }
	const std::vector<CullDistance>& GetCullDistanceVec() { return m_vecCullDistance; }
	bool IsServer() { return m_bServer; }
	const char* GetLoginIp() { return strLoginIp.c_str(); }
	int GetLoginPort() { return nLoginPort; }
	const TouchSightInfo* GetSightInfo(int nTpy, bool bRight = false);

	int GetPlayerInitSpeed() const { return m_nInitSpeed; }

	int GetStrenthDec() const { return m_nStrenthDec; }
	int GetStrenthInc() const { return m_nStrenthInc; }
	int GetBreathInc() const { return m_nBreathInc; }

	int GetHelpLvLimt() const { return m_helpLvLimit; }

	float GetHeroReviveTm() const { return m_fHeroReviveTm; }
private:
	SightInfo m_stuInfoSight;
	NpcTalkSight m_stuTalkSight;
	SelfSight m_stuSelfSight;
	std::vector<CullDistance> m_vecCullDistance;
	std::string strLoginIp;
	int nLoginPort;
	bool m_bServer{ false };
	MAPSIGHTINFO m_mapSightInfo;		//左右上下全视角的初始值
	LoggingCampInfo m_LoggingInfo;      //场景上植物生长
	int  m_nInitSpeed{ 500 };

	int m_nStrenthDec{ 0 };  //跑动时每秒体力消耗
	int m_nStrenthInc{ 0 };  //跑动时每秒体力恢复
	int m_nBreathInc{ 0 };   //平静值回复，愤怒值减少

	float m_fHeroReviveTm{ 0.f }; //hero复活时间
	int m_helpLvLimit{ 20 };	//帮助按钮等级限制
};
