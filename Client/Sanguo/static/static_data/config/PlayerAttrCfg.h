/*!
 * \file PlayerAttrCfg.h
 *
 * \author zhijieli
 * \date 2017/02/09
 *
 * 
 */
#pragma once

#include <unordered_map>
#include <vector>
#include "tinyxml2.h"
#include "../data/role/Role.h"

class PlayerAttrCfg
{
public:
	struct AttachInfo
	{
		AttachInfo() { reset(); };
		void reset()
		{
			m_mainAttachId = bzbee::EMPTY_STRING;
			m_attachPos = -1;
			m_attachName = bzbee::EMPTY_STRING;
			m_mainAttachNode = bzbee::EMPTY_STRING;
			m_weight = 0;
		}
		NS::tstring m_mainAttachId;
		NS::tstring m_subAttachId;
		int m_attachPos;
		NS::tstring m_mainAttachNode;
		NS::tstring m_subAttachNode;
		NS::tstring m_attachName;
		NS::tstring m_attachName_untrans;
		int m_weight;
		bzbee::IntegerList m_vHideNode;
		bzbee::IntegerList m_vHideEquip;
		bzbee::IntegerList m_vDisablePos;
		glm::vec3 m_vRotation;
		NS::tstring m_mainPivotNode;
		NS::tstring m_subPivotNode;
		bzbee::tstring m_animPath;

		bool operator <(const AttachInfo &r)const   //升序排序  
		{
			return m_weight < r.m_weight;
		}

		bool operator >(const AttachInfo &r) const   //降序排序  
		{
			return m_weight > r.m_weight;
		}
	};

	struct NodeInfo
	{
		int m_nodePos;
		NS::tstring m_male_nodeId;
		NS::tstring m_female_nodeId;
		NS::tstring m_nodeName;
		NS::tstring m_nodeName_untrans;
	};

	struct PlayerAttr  //与等级挂钩的属性值
	{
		int n_ID;
		NS::I64 n_exp;			 //当前等级需要的经验
		int n_HPM;					 // 生命最大值
		int n_HPR;                   // 生命回復
		int n_MPM;					 // 魔力最大值
		int n_MPR;                   // 魔力回復
		int n_AD;				     // 物理攻擊
		int n_AP;				     // 魔法攻擊
		int n_ARM;			         // 物理防禦
		int n_MARM;			         // 魔法防禦
		int n_Speed;				 // 移动速度
		int n_MaxHuman;				 // 最多人数
	};

	int n_Breath;
	float f_PlayerHeight;
	float f_PlayerWidth;
	float f_AtkRange;
	float f_AtkSpeed;
	int n_AR;                        // 物理減免	
	int n_MR_Fire;                   // 魔法減免 - 火
	int n_MR_Forst;                  // 魔法減免 - 冰
	int n_MR_Thunder;                // 魔法減免 - 雷
	int n_MR_Earth;                  // 魔法減免 - 土
	int n_MR_Poison;                 // 魔法減免 - 毒
	int n_MR_Light;                  // 魔法減免 - 光
	int n_MR_Dark;                   // 魔法減免 - 暗
	float f_RES;                     // 物理韌性	
	float f_MRES;                    // 魔法韌性
	float f_CRI;		             // 物理爆擊率
	float f_MCRI;		             // 魔法爆擊率
	float f_ARP;		             // 物理穿透
	float f_MRP;		             // 魔法穿透
	float f_CRID;		             // 物理爆擊傷害增加
	float f_MCRID;	                 // 魔法爆擊傷害增加
	int n_CC_Hit;                    // 控制命中
	int n_CC_Res;                    // 控制抵抗
	float f_Move;	                 // 移動	
	float f_Run;

	int n_DeadCoins;				//死亡消耗金币
	int n_DeadExp;					//死亡经验
	int	n_DeadDurability;			//死亡耐久
	int	n_Jump;						//跳高度
	float f_Swim;						//游泳
	float f_ShortJumpTime;			// 小跳匀速上升时间
	float f_LongJumpTime;			// 大跳匀速上升时间
	float f_InitialVerticalVelocity; // 起跳初速度
	float f_AccelerationUp;
	int n_Dig;						//挖掘
	int n_Log;						//标记
	int n_Fish;						//钓鱼
	float n_Catch;	                 // 拾取范围
	float f_HpUICheck{};		//距离血条高度
	int n_fog_view = 4;
	int n_fog_empty_view = 3;
	int n_HPRS = 5;
	int n_MPRS = 5;
	int n_ResourceAtkDmg{};		// 对物资攻击力.


	typedef std::unordered_map<int, PlayerAttr> Level2PlayerAttr;
	typedef std::vector<AttachInfo> AttachInfoMap;
	typedef std::map<int, std::shared_ptr<NodeInfo>> NodeInfoMap;

public:
	static PlayerAttrCfg& getInstance();

	bool initialize();
	void terminate();

	const PlayerAttr* findPlayerAttr(int id) const;

	const AttrMap& findPlayerAttrInfo(int nLev);

	void getPlayerAttrInfo(int nLev, AttrMap &attrInfo);
	const Level2PlayerAttr& getPlayerAttr() const { return m_playerAttrs; };
	inline const int getMaxSkinId() const { return m_nMaxSkinId; };


	const AttachInfoMap& getAttachInfoList() const { return m_attachInfoMap; };
	const bzbee::tstring& getAttachId(int nPos);
	const bzbee::tstring& getSubAttachId(int nPos);
	int getAttachPos(const bzbee::tstring& attachId);
	const bzbee::tstring& getNodeId(int nPos, const int gender = 0);
	int getNodePos(const bzbee::tstring& nodeId);
	const AttachInfo* getAttach(int nPos);
	const AttachInfo* getAttachById(const bzbee::tstring& attachId);
	bzbee::StringList getAttachIds(const bzbee::IntegerList& ids, bool unique = false);
	bzbee::StringList getNodeIds(const bzbee::IntegerList& ids, const int gender = 0, bool unique = false);

	bool reloadMultiLanguage();

private:
	bool parsePlayerAttrConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseAttachAttCfg(tinyxml2::XMLElement* pXmlRoot);

public:
	NS::int32 m_maxLevel{ INT_MIN };			//最大等级上限

private:
	Level2PlayerAttr m_playerAttrs;

	AttrMap m_mapAttrInfo;
	AttachInfoMap m_attachInfoMap;
	NodeInfoMap m_nodeInfoMap;
	int m_nMaxSkinId{};
};
