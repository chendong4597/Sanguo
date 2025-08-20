/*!
 * \file BuffCfg.cpp
 *
 * \author zhijieli
 * \date 2017/06/13
 *
 * 
 */
#include "stdafx.h"
#include "BuffCfg.h"
#include "../common/calculate.h"
#include "StringUtil.h"
#include <XmlUtil.h>

////////////////////////////////////////////////////////////////
BuffCfg& BuffCfg::getInstance()
{
	static BuffCfg s_BuffCfg;
	return s_BuffCfg;
}

bool BuffCfg::initializeCfg()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Buff.bin")) return false;
	if (!parseBuffConfig(doc.RootElement())) return false;

    reloadMultiLanguage();

    return true;
}

void BuffCfg::terminate()
{
	m_buffTypeMap.clear();
}

void BuffCfg::reloadMultiLanguage()
{
    for (auto& pr : m_buffTypeMap)
    {
        auto& buffType = pr.second;
        buffType.s_name = _LC(buffType.s_name_untrans);
        buffType.s_buffdes = calcluate::getMedianValueName(buffType.s_buffdes_untrans, [&, buffType](int index)
        {
            return static_cast<int>(buffType.m_value[index]);
        });
        buffType.s_buffdes = _LC(buffType.s_buffdes);
    }
}

const BuffType* BuffCfg::findBuffType(bzbee::uint32 id) const
{
	auto it = m_buffTypeMap.find(id);
	return it == m_buffTypeMap.end() ? nullptr : &it->second;
}

