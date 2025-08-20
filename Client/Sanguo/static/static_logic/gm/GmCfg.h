/*!
 * \file Gm.h
 *
 * \author ChenBK;
 * \date 2017/06/12
 *
 * 
 */

#include "BaseTypeDefine.h"
#include <map>
#include <list>

class GmCfg
{
	static size_t GM_HISTORY_MAX;
	static const NS::tstring GmHistoryPath;

public:
	enum GmType : NS::uint
	{
		None = 0,
		ServerGm  = 1,
		ClientGm  = 2
	};

public:
	typedef struct GmItemTag
	{
		NS::tstring		m_gmName;
		NS::tstring		m_gmDefaultPara;
		NS::tstring		m_gmNote;
		NS::uint		m_gmLvel{ 0 };
		GmType			m_gmType{ GmType::None };
	} GmItem;

	typedef std::vector<GmItem> GmItemList;
	typedef std::list<NS::tstring> GmHistoryList;

public:
	GmCfg();
	~GmCfg();

	static GmCfg& getInstance();

	bool initialize();
	void terminate();

	bool readCfg();
	bool readGmHistory();

	size_t getGmCount(){ return m_gmItemList.size(); };
	const GmItem* getGmItemById(size_t idx);
	const GmItem* getGmItemByName(const NS::tstring& gmName);
	GmType getGmType(const NS::tstring& gmName);

	bool addGmHistory(const NS::tstring& gmline);
	GmHistoryList getGmHistory(size_t getNum = 10);
	void flushGmHistory();

private:
	GmItemList m_gmItemList;
	GmHistoryList m_gmHistoryList;
};
