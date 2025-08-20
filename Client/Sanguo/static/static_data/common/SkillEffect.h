#pragma once

#include "SkillType.h"
#include "SkillTemplate.h"
#include "EffectCalculation.h"
#include "MobTemplate.h"
#include "GameDefine.h"
#include "SkillRoleValue.h"

namespace skill {
    // 執行技能作用效果時使用通用結構
    template<
        typename MapObject
        , typename MapCharacter /*必须派生自MapObject，参考GameDefine.h的IS_MAP_CHARACTER*/
        , typename AttrVector /*必须派生自CharacterAttrVectorBase，且实现[]操作符，参考TCharacterAttrVector*/
    >
        struct TSkillEffectParam
    {
        using PtrMapObject = std::shared_ptr<MapObject>;
        using SkillRoleValue_t = TSkillRoleValue<MapObject, MapCharacter, AttrVector>;

        PtrMapObject			pCaster;	    // 施法者
        SkillRoleValue_t		CasterRoleValue;// 施法者角色屬性
        PtrMapObject			pTarget;		// 目標
        SkillRoleValue_t		TargetRoleValue;// 目標角色屬性
        S_SkillTemplate_effect	theEffect;	// 作用效果	
        int32			EffectRate = 1000;	// 作用效果(千分比)
        NS::I64			SerialID;			// Client施法時使用的Serial
        //Vector2			TargetPos;			// 目标坐标
        //Vector2			CasterPos;			// 施法坐标
        int32			nEffectCount = 1;	// 效果次数 总时间/间隔时间
        int32			nDmg = 0;			// 伤害
        bool			bCritical = false;	// 是否暴击

        TSkillEffectParam()
        {
        };

        TSkillEffectParam(PtrMapObject Caster, PtrMapObject	Target)
            : pCaster(Caster)
            , pTarget(Target)
        {
        };

        virtual ~TSkillEffectParam()
        {
        };
    };

    template<
        typename SkillRoleValue_t /*TSkillRoleValue的实例类*/
        , typename SkillEffectParam_t /*TSkillEffectParam的实例类*/
        , typename Mob /*地图怪物类型，继承自MapObject*/
    >
        class TSkillEffect
    {
    public:
        TSkillEffect()
        {
        };

        virtual ~TSkillEffect()
        {
        };

