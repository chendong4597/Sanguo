/*!
* \file LocalServer_Buff.h
*
* \author zhichenxia
* \date 2018/6/13
*
*
*/
#pragma once
#include "LocalServer.h"
#include "msg/MsgInteraction.pb.h"


class SkillBuffInfo;
class SkillBuffContainer;

NS_BEGIN

class LocalServer_Action : public LocalServer
{
	RTTI_DEF(LocalServer_Action);
	//friend class SkillBuff;

	LocalServer_Action();

public:
	static LocalServer_Action& getInstance(void);

public:
	void onMsgInteractionWalkC2S(const MsgPB::MsgInteractionWalkC2S& msg);
	void onMsgInteractionC2S(const MsgPB::MsgInteractionC2S& msg);
	void onMsgInteractionTurnC2S(const MsgPB::MsgInteractionTurnC2S& msg);
private:

};

NS_END
