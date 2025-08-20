/*!
* \file ItemData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include "msg/MsgPlant.pb.h"
#include <array>
#include <map>

using namespace NS;



class SurvivalData
{
public:
	SurvivalData();
	virtual ~SurvivalData();

	bool initialize();
	void reset();

	NS::I64 GetUuid() { return m_uuid; }
	void SetUuid(NS::I64 uuid) { m_uuid = uuid; }

public:
	NS::I64 m_uuid{ 0 };
	NS::int32 m_plantTpyId{ 0 };
	int n_PosX{ 0 };
	int n_PosY{ 0 };
	NS::I64 m_beginTm{ 0 };
	NS::I64 m_growTm{ 0 };
	int m_growStage{ 0 };
};

//------------------------------------------------------------------
//SurvivalDataMgr
//------------------------------------------------------------------
class SurvivalDataMgr
{
public:
	typedef std::unordered_map<NS::I64/*uuid*/, std::shared_ptr<SurvivalData>> SurvivalDataMap;					//所有数据, Key: UUID

public:
	SurvivalDataMgr();
	virtual ~SurvivalDataMgr();

	static SurvivalDataMgr& getInstance();
	void terminate();

	void clearData();


private:
};