        // 获取单个技能效果值
        static int GetOneSkillEffectValue(
            const SkillRoleValue_t& CasterRoleValue,
            const SkillRoleValue_t& TargetRoleValue,
            const AttrEffectValue& EffectValue,
            int32 EffectRate)
        {
            if (EffectValue.attrref == ATTR_REF_STATIC)
            {
                if (EffectRate != 0)
                    return EffectValue.value * EffectRate / 1000;
                return EffectValue.value;
            }

            // 要取得目標
            const SkillRoleValue_t* pRoleValue = nullptr;
            switch (EffectValue.attrref)
            {
            case ATTR_REF_SELF_HP://	= 1,//自身目前血量					
            case ATTR_REF_SELF_MP://	= 2,//自身目前魔力
            case ATTR_REF_CASTER_HP://	= 5,//施法者目前血量
            case ATTR_REF_CASTER_MP://	= 6,//施法者目前魔力 
                pRoleValue = &CasterRoleValue;
                break;
            case ATTR_REF_TARGET_HP://	= 3,//目標目前血量
            case ATTR_REF_TARGET_MP://	= 4,//目標目前魔力
                pRoleValue = &TargetRoleValue;
                break;
            default:
                if ((EffectValue.attrref >= ATTR_REF_SELF_HPM_BASE && EffectValue.attrref < ATTR_REF_SELF_NOW_MAX) ||
                    (EffectValue.attrref >= ATTR_REF_CASTER_HP_BASE && EffectValue.attrref < ATTR_REF_CASTER_NOW_MAX))
                    pRoleValue = &CasterRoleValue;
                else if (EffectValue.attrref >= ATTR_REF_TARGET_HPM_BASE && EffectValue.attrref < ATTR_REF_TARGET_NOW_MAX)
                    pRoleValue = &TargetRoleValue;
                break;
            }
            if (pRoleValue == nullptr)
                return 0;

            int32 theValue = 0;
            switch (EffectValue.attrref)
            {
            case ATTR_REF_SELF_HP:// = 1,//自身目前血量千分比
            case ATTR_REF_TARGET_HP:// = 3,//目標目前血量千分比
            case ATTR_REF_CASTER_HP:// = 5,//施法者目前血量千分比
                theValue = pRoleValue->HP; 	break;
            case ATTR_REF_SELF_MP:// = 2,//自身目前魔力千分比	
            case ATTR_REF_TARGET_MP:// = 4,//目標目前魔力千分比	
            case ATTR_REF_CASTER_MP:// = 6,//施法者目前魔力千分比	
                theValue = pRoleValue->MP; break;
            case ATTR_REF_SELF_HPM_BASE:// =101,//自身基礎血量千分比
            case ATTR_REF_TARGET_HPM_BASE:// =301,//自身基礎血量千分比
            case ATTR_REF_CASTER_HP_BASE:// =501,//自身基礎血量千分比
                theValue = pRoleValue->BaseHP; break;
            case ATTR_REF_SELF_MPM_BASE:// =102,//自身基礎魔力千分比
            case ATTR_REF_TARGET_MPM_BASE:// =302,//自身基礎魔力千分比
            case ATTR_REF_CASTER_MP_BASE:// =502,//自身基礎魔力千分比
                theValue = pRoleValue->BaseMP; break;
            case ATTR_REF_SELF_AD_BASE:// =103,//自身基礎物理攻擊千分比
            case ATTR_REF_TARGET_AD_BASE:// =303,//自身基礎物理攻擊千分比
            case ATTR_REF_CASTER_AD_BASE:// =503,//自身基礎物理攻擊千分比
                theValue = pRoleValue->BaseAD; break;
            case ATTR_REF_SELF_AP_BASE:// =104,//自身基礎魔法攻擊千分比
            case ATTR_REF_TARGET_AP_BASE:// =304,//自身基礎魔法攻擊千分比
            case ATTR_REF_CASTER_AP_BASE:// =504,//自身基礎魔法攻擊千分比	
                theValue = pRoleValue->BaseAP; break;
            case ATTR_REF_SELF_ARM_BASE:// =105,//自身基礎物理防禦千分比
            case ATTR_REF_TARGET_ARM_BASE:// =305,//自身基礎物理防禦千分比
            case ATTR_REF_CASTER_ARM_BASE:// =505,//自身基礎物理防禦千分比
                theValue = pRoleValue->BaseARM; break;
            case ATTR_REF_SELF_MARM_BASE:// =106,//自身基礎魔法防禦千分比
            case ATTR_REF_TARGET_MARM_BASE:// =306,//自身基礎魔法防禦千分比
            case ATTR_REF_CASTER_MARM_BASE:// =506,//自身基礎魔法防禦千分比
                theValue = pRoleValue->BaseMARM; break;
            case ATTR_REF_SELF_AR_BASE:// =107,//自身基礎物理減免千分比
            case ATTR_REF_TARGET_AR_BASE:// =307,//自身基礎物理減免千分比
            case ATTR_REF_CASTER_AR_BASE:// =507,//自身基礎物理減免千分比
                theValue = pRoleValue->BaseAR; break;
            case ATTR_REF_SELF_MR_FIRE_BASE:// =108,//自身基礎魔法減免火千分比
            case ATTR_REF_TARGET_MR_FIRE_BASE:// =308,//自身基礎魔法減免火千分比
            case ATTR_REF_CASTER_MR_FIRE_BASE:// =508,//自身基礎魔法減免火千分比
                theValue = pRoleValue->BaseMR_FIRE; break;
            case ATTR_REF_SELF_MR_FORST_BASE:// =109,//自身基礎魔法減免冰千分比
            case ATTR_REF_TARGET_MR_FORST_BASE:// =309,//自身基礎魔法減免冰千分比
            case ATTR_REF_CASTER_MR_FORST_BASE:// =509,//自身基礎魔法減免冰千分比
                theValue = pRoleValue->BaseMR_FORST; break;
            case ATTR_REF_SELF_MR_THUNDER_BASE:// =110,//自身基礎魔法減免雷千分比
            case ATTR_REF_TARGET_MR_THUNDER_BASE:// =310,//自身基礎魔法減免雷千分比
            case ATTR_REF_CASTER_MR_THUNDER_BASE:// =510,//自身基礎魔法減免雷千分比
                theValue = pRoleValue->BaseMR_THUNDER; break;
            case ATTR_REF_SELF_MR_EARTH_BASE:// =111,//自身基礎魔法減免土千分比
            case ATTR_REF_TARGET_MR_EARTH_BASE:// =311,//目標基礎魔法減免土千分比
            case ATTR_REF_CASTER_MR_EARTH_BASE:// =511,//施法者基礎魔法減免土千分比
                theValue = pRoleValue->BaseMR_EARTH; break;
            case ATTR_REF_SELF_MR_POSION_BASE:// =111,//自身基礎魔法減免毒千分比
            case ATTR_REF_TARGET_MR_POSION_BASE:// =311,//自身基礎魔法減免毒千分比
            case ATTR_REF_CASTER_MR_POSION_BASE:// =511,//自身基礎魔法減免毒千分比
                theValue = pRoleValue->BaseMR_POSION; break;
            case ATTR_REF_SELF_MR_LIGHT_BASE:// =112,//自身基礎魔法減免光千分比
            case ATTR_REF_TARGET_MR_LIGHT_BASE:// =312,//自身基礎魔法減免光千分比
            case ATTR_REF_CASTER_MR_LIGHT_BASE:// =512,//自身基礎魔法減免光千分比
                theValue = pRoleValue->BaseMR_LIGHT; break;
            case ATTR_REF_SELF_MR_DARK_BASE:// =113,//自身基礎魔法減免暗千分比
            case ATTR_REF_TARGET_MR_DARK_BASE:// =313,//自身基礎魔法減免暗千分比
            case ATTR_REF_CASTER_MR_DARK_BASE:// =513,//自身基礎魔法減免暗千分比
                theValue = pRoleValue->BaseMR_DARK; break;
            case ATTR_REF_SELF_ARP_BASE:// =114,//自身基礎物裡穿透千分比
            case ATTR_REF_TARGET_ARP_BASE:// =314,//自身基礎物裡穿透千分比
            case ATTR_REF_CASTER_ARP_BASE:// =514,//自身基礎物裡穿透千分比
                theValue = pRoleValue->BaseARP; break;
            case ATTR_REF_SELF_MRP_BASE:// =115,//自身基礎魔法穿透千分比
            case ATTR_REF_TARGET_MRP_BASE:// =315,//自身基礎魔法穿透千分比
            case ATTR_REF_CASTER_MRP_BASE:// =515,//自身基礎魔法穿透千分比
                theValue = pRoleValue->BaseMRP; break;
            case ATTR_REF_SELF_CRI_BASE:// =116,//自身基礎物理爆擊率千分比
            case ATTR_REF_TARGET_CRI_BASE:// =316,//自身基礎物理爆擊率千分比
            case ATTR_REF_CASTER_CRI_BASE:// =516,//自身基礎物理爆擊率千分比
                theValue = pRoleValue->BaseCRI; break;
            case ATTR_REF_SELF_MCRI_BASE:// =117,//自身基礎魔法爆擊率千分比
            case ATTR_REF_TARGET_MCRI_BASE:// =317,//自身基礎魔法爆擊率千分比
            case ATTR_REF_CASTER_MCRI_BASE:// =517,//自身基礎魔法爆擊率千分比
                theValue = pRoleValue->BaseMCRI; break;
            case ATTR_REF_SELF_CRID_BASE:// =118,//自身基礎物理爆擊增傷千分比
            case ATTR_REF_TARGET_CRID_BASE:// =318,//自身基礎物理爆擊增傷千分比
            case ATTR_REF_CASTER_CRID_BASE:// =518,//自身基礎物理爆擊增傷千分比
                theValue = pRoleValue->BaseCRID; break;
            case ATTR_REF_SELF_MCRID_BASE:// =119,//自身基礎魔法爆擊增傷千分比
            case ATTR_REF_TARGET_MCRID_BASE:// =319,//自身基礎魔法爆擊增傷千分比
            case ATTR_REF_CASTER_MCRID_BASE:// =519,//自身基礎魔法爆擊增傷千分比
                theValue = pRoleValue->BaseMCRID; break;
            case ATTR_REF_SELF_RES_BASE:// =120,//自身基礎物裡韌性千分比
            case ATTR_REF_TARGET_RES_BASE:// =320,//自身基礎物裡韌性千分比
            case ATTR_REF_CASTER_RES_BASE:// =520,//自身基礎物裡韌性千分比
                theValue = pRoleValue->BaseRES; break;
            case ATTR_REF_SELF_MRES_BASE:// =121,//自身基礎魔法韌性千分比
            case ATTR_REF_TARGET_MRES_BASE:// =321,//自身基礎魔法韌性千分比
            case ATTR_REF_CASTER_MRES_BASE:// =521,//自身基礎魔法韌性千分比
                theValue = pRoleValue->BaseMRES; break;
            case ATTR_REF_SELF_CC_HIT_BASE:// =122,//自身基礎控制命中千分比
            case ATTR_REF_TARGET_CC_HIT_BASE:// =322,//自身基礎控制命中千分比
            case ATTR_REF_CASTER_CC_HIT_BASE:// =522,//自身基礎控制命中千分比
                theValue = pRoleValue->BaseCC_HIT; break;
            case ATTR_REF_SELF_CC_RES_BASE:// =123,//自身基礎控制抵抗千分比
            case ATTR_REF_TARGET_CC_RES_BASE:// =323,//自身基礎控制抵抗千分比
            case ATTR_REF_CASTER_CC_RES_BASE:// =523,//自身基礎控制抵抗千分比
                theValue = pRoleValue->BaseCC_RES; break;
            case ATTR_REF_SELF_ATK_SP_BASE:// =124,//自身基礎攻速千分比
            case ATTR_REF_TARGET_ATK_SP_BASE:// =324,//自身基礎攻速千分比	
            case ATTR_REF_CASTER_ATK_SP_BASE:// =524,//自身基礎攻速千分比
                theValue = pRoleValue->BaseATK_SP; break;
            case ATTR_REF_SELF_MV_SP_BASE:// =125,//自身基礎移動速度	千分比
            case ATTR_REF_TARGET_MV_SP_BASE:// =325,//自身基礎移動速度	千分比
            case ATTR_REF_CASTER_MV_SP_BASE:// =525,//自身基礎移動速度千分比	
                theValue = pRoleValue->BaseMV; break;
            case ATTR_REF_SELF_HB_BASE:// =126,//自身基礎擊退千分比																										
            case ATTR_REF_TARGET_HB_BASE:// =326,//自身基礎擊退千分比			
            case ATTR_REF_CASTER_HB_BASE:// =526,//自身基礎擊退千分比		
                theValue = pRoleValue->BaseHB; break;
            case ATTR_REF_SELF_HPR_BASE:// =127,//自身基礎擊生命回復
            case ATTR_REF_TARGET_HPR_BASE:// =327,//自身基礎擊生命回復
            case ATTR_REF_CASTER_HPR_BASE:// =527,//自身基礎擊生命回復
                theValue = pRoleValue->BaseHPR; break;
            case ATTR_REF_SELF_MPR_BASE:// =129,//自身基礎擊魔法回復
            case ATTR_REF_TARGET_MPR_BASE:// =329,//自身基礎擊魔法回復	
            case ATTR_REF_CASTER_MPR_BASE:// =529,//自身基礎擊魔法回復
                theValue = pRoleValue->BaseMPR; break;
            case ATTR_REF_SELF_HPRS_BASE:// =128,//自身基礎擊生命回復速度
            case ATTR_REF_TARGET_HPRS_BASE:// =328,//自身基礎擊生命回復速度
            case ATTR_REF_CASTER_HPRS_BASE:// =528,//自身基礎擊生命回復速度
                theValue = pRoleValue->BaseHPRS; break;
            case ATTR_REF_SELF_MPRS_BASE:// =130,//自身基礎擊魔法回復速度			
            case ATTR_REF_TARGET_MPRS_BASE:// =330,//自身基礎擊魔法回復速度			
            case ATTR_REF_CASTER_MPRS_BASE:// =530,//自身基礎擊魔法回復速度
                theValue = pRoleValue->BaseMPRS; break;
            case ATTR_REF_SELF_GATHER_ATK_BASE://=131,//自身基礎擊採集攻擊力
            case ATTR_REF_TARGET_GATHER_ATK_BASE://=331,//目標基礎擊採集攻擊力
            case ATTR_REF_CASTER_GATHER_ATK_BASE://=531,//施法者基礎採集攻擊力
                theValue = pRoleValue->BaseDigAtk; break;
            case ATTR_REF_SELF_GATHER_SP_BASE://=132,//自身基礎擊採集速度
            case ATTR_REF_TARGET_GATHER_SP_BASE://=332,//目標基礎擊採集速度
            case ATTR_REF_CASTER_GATHER_SP_BASE://=532,//施法者基礎採集速度
                theValue = pRoleValue->BaseDigAtkSP; break;
            case ATTR_REF_SELF_JUMP_BASE://=133,//自身基礎跳跃力
            case ATTR_REF_TARGET_JUMP_BASE://=333,//目標基礎跳跃力
            case ATTR_REF_CASTER_JUMP_BASE://=533,//施法者基礎跳跃力
                theValue = pRoleValue->BaseJump; break;
            case ATTR_REF_SELF_SWIM_BASE://=134,//自身基礎游泳速度
            case ATTR_REF_TARGET_SWIM_BASE://=334,//目標基礎游泳速度
            case ATTR_REF_CASTER_SWIM_BASE://=534,//施法者基礎游泳速度
                theValue = pRoleValue->BaseSwin; break;
            case ATTR_REF_SELF_BOSS_HURT_BASE:	//=138
            case ATTR_REF_TARGET_BOSS_HURT_BASE://=338
            case ATTR_REF_CASTER_BOSS_HURT_BASE://=538
                theValue = pRoleValue->BaseBossHurt; break;
            case ATTR_REF_SELF_BOSS_DMGREDUC_BASE:	//=139
            case ATTR_REF_TARGET_BOSS_DMGREDUC_BASE://=339
            case ATTR_REF_CASTER_BOSS_DMGREDUC_BASE://=539
                theValue = pRoleValue->BaseBossDmgRed; break;
            case ATTR_REF_SELF_JUMP_INIT_SPEED_BASE:	//=140
            case ATTR_REF_TARGET_JUMP_INIT_SPEED_BASE://=340
            case ATTR_REF_CASTER_JUMP_INIT_SPEED_BASE://=540
                theValue = pRoleValue->BaseJumpInitSpeed; break;
            case ATTR_REF_SELF_HPM_NOW:// =201,//自身(最大)血量千分比
            case ATTR_REF_TARGET_HPM_NOW:// =401,//自身(最大)血量千分比
            case ATTR_REF_CASTER_HP_NOW:// =601,//自身(最大)血量千分比
                theValue = pRoleValue->NowHP; break;
            case ATTR_REF_SELF_MPM_NOW:// =202,//自身(最大)魔力千分比
            case ATTR_REF_TARGET_MPM_NOW:// =402,//自身(最大)魔力千分比
            case ATTR_REF_CASTER_MP_NOW:// =602,//自身(最大)魔力千分比
                theValue = pRoleValue->NowMP; break;
            case ATTR_REF_SELF_AD_NOW:// =203,//自身當前物理攻擊	千分比
            case ATTR_REF_TARGET_AD_NOW:// =403,//自身當前物理攻擊	千分比
            case ATTR_REF_CASTER_AD_NOW:// =603,//自身當前物理攻擊	千分比
                theValue = pRoleValue->NowAD; break;
            case ATTR_REF_SELF_AP_NOW:// =204,//自身當前魔法攻擊	千分比
            case ATTR_REF_TARGET_AP_NOW:// =404,//自身當前魔法攻擊	千分比
            case ATTR_REF_CASTER_AP_NOW:// =604,//自身當前魔法攻擊	千分比
                theValue = pRoleValue->NowAP; break;
            case ATTR_REF_SELF_ARM_NOW:// =205,//自身當前物理防禦	千分比
            case ATTR_REF_TARGET_ARM_NOW:// =405,//自身當前物理防禦	千分比
            case ATTR_REF_CASTER_ARM_NOW:// =605,//自身當前物理防禦	千分比
                theValue = pRoleValue->NowARM; break;
            case ATTR_REF_SELF_MARM_NOW:// =206,//自身當前魔法防禦	千分比
            case ATTR_REF_TARGET_MARM_NOW:// =406,//自身當前魔法防禦	千分比
            case ATTR_REF_CASTER_MARM_NOW:// =606,//自身當前魔法防禦	千分比
                theValue = pRoleValue->NowMARM; break;
            case ATTR_REF_SELF_AR_NOW:// =207,//自身當前物理減免	千分比
            case ATTR_REF_TARGET_AR_NOW:// =407,//自身當前物理減免	千分比
            case ATTR_REF_CASTER_AR_NOW:// =607,//自身當前物理減免	千分比
                theValue = pRoleValue->NowAR; break;
            case ATTR_REF_SELF_MR_FIRE_NOW:// =208,//自身當前魔法減免火千分比
            case ATTR_REF_TARGET_MR_FIRE_NOW:// =408,//自身當前魔法減免火千分比	
            case ATTR_REF_CASTER_MR_FIRE_NOW:// =608,//自身當前魔法減免火千分比
                theValue = pRoleValue->NowMR_FIRE; break;
            case ATTR_REF_SELF_MR_FORST_NOW:// =209,//自身當前魔法減免冰千分比
            case ATTR_REF_TARGET_MR_FORST_NOW:// =409,//自身當前魔法減免冰千分比	
            case ATTR_REF_CASTER_MR_FORST_NOW:// =609,//自身當前魔法減免冰千分比
                theValue = pRoleValue->NowMR_FORST; break;
            case ATTR_REF_SELF_MR_THUNDER_NOW:// =210,//自身當前魔法減免雷千分比
            case ATTR_REF_TARGET_MR_THUNDER_NOW:// =410,//自身當前魔法減免雷千分比	
            case ATTR_REF_CASTER_MR_THUNDER_NOW:// =610,//自身當前魔法減免雷千分比	
                theValue = pRoleValue->NowMR_THUNDER; break;
            case ATTR_REF_SELF_MR_EARTH_NOW:// =211,//自身當前魔法減免土千分比
            case ATTR_REF_TARGET_MR_EARTH_NOW:// =411,//目標魔法減免土千分比
            case ATTR_REF_CASTER_MR_EARTH_NOW:// =611,//施法者當前魔法減免土千分比	
                theValue = pRoleValue->NowMR_EARTH; break;
            case ATTR_REF_SELF_MR_POSION_NOW:// =211,//自身當前魔法減免毒千分比	
            case ATTR_REF_TARGET_MR_POSION_NOW:// =411,//自身當前魔法減免毒千分比	
            case ATTR_REF_CASTER_MR_POSION_NOW:// =611,//自身當前魔法減免毒千分比	
                theValue = pRoleValue->NowMR_POSION; break;
            case ATTR_REF_SELF_MR_LIGHT_NOW:// =212,//自身當前魔法減免光千分比	
            case ATTR_REF_TARGET_MR_LIGHT_NOW:// =412,//自身當前魔法減免光千分比	
            case ATTR_REF_CASTER_MR_LIGHT:// =612,//自身當前魔法減免光千分比	
                theValue = pRoleValue->NowMR_LIGHT; break;
            case ATTR_REF_SELF_MR_DARK_NOW:// =213,//自身當前魔法減免暗千分比	
            case ATTR_REF_TARGET_MR_DARK_NOW:// =413,//自身當前魔法減免暗千分比
            case ATTR_REF_CASTER_MR_DARK_NOW:// =613,//自身當前魔法減免暗千分比
                theValue = pRoleValue->NowMR_DARK; break;
            case ATTR_REF_SELF_ARP_NOW:// =214,//自身當前物裡穿透	千分比
            case ATTR_REF_TARGET_ARP_NOW:// =414,//自身當前物裡穿透	千分比	
            case ATTR_REF_CASTER_ARP_NOW:// =614,//自身當前物裡穿透千分比	
                theValue = pRoleValue->NowARP; break;
            case ATTR_REF_SELF_MRP_NOW:// =215,//自身當前魔法穿透	千分比
            case ATTR_REF_TARGET_MRP_NOW:// =415,//自身當前魔法穿透	千分比
            case ATTR_REF_CASTER_MRP_NOW:// =615,//自身當前魔法穿透千分比
                theValue = pRoleValue->NowMRP; break;
            case ATTR_REF_SELF_CRI_NOW:// =216,//自身當前物理爆擊率千分比	
            case ATTR_REF_TARGET_CRI_NOW:// =416,//自身當前物理爆擊率千分比		
            case ATTR_REF_CASTER_CRI_NOW:// =616,//自身當前物理爆擊率千分比	
                theValue = pRoleValue->NowCRI; break;
            case ATTR_REF_SELF_MCRI_NOW:// =217,//自身當前魔法爆擊率千分比	
            case ATTR_REF_TARGET_MCRI_NOW:// =417,//自身當前魔法爆擊率千分比	
            case ATTR_REF_CASTER_MCRI_NOW:// =617,//自身當前魔法爆擊率千分比	
                theValue = pRoleValue->NowMCRI; break;
            case ATTR_REF_SELF_CRID_NOW:// =218,//自身當前物理爆擊增傷千分比	
            case ATTR_REF_TARGET_CRID_NOW:// =418,//自身當前物理爆擊增傷千分比	
            case ATTR_REF_CASTER_CRID_NOW:// =618,//自身當前物理爆擊增傷千分比	
                theValue = pRoleValue->NowCRID; break;
            case ATTR_REF_SELF_MCRID_NOW:// =219,//自身當前魔法爆擊增傷千分比	
            case ATTR_REF_TARGET_MCRID_NOW:// =419,//自身當前魔法爆擊增傷千分比	
            case ATTR_REF_CASTER_MCRID_NOW:// =619,//自身當前魔法爆擊增傷千分比	
                theValue = pRoleValue->NowMCRID; break;
            case ATTR_REF_SELF_RES_NOW:// =220,//自身當前物裡韌性千分比
            case ATTR_REF_TARGET_RES_NOW:// =420,//自身當前物裡韌性千分比	
            case ATTR_REF_CASTER_RES_NOW:// =620,//自身當前物裡韌性千分比	
                theValue = pRoleValue->NowRES; break;
            case ATTR_REF_SELF_MRES_NOW:// =221,//自身當前魔法韌性千分比	
            case ATTR_REF_TARGET_MRES_NOW:// =421,//自身當前魔法韌性千分比	
            case ATTR_REF_CASTER_MRES_NOW:// =621,//自身當前魔法韌性千分比	
                theValue = pRoleValue->NowMRES; break;
            case ATTR_REF_SELF_CC_HIT_NOW:// =222,//自身當前控制命中	千分比
            case ATTR_REF_TARGET_CC_HIT_NOW:// =422,//自身當前控制命中千分比	
            case ATTR_REF_CASTER_CC_HIT_NOW:// =622,//自身當前控制命中	千分比
                theValue = pRoleValue->NowCC_HIT; break;
            case ATTR_REF_SELF_CC_RES_NOW:// =223,//自身當前控制抵抗千分比	
            case ATTR_REF_TARGET_CC_RES_NOW:// =423,//自身當前控制抵抗	千分比		
            case ATTR_REF_CASTER_CC_RES_NOW:// =623,//自身當前控制抵抗	千分比		
                theValue = pRoleValue->NowCC_RES; break;
            case ATTR_REF_SELF_ATK_SP_NOW:// =224,//自身當前攻速千分比	
            case ATTR_REF_TARGET_ATK_SP_NOW:// =424,//自身當前攻速千分比	
            case ATTR_REF_CASTER_ATK_SP_NOW:// =624,//自身當前攻速千分比	
                theValue = pRoleValue->NowATK_SP; break;
            case ATTR_REF_SELF_MV_SP_NOW:// =225,//自身當前移動速度	千分比
            case ATTR_REF_TARGET_MV_SP_NOW:// =425,//自身當前移動速度千分比	
            case ATTR_REF_CASTER_MV_SP_NOW:// =625,//自身當前移動速度千分比	
                theValue = pRoleValue->NowMV; break;
            case ATTR_REF_SELF_HB_NOW:// =226,//自身當前擊退千分比											 									 									 												 																										
            case ATTR_REF_TARGET_HB_NOW:// =426,//自身當前擊退千分比												
            case ATTR_REF_CASTER_HB_NOW:// =626,//自身當前擊退千分比
                theValue = pRoleValue->NowHB; break;
            case ATTR_REF_SELF_HPR_NOW:// =227,//自身基礎擊生命回復
            case ATTR_REF_TARGET_HPR_NOW:// =427,//自身基礎擊生命回復
            case ATTR_REF_CASTER_HPR_NOW:// =627,//自身基礎擊生命回復
                theValue = pRoleValue->NowHPR; break;
            case ATTR_REF_SELF_HPRS_NOW:// =228,//自身基礎擊生命回復速度
            case ATTR_REF_TARGET_HPRS_NOW:// =428,//自身基礎擊生命回復速度
            case ATTR_REF_CASTER_HPRS_NOW:// =628,//自身基礎擊生命回復速度
                theValue = pRoleValue->NowHPRS; break;
            case ATTR_REF_SELF_MPR_NOW:// =229,//自身基礎擊魔法回復
            case ATTR_REF_TARGET_MPR_NOW:// =429,//自身基礎擊魔法回復
            case ATTR_REF_CASTER_MPR_NOW:// =629,//自身基礎擊魔法回復
                theValue = pRoleValue->NowMPR; break;
            case ATTR_REF_SELF_MPRS_NOW:// =230,//自身基礎擊魔法回復速度			
            case ATTR_REF_TARGET_MPRS_NOW:// =430,//自身基礎擊魔法回復速度			
            case ATTR_REF_CASTER_MPRS_NOW:// =630,//自身基礎擊魔法回復速度
                theValue = pRoleValue->NowMPRS; break;
            case ATTR_REF_SELF_GATHER_ATK_NOW://=231,//自身當前擊採集攻擊力
            case ATTR_REF_TARGET_GATHER_ATK_NOW://=431,//目標當前採集攻擊力
            case ATTR_REF_CASTER_GATHER_ATK_NOW://=631,//施法者當前擊採集攻擊力
                theValue = pRoleValue->NowGatherAtk; break;
            case ATTR_REF_SELF_GATHER_SP_NOW://=232,//自身當前擊採集速度
            case ATTR_REF_TARGET_GATHER_SP_NOW://=432,//目標當前採集速度
            case ATTR_REF_CASTER_GATHER_SP_NOW://=632,//施法者當前擊採集速度 							 
                theValue = pRoleValue->NowGatherSpeed; break;
            case ATTR_REF_SELF_JUMP_NOW://=233,//自身當前跳跃力
            case ATTR_REF_TARGET_JUMP_NOW://=433,//目標當前跳跃力
            case ATTR_REF_CASTER_JUMP_NOW://=633,//施法者當前跳跃力
                theValue = pRoleValue->NowJump; break;
            case ATTR_REF_SELF_SWIM_NOW://=234,//自身當前游泳速度
            case ATTR_REF_TARGET_SWIM_NOW://=434,//目標當前游泳速度
            case ATTR_REF_CASTER_SWIM_NOW://=634,//施法者當前游泳速度
                theValue = pRoleValue->NowSwin; break;
            case ATTR_REF_SELF_BOSS_HURT_NOW:	//=238,//
            case ATTR_REF_TARGET_BOSS_HURT_NOW:	//=438,//
            case ATTR_REF_CASTER_BOSS_HURT_NOW:	//=638,//
                theValue = pRoleValue->NowBossHurt; break;
            case ATTR_REF_SELF_BOSS_DMGREDUC_NOW:	//=239,//
            case ATTR_REF_TARGET_BOSS_DMGREDUC_NOW:	//=439,//
            case ATTR_REF_CASTER_BOSS_DMGREDUC_NOW:	//=639,//
                theValue = pRoleValue->NowBossDmgRed; break;
            case ATTR_REF_SELF_JUMP_INIT_SPEED_NOW:	//=240,//
            case ATTR_REF_TARGET_JUMP_INIT_SPEED_NOW:	//=440,//
            case ATTR_REF_CASTER_JUMP_INIT_SPEED_NOW:	//=640,//
                theValue = pRoleValue->NowJumpInitSpeed; break;
            default:
                break;
            }

            // 千分位
            int32 ResultValue = theValue * EffectValue.value / 1000;
            // 再乘上效果
            if (EffectRate != 0)
                ResultValue = ResultValue * EffectRate / 1000;
            return ResultValue;
        };

