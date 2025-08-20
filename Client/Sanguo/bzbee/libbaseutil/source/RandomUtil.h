/*!
 * \file RandomUtil.h
 *
 * \author zhijieli
 * \date 2016/12/20
 *
 * 
 */
#pragma once

#include "BaseUtil_Config.h"

#undef log2
#include <random>

NS_BEGIN

class RandomUtil
{
public:
	static std::default_random_engine& getRandomEngine();
	static int getRandomInt(int min, int max);
	static float getRandomFloat(float min, float max);

protected:
	static bool initialize();

private:
	static std::default_random_engine m_dre;
};

NS_END
