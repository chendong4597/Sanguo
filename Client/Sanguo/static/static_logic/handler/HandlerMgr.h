/*!
 * \file HandlerMgrC.h
 *
 * \author laizhisheng
 * \date 2016/10/31 11:28
 *
 */
#pragma once
#include <component/Component.h>
#include "MsgHandler.h"


class HandlerMgr : public NS::Component
{
	RTTI_DEF(HandlerMgr);

public:
    static HandlerMgr& getInstance();

	HandlerMgr();
	virtual ~HandlerMgr();

	virtual bool initialize(NS::Component* pEntity) override;
	virtual void terminate() override;
	virtual void update(float dt) override;

	void initializeLocalServers();
	void regAllMsgId();

	void openClientTransfer(int msgId);
	void closeClientTransfer(int msgId);

	bool initializeHandlers();

public:
	void addHandler(NS::MsgHandler* handler);

	void regAllClientMsgId();
	void regClientMsgId(int sendId, int recvId);

private:
	typedef std::vector<NS::MsgHandler*> MsgHandlerList;
	MsgHandlerList m_vMsgHanders;

	// ClientMsg Vector
	std::vector<std::pair<int, int>> m_vecClientMsgIds{};
};
