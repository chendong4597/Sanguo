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
#include "tinyxml2.h"

#define PER_CELL_SEL_WIDTH  1280
#define PER_CELL_SEL_HEIGHT 1280

enum {
	Defence_Build_PosTpy_B = 0,//防御塔
	Defence_Build_PosTpy_P,//种植物
};

enum {
	Defence_Build_PosTpy_Land = 1 << 0,//城墙前面位置
	Defence_Build_PosTpy_LandFloor = 1 << 1,//城墙脚边位置
	Defence_Build_PosTpy_LandCeil = 1 << 2,//城墙上面边缘
	Defence_Build_PosTpy_LandWall = 1 << 3 //城墙上面
};

class DefencePosTpyCfg
{
public:
	struct DefencePosInfo
	{
		int X{};
		int Y{};
	};
	struct DefencePosTpyInfo
	{
		int n_ID{};
		int n_Type{};
		int n_PosTpy{};           //位置信息 ， 对应
		int n_CameraLen{};
		int n_AnglesR{};
		int n_AnglesY{};
		int n_BeginX{};
		int n_BeginY{};
		int n_BeginZ{};
		int n_Width{};
		int n_Height{};
		std::string s_BPTag;   //Tag信息
		std::vector<DefencePosInfo> vecHeroRangeInfo;
		std::vector<DefencePosInfo> vecRangeInfo;
	};

	typedef std::map<int, DefencePosTpyInfo> MAP_DEFENCEPOSINFO;

public:

	DefencePosTpyCfg() {}
	static DefencePosTpyCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const DefencePosTpyInfo* GetDefencePosById(int id);
	const MAP_DEFENCEPOSINFO& GetDefencePos() { return m_mapDefencePosInfo; }
	void GetDefenceJustPos(int nId , int& nPosX , int& nPosY , int nWidth , int nHeight);
	void GetDefenceJustPosWall(int nId, int& nPosX, int& nPosY, int nWidth, int nHeight);

	int GetBlockWidth() { return m_nBlockWidth; }
	int GetBlockHeight() { return m_nBlockHeight; }

private:
	MAP_DEFENCEPOSINFO m_mapDefencePosInfo;
	int m_nBlockWidth{};
	int m_nBlockHeight{};
};
