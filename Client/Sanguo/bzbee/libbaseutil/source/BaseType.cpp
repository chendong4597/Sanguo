/*!
 * \file BaseType.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/14/2016
 *
 * 
 */
#include "BaseType.h"
#include "StringUtil.h"
#include "i18n/Localization.h"

NS_BEGIN

const tstring EMPTY_STRING = "";
const tstring NULL_STRING = "null";

const glm::ivec2 IVEC2_ZERO = glm::ivec2(0, 0);
const glm::ivec3 IVEC3_ZERO = glm::ivec3(0, 0, 0);
const glm::ivec4 IVEC4_ZERO = glm::ivec4(0, 0, 0, 0);

const glm::ivec2 IVEC2_ONE = glm::ivec2(1, 1);
const glm::ivec3 IVEC3_ONE = glm::ivec3(1, 1, 1);
const glm::ivec4 IVEC4_ONE = glm::ivec4(1, 1, 1, 1);

const glm::vec2 VEC2_ZERO = glm::vec2(0.0f, 0.0f);
const glm::vec3 VEC3_ZERO = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec4 VEC4_ZERO = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

const glm::vec2 VEC2_HALF = glm::vec2(0.5f, 0.5f);
const glm::vec3 VEC3_HALF = glm::vec3(0.5f, 0.5f, 0.5f);
const glm::vec4 VEC4_HALF = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);

const glm::vec2 VEC2_ONE = glm::vec2(1.0f, 1.0f);
const glm::vec3 VEC3_ONE = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec4 VEC4_ONE = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

const glm::vec3 VEC3_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 VEC3_X = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 VEC3_Y = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 VEC3_Z = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 VEC3_MAP_UI_SCALE = glm::vec3(1.f / 60.f);

const glm::quat QUAT_ZERO = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

const glm::vec4 VEC4_WHITE = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 VEC4_BLACK = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 VEC4_GRAY = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
const glm::vec4 VEC4_RED = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 VEC4_GREEN = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
const glm::vec4 VEC4_BLUE = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

const glm::mat4 MAT4_IDENTITY{};

glm::vec4 Color::colorToVector(const Color& color)
{
	return glm::vec4(color.m_r / 255.0f, color.m_g / 255.0f, color.m_b / 255.0f, color.m_a / 255.0f);
}

Color Color::channelListToColor(const IntegerList& channelList)
{
	Color color;
	if (channelList.size() < 4) return color;

	color.m_r = channelList[0];
	color.m_g = channelList[1];
	color.m_b = channelList[2];
	color.m_a = channelList[3];
	return color;
}

Color Color::vectorToColor(const glm::vec4& vec)
{
	Color color;

	color.m_r = static_cast<uint8>(vec.r * 255.0f + 0.5f);
	color.m_g = static_cast<uint8>(vec.g * 255.0f + 0.5f);
	color.m_b = static_cast<uint8>(vec.b * 255.0f + 0.5f);
	color.m_a = static_cast<uint8>(vec.a * 255.0f + 0.5f);
	return color;
}

Color Color::vectorToColor(const glm::vec3& vec)
{
	Color color;

	color.m_r = static_cast<uint8>(vec.r * 255.0f + 0.5f);
	color.m_g = static_cast<uint8>(vec.g * 255.0f + 0.5f);
	color.m_b = static_cast<uint8>(vec.b * 255.0f + 0.5f);
	color.m_a = 255;
	return color;
}

const Color Color::WHITE(255, 255, 255, 255);
const Color Color::BLACK(0, 0, 0, 255);
const Color Color::GRAY(127, 127, 127, 255);
const Color Color::RED(255, 0, 0, 255);
const Color Color::GREEN(0, 255, 0, 255);
const Color Color::BLUE(0, 0, 255, 255);
const Color Color::YELLOW(255, 255, 0, 255);
const Color Color::CYAN(0, 255, 255, 255);
const Color Color::PURPLE(255, 0, 255, 255);

std::ostream& operator<<(std::ostream& os, const Color& color)
{
	os << (uint32)color.m_r << ',' << (uint32)color.m_g << ',' << (uint32)color.m_b << ',' << (uint32)color.m_a;
	return os;
}

std::istream& operator>>(std::istream& is, Color& color)
{
	tstring str;
	std::getline(is, str);

	std::vector<uint32> arrValue;
	if (StringUtil::splitString(arrValue, str, ",") != 4) return is;

	color.m_r = arrValue[0];
	color.m_g = arrValue[1];
	color.m_b = arrValue[2];
	color.m_a = arrValue[3];
	return is;
}

NS_END

const NS::tstring locateGetTranslation(const NS::tstring& key)
{
    return Localization::getInstance().translate(key);
}

const std::wstring locateGetTranslationW(const NS::tstring& key)
{
	NS::tstring strLow = Localization::getInstance().translate(key);
	return NS::StringUtil::utf8to16(strLow);
}