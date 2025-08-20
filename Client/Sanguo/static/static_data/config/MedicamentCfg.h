#pragma once

#include "BaseTypeDefine.h"
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"

class ItemData;

//------------------------------------------------------------------------------
class MedicamentCfg
{
public:

	enum EN_MEDICAMENT_TYPE
	{
		EN_MEDICAMENT_TYPE_HP,				// HP
		EN_MEDICAMENT_TYPE_HP_PER,			// HP百分比
		EN_MEDICAMENT_TYPE_MP,				// MP
		EN_MEDICAMENT_TYPE_MP_PER,			// MP百分比
		EN_MEDICAMENT_TYPE_HPMP,			// HP MP
		EN_MEDICAMENT_TYPE_HPMP_PER,		// HP MP百分比
		EN_MEDICAMENT_TYPE_BUFF,			// BUFF
		EN_MEDICAMENT_TYPE_BREATH,

		EN_MEDICAMENT_TYPE_NUM,
		EN_MEDICAMENT_TYPE_NONE = EN_MEDICAMENT_TYPE_NUM,
	}; // End of TYPE

	enum EN_BUF_EFFECT
	{
		EN_BUF_EFFECT_NONE,
		EN_BUF_EFFECT_HP,			// HP
		EN_BUF_EFFECT_MP,			// MP
		EN_BUF_EFFECT_HPMP,			// HP MP

		EN_BUF_EFFECT_MAX,
	}; // BUF_EFFECT

	struct MedicamentType
	{
		NS::uint32 n_ID;
		NS::tstring s_name;
		NS::tstring s_name_untrans;
		NS::tstring s_note;				// 描述
		NS::tstring s_note_untrans;
		NS::uint32 n_effect_type;		// 特效
		NS::uint32 n_medicament_type;	// BUF类 HP,MP 区分字段
		//NS::uint32 n_cd;
		NS::uint32 n_val;
	};

    using MedicamentTypeMap = std::unordered_map<NS::uint32, MedicamentType>;

public:
	static MedicamentCfg& getInstance();

	bool initialize();
	void terminate();

	const MedicamentType* findMedicament(NS::uint32 id) const;

private:
	bool parseMedicamentXml(tinyxml2::XMLElement* pXmlRoot);

private:
	MedicamentTypeMap m_medicamentTypeMap;

};

