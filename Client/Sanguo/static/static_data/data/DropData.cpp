/*!
* \file ItemData.cpp
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#include "stdafx.h"
#include "DropData.h"
#include "ItemData.h"


DropDataMgr::DropDataMgr()
{

}

DropDataMgr::~DropDataMgr()
{

}

DropDataMgr& DropDataMgr::getInstance()
{
	static DropDataMgr s_DropDataMgr;
	return s_DropDataMgr;
}

void DropDataMgr::terminate()
{

}

bool DropDataMgr::addItem(MsgPB::PBDropInfo dropData)
{
	MP_DROP::iterator iter = m_mapDrop.find(dropData.data_id());
	if (iter != m_mapDrop.end())
	{
		return false;
	}
	m_mapDrop[dropData.data_id()] = dropData;
	return true;
}

bool DropDataMgr::removeItem(NS::I64 delUuid)
{
	auto iter = m_mapDrop.find(delUuid);
	if (iter != m_mapDrop.end())
	{
		m_mapDrop.erase(iter);
	}
	return true;
}

MsgPB::PBDropInfo* DropDataMgr::getDropItem(NS::I64 delUuid)
{
	auto iter = m_mapDrop.find(delUuid);
	if (iter != m_mapDrop.end())
	{
		return &iter->second;
	}
	return nullptr;
}

const bzbee::tstring& DropInfoItemData::getPiceId()
{
	ItemData itemData;
	itemData.initWithItemId(data_id);
	if (itemData.m_pItem)
	{
		return itemData.m_pItem->s_icon;
	}
	return bzbee::EMPTY_STRING;
}
