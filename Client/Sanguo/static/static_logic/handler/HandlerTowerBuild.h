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
#include "msg/MsgBuildTower.pb.h"
#include "common/Timer.h"

namespace MsgPB
{
	class MsgBuildTowerInfoListS2C;
	class MsgBuildTowerAddTowerData_S2C;
	class MsgBuildTowerDelTowerData_MS2GS;
	class MsgBuildTowerUpdateRes_MS2C;
}

//class DropInfoItemData;

class HandlerTowerBuild : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerTowerBuild);

public:
	static HandlerTowerBuild& getInstance();

	HandlerTowerBuild();
	virtual ~HandlerTowerBuild();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

	bool requestPlace(const glm::ivec2& globalCoord, int templateId, NS::I64 uuid);
	bool requestRemove(NS::I64 uuid);

private:

	void onMsgBuildTowerInfoListS2C(const MsgPB::MsgBuildTowerInfoListS2C& msg);
	void onMsgBuildTowerAddTowerData_S2C(const MsgPB::MsgBuildTowerAddTowerData_S2C& msg);
	void onMsgBuildTowerDelTowerData_S2C(const MsgPB::MsgBuildTowerDelTowerData_S2C& msg);
	void onMsgBuildTowerUpdateRes_MS2C(const MsgPB::MsgBuildTowerUpdateRes_MS2C& msg);

};


