/*!
 * \file StringUtil.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 06/07/2016
 *
 * 
 */
#pragma once

#include "BaseType.h"
#include "format.h"
#include <sstream>

NS_BEGIN

class StringUtil
{
public:
	static int replaceString(tstring& strInOut, const tstring& strSearch, const tstring& strReplace);

	template <typename CONTAINER> static int splitString(CONTAINER& arrOut, const tstring& strIn, const tstring& strSplit, bool eraseEmpty = false)
	{
		StringList arrString;
		if (splitString(arrString, strIn, strSplit, eraseEmpty) <= 0) return 0;

		for (const auto& strValue : arrString)
		{
			std::stringstream ss(strValue);
			typename CONTAINER::value_type value;
			ss >> value;
			if (!ss)
			{
				arrOut.clear();
				return 0;
			}
			arrOut.push_back(value);
		}

		return static_cast<int>(arrOut.size());
	}

	template <typename T> static int splitString(std::set<T>& arrOut, const tstring& strIn, const tstring& strSplit, bool eraseEmpty = false)
	{
		StringList arrString;
		if (splitString(arrString, strIn, strSplit, eraseEmpty) <= 0) return 0;

		for (const auto& strValue : arrString)
		{
			std::stringstream ss(strValue);
			T value;
			ss >> value;
			if (!ss)
			{
				arrOut.clear();
				return 0;
			}

			arrOut.insert(value);
		}

		return arrOut.size();
	}

	template <typename T> static tstring joinString(const T& arrIn, const tstring& strConnect, bool reverse = false)
	{
		tstring strOut;

		int nLength = int(arrIn.size());
		if (nLength <= 0) return strOut;

		if (reverse)
		{
			auto it = arrIn.rbegin();
			strOut += fmt::format("{0}", *it++);

			for (; it != arrIn.rend(); ++it)
			{
				strOut += strConnect;
				strOut += fmt::format("{0}", *it);
			}
		}
		else
		{
			auto it = arrIn.begin();
			strOut += fmt::format("{0}", *it++);

			for (; it != arrIn.end(); ++it)
			{
				strOut += strConnect;
				strOut += fmt::format("{0}", *it);
			}
		}

		return strOut;
	}
	
	template<typename T> static bool stringToType(T& valueOut, const tstring& strIn)
	{
		std::stringstream ss(strIn);
		ss >> valueOut;

		return static_cast<bool>(ss);
	}

	template<typename T> static bool typeToString(tstring& strOut, const T& valueIn)
	{
		std::stringstream ss;
		ss << valueIn;
		strOut = ss.str();

		return static_cast<bool>(ss);
	}

	static bool compressData(BufferData& dataOut, const void* pData, int size);
	static bool uncompressData(BufferData& dataOut, const void* pData, int size);

	static bool base64Encode(tstring& textOut, const void* pData, int dataLength);
	static bool base64Decode(BufferData& dataOut, const tstring& textIn);

	static void extractNumberFromString(tstring& strOut, const tstring& strIn);

	static bool trim(std::string& strOut, const std::string& strIn);
    static bool trim(std::wstring& wstrOut, const std::wstring& wstrIn);
   
    static std::string trimLeft(const std::string& str);
    static std::wstring trimLeft(const std::wstring& wstr);

    static std::string trimRight(const std::string& str);
    static std::wstring trimRight(const std::wstring& wstr);

    static bool startsWith(const tstring& str, const tstring& subStr);
    static bool isEqualCaseInsensitive(const tstring& str1, const tstring& str2);

	static void toupper(tstring& strOut, const tstring& strIn);
	static void tolower(tstring& strOut, const tstring& strIn);
	static void towupper(std::wstring& strOut, const std::wstring& strIn);
	static void towlower(std::wstring& strOut, const std::wstring& strIn);
	static bool isSpace(const tstring& strIn);
    static bool isLinebreak(char ch);
    static bool isLinebreak(wchar_t ch);
    static bool hasSpace(const tstring& strIn);

	static bool str2bool(bool& bValue, const tchar* pstrIn);
	static bool str2int(int& nValue, const tchar* pstrIn);
	static bool str2Uint(uint& nValue, const tchar* pstrIn);
	static bool str2int64(I64& nValue, const tchar* pstrIn);
	static bool str2Uint64(UI64& nValue, const tchar* pstrIn);
	static bool str2float(float& fValue, const tchar* pstrIn);
	static bool strHex2Uint(uint& nValue, const tchar* pstrIn);
	static bool strNCopy(tchar* pstrOut, uint size, const tstring& strIn);

	static void bool2str(tstring& strOut, bool bValue);
	static void int2str(tstring& strOut, int nValue);
	static tstring int2str(int value);
	static void uint2str(tstring& strOut, uint nValue);
	static void int642str(tstring& strOut, I64 nValue);
	static tstring int642str(I64 nValue);
	static void uint642str(tstring& strOut, UI64 nValue);
	static void hex2str(tstring& strOut, uint nValue);
	static void float2str(tstring& strOut, float fValue);
	static void formatChat(tstring& strOut, const tstring& strIn);
	static void clearStrTag(tstring& strInOut, const tstring& strIn = "<>", char replaceChar = '*');

	static std::wstring utf8to16(const std::string& strUtf8);
	static std::string utf16to8(const std::wstring& strUtf16);

    static std::wstring utf8to32(const std::string& strUtf8);
    static std::string utf32to8(const std::wstring& strUtf32);

	static int32 str2HashID(const char *string);

	template<typename T> static tstring number2CurrencyStr(T nValue)
	{
		tstring str;
		if (nValue >= 0)
		{
			typeToString<I64>(str, nValue);
		}
		else
		{
			typeToString<UI64>(str, nValue);
		}

		std::vector<char> list;

		auto&& strSize = str.size();
		if (strSize > 0)
		{
			for (size_t i = 0; i < strSize; i++)
			{
				auto&& c = str[strSize - i - 1];

				list.push_back(c);

				if ((i + 1) % 3 == 0 && c != '-' && i < strSize - 1)
				{
					list.push_back(',');
				}
			}
		}

		std::stringstream ss;
		for (int i = list.size() - 1; i >= 0; --i)
		{
			ss << list[i];
		}

		return ss.str();
	}
};

template<> int StringUtil::splitString<StringList>(StringList& arrOut, const tstring& strIn, const tstring& strSplit, bool eraseEmpty);
template<> int StringUtil::splitString<glm::ivec2>(glm::ivec2& vec2Out, const tstring& strIn, const tstring& strSplit, bool eraseEmpty);
template<> int StringUtil::splitString<glm::ivec3>(glm::ivec3& vec3Out, const tstring& strIn, const tstring& strSplit, bool eraseEmpty);
template<> int StringUtil::splitString<glm::ivec4>(glm::ivec4& vec4Out, const tstring& strIn, const tstring& strSplit, bool eraseEmpty);
template<> int StringUtil::splitString<glm::vec2>(glm::vec2& vec2Out, const tstring& strIn, const tstring& strSplit, bool eraseEmpty);
template<> int StringUtil::splitString<glm::vec3>(glm::vec3& vec3Out, const tstring& strIn, const tstring& strSplit, bool eraseEmpty);
template<> int StringUtil::splitString<glm::vec4>(glm::vec4& vec4Out, const tstring& strIn, const tstring& strSplit, bool eraseEmpty);
template<> bool StringUtil::stringToType<tstring>(tstring& valueOut, const tstring& strIn);
template<> bool StringUtil::typeToString<tstring>(tstring& strOut, const tstring& valueIn);

NS_END
