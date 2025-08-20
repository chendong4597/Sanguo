
/*!
 * \file Debug.cpp
 * \date 5-2-2017 17:57:20
 * 
 * 
 * \author cs (04nycs@gmail.com)
 */

#include "Debug.h"

bool Debug::isFlagSet(bzbee::uint flags)
{
	return (s_flags & flags) != 0; 
}

void Debug::setFlag(bzbee::uint flags, bool set)
{
	if (set)
	{
		s_flags |= flags;
	}
	else
	{
		s_flags &= ~flags;
	}
}

float Debug::getTimeScale()
{
	return s_timeScale;
}

void Debug::setTimeScale(float scale)
{
	s_timeScale = scale;
}

bool Debug::playControl()
{
	if (s_bStepInto)
	{
		s_bPause = true;
		s_bStepInto = false;

		return true;
	}

	if (s_bPause) return false;

	return true;
}

bzbee::uint Debug::s_flags = 0;
float Debug::s_timeScale = 1.0f;

bool Debug::s_bPause = false;
bool Debug::s_bStepInto = false;

int Debug::m_nRenderOrderMax = INT_MAX;
int Debug::m_nRenderOrderMin = INT_MIN;