        // 获取技能效果值
        static int GetSkillEffectValue(
            const SkillRoleValue_t& CasterRoleValue,
            const SkillRoleValue_t& TargetRoleValue,
            const std::vector<AttrEffectValue>& EffectValues,
            int32 EffectRate,
            std::vector<int32>& EffectResults)
        {
            if (EffectValues.empty())
            {
                return 0;
            }

            int SumValue = 0;
            for (auto& EffectValue : EffectValues)
            {
                int32 val = GetOneSkillEffectValue(CasterRoleValue, TargetRoleValue, EffectValue, EffectRate);
                SumValue += val;
                EffectResults.push_back(val);
            }

            return SumValue;
        };

        // 目标护盾抵消伤害，返回抵消后的伤害
        virtual int TargetShieldOffsetDamage(SkillEffectParam_t& param, int dmg)
        {
            return dmg;
        };
    };

    // 真實傷害
    template<
        typename SkillRoleValue_t /*TSkillRoleValue的实例类*/
        , typename SkillEffectParam_t /*TSkillEffectParam的实例类*/
        , typename Mob /*地图怪物类型，继承自MapObject*/
        , typename MapObject
        , typename MapCharacter /*必须派生自MapObject，参考GameDefine.h的IS_MAP_CHARACTER*/
        , typename AttrVector /*必须派生自CharacterAttrVectorBase，且实现[]操作符，参考TCharacterAttrVector*/
    >
        class ISkillEffect_RealDmg
        : public TSkillEffect<SkillRoleValue_t, SkillEffectParam_t, Mob>
    {
    public:
        ISkillEffect_RealDmg()
        {
        };

        virtual ~ISkillEffect_RealDmg()
        {
        };

        int CalEffect(TSkillEffectParam<MapObject, MapCharacter, AttrVector>& Param)
        {
            if (Param.pCaster == nullptr || Param.pTarget == nullptr)
                return 0;

            std::vector<int32> vec;
            int DmgValue = this->TSkillEffect<SkillRoleValue_t, SkillEffectParam_t, Mob>::GetSkillEffectValue(
                Param.CasterRoleValue,
                Param.TargetRoleValue,
                Param.theEffect.values,
                Param.EffectRate,
                vec);
            return DmgValue;
        };
    };

