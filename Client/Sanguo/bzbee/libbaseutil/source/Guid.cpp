/*!
 * \file Guid.cpp
 *
 * \author zjhlogo
 * \date 2019/07/22
 *
 * 
 */
#include "Guid.h"
#include "MemCrc.h"
#include <cstdlib>
#include <chrono>
#include <ctime>

NS_BEGIN

Guid::operator ref_type<uint32[4]>()
{
    return data;
}

bool Guid::isValid() const
{
    return ((data[0] | data[1] | data[2] | data[3]) != 0);
}

bool Guid::operator==(const Guid& inOther) const
{
    return ((
        (data[0] ^ inOther.data[0]) |
        (data[1] ^ inOther.data[1]) |
        (data[2] ^ inOther.data[2]) |
        (data[3] ^ inOther.data[3])) == 0);
}

bool Guid::operator!=(const Guid& inOther) const
{
    return !(*this == inOther);
}

tstring Guid::toString() const
{
    return EMPTY_STRING;
}

Guid Guid::create()
{
    static uint16 GIncrementCounter = 0;

    Guid OutGuid;

    // Add sequential bits to ensure sequentially generated guids are unique even if Cycles is wrong
    const uint32 SequentialBits = static_cast<uint32>(GIncrementCounter++);
    // Add randomness to improve uniqueness across machines
    const uint32 RandBits = std::rand() & 0xffff;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto date_time = std::localtime(&in_time_t);

    const uint32 Cycles =
        std::chrono::duration_cast<std::chrono::duration<uint32, std::milli>>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    OutGuid[0] = (SequentialBits << 16) | RandBits;
    OutGuid[1] = (date_time->tm_sec << 24) | (date_time->tm_mon << 16) | (date_time->tm_hour << 8) | date_time->tm_mday;
    OutGuid[2] = (date_time->tm_min << 16) | date_time->tm_sec;
    OutGuid[3] = (date_time->tm_year ^ Cycles);

    return OutGuid;
}

uint32 Guid::getTypeHash(const Guid& inKey)
{
    return memcrc32(inKey.data, sizeof(inKey.data));
}

NS_END
