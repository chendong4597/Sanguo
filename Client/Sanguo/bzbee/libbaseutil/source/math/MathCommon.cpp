/*!
* \file MathCommon.cpp
*
* \author kechen
* \date 2018/11/22
*
*
*/

#include "MathCommon.h"

NS_BEGIN

bool Math::RealEqual(Real a, Real b, Real tolerance)
{
	if (fabs(b - a) <= tolerance)
		return true;
	else
		return false;
}

NS_END