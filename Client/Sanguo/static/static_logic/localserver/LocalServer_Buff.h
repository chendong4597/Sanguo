/*!
* \file LocalServer_Buff.h
*
* \author zhichenxia
* \date 2018/6/13
*
*
*/
#pragma once
#include "LocalServer.h"
#include <msg/MsgBuff.pb.h>


class SkillBuffInfo;
class SkillBuffContainer;

NS_BEGIN

class LocalServer_Buff : public LocalServer
{
	RTTI_DEF(LocalServer_Buff);
	//friend class SkillBuff;

	LocalServer_Buff(void) = default;

public:
	//struct stLocalBuff
	//{
	//	int32 buffId{};
	//	int32 caster_id{};
	//	int32 lastTime{};
	//};
	static LocalServer_Buff& getInstance(void);

	using LocalServer::send;

public:
	bool addBuff(int32 buffId, int32 target_id, int32 caster_id = -1, NS::uint32 lastTime = 0, NS::int32 nAddHeap = 1);
	bool removeBuff(int32 buffId, int32 target_id);
	bool delAllBuff(int target_id, bool bDebuff = false);
	bool delOneDebuff(int target_id);
	int SearchBuffIndex(int CasterMapObjectID, int BuffDataID);

	void clearBuffContainerMap();
	void saveRestoreBuff(const MsgPB::MsgUserBuffData::Buff& msgBuff);
	void restoreBuff();

	//同步Buff数据
	bool syncBuffCache(const SkillBuffInfo* pBuff, int buffEvent);
	SkillBuffContainer* FindBuffContainer(int idObj);

	// 统计角色Debuff个数
	int countDebuff(int nTargetId);

	// 获取掉落Buff的掉落物品数量的倍率 (这个函数会直接根据配合的概率进行计算并返回结果，所以每次调用此函数得到的结果都不一定是一样的)，无掉落Buff时返回 1
	int DropBuffMultiplyingPower(int nMapObjID);

	virtual void update(float dt) override;
private:
	
	//std::vector<std::shared_ptr<stLocalBuff>> m_vecPlayerRestoreBuff;

	std::vector<MsgPB::MsgUserBuffData::Buff> m_vecPlayerRestoreBuffs;
};

NS_END
