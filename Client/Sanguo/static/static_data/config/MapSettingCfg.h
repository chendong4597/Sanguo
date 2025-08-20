/*!
* \file MapSettingCfg.h
*
* \
* \date 2017/07/12
*
*
*/
#pragma once
#include "BaseTypeDefine.h"
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"
#include "../common/EffectEnum.h"


class MapSettingCfg final
{
	MapSettingCfg();
public:
	enum DungeonType
	{
		Endless_Dungeon = 6,
		Team_Dungeon = 3,
	};

	enum BossRoomType
	{
		BossRoom_None = 0,
		BossRoom_Stronghold = 1,
		BossRoom_PrivateDungeon = 2,
	};

	enum DunGeneral_Type
	{
		DunGeneral_Type_All = 0,
		DunGeneral_Type_General = 1,
		DunGeneral_Type_Self = 2,
	};
public:
	struct S_DungeonMobInit
	{
		int m_mobTpy = 0; //怪物类型  0:怪物 1：防御塔
		int m_nMobStage = 0; //怪物出现阶段
		int m_nId = 0; //怪物ID
		int m_iMobId = 0; // 怪物 配置ID
		int m_iSubMobId = 0; // 怪物 小弟配置ID
		int m_iSubMobNum = 0; // 怪物数量
		int m_X = 0;
		int m_Y = 0;
		int m_Z = 0;
		int m_Yaw = 0;
	};

	struct MapSetting
	{
		NS::int32   id;
		NS::int32   nextMapId;
		NS::tstring name;
		NS::tstring name_untrans;
		NS::int32   type;
		NS::int32	biome;
		NS::tstring maptemplate;
        NS::tstring mapfilename;
		NS::int32   dungeontype;
		NS::int32   generaltype; //带兵类型  0：兵和武将  1：武将 2：单打独斗
		NS::tstring icon;
		NS::int32   level;
		NS::int32 suggestLv;
		NS::int32   difficulty;
		NS::int32   grouptype;
		NS::int32   hunger;
		NS::int32   passtime{};  //通关时间
		std::vector<NS::tstring> dayBgm;
		std::vector<NS::tstring> nightBgm;
		std::vector<NS::tstring> underGroudBgm;
		NS::int32  bgmChangeBorderUp;
		NS::int32  bgmChangeBorderDown;
		NS::int32  m_undergroundBorderUp;
		NS::int32  m_undergroundBorderDown;
		NS::int32  m_lavaBorderUp;
		NS::int32  m_laveBorderDown;
		std::vector<NS::tstring> lavaBgm;
		float m_maxZoom;
		float m_minZoom;
		NS::tstring  endlessFaithIcon;
		NS::int32 m_nMinPlayerNum;
		NS::tstring  m_bgId;
		NS::tstring m_loadingPictureId;
		NS::tstring m_characterPanelBg;
		NS::tstring m_mapPath;

		std::unordered_map<NS::int32, NS::int32> mapEntranceItemCost;
		std::vector<NS::int32> m_availibleItemRewards;

		bool m_bIsDayNight{};
		bool n_enable_lightmap{ true };
		int n_Transfer{};

		NS::int32  m_nMoleExistHeight;
		int m_nMoleBirthProb{ 0 };
		int m_nMoleBirthCD{ 0 };
		int m_nMoleBirthRadius{ 0 };
		int m_nMoleBirthCheckTime{ 0 };
		int m_nMoleMobId{ 0 };

		NS::int32 n_NeedDungeon{ 0 };
		NS::int32 n_first_drop_Id{0};
		NS::int32 n_drop_Id{0};

		NS::int32 m_nMaxTreeMan{};

		std::vector<glm::ivec4> m_npcTalkArea{};
		std::vector<std::vector<NS::tstring>> m_npcTalkKey{};

		std::vector<int> m_npcTalkStrongholdId{};
		std::vector<std::vector<NS::tstring>> m_npcTalkKeyStronghold{};

		std::vector<NS::tstring> m_npcTalkKeyDungeon{};
		BossRoomType m_eBoosRoomType{ BossRoom_None };
		int n_mine_reset_time{ 0 };
		int n_mine_reset_num{ 0 };
		std::vector<S_DungeonMobInit> m_vecInsMobs;
	};

    typedef std::unordered_map<NS::int32, MapSetting> Map;

	struct MainCityMob
	{
		NS::int32   id;
		NS::int32   popTime;
		NS::int32   prob;
	};

	typedef std::unordered_map<NS::int32, MainCityMob> MapMob;

public:
	static MapSettingCfg& getInstance();

	bool initialize();
	void terminate();
public:
	const MapSetting* findMap(NS::int32 id) const;
	void GetInstanceTpy(int mapIds, std::vector<MapSetting>& vecMaps);
	void GetGenInstance(std::vector<MapSetting>& vecMaps);
	NS::int32 GetNextDungeonMapId(NS::int32 id);


	//
	void GetMapMobs(int nMinLv, int nMaxLv, std::vector<MainCityMob>& mapMobs);
public:
    bool parseMapSettingConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseMapMobConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	Map m_settingMap;
	MapMob m_mapMaincityMob;
};
