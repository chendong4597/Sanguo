#include "BlockManager.h"



using namespace NS;


void BlockManager::update(float dt)
{
	static const std::string REASONS[] =
	{
		"Connecting...",
		"Sending:",
		"Receiving:",
	};

	auto block = getBlock();

	if (m_block != block)
	{
		m_block = block;
		if (block)
		{
			if (!m_pWaitingDialog)
			{
				//m_pWaitingDialog = UiInternetConnection::create();
				//UiMgr::getInstance().showModalDialog(m_pWaitingDialog);
			}
		}
		else
		{
			if (m_pWaitingDialog)
			{
				//m_pWaitingDialog->closeMsgBox();
				//UiMgr::getInstance().closeDialog(m_pWaitingDialog);
				m_pWaitingDialog = nullptr;
			}

			m_reason.clear();
		}
	}

	if (block && m_pWaitingDialog)
	{
		std::ostringstream os;
		for (const auto& v : m_map)
			os << ">>> " << REASONS[v.first] << v.second << std::endl;
		auto reason = os.str();
		if (m_reason != reason)
		{
			m_reason = reason;
		}
	}
}


void BlockManager::setBlock(int type, const tstring& reason)
{
	m_map[type] = reason;
}


void BlockManager::clearBlock(int type)
{
	m_map.erase(type);
}


bool BlockManager::getRetry(void) const
{
	return false;
}
