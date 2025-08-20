/*!
 * \file GlobalCfg.h
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"



class BuffTypeCfg
{
public:
	struct BuffTypeInfo
	{
		int n_id{};
		std::string s_Name{};
		std::string s_icon{};
		int n_nOverlay{};
		int n_FollowRat{};
		float f_tm{};
		std::string s_MeshTag;
		std::string s_SockName;
		std::string s_path;
		std::string s_desc;
	};

	typedef std::map<int, BuffTypeInfo> MAP_BUFFTYPE;

public:

public:

	BuffTypeCfg() {}
	static BuffTypeCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const BuffTypeInfo* GetBuffType(int id);
	std::string GetBuffPath() { return m_strPath; }
private:
	MAP_BUFFTYPE m_mapBuffType;
	std::string m_strPath;
};
