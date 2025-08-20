/*!
* \file RandomDataMgr.h
*
* \author ChenBK
* \date 2018/05/25
*
*
*/

#pragma once
#include "BaseTypeDefine.h"

class RandomDataMgr
{
public:
	RandomDataMgr() = default;
	~RandomDataMgr() = default;

	static RandomDataMgr& getInstance();

	NS::I64 CreateGuidInt64();
};
