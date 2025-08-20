/*!
* \file ItemCfg.h
*
* \author cd
* \date 12/01/2016
*
*
*/
#pragma once

//#include <baseutil.h>
#include "BaseTypeDefine.h"
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"

namespace config
{
    //class ItemTagSet;
    //class ItemTagSet_ItemTag;

    //class ItemClassnameSet;
    //class ItemClassnameSet_ItemClassname;

    //class CooldownSet;
    //class CooldownSet_Cooldown;
}

enum {
	COIN_ITEM_ID = 1,  //银币
	TICKET_ITEM_ID,   //门票
	CASH_ITEM_ID,     //金砖，晶砖
};

enum {
	ITEM_CLASS_TPY_NONE = 0,
	ITEM_CLASS_TPY_MATRIAL,  //材料
	ITEM_CLASS_TPY_EQUIP, //装备
	ITEM_CLASS_TPY_PROP,  //道具
	ITEM_CLASS_TPY_DEF_USE, //零件
	ITEM_CLASS_TPY_NPC_SOUL, //灵魂
	ITEM_CLASS_TPY_DEF,		//炮台
	ITEM_CLASS_TPY_SEED,		//种子
	ITEM_CLASS_TPY_FOOD,		//食物
	ITEM_CLASS_TPY_MEDIA,		//药剂
	ITEM_CLASS_TPY_FUWEN,		//符文
};

class ItemCfg
{
public:
    enum BagClass : int
	{
		All = 0,
		Weqpon,
		Tool,
		Building,
		Armor,
		Materlal,
		Food,
		Other,
		BagClassMax,
	};

	// 使用道具需處理的系統-迁移Server Enum
	enum ITEM_USE_BY_SYSTEM
	{
		ITEM_USE_BY_SYSTEM_ITEM = 0,		// 僅道具系統
		ITEM_USE_BY_SYSTEM_SURVIVAL = 1,	// 生存塔防系統
		ITEM_USE_BY_SYSTEM_FOOD = 2,		// 食物系統
		ITEM_USE_BY_SYSTEM_MEDICAMENT = 3,	// 藥劑系統
		ITEM_USE_BY_SYSTEM_INTERACTIVE = 4,	// 互動效果系統
		ITEM_USE_BY_ENDLESS_POTION = 5,		// 无尽模式 高级药水
		ITEM_USE_BY_SYSTEM_PET_FOOD = 6,	// 宠物饲料
		ITEM_USE_BY_SYSTEM_PET_EGG = 7,		// 宠物蛋
		ITEM_USE_BY_SYSTEM_PET_CAPTURE = 8,	// 宠物捕捉器
	};

	struct S_ItemTemplate_use
	{
		int nEffect{ 0 };
		int nTarget{ 0 };
		int nValue{ 0 };
	};

	struct S_Material
	{
		int n_ID{ 0 };
		int n_Type{ 0 };
		std::string  s_icon;
		std::string  s_name;
		std::string  s_desc;
	};

	typedef std::vector<S_Material> VEC_MATERIAL;
	typedef std::map<int, VEC_MATERIAL> MAP_MATERIAL;

	// Item.xml
	//------------------------------------------------------------------
	typedef std::vector<std::string> VEC_DES;
	class Item
	{
	public:
		const std::string& getUseModle() const { return s_use_model; }
		const bool getUseModleFollow() const { return n_use_model_follow; } // 角色跟随.
		const bool getUseModleTrigger() const { return n_use_model_trigger; } // 发送Item使用消息[0 = 动画播放前发, 1 = 动画播放完后发].
        std::string getNameByQgStyle() const; // 获取带品级的 name Text.
        std::string getBgQgStyle() const;
        std::string getQgStyleColor() const; // 获取品级 ColorId
        bool canUse() const;
		bool checkUsefulMap(int mapId) const;						// 检查可使用 mapid
		bool canHeap() const { return n_max_heap > 1; };				// 检查可堆叠

