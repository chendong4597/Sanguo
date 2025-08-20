/*!
* \file ItemData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include "msg/MsgItem.pb.h"
#include "ItemData.h"
#include <array>
#include <map>

using namespace NS;



struct TeamMemberInfo
{
	NS::UI64 iggId{};
	NS::tstring nickname;
	NS::int32 level{};

	NS::int32 skin{};
	NS::int32 hair{};
	NS::int32 hairColor{};
	NS::int32 seatIndex{ -1 };
	NS::int32 gender{ 0 };
	std::unordered_map<int, std::shared_ptr<ItemData>> equipMap;
};

//TowerBuildMgr
//------------------------------------------------------------------
class TeamMemberDataMgr
{
public:
	typedef std::map<NS::I64, TeamMemberInfo> MapTeamMembers;				//所有数据, Key: UUID

public:
	TeamMemberDataMgr();
	virtual ~TeamMemberDataMgr();

	static TeamMemberDataMgr& getInstance();
	void terminate();

	void clearData();
private:
	MapTeamMembers m_mapTeamMembers;
};