    // 公式形傷害
    template< typename SkillRoleValue_t >
    struct ISkillEffect_FormulaDmg
    {
        // 取得目標的免復值
        virtual int GetTargetElemetRemitValue(const SkillRoleValue_t& TargetRoleValue) = 0;
    };

    // 物理伤害
    template<
        typename SkillRoleValue_t /*TSkillRoleValue的实例类*/
        , typename SkillEffectParam_t /*TSkillEffectParam的实例类*/
        , typename Mob /*地图怪物类型，继承自MapObject*/
    >
        struct TSkillEffect_WeaponDmg
        : public TSkillEffect<SkillRoleValue_t, SkillEffectParam_t, Mob>
        , ISkillEffect_FormulaDmg<SkillRoleValue_t>
    {
        // 获取伤害公式参数
        virtual void GetFormulaParam(SkillFormulaParam& FormulaParam, const SkillEffectParam_t& Param)
        {
            FormulaParam.CasterCrit = Param.CasterRoleValue.NowCRI;			 //物理暴击率
            FormulaParam.TargetTenacity = Param.TargetRoleValue.NowRES;		 //物理韧性率
            FormulaParam.CasterAtk = Param.CasterRoleValue.NowAD;			 //物理攻击力
            FormulaParam.TargetDef = Param.TargetRoleValue.NowARM;			 //物理防御力
            FormulaParam.CasterPenetrate = Param.CasterRoleValue.NowARP;  //物理穿透率
            FormulaParam.TargetDmgAvoid = this->GetTargetElemetRemitValue(Param.TargetRoleValue); //免伤率
            FormulaParam.CasterCritAdd = Param.CasterRoleValue.NowCRID;	//物理暴击增伤率
            FormulaParam.vecEffect = Param.theEffect.values;
            FormulaParam.EffectValue = TSkillEffect<SkillRoleValue_t, SkillEffectParam_t, Mob>::GetSkillEffectValue(
                Param.CasterRoleValue,
                Param.TargetRoleValue,
                Param.theEffect.values,
                Param.EffectRate,
                FormulaParam.EffectValues);

            FormulaParam.BossHurt = 1000;
            FormulaParam.BossDmgRed = 1000;

            if (Param.pTarget && Param.pTarget && Param.pTarget->GetObjectType() == OBJECT_MOB)
            {
                auto pMob = std::dynamic_pointer_cast<Mob>(Param.pTarget);
                if (!pMob)
                {
                    return;
                }

                if (MOB_RARE_BOSS == pMob->GetMobRare())
                {			// boss 伤害加成
                    FormulaParam.BossHurt += Param.CasterRoleValue.NowBossHurt;
                }
            }

            if (Param.pTarget && Param.pTarget->GetObjectType() == OBJECT_PLAYER && Param.pCaster->GetObjectType() == OBJECT_MOB)
            {
                auto pMob = std::dynamic_pointer_cast<Mob>(Param.pCaster);
                if (!pMob)
                {
                    return;
                }

                if (MOB_RARE_BOSS == pMob->GetMobRare())
                {			// boss 伤害加成
                    FormulaParam.BossDmgRed -= Param.CasterRoleValue.NowBossDmgRed;
                }
            }
        };
    };

