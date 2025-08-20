#pragma once
#include "component/Component.h"



class UiInternetConnection;
class BlockManager final
{
public:
	void update(float dt);

	inline bool getBlock(void) const { return !m_map.empty(); }
	void setBlock(int type, const NS::tstring& reason = "");
	void clearBlock(int type);

	bool getRetry(void) const;

private:
	NS::ComponentPtr<UiInternetConnection> m_pWaitingDialog;

	std::map<int, NS::tstring> m_map;
	bool m_block{ false };
	NS::tstring m_reason;
};
