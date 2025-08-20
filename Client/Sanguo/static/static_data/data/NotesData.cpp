
#include "stdafx.h"

#include "StringUtil.h"

#include "config/NpcTypeCfg.h"
#include "config/PlantCfg.h"
#include "config/HouseCfg.h"
#include "config/CraftCfg.h"
#include "config/ItemCfg.h"

#include "PlantData.h"
#include "CraftData.h"
#include "HouseData.h"
#include "NpcProtectorData.h"
#include "TowerBuildData.h"
#include "NpcProtectorData.h"


#include "NotesData.h"

using namespace NS;
using namespace MsgPB;

NotesData::NotesData()
{

}

NotesData::~NotesData()
{

}


//------------------------------------------------------------------------------------------------
// TowerBuildMgr
//------------------------------------------------------------------------------------------------

NotesDataMgr::NotesDataMgr()
{

}

NotesDataMgr::~NotesDataMgr()
{

}


NotesDataMgr& NotesDataMgr::getInstance()
{
	static NotesDataMgr s_noteDataMgr;
	return s_noteDataMgr;
}

bool NotesDataMgr::addItem(std::shared_ptr<NotesData> noteData)
{
	m_vecNotes.push_front(noteData);
	if (m_vecNotes.size() >= 100)
	{
		m_vecNotes.pop_back();
	}
	return true;
}

