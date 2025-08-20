/*!
 * \file calcluate.cpp
 *
 * \author zjy
 * \date 2018/1/25
 *
 */
#include "stdafx.h"
#include "calculate.h"
#include <StringUtil.h>

//获取中间值
std::string calcluate::getMedianValueName(const std::string& input, const GetAttributeCallback& cb)
{
	if (input.empty()) return input;

	auto input_trans = _LC(input);

	while (true)
	{
		if(!getName(input_trans, cb)) break;
	}
	
	return input_trans;
}

bool calcluate::getName(std::string& input, const GetAttributeCallback& cb)
{
	if (input.empty()) return false;

	auto firstApperPos = input.find_first_of("[");
	if (firstApperPos == std::string::npos) return false;

	auto endPos = input.find_first_of("]");
	if (endPos == std::string::npos) return false;

	auto firstStr = input.substr(0, firstApperPos);
	auto secondStr = input.substr(firstApperPos, endPos);
	auto thirdStr = input.substr(endPos + 1);

	auto newPos = secondStr.find_first_of("]");
    if (newPos != std::string::npos)
    {
        secondStr = secondStr.substr(0, newPos);
    }

	auto pos = secondStr.find_last_of("v");

    int index = 0;
    if (pos != std::string::npos)
    {
		auto numStr = secondStr.substr(pos + 1, 1);
		NS::StringUtil::str2int(index, numStr.c_str());
	}

    int result = cb(index);
	input = firstStr + fmt::format("<font color='ui.cs_green'>{0}</font>", result) + thirdStr;

	return true;
}

std::vector<std::string> calcluate::splitBracket(const std::string &inPut, const std::string &pattern)
{
	std::vector<std::string> out{};
	auto str = inPut;
	while (1)
	{
		auto pos = str.find_first_of(pattern.c_str());
		if (pos != std::string::npos)
		{
			auto tempStr = str.substr(1, pos-1);
			out.push_back(tempStr);
			str = str.substr(pos + 1);
		}
		else
		{
			break;
		}
	}
	return out;
}

