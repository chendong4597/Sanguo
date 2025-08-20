/*!
 * \file DropCfg.h
 *
 * \author jiangping huang
 * \date 03/02/2017
 *
 * 
 */
#pragma once

#include "BaseTypeDefine.h"
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"

class DropCfg
{
public:
	class DropType
	{
		typedef std::vector<bzbee::tstring> ParamVector;
	public:
		int n_ID{};
		int n_process_type;
		int n_process_data;
		int n_limit_min = -1;
		int n_limit_max = -1;
		ParamVector tv_param;
	};

	typedef std::unordered_map<int, DropType> DropTypeMap;

public:
	static DropCfg& getInstance();

	bool initialize();
	void terminate();

	const DropType* findDropType(int id) const;
	const DropTypeMap& getDropTypeMap() const { return m_dropTypeMap; };
	void getRewardVecById(int dropId, std::vector<std::tuple<int, int>>& outRewards);


private:
	bool parseDropCfg(tinyxml2::XMLElement* pXmlRoot);
	void parseDropInfo(const std::vector<bzbee::tstring>& source, std::vector<std::tuple<int, int>>& target);

private:
	DropTypeMap m_dropTypeMap;

};
