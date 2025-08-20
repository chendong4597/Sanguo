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
#include "msg/MsgBuild.pb.h"
#include "common/Timer.h"

namespace MsgPB
{
	class MsgBuildingInfo;
	class MsgBuildPlaceMult;
	class MsgBuildingRemoveMult;
	class MsgPrivateDropBuildDisappearS2C;
	class MsgBuildRoomInfo_S2C;
	class MsgBuildRoomAddNTF_S2C;
	class MsgBuildRoomDelNTF_S2C;
	class MsgBuildTowerDelTowerData_S2C;
	class MsgBuildTowerAddTowerData_S2C;
}

//class DropInfoItemData;

class HandlerBuilding : public bzbee::MsgHandler
{
	friend class BuildCallBackCli;

	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerBuilding);

public:
	static HandlerBuilding& getInstance();

	HandlerBuilding();
	virtual ~HandlerBuilding();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

	//bool requestDig(const glm::ivec2& globalCoord, NS::I64 digId);
	//bool requestValuableBuildDig(NS::I64 uuid, NS::I64 digId);
	bool requestPlace(const glm::ivec2& globalCoord, bzbee::uint templateId, NS::I64 uuid);
	//bool requestValuableBuildPlace(const glm::ivec2& globalCoord, bzbee::uint templateId, NS::I64 uuid);
	//
	//bool requestTowerBuildDig(NS::I64 uuid, NS::I64 digId);
	//bool requestTowerBuildPlace(const glm::ivec2& globalCoord, bzbee::uint templateId, NS::I64 uuid);

	//bool updateViewByDigData(NS::I64 digId, bool isPublicDrop = false, bool isDelay = false, std::function<void()>* delayFunc = nullptr);
	//void removeBuilding(const std::vector<NS::I64>& coordIdList, const DropInfoList& dropInfoList, const std::vector<NS::I64>& refreshCoordIdList, bool isPublicDrop = false, bool isInteractive = false);

	//void showPlaceText(::MsgPB::BUILDING_RESULT res);
	//void onMsgPlace(const MsgPB::MsgBuildPlaceMult& msg);

	//bool requestValuableBuildGet();
	//bool requestValuableBuildExcept(NS::I64 uuid);

	//bool replaceBadge(const glm::ivec2& globalCoord, int m_replaceTempId, int m_replaceBadgeId);

private:

	void onMsgBuildReplace(const MsgPB::MsgBuildReplace_MS2C& msg);
	void onMsgBuildingInfo(const MsgPB::MsgBuildingInfo& msg);
	//void onMsgDig(const MsgPB::MsgBuildingRemoveMult& msg);
	//void onMsgDead(const MsgPB::MsgPrivateDropBuildDisappearS2C& msg);
	//void onMsgBuildRoomInfo(const MsgPB::MsgBuildRoomInfo_S2C& msg);
	//void onMsgBuildRoomError(const MsgPB::MsgBuildPutBadgeCondition_S2C &msg);

	//void onMsgBuildRoomAddNTF_S2C(const MsgPB::MsgBuildRoomAddNTF_S2C& msg);
	//void onMsgBuildRoomDelNTF_S2C(const MsgPB::MsgBuildRoomDelNTF_S2C& msg);

	//void onMsgValuableBuildPutS2C(const MsgPB::MsgValuableBuildPutS2C& msg);
	//void onMsgValuableBuildDigS2C(const MsgPB::MsgValuableBuildDigS2C& msg);
	//void onMsgValuableBuildGetS2C(const MsgPB::MsgValuableBuildGetS2C& msg);
	//void onMsgValuableBuildExceptDigS2C(const MsgPB::MsgValuableBuildExceptDigS2C& msg);
	//
	//void onMsgBuildTowerAddTowerData_S2C(const MsgPB::MsgBuildTowerAddTowerData_S2C& msg);
	//void onMsgBuildTowerDelTowerData_S2C(const MsgPB::MsgBuildTowerDelTowerData_S2C& msg);
};

class HandlerBuildingEvent : public bzbee::Event
{
public:
	bool isBadge{ false };
	bool isPlace{ false };
	int coordX{ 0 };
	int coordY{ 0 };
	int badgeId{ 0 };
};