    // 法术伤害
    template<
        typename SkillRoleValue_t /*TSkillRoleValue的实例类*/
        , typename SkillEffectParam_t /*TSkillEffectParam的实例类*/
        , typename Mob /*地图怪物类型，继承自MapObject*/
    >
        struct TSkillEffect_ElemtDmg
        : public TSkillEffect<SkillRoleValue_t, SkillEffectParam_t, Mob>
        , public ISkillEffect_FormulaDmg<SkillRoleValue_t>
    {
        // 获取伤害公式参数
        virtual void GetFormulaParam(SkillFormulaParam& FormulaParam, const SkillEffectParam_t& Param)
        {
            FormulaParam.CasterCrit = Param.CasterRoleValue.NowMCRI; //法術暴击率
            FormulaParam.TargetTenacity = Param.TargetRoleValue.NowMRES; //法術韧性率
            FormulaParam.CasterAtk = Param.CasterRoleValue.NowAP;   //法術攻击力
            FormulaParam.TargetDef = Param.TargetRoleValue.NowMARM; //法術防御力
            FormulaParam.CasterPenetrate = Param.CasterRoleValue.NowMRP;  //法術穿透率
            FormulaParam.TargetDmgAvoid = this->GetTargetElemetRemitValue(Param.TargetRoleValue); //免伤率
            FormulaParam.CasterCritAdd = Param.CasterRoleValue.NowMCRID;//法術暴击增伤率
            FormulaParam.vecEffect = Param.theEffect.values;
            FormulaParam.EffectValue = TSkillEffect<SkillRoleValue_t, SkillEffectParam_t, Mob>::GetSkillEffectValue(
                Param.CasterRoleValue,
                Param.TargetRoleValue,
                Param.theEffect.values,
                Param.EffectRate,
                FormulaParam.EffectValues);

            FormulaParam.BossHurt = 1000;
            FormulaParam.BossDmgRed = 1000;

            if (Param.pTarget && Param.pTarget->GetObjectType() == OBJECT_MOB)
            {
                auto pMob = std::dynamic_pointer_cast<Mob>(Param.pTarget);
                if (!pMob)
                {
                    return;
                }

                if (MOB_RARE_BOSS == pMob->GetMobRare())
                {
                    // boss 伤害加成
                    FormulaParam.BossHurt += Param.CasterRoleValue.NowBossHurt;
                }
            }

            if (Param.pTarget && Param.pTarget->GetObjectType() == OBJECT_PLAYER && Param.pCaster->GetObjectType() == OBJECT_MOB)
            {
                auto pMob = std::dynamic_pointer_cast<Mob>(Param.pCaster);
                if (!pMob)
                {
                    return;
                }

                if (MOB_RARE_BOSS == pMob->GetMobRare())
                {
                    // boss 伤害加成
                    FormulaParam.BossDmgRed -= Param.CasterRoleValue.NowBossDmgRed;
                }
            }
        };
    };

