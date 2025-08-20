/*!
* \file ItemData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include "../config/ItemCfg.h"
#include "../config/PlantCfg.h"
#include <array>
#include <map>

using namespace NS;


enum {
	NOTES_DATA_TPY_NONE = 0,
	NOTES_DATA_TPY_NPC_LV,  //NPC升级
	NOTES_DATA_TPY_GEN_DEAD, //武将死亡
	NOTES_DATA_TPY_NPC_IN,   //NPC入住
	NOTES_DATA_TPY_NPC_OUT,  //NPC移除房间
	NOTES_DATA_TPY_NPC_PLANT,  //NPC管理种植
	NOTES_DATA_TPY_PLANT_IN, //植物种植
	NOTES_DATA_TPY_PLANT_GET, //植物丰收
	NOTES_DATA_TPY_PLANT_CUT, //植物收割
	NOTES_DATA_TPY_PLANT_MANA, //植物管理
	NOTES_DATA_TPY_CRAFT_BEGIN, //锻造开始
	NOTES_DATA_TPY_CRAFT_END, //锻造完成
	NOTES_DATA_TPY_CRAFT_GET, //锻造领取
	NOTES_DATA_TPY_DEFENCE_IN, //防御塔放置
	NOTES_DATA_TPY_DEFENCE_OUT, //防御塔移除
	NOTES_DATA_TPY_DEFENCE_LV, //防御塔升级
	NOTES_DATA_TPY_HOUSE_LV, //房屋升级
	NOTES_DATA_TPY_HOUSE_LV_END, //房屋升级完成
	NOTES_DATA_TPY_LOTTERY, //祈福
	NOTES_DATA_TPY_RENT, //收取租金
};


class NotesData
{
public:
	NotesData();
	virtual ~NotesData();

public:
	int m_noteTpy{ NOTES_DATA_TPY_NONE };
	std::string m_strNotes;
};

class NotesDataMgr
{
public:
	typedef std::deque<std::shared_ptr<NotesData>> VecNotesData;

public:
	NotesDataMgr();
	virtual ~NotesDataMgr();

	static NotesDataMgr& getInstance();
	void terminate();

	void Clear() { m_vecNotes.clear(); }

	//add\remove
	bool addItem(int nNoteTpy, NS::I64 uuid = 0, NS::I64 uuid2 = 0, int nParam1 = 0, int nParam2 = 0);
	bool addItem(std::shared_ptr<NotesData> noteData);
	const VecNotesData& GetNoteDatas() { return m_vecNotes; }

private:
	VecNotesData m_vecNotes;
};
