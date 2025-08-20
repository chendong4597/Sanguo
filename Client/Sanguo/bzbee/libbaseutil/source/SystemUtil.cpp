/*!
 * \file SystemUtil.cpp
 * \date 6-18-2014 19:21:40
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "SystemUtil.h"

NS_BEGIN

void SystemUtil::setDeviceLevel(CapacityLevel deviceLevel)
{

}

I64 SystemUtil::getCurrentGameId()
{
    I64 retGameId{};
    tstring strGameId = getGameId(m_userLanguage);
    StringUtil::str2int64(retGameId, strGameId.c_str());

    return retGameId;
}

NS_END
