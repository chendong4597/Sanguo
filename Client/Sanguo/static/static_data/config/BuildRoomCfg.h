/*!
* \file NpcBadgeCfg.h
* \date 08-30-2017 15:29:14
*
* \author LiQiangqiang
*/
#pragma once
#include <unordered_map>
#include <memory>
#include <vector>


class BuildRoomCfg
{
public:
	class BuilRoomDef
	{
	public:
		int n_ID{};
		NS::tstring s_name{};
		NS::tstring s_test_note{};
		int n_min_grid{};
		int n_max_grid{};
		int n_back_ground{};
		int n_furnitures[5];
		int n_comfortable_oper{};
		int n_comfortable_need{};
		int n_height_oper{};
		int n_height_need{};
		int n_resurrection_cost{};
		int n_npcId{};
		int n_Rent{};
		int n_Gift{};
		int n_equipSubType{};
		int n_replace_Npc;
		float f_GiftProb{};
		float f_GiftProbMax{};
		float f_watiTimeSec{};

		int n_Check_Priority{};

		std::vector<NS::int32> v_build_TypeId; // BuildCfgType.xlsx n_TypeId,所需要的家俱类型(门类,火把类)
		NS::tstring s_Npc_Video;
		NS::tstring s_Sub_EquipType_Icon;
	};
	float m_delTime{};
	float m_RefreshRentalTime{};

	typedef std::unordered_map<int, BuilRoomDef> BuilRoomDefMap;

public:
	static BuildRoomCfg& getInstance();

	bool initialize();
	void terminate();

	const BuildRoomCfg::BuilRoomDef* findBuilRoomDef(int id);
	const BuilRoomDefMap& getNpcBadgeDefMap() const { return m_builRoomDefMap; };
	int getBadgeId(int npcId);
	bool isBadge(int badgeId);
	int getNpcId(int badgeId);
	int getNpcRebornTime() { return m_npcRebornTime; };
	int getRebornCastMoney(int leftTime,int badgeLevel);
	int getReplaceNpcId(int npcId);
	
private:
	bool parseBuilRoomConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	BuilRoomDefMap m_builRoomDefMap;
	float m_paramA{};
	float m_paramB{};
	float m_paramC{};
	int m_npcRebornTime{};
};
