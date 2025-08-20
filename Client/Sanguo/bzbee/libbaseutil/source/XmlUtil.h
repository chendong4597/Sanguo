/*!
 * \file XmlUtil.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 05/13/2016
 *
 * 
 */
#pragma once

#include "BaseType.h"
#include "StringUtil.h"
#include "FileUtil.h"
#include "tinyxml2.h"
#include <future>

NS_BEGIN

#define DECLEAR_HAS_MEMBER(name) \
template<typename T, typename = void> \
struct has_member_##name : std::false_type{}; \
template<typename T> \
struct has_member_##name<T, decltype(std::declval<T>().name, void())> : std::true_type{};

template<typename T> struct has_member_x : std::false_type{};
template<typename T> struct has_member_y : std::false_type{};
template<typename T> struct has_member_z : std::false_type{};
template<typename T> struct has_member_w : std::false_type{};
template<> struct has_member_x<glm::vec2> : std::true_type{};
template<> struct has_member_x<glm::ivec2> : std::true_type{};
template<> struct has_member_x<glm::vec3> : std::true_type{};
template<> struct has_member_x<glm::ivec3> : std::true_type{};
template<> struct has_member_x<glm::vec4> : std::true_type{};
template<> struct has_member_x<glm::ivec4> : std::true_type{};
template<> struct has_member_x<glm::quat> : std::true_type{};
template<> struct has_member_y<glm::vec2> : std::true_type{};
template<> struct has_member_y<glm::ivec2> : std::true_type{};
template<> struct has_member_y<glm::vec3> : std::true_type{};
template<> struct has_member_y<glm::ivec3> : std::true_type{};
template<> struct has_member_y<glm::vec4> : std::true_type{};
template<> struct has_member_y<glm::ivec4> : std::true_type{};
template<> struct has_member_y<glm::quat> : std::true_type{};
template<> struct has_member_z<glm::vec3> : std::true_type{};
template<> struct has_member_z<glm::ivec3> : std::true_type{};
template<> struct has_member_z<glm::vec4> : std::true_type{};
template<> struct has_member_z<glm::ivec4> : std::true_type{};
template<> struct has_member_z<glm::quat> : std::true_type{};
template<> struct has_member_w<glm::vec4> : std::true_type{};
template<> struct has_member_w<glm::ivec4> : std::true_type{};
template<> struct has_member_w<glm::quat> : std::true_type{};

template<typename T>
typename std::enable_if<has_member_x<T>::value && has_member_y<T>::value && !has_member_z<T>::value && !has_member_w<T>::value, std::ostream&>::type operator<<(std::ostream& os, const T& t)
{
	os << t.x << ',' << t.y;
	return os;
}
template<typename T>
typename std::enable_if<has_member_x<T>::value && has_member_y<T>::value && !has_member_z<T>::value && !has_member_w<T>::value, std::istream&>::type operator>>(std::istream& is, T& t)
{
	tstring str;
	std::getline(is, str);

	std::vector<typename T::value_type> arrValue;
	if (StringUtil::splitString(arrValue, str, ",") != 2) return is;

	t.x = arrValue[0];
	t.y = arrValue[1];

	return is;
}
template<typename T>
typename std::enable_if<has_member_x<T>::value && has_member_y<T>::value && has_member_z<T>::value && !has_member_w<T>::value, std::ostream&>::type operator<<(std::ostream& os, const T& t)
{
	os << t.x << ',' << t.y << ',' << t.z;
	return os;
}
template<typename T>
typename std::enable_if<has_member_x<T>::value && has_member_y<T>::value && has_member_z<T>::value && !has_member_w<T>::value, std::istream&>::type operator>>(std::istream& is, T& t)
{
	tstring str;
	std::getline(is, str);

	std::vector<typename T::value_type> arrValue;
	if (StringUtil::splitString(arrValue, str, ",") != 3) return is;

	t.x = arrValue[0];
	t.y = arrValue[1];
	t.z = arrValue[2];

	return is;
}
template<typename T>
typename std::enable_if<has_member_x<T>::value && has_member_y<T>::value && has_member_z<T>::value && has_member_w<T>::value, std::ostream&>::type operator<<(std::ostream& os, const T& t)
{
	os << t.x << ',' << t.y << ',' << t.z << ',' << t.w;
	return os;
}
template<typename T>
typename std::enable_if<has_member_x<T>::value && has_member_y<T>::value && has_member_z<T>::value && has_member_w<T>::value, std::istream&>::type operator>>(std::istream& is, T& t)
{
	tstring str;
	std::getline(is, str);

	std::vector<typename T::value_type> arrValue;
	if (StringUtil::splitString(arrValue, str, ",") != 4) return is;

	t.x = arrValue[0];
	t.y = arrValue[1];
	t.z = arrValue[2];
	t.w = arrValue[3];

	return is;
}

