
#include "stdafx.h"
#include "TeamMemberData.h"

using namespace NS;
using namespace MsgPB;




//------------------------------------------------------------------------------------------------
// TowerBuildMgr
//------------------------------------------------------------------------------------------------

TeamMemberDataMgr::TeamMemberDataMgr()
{

}

TeamMemberDataMgr::~TeamMemberDataMgr()
{

}

void TeamMemberDataMgr::terminate()
{
	clearData();
}

void TeamMemberDataMgr::clearData()
{
	m_mapTeamMembers.clear();
}


TeamMemberDataMgr& TeamMemberDataMgr::getInstance()
{
	static TeamMemberDataMgr s_teamMemberMgr;
	return s_teamMemberMgr;
}



