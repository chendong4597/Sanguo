/*!
 * \file calcluate.h
 *
 * \author zjy
 * \date 2018/1/25
 *
 */

#pragma once
#include <baseutil.h>
#include <functional>

class calcluate
{
public:
    typedef std::function<int(int index)> GetAttributeCallback;

public:
	static std::string getMedianValueName(const std::string& input, const GetAttributeCallback& cb);
    static std::vector<std::string> splitBracket(const std::string &inPut, const std::string &pattern);

private:
    static bool getName(std::string& input, const GetAttributeCallback& cb);

};
