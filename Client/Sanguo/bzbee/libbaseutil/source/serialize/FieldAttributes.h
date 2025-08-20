/*!
 * \file FieldAttributes.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/14/2016
 *
 * 
 */
#pragma once

#include "../BaseType.h"
#include "../StringUtil.h"

NS_BEGIN

class IFieldAttribute
{
public:
	static const int ATTR_DISABLED = 1 << 0;
	static const int ATTR_EXPANDED = 1 << 1;
	static const int ATTR_FIXED_SIZE = 1 << 2;
	static const int ATTR_DONNOT_CREATE_PROP = 1 << 3;
	static const int ATTR_DEFAULT = ATTR_EXPANDED;

	enum class SubType
	{
		Basic = 0,
		PieceInfoListLocal,
		PieceInfoListGlobal,
		ColorInfoListLocal,
		ColorInfoListGlobal,
		StyleListLocal,
		StyleListGlobal,
		FileList,
		FontList,
	};

public:
	IFieldAttribute(int flag = ATTR_DEFAULT, SubType subType = SubType::Basic) : m_attrFlag(flag), m_subType(subType) {};
	virtual ~IFieldAttribute() {};

	bool checkAttributeFlag(int flag) const { return ((m_attrFlag & flag) == flag); };
	SubType getSubType() const { return m_subType; };

private:
	int m_attrFlag{};
	SubType m_subType{ SubType::Basic };

};

class IntegerFieldAttribute : public IFieldAttribute
{
public:
	IntegerFieldAttribute(I64 rmin, I64 rmax, I64 s, bool spin = true, bool wrap = false, int flag = ATTR_DEFAULT) :IFieldAttribute(flag), rangeMin(rmin), rangeMax(rmax), step(s), useSpin(spin), useWrap(wrap) {};
	virtual ~IntegerFieldAttribute() {};

public:
	I64 rangeMin{};
	I64 rangeMax{};
	I64 step{ 1 };
	bool useSpin{ true };
	bool useWrap{ false };

};

class FloatFieldAttribute : public IFieldAttribute
{
public:
	FloatFieldAttribute(double rmin, double rmax, double s, bool spin = true, bool wrap = false, int flag = ATTR_DEFAULT) :IFieldAttribute(flag), rangeMin(rmin), rangeMax(rmax), step(s), useSpin(spin), useWrap(wrap) {};
	virtual ~FloatFieldAttribute() {};

public:
	double rangeMin{};
	double rangeMax{};
	double step{ 0.1 };
	bool useSpin{ true };
	bool useWrap{ false };

};

class EnumFieldAttribute : public IFieldAttribute
{
public:
	class EnumItemInfo
	{
	public:
		EnumItemInfo(int value, const tstring& name) :m_value(value), m_name(name) {};

	public:
		int m_value{};
		tstring m_name;
	};

	typedef std::vector<EnumItemInfo> EnumItemInfoList;

public:
	EnumFieldAttribute(const EnumItemInfoList* pInfoList, int flag = ATTR_DEFAULT) :IFieldAttribute(flag), m_pInfoList(pInfoList) {};
	virtual ~EnumFieldAttribute() {};

	const tstring& getEnumString(int value) const
	{
		if (m_pInfoList)
		{
			for (const auto& enumItemInfo : *m_pInfoList)
			{
				if (enumItemInfo.m_value == value) return enumItemInfo.m_name;
			}
		}

		return EMPTY_STRING;
	}

public:
	const EnumItemInfoList* m_pInfoList{};

};

class FlagFieldAttribute : public IFieldAttribute
{
public:
	class FlagItemInfo
	{
	public:
		FlagItemInfo(uint value, const tstring& name, const tstring& displayName) :m_value(value), m_name(name), m_displayName(displayName) {};

	public:
		uint m_value{};
		tstring m_name;
		tstring m_displayName;
	};

	typedef std::vector<FlagItemInfo> FlagItemInfoList;

public:
	FlagFieldAttribute(const FlagItemInfoList* pInfoList, int flag = ATTR_DEFAULT) :IFieldAttribute(flag), m_pInfoList(pInfoList) {};
	virtual ~FlagFieldAttribute() {};

