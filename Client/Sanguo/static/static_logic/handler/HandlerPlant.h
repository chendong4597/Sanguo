/*!
 * \file HandlerBuilding.h
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */

#pragma once

#include <MsgHandler.h>
#include "BaseTypeDefine.h"
#include <memory>
#include "msg/MsgPlant.pb.h"
#include "common/Timer.h"

namespace MsgPB
{
	class MsgPlantInfo;
	class MsgPlantSet;
	class MsgPlantCut;
}

//class DropInfoItemData;

class HandlerPlant : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerPlant);

public:
	static HandlerPlant& getInstance();

	HandlerPlant();
	virtual ~HandlerPlant();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

	bool requestPlant(const glm::ivec2& globalCoord, int templateId, NS::I64 uuid);
	bool requestCut(NS::I64 uuid);
	bool requestMangerPlant(int X, int Y, NS::I64 protector_id);

private:
	void onMsgPlantInfo(const MsgPB::MsgPlantInfo& msg);
	void onMsgPlantSet(const MsgPB::MsgPlantSet& msg);
	void onMsgPlantCut(const MsgPB::MsgPlantCut& msg);

};