bool NotesDataMgr::addItem(int nNoteTpy, NS::I64 uuid, NS::I64 uuid2, int nParam1, int nParam2)
{
	auto note = std::make_shared<NotesData>();
	if (!note)
	{
		return false;
	}
	std::string strNotes;

	switch (nNoteTpy) {
	case NOTES_DATA_TPY_NPC_LV:
	{
		auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(uuid);
		if (!data) {
			return false;
		}

		auto&& npcType = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
		if (!npcType) {
			return false;
		}
		strNotes = fmt::format(_LC("UI_NOTES_TIPS_NPC_LV"), _LC(npcType->s_name.c_str()), nParam1);
	}
	break;
	case NOTES_DATA_TPY_PLANT_IN:
	{
		auto&& data = PlantDataMgr::getInstance().findPlantByUuid(uuid);
		if (!data) {
			return false;
		}
		
		auto&& pPlantCfg = PlantCfg::getInstance().findPlantType(data->m_plantTpyId);
		if (!pPlantCfg) {
			return false;
		}
		int nPos = data->n_PosX + data->n_PosY*PLANT_ROW_NUM;
		if (pPlantCfg->m_growType == PlantCfg::GrowType::TREE)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_IN"), nPos, _LC(pPlantCfg->s_name.c_str()));
		} else if (pPlantCfg->m_growType == PlantCfg::GrowType::ANIMAL)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_FARM_IN"), nPos, _LC(pPlantCfg->s_name.c_str()));
		}
		else if (pPlantCfg->m_growType == PlantCfg::GrowType::MINE)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_MINE_IN"), nPos, _LC(pPlantCfg->s_name.c_str()));
		}
	}
	break;
	case NOTES_DATA_TPY_PLANT_CUT:
	{
		auto&& data = PlantDataMgr::getInstance().findPlantByUuid(uuid);
		if (!data) {
			return false;
		}

		auto&& pPlantCfg = PlantCfg::getInstance().findPlantType(data->m_plantTpyId);
		if (!pPlantCfg) {
			return false;
		}
		int nPos = data->n_PosX + data->n_PosY*PLANT_ROW_NUM;
		if (pPlantCfg->m_growType == PlantCfg::GrowType::TREE)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_CUT"), nPos, _LC(pPlantCfg->s_name.c_str()));
		} else if (pPlantCfg->m_growType == PlantCfg::GrowType::TREE)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_FARM_CUT"), nPos, _LC(pPlantCfg->s_name.c_str()));
		}
	}
	break;
	case NOTES_DATA_TPY_PLANT_GET:
	{
		auto&& data = PlantDataMgr::getInstance().findPlantByUuid(uuid);
		if (!data) {
			return false;
		}

		auto&& pPlantCfg = PlantCfg::getInstance().findPlantType(data->m_plantTpyId);
		if (!pPlantCfg) {
			return false;
		}
		std::string strNpcName;
		int nPos = data->n_PosX + data->n_PosY*PLANT_ROW_NUM;
		if (uuid2 != 0)
		{
			auto&& dataPro = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(uuid2);
			if (!dataPro) {
				return false;
			}

			auto&& npcType = NpcTypeCfg::getInstance().GetNpcType(dataPro->itemid);
			if (!npcType) {
				return false;
			}
			strNpcName = npcType->s_name;
		}
		if (pPlantCfg->m_growType == PlantCfg::GrowType::TREE)
		{
			if (strNpcName.empty())
			{
				strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_OUT"), nPos, _LC(pPlantCfg->s_name.c_str()));
			}
			else {
				strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_OUT_NPC"), nPos, _LC(pPlantCfg->s_name.c_str()), _LC(strNpcName.c_str()));
			}
			
		}
		else if (pPlantCfg->m_growType == PlantCfg::GrowType::ANIMAL)
		{
			if (strNpcName.empty())
			{
				strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_FARM_OUT"), nPos, _LC(pPlantCfg->s_name.c_str()));
			}
			else {
				strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_FARM_OUT_NPC"), nPos, _LC(pPlantCfg->s_name.c_str()), _LC(strNpcName.c_str()));
			}
		}
		else if (pPlantCfg->m_growType == PlantCfg::GrowType::MINE)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_MINE_OUT"), nPos, _LC(pPlantCfg->s_name.c_str()));
		}
	}
	break;
	case NOTES_DATA_TPY_PLANT_MANA:
	{
		auto&& data = PlantDataMgr::getInstance().findPlantByUuid(uuid);
		if (!data) {
			return false;
		}

		auto&& pPlantCfg = PlantCfg::getInstance().findPlantType(data->m_plantTpyId);
		if (!pPlantCfg) {
			return false;
		}

		auto dataNpc = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(uuid2);
		if (!dataNpc) {
			return false;
		}

		auto&& npcType = NpcTypeCfg::getInstance().GetNpcType(dataNpc->itemid);
		if (!npcType) {
			return false;
		}
		int nPos = data->n_PosX + data->n_PosY*PLANT_ROW_NUM;
		strNotes = fmt::format(_LC("UI_NOTES_TIPS_PLANT_MAN"), nPos, _LC(pPlantCfg->s_name.c_str()), _LC(npcType->s_name.c_str()));
	}
	break;
	case NOTES_DATA_TPY_NPC_IN:
	{
		auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(uuid);
		if (!data) {
			return false;
		}
		auto&& npcType = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
		if (!npcType) {
			return false;
		}
		strNotes = fmt::format(_LC("UI_NOTES_TIPS_NPC_IN"), _LC(npcType->s_name.c_str()), nParam1);
	}
	break;
	case NOTES_DATA_TPY_NPC_OUT:
	{
		auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(uuid);
		if (!data) {
			return false;
		}
		auto&& npcType = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
		if (!npcType) {
			return false;
		}
		strNotes = fmt::format(_LC("UI_NOTES_TIPS_NPC_OUT"), _LC(npcType->s_name.c_str()), nParam1);
	}
	break;
	case NOTES_DATA_TPY_HOUSE_LV_END:
	{
		auto&& pCfg = HouseCfg::getInstance().getHouseTypeInfo(nParam2);
		if (!pCfg)
		{
			return false;
		}

		if (nParam1 == BUILD_CITY_ID_WALL)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_HOUSE_UP_WALL_END"), _LC(pCfg->s_Name.c_str()));
		} else if(nParam1 == BUILD_CITY_ID_CHAMER)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_HOUSE_UP_FLAG_END"), _LC(pCfg->s_Name.c_str()));
		}
		else if (nParam1 == BUILD_CITY_ID_ROAD)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_HOUSE_UP_ROAD_END"), _LC(pCfg->s_Name.c_str()));
		}
		else {
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_HOUSE_UP_END"), nParam1, _LC(pCfg->s_Name.c_str()));
		}
	}
	break;
	case NOTES_DATA_TPY_HOUSE_LV:
	{
		auto&& pCfg = HouseCfg::getInstance().getHouseTypeInfo(nParam2);
		if (!pCfg)
		{
			return false;
		}
		if (nParam1 == BUILD_CITY_ID_WALL)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_HOUSE_UP_WALL_BEGIN"), _LC(pCfg->s_Name.c_str()));
		}
		else if (nParam1 == BUILD_CITY_ID_CHAMER)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_HOUSE_UP_FLAG_BEGIN"), _LC(pCfg->s_Name.c_str()));
		}
		else if (nParam1 == BUILD_CITY_ID_ROAD)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_HOUSE_UP_ROAD_BEGIN"), _LC(pCfg->s_Name.c_str()));
		}
		else {
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_HOUSE_UP_BEGIN"), nParam1, _LC(pCfg->s_Name.c_str()));
		}
	}
	break;
	case NOTES_DATA_TPY_CRAFT_BEGIN:
	case NOTES_DATA_TPY_CRAFT_END:
	case NOTES_DATA_TPY_CRAFT_GET:
	{
		auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(uuid);
		if (!data) {
			return false;
		}

		auto&& npcType = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
		if (!npcType) {
			return false;
		}

		const CraftCfg::Craft* pCfgItem = CraftCfg::getInstance().getCraft(nParam1);
		if (!pCfgItem)
		{
			return false;
		}

		NS::uint itemId{};
		NS::StringUtil::stringToType(itemId, pCfgItem->m_resultParam);
		auto&& pItem = ItemCfg::getInstance().getItemById(itemId);
		if (!pItem)
		{
			return false;
		}
		if (nNoteTpy == NOTES_DATA_TPY_CRAFT_BEGIN)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_CRAFT_BEGIN"), _LC(npcType->s_name.c_str()), _LC(pItem->s_name.c_str()));
		} else if (nNoteTpy == NOTES_DATA_TPY_CRAFT_END)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_CRAFT_END"), _LC(npcType->s_name.c_str()), _LC(pItem->s_name.c_str()));
		}
		else if (nNoteTpy == NOTES_DATA_TPY_CRAFT_GET)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_CRAFT_GET"), _LC(npcType->s_name.c_str()), _LC(pItem->s_name.c_str()));
		}
	}
	break;
	case NOTES_DATA_TPY_LOTTERY:
	{
		auto&& pItem = ItemCfg::getInstance().getItemById(nParam1);
		if (!pItem)
		{
			return false;
		}
		if (nParam2 == 0)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_LOTTERY_0"), _LC(pItem->s_name.c_str()));
		} else if (nParam2 == 1)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_LOTTERY_1"), _LC(pItem->s_name.c_str()));
		}
		else if (nParam2 == 2)
		{
			strNotes = fmt::format(_LC("UI_NOTES_TIPS_LOTTERY_2"), _LC(pItem->s_name.c_str()));
		}
	}
	break;
	case NOTES_DATA_TPY_RENT:
	{
		strNotes = fmt::format(_LC("UI_NOTES_TIPS_RENT"), nParam1);
	}
	break;
	default:
		break;
	}


	if (!strNotes.empty())
	{
		note->m_noteTpy = nNoteTpy;
		note->m_strNotes = strNotes;
		addItem(note);
		return true;
	}
	return false;
}



