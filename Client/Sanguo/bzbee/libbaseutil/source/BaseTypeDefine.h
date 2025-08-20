/*!
 * \file BaseTypeDefine.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/14/2016
 *
 * 
 */
#pragma once
#include "BaseUtil_Config.h"
#include <string>
#include <vector>
#include <set>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

NS_BEGIN

typedef int8_t             int8;
typedef uint8_t            uint8;
typedef int16_t            int16;
typedef uint16_t           uint16;
typedef int32_t            int32;
typedef uint32_t           uint32;
typedef int64_t			   I64;
typedef uint64_t		   UI64;
typedef unsigned int       uint;

typedef unsigned char			  uchar;
typedef char					  tchar;
typedef std::string               tstring;
typedef std::vector<uint8>        BufferData;
typedef std::vector<tstring>      StringList;
typedef std::vector<int>          IntegerList;
typedef std::vector<uint>		  UintList;
typedef std::vector<float>        FloatList;
typedef std::set<tstring>         StringSet;
typedef std::set<int>             IntegerSet;
typedef std::vector<glm::mat4>    Mat4List;
typedef std::vector<glm::vec2>	  Vec2List;
typedef std::vector<glm::vec3>	  Vec3List;
typedef std::vector<glm::ivec2>	  Ivec2List;
typedef std::vector<glm::ivec3>	  Ivec3List;
typedef std::vector<uint16>       Uint16List;
typedef std::vector<uint32>       Uint32List;
typedef std::set<glm::ivec2>	  Ivec2Set;

extern const tstring              EMPTY_STRING;
extern const tstring              NULL_STRING;


extern const glm::ivec2           IVEC2_ZERO;
extern const glm::ivec3           IVEC3_ZERO;
extern const glm::ivec4           IVEC4_ZERO;

extern const glm::ivec2           IVEC2_ONE;
extern const glm::ivec3           IVEC3_ONE;
extern const glm::ivec4           IVEC4_ONE;

extern const glm::vec2            VEC2_ZERO;
extern const glm::vec3            VEC3_ZERO;
extern const glm::vec4            VEC4_ZERO;

extern const glm::vec2            VEC2_HALF;
extern const glm::vec3            VEC3_HALF;
extern const glm::vec4            VEC4_HALF;

extern const glm::vec2            VEC2_ONE;
extern const glm::vec3            VEC3_ONE;
extern const glm::vec4            VEC4_ONE;

extern const glm::vec3            VEC3_UP;
extern const glm::vec3            VEC3_X;
extern const glm::vec3            VEC3_Y;
extern const glm::vec3            VEC3_Z;
extern const glm::vec3            VEC3_MAP_UI_SCALE;

extern const glm::quat            QUAT_ZERO;

extern const glm::vec4            VEC4_WHITE;
extern const glm::vec4            VEC4_BLACK;
extern const glm::vec4            VEC4_GRAY;
extern const glm::vec4            VEC4_RED;
extern const glm::vec4            VEC4_GREEN;
extern const glm::vec4            VEC4_BLUE;

extern const glm::mat4            MAT4_IDENTITY;


NS_END