    // 技能效果: hp和mp恢復
    template<
        typename SkillRoleValue_t /*TSkillRoleValue的实例类*/
        , typename SkillEffectParam_t /*TSkillEffectParam的实例类*/
        , typename Mob /*地图怪物类型，继承自MapObject*/
    >
        class TSkillEffect_Restore
        : public TSkillEffect<SkillRoleValue_t, SkillEffectParam_t, Mob>
    {
    public:
        bool CalEffect(SkillEffectParam_t& Param, int& RestoreValue)
        {
            if (Param.pCaster == nullptr || Param.pTarget == nullptr)
                return false;

            // 治疗值计算：A法术攻击力 * 技能治疗倍数+ 技能治疗值
            std::vector<int32> vec;
            RestoreValue = TSkillEffect<SkillRoleValue_t, SkillEffectParam_t, Mob>::GetSkillEffectValue(
                Param.CasterRoleValue,//效果值
                Param.TargetRoleValue,
                Param.theEffect.values,
                Param.EffectRate,
                vec);

            // 浮動值	
            int RandValue = 100 - (rand() % 10);
            RestoreValue = static_cast<int>(static_cast<float>(RestoreValue * RandValue) / 100.0f + 0.5f);

            // 最終恢復 : 法術/物理攻击力5%作比较，取最大值作为最终值
            //int CheckRestore = Param.CasterRoleValue.NowAP * 5 / 100;
            //RestoreValue = max(CheckRestore, RestoreValue);

            return true;
        };
    };
};