	tstring getFlagString(uint value, const tstring& prefix) const
	{
		StringList stringList;
		if (m_pInfoList)
		{
			for (const auto& flagItemInfo : *m_pInfoList)
			{
				if ((flagItemInfo.m_value & value) == flagItemInfo.m_value)
				{
					stringList.push_back(prefix + flagItemInfo.m_name);
				}
			}
		}

		return StringUtil::joinString(stringList, " | ");
	}

	uint getExportMask() const
	{
		uint exportMask = 0;

		if (m_pInfoList)
		{
			for (const auto& flagItemInfo : *m_pInfoList)
			{
				exportMask |= flagItemInfo.m_value;
			}
		}

		return exportMask;
	}

public:
	const FlagItemInfoList* m_pInfoList{};

};

class FileFieldAttribut : public IFieldAttribute
{
public:
	FileFieldAttribut(const tstring& dir, const tstring& wildcard, int flag = ATTR_DEFAULT) :IFieldAttribute(flag, SubType::FileList), m_strDir(dir), m_strWildcard(wildcard){};
	virtual ~FileFieldAttribut() {};

public:
	const tstring m_strDir;
	const tstring m_strWildcard;
};

class ColorFieldAttribute : public IFieldAttribute
{
public:
	ColorFieldAttribute(bool bHasAlpha, int flag = ATTR_DEFAULT)
		: IFieldAttribute(flag)
		, m_bHasAlpha(bHasAlpha)
	{}
	virtual ~ColorFieldAttribute()
	{}

public:
	bool	m_bHasAlpha{};
};

extern const IFieldAttribute FFA_DISABLED;
extern const IFieldAttribute FFA_FIXED_SIZE;
extern const IFieldAttribute FFA_DONNOT_CREATE_PROP;
extern const IntegerFieldAttribute FFA_DEFAULT_INTEGER;
extern const IntegerFieldAttribute FFA_DEFAULT_INTEGER_UNEXPAND;
extern const IntegerFieldAttribute FFA_DEFAULT_INTEGER_UNBOUND;
extern const IntegerFieldAttribute FFA_DEFAULT_INTEGER_1_100_1;
extern const IntegerFieldAttribute FFA_DEFAULT_INTEGER_DEGREE;
extern const IntegerFieldAttribute FFA_DEFAULT_COLOR_CHANNEL;
extern const FloatFieldAttribute FFA_DEFAULT_FLOAT;

extern const IntegerFieldAttribute FFA_POSITIVE_INTEGER;
extern const FloatFieldAttribute FFA_FLOAT_STEP_01;
extern const FloatFieldAttribute FFA_FLOAT_STEP_001;
extern const FloatFieldAttribute FFA_FLOAT_STEP_0001;
extern const FloatFieldAttribute FFA_FLOAT_0_1_STEP_001;
extern const FloatFieldAttribute FFA_DEFAULT_SCALE;

extern const IFieldAttribute FFA_PIECE_INFO_LIST_LOCAL;
extern const IFieldAttribute FFA_PIECE_INFO_LIST_GLOBAL;
extern const IFieldAttribute FFA_COLOR_INFO_LIST_LOCAL;
extern const IFieldAttribute FFA_COLOR_INFO_LIST_GLOBAL;
extern const IFieldAttribute FFA_STYLE_LIST_LOCAL;
extern const IFieldAttribute FFA_STYLE_LIST_GLOBAL;
extern const IFieldAttribute FFA_FONT_FILE;

extern const FileFieldAttribut FFA_SPRITE_FILE;
extern const FileFieldAttribut FFA_PARTICLE_FILE;
extern const FileFieldAttribut FFA_MODEL_FILE;
extern const FileFieldAttribut FFA_TEXTURE_FILE;
extern const FileFieldAttribut FFA_SPINE_FILE;

extern const ColorFieldAttribute FFA_DEFAULT_COLOR;
extern const ColorFieldAttribute FFA_ALPHA_COLOR;

NS_END

#define ENUM_ITEM(x) { static_cast<int>(x), #x }
#define FLAG_ITEM(x, n) { static_cast<NS::uint>(x), #x, n}