        ITEM_USE_BY_SYSTEM GetUseBySystem() const;
        bool isHightQg() const;		// 是否高品级.
    public:
		int m_itemId;
		std::string s_name;  //道具名字
		std::string s_name_untrans;  //道具名字
		std::string s_icon;			//Icon
		int m_qualityGrade;			//道具品级
		std::string s_foder;		//文件夹位置
		std::string s_drop_model;	  //掉落外观
		std::string s_hand_model;	  //手持外观
		std::string s_use_model;			// 使用Item 播放动画.
		bool n_use_model_trigger;			// 使用Item 播放动画触发时机[0 = 播放前, 1 = 播放完后].
		bool n_use_model_follow;			// 使用Item 播放动画, 角色跟随.
		std::string s_use_play_animation;	// 使用Item 播放角色动作.
		int m_maxHeap;		//最大叠加数		
		float f_model_scale;  //掉落模型缩放
		int n_use_lv;				//使用等级	
		int n_sub_type;					//0:Normal 1 : Furniture 2 : Armor 道具类型
		int n_bag_class;				// 背包分类
		int n_class_name;  //道具显示类型
		NS::tstring m_tags;			//TAG(ex: <wood>, <stone>)
		std::string m_itemClassName;
		std::string m_itemClassName_untrans;
		int n_drop_id;			//掉落ID		
		int n_cost;					//售价	
		int n_recyclingpricetype;		//商品回收价类型
		int n_recyclingpricevalue;		//商品回收价
		int n_menu_id;					//菜单ID
		int n_canauction;			  //可拍卖
		int n_can_repair;				//可修复
		int n_useful;					//可使用
		int n_max_heap;		//最大叠加数	
		int n_cooldown_type;				//使用冷却类型
		int n_cooldown;					//(ms)使用冷却时间
		int n_show_info_type;				//使用数量限制
		std::string s_expried_time;				//yy:mm:dd hh:mm:ss 使用期限
		int n_countdown_time;			//有效期 分
		int n_shortcut_type;				//快捷键对应栏位 
		int n_can_trade;					//可交易
		int n_can_drop;					//可丢弃
		int n_consume;					//可消耗 
		int n_gift;						//可赠送
		int n_gift_max;					//可赠送数量上限
		std::string s_tags;			//TAG(ex: <wood>, <stone>)
		int n_magic_injection_score{};	// 能为附魔提供的能量
		int n_selllimit{ 0 };		//出售上限;
		std::string s_use_toast;	// 使用Item 弹提示Toast.
		int n_use_effect_1{};
		int n_item_num{ 0 };
		int n_item_tipnum{ 0 };
		std::string s_from_where{};
		std::string m_fromWhere_untrans{};
		bool n_is_npc_addition{};

		VEC_DES m_vDes;  //功能说明
		VEC_DES m_vDes_untrans;
		std::vector<int> m_usefulMapIds;           //使用的地图id列表.
		std::vector<S_ItemTemplate_use> m_vecUse;			// 使用 效果, 目标, 值.
	};

public:
	typedef std::unordered_map<unsigned int, Item> ItemMap;
    typedef std::unordered_map<unsigned int, const Item*> ItemPtrMap;

   // typedef std::unordered_map<int, const config::ItemTagSet_ItemTag*> ItemTagMap;
   // typedef std::unordered_map<int, const config::ItemClassnameSet_ItemClassname*> ItemClassnameMap;
   typedef std::unordered_map<int, int> CooldownMap;


public:
	static ItemCfg& getInstance();

	bool initializeCfg();
	void terminate();

    const Item* getItemById(int itemId);
    const std::string& getUseToast(int itemId);
	int getCooldownType(int cdType);

    void findItemMapByTag(ItemPtrMap& mapOut, const std::string& strTag, int nSubType = 0);
    bool checkItemTag(const std::string& strTags, const std::string& strTag);
	bool checkCanUse(int itemId);

   // const config::ItemTagSet_ItemTag* getItemTag(int tagId);
	//void getVecItem(std::vector<int>& vecOut, const std::string &name);

    bool reloadMultiLanguage();

	std::string getItemClassName(int nItemId);
	const std::vector<int> getRefiningVec() { return m_itemRefiningVec; };
	const std::vector<int> getRefineFoodVec() { return m_itemRefineFoodVec; };

	const MAP_MATERIAL& getMapMaterial() { return m_mapItemMaterial; }
	const S_Material* getMaterial(int nID);

private:
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseMatConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseCooldownTypeXml(tinyxml2::XMLElement* pXmlRoot);
private:
	ItemMap m_itemMap;

    //config::ItemClassnameSet* m_itemClassNameList{};
    //ItemClassnameMap m_itemClassNameMap;

   // config::ItemTagSet* m_itemTagList{};
   // ItemTagMap m_itemTagMap;

   // config::CooldownSet* m_cooldownList{};
    CooldownMap m_cooldownMap;

	std::vector<int> m_itemRefiningVec{};
	std::vector<int> m_itemRefineFoodVec{};
	MAP_MATERIAL	 m_mapItemMaterial;
};
