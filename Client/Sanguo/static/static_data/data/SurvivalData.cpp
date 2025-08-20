
#include "stdafx.h"
#include "SurvivalData.h"

using namespace NS;
using namespace MsgPB;

SurvivalData::SurvivalData()
{

}

SurvivalData::~SurvivalData()
{

}

bool SurvivalData::initialize()
{
	return true;
}


void SurvivalData::reset()
{
}



//------------------------------------------------------------------------------------------------
// SurvivalDataMgr
//------------------------------------------------------------------------------------------------

SurvivalDataMgr::SurvivalDataMgr()
{

}

SurvivalDataMgr::~SurvivalDataMgr()
{

}


SurvivalDataMgr& SurvivalDataMgr::getInstance()
{
	static SurvivalDataMgr s_survivalDataMgr;
	return s_survivalDataMgr;
}