bool BuffCfg::parseBuffConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		BuffType buffType{};

		NS::XmlUtil::getAttribute(buffType.n_ID, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(buffType.s_name_untrans, xmlItem, "s_name");// = proto.s_name();
		NS::XmlUtil::getAttribute(buffType.s_buffdes_untrans, xmlItem, "s_buffdes");// = proto.s_buffdes();
		NS::XmlUtil::getAttribute(buffType.s_effectres, xmlItem, "s_effectres");// = proto.s_effectres();
		NS::XmlUtil::getAttribute(buffType.s_bufficon, xmlItem, "s_bufficon");// = proto.s_bufficon();
		NS::XmlUtil::getAttribute(buffType.s_buff_color, xmlItem, "s_buff_color");// = proto.s_buff_color();
		NS::XmlUtil::getAttribute(buffType.s_action, xmlItem, "s_action");// = proto.s_action();
		NS::XmlUtil::getAttribute(buffType.n_groupid, xmlItem, "n_groupid");// = proto.n_groupid();
		NS::XmlUtil::getAttribute(buffType.n_group_rank, xmlItem, "n_group_rank");// = proto.n_group_rank();
		NS::XmlUtil::getAttribute(buffType.n_interval_time, xmlItem, "n_interval_time");// = proto.n_interval_time();
		NS::XmlUtil::getAttribute(buffType.n_max_heap, xmlItem, "n_max_heap");// = proto.n_max_heap();
		NS::XmlUtil::getAttribute(buffType.n_auto_heap, xmlItem, "n_auto_heap");// = proto.n_auto_heap();
		NS::XmlUtil::getAttribute(buffType.n_logout_save, xmlItem, "n_logout_save");// = (BuffType::BUFF_LOGOUT_SAVE)proto.n_logout_save();
		NS::XmlUtil::getAttribute(buffType.n_definition, xmlItem, "n_buff_positioning");// = proto.n_buff_positioning();
		NS::XmlUtil::getAttribute(buffType.s_effect_soundKey, xmlItem, "s_effect_sound");// = proto.s_effect_sound();
		std::string s_effectattachment;
		NS::XmlUtil::getAttribute(s_effectattachment, xmlItem, "s_effectattachment");
		bzbee::StringUtil::splitString(buffType.s_effectattachment, s_effectattachment, ",");
		NS::XmlUtil::getAttribute(buffType.n_alive_time, xmlItem, "n_alive_time");// = proto.n_alive_time();
		NS::XmlUtil::getAttribute(buffType.n_effectplaytype, xmlItem, "n_effectplaytype");// = proto.n_effectplaytype();
	
		NS::XmlUtil::getAttribute(buffType.n_BuffEffID, xmlItem, "n_bufEffId");// = proto.n_effectplaytype();

		for (int i = 0 ; i < 8 ; i++)
		{
			int n_effect_type = 0;
			AttrEffectValue attr;
			AttrEffectValue attr2;
			NS::XmlUtil::getAttribute(n_effect_type, xmlItem, fmt::format("n_effect_type_{0}", i + 1).c_str());
			NS::XmlUtil::getAttribute(attr.attrref, xmlItem, fmt::format("n_effect_attrref1_{0}", i + 1).c_str());
			NS::XmlUtil::getAttribute(attr.value, xmlItem, fmt::format("n_effect_value1_{0}", i + 1).c_str());
			NS::XmlUtil::getAttribute(attr2.attrref, xmlItem, fmt::format("n_effect_attrref2_{0}", i + 1).c_str());
			NS::XmlUtil::getAttribute(attr2.value, xmlItem, fmt::format("n_effect_value2_{0}", i + 1).c_str());
			buffType.effects[i].type = (MsgPB::ROLE_EFFECT_TYPE)n_effect_type;
			buffType.effects[i].ref_values.push_back(attr);
			buffType.effects[i].ref_values.push_back(attr2);
		}

		NS::XmlUtil::getAttribute(buffType.n_buff_del, xmlItem, "n_buff_del");// = proto.n_buff_del();
		NS::XmlUtil::getAttribute(buffType.n_buff_display, xmlItem, "n_buff_display");// = proto.n_buff_display();
		NS::XmlUtil::getAttribute(buffType.n_lock_buff_display, xmlItem, "n_lock_buff_display");// = proto.n_lock_buff_display();
		NS::XmlUtil::getAttribute(buffType.n_effect_solution, xmlItem, "n_effect_solution");// = proto.n_effect_solution();
		NS::XmlUtil::getAttribute(buffType.n_buff_effect_orientation, xmlItem, "n_buff_effect_orientation");// = proto.n_buff_effect_orientation();

		int n_buff_gob{0};
		NS::XmlUtil::getAttribute(n_buff_gob, xmlItem, "n_buff_gob");
        buffType.is_debuff = (n_buff_gob == 2);

		//bzbee::StringList avaClr;
		//
		//if (bzbee::StringUtil::splitString(avaClr, proto.s_ava_color(), ",") == 3)
		//{
		//	buffType.ava_clr.r = std::atoi(avaClr[0].c_str()) / 255.f;
		//	buffType.ava_clr.g = std::atoi(avaClr[1].c_str()) / 255.f;
		//	buffType.ava_clr.b = std::atoi(avaClr[2].c_str()) / 255.f;
		//	buffType.ava_clr.a = 1.f;
		//}
		//avaClr.clear();
  //      if (bzbee::StringUtil::splitString(avaClr, proto.s_ava_frame_color(), ",") == 3)
		//{
		//	buffType.ava_frame_color.r = std::atoi(avaClr[0].c_str()) / 255.f;
		//	buffType.ava_frame_color.g = std::atoi(avaClr[1].c_str()) / 255.f;
		//	buffType.ava_frame_color.b = std::atoi(avaClr[2].c_str()) / 255.f;
		//	buffType.ava_frame_color.a = 1.f;
		//}

		NS::XmlUtil::getAttribute(buffType.m_value[0], xmlItem, "f_value_0");// = proto.f_value_0();
		NS::XmlUtil::getAttribute(buffType.m_value[1], xmlItem, "f_value_1");// = proto.f_value_1();
		NS::XmlUtil::getAttribute(buffType.m_value[2], xmlItem, "f_value_2");// = proto.f_value_2();
		NS::XmlUtil::getAttribute(buffType.m_value[3], xmlItem, "f_value_3");// = proto.f_value_3();
		NS::XmlUtil::getAttribute(buffType.m_value[4], xmlItem, "f_value_4");// = proto.f_value_4();
		
		m_buffTypeMap.insert(std::make_pair(buffType.n_ID, buffType));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}
