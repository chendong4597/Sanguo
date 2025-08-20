/*!
 * \file HandlerBuilding.cpp
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */
#include "stdafx.h"
#include "../common/MsgMgr.h"
#include "msg/MsgPlant.pb.h"
#include "../GameAppExtern.h"
#include "data/StoreData.h"
#include "HandlerStore.h"

using namespace NS;

static HandlerStore* g_HandlerStore = nullptr;
HandlerStore& HandlerStore::getInstance()
{
    assert(g_HandlerStore);
    return *g_HandlerStore;
}

HandlerStore::HandlerStore()
{
    assert(!g_HandlerStore);
	g_HandlerStore = this;
    reset();
}

HandlerStore::~HandlerStore()
{
	g_HandlerStore = nullptr;
}

bool HandlerStore::initialize()
{
	REGISTER_S2C(onMsgStoreGetDataResultS2C);
	REGISTER_S2C(onMsgStoreBuyResultS2C);
	REGISTER_S2C(onMsgStoreSellResultS2C);
    return true;
}

void HandlerStore::terminate()
{
    reset();
}

bool HandlerStore::reset()
{
    return true;
}

void HandlerStore::onMsgStoreGetDataResultS2C(const MsgPB::MsgStoreGetDataResultS2C& msg)
{	
	if (msg.result() != MsgStoreGetDataResultS2C_RESULT_SUCCESS) {
		return;
	}
	std::vector<NS::int32> vecInventory;
	for (int i = 0; i < msg.data().inventory_size(); i++)
	{
		vecInventory.push_back(msg.data().inventory(i));
	}
	StoreDataMgr::getInstance().AddStoreData(msg.data().uuid(), vecInventory);
	HandlerStoreEvent evt;
	evt.m_uuid = msg.data().uuid();
	POST_DATA_EVENT(evt);
}

void HandlerStore::onMsgStoreBuyResultS2C(const MsgPB::MsgStoreBuyResultS2C& msg)
{
	if (msg.result() != MsgStoreBuyResultS2C_RESULT_SUCCESS) {
		return;
	}
	std::vector<NS::int32> vecInventory;
	for (int i = 0; i < msg.data().inventory_size(); i++)
	{
		vecInventory.push_back(msg.data().inventory(i));
	}
	StoreDataMgr::getInstance().AddStoreData(msg.data().uuid(), vecInventory);
	HandlerStoreEvent evt;
	evt.m_uuid = msg.data().uuid();
	POST_DATA_EVENT(evt);
}

void HandlerStore::onMsgStoreSellResultS2C(const MsgPB::MsgStoreSellResultS2C& msg)
{

}


