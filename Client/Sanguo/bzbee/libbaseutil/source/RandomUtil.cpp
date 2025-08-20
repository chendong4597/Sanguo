/*!
 * \file RandomUtil.cpp
 *
 * \author zhijieli
 * \date 2016/12/20
 *
 * 
 */
#include "RandomUtil.h"

namespace
{
	std::random_device rd;
}

NS_BEGIN

std::default_random_engine RandomUtil::m_dre;

int RandomUtil::getRandomInt(int min, int max)
{
	if (min > max) { return {}; }
	std::uniform_int_distribution<> dist{ min, max };

	return dist(RandomUtil::getRandomEngine());
}

float RandomUtil::getRandomFloat(float min, float max)
{
	if (min > max) { return{}; }
	std::uniform_real_distribution<float> dist{ min, max };

	return dist(RandomUtil::getRandomEngine());
}

bool RandomUtil::initialize()
{
	m_dre.seed(rd());

	return true;
}

std::default_random_engine& RandomUtil::getRandomEngine()
{
	static bool initialized{ initialize() };

	return m_dre;
}

NS_END