template<typename T>
typename std::enable_if<std::is_enum<T>::value, std::ostream&>::type operator<<(std::ostream& os, const T& t)
{
	os << (int) t;
	return os;
}
template<typename T>
typename std::enable_if<std::is_enum<T>::value, std::istream&>::type operator>>(std::istream& is, T& t)
{
	int nValue = 0;
	is >> nValue;

	t = (T) nValue;

	return is;
}
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& t)
{
	os << StringUtil::joinString<std::vector<T>>(t, ",");
	return os;
}
template<typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& t)
{
	tstring str;
	std::getline(is, str);
	is.clear();
	StringUtil::splitString(t, str, ",");

	return is;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::map<int, std::vector<T>>& t)
{
	tstring str;
	for (uint i = 0; i < t.size(); ++i)
	{
		str += StringUtil::joinString<std::vector<T>>(t[i], ",");
		if (t.size() > 1 && i < t.size()-1) str += "|";
	}
	os << str;
	return os;
}
template<typename T>
std::istream& operator>>(std::istream& is, std::map<int, std::vector<T>>& t)
{
	tstring str;
	std::getline(is, str);
	is.clear();

	std::vector<T> v;
	StringUtil::splitString(v, str, "|");
	for (uint i = 0; i < v.size(); ++i)
	{
		StringUtil::splitString(t[i], v[i], ",");
	}

	return is;
}

class XmlUtil
{
public:
    typedef std::function<bool(tstring& dataOut, const tstring& dataIn, const tstring& filePath)> DataEncryptFunc;

public:
	template <typename T>
	static bool getAttribute(T& valueOut, const tinyxml2::XMLElement* xmlElement, const char* attrName)
	{
		if (!xmlElement || !attrName) return false;

		return extractValue(valueOut, xmlElement->Attribute(attrName));
	};

	template <typename T>
	static bool setAttribute(const T& value, tinyxml2::XMLElement* xmlElement, const char* attrName)
	{
		if (!xmlElement || !attrName) return false;

		std::ostringstream ss;
		ss << value;

		xmlElement->SetAttribute(attrName, ss.str().c_str());

		return true;
	};

	template <typename T>
	static bool getElement(T& valueOut, const tinyxml2::XMLElement* xmlElement, const char* elementName)
	{
		if (!xmlElement || !elementName) return false;

		auto&& xmlSubElement = xmlElement->FirstChildElement(elementName);
		if (!xmlSubElement) return false;

		return extractValue(valueOut, xmlSubElement->GetText());
	};

	template <typename T>
	static bool setElement(const T& value, tinyxml2::XMLElement* xmlElement, const char* elementName)
	{
		if (!xmlElement || !elementName) return false;

		tinyxml2::XMLDocument* pXmlDoc = xmlElement->GetDocument();
		if (!pXmlDoc) return false;

		tinyxml2::XMLElement* pXmlSubElement = pXmlDoc->NewElement(elementName);
		xmlElement->LinkEndChild(pXmlSubElement);

		std::ostringstream ss;
		ss << value;

		tinyxml2::XMLText* pXmlText = pXmlDoc->NewText(ss.str().c_str());
		pXmlSubElement->LinkEndChild(pXmlText);

		return true;
	};

	static bool loadXmlFile(tinyxml2::XMLDocument& doc, const tstring& filePath, int storage = 0);
	static std::future<tinyxml2::XMLDocument*> asyncLoadXmlFile(const tstring& filePath, int storage = 0);
	static bool saveXmlFile(tinyxml2::XMLDocument& doc, const tstring& filePath, int storage = SubFileSystem::FS_EXTERNAL, bool encode = false);

    static void setDataEncoder(const DataEncryptFunc& encoder) { m_encoder = encoder; };
    static const DataEncryptFunc& getDataEncoder() { return m_encoder; };

    static void setDataDecoder(const DataEncryptFunc& decoder) { m_decoder = decoder; };
    static const DataEncryptFunc& getDataDecoder() { return m_decoder; };

	template<typename T>
	static bool extractValue(T& valueOut, const char* pszValue)
	{
		if (!pszValue) { return false; }
		std::istringstream ss(pszValue);
		return static_cast<bool>(ss >> valueOut);
	}
	static bool extractValue(tstring& valueOut, const char* pszValue)
	{
		if (!pszValue) { return false; }
		valueOut = pszValue;

		return true;
	}
	static bool extractValue(unsigned char& valueOut, const char* pszValue)
	{
		if (!pszValue) { return false; }
		int intValue{};
		std::istringstream ss(pszValue);
		bool ret{ static_cast<bool>(ss >> intValue) };
		valueOut = static_cast<unsigned char>(intValue);

		return ret;
	}
	static bool extractValue(bool& valueOut, const char* pszValue)
	{
		if (!pszValue) { return false; }
		int intValue{};
		std::istringstream ss(pszValue);
		bool ret{ static_cast<bool>(ss >> intValue) };
		valueOut = (intValue != 0);

		return ret;
	}
    static bool extractValue(std::vector<int>& valueOut, const char* pszValue)
    {
        if (!pszValue) { return false; }
        NS::StringUtil::splitString<std::vector<int>>(valueOut, pszValue, ",");
        return !valueOut.empty();
    }

private:
    static DataEncryptFunc m_encoder;
    static DataEncryptFunc m_decoder;

};

NS_END
