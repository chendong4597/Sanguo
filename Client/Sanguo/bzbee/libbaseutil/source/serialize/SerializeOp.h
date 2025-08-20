/*!
 * \file SerializeOp.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/11/2016
 *
 * 
 */
#pragma once

#include "SerializeField.h"
#include "../XmlUtil.h"
#include <typeindex>

NS_BEGIN

struct ISerializeField;

//////////////////////////////////////////////////////////////////////////
class ISerializeOp
{
public:
	ISerializeOp(const std::type_index& classId) :m_classId(classId) {};
	virtual ~ISerializeOp() {};

	const std::type_index& getClassId() const { return m_classId; };

	virtual bool fromXml(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) = 0;
	virtual bool toXml(ISerializeField* pFieldBase, tinyxml2::XMLElement* pXmlElement) = 0;
	virtual bool fromXmlOrDefaultValue(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) { return  true; }

private:
	std::type_index m_classId;

};

//////////////////////////////////////////////////////////////////////////
template <typename T, typename = void>
class SerializeOp : public ISerializeOp
{
public:
	SerializeOp() :ISerializeOp(typeid(T)) {};

	virtual bool fromXml(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		auto pField = static_cast<SerializeField<T>*>(pFieldBase);

		T value;
		if (!XmlUtil::getAttribute<T>(value, pXmlElement, pField->getName().c_str())) return false;

		return pField->setter(value);
	}

	virtual bool toXml(ISerializeField* pFieldBase, tinyxml2::XMLElement* pXmlElement) override
	{
		auto pField = static_cast<SerializeField<T>*>(pFieldBase);

		const T& value = pField->getter();
		if (value != pField->getDefaultValue()) return XmlUtil::setAttribute<T>(value, pXmlElement, pField->getName().c_str());

		return true;
	}

	virtual bool fromXmlOrDefaultValue(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		auto pField = static_cast<SerializeField<T>*>(pFieldBase);

		T value;
		if (!XmlUtil::getAttribute<T>(value, pXmlElement, pField->getName().c_str()))
		{
			value = pField->getDefaultValue();
		}

		const T& getValue = pField->getter();
		if (getValue != value)
		{
			return pField->setter(value);
		}

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
template<>
class SerializeOp<tstring> : public ISerializeOp
{
public:
	SerializeOp() :ISerializeOp(typeid(tstring)) {};

	virtual bool fromXml(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		auto pField = static_cast<SerializeField<tstring>*>(pFieldBase);

		tstring value;
		if (!XmlUtil::getAttribute(value, pXmlElement, pField->getName().c_str())) return false;

		return pField->setter(value);
	}

	virtual bool toXml(ISerializeField* pFieldBase, tinyxml2::XMLElement* pXmlElement) override
	{
		auto pField = static_cast<SerializeField<tstring>*>(pFieldBase);

		const tstring& value = pField->getter();
		if (value != pField->getDefaultValue()) return XmlUtil::setAttribute<tstring>(value, pXmlElement, pField->getName().c_str());

		return true;
	}

	virtual bool fromXmlOrDefaultValue(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		auto pField = static_cast<SerializeField<tstring>*>(pFieldBase);

		tstring value;
		if (!XmlUtil::getAttribute(value, pXmlElement, pField->getName().c_str()))
		{
			value = pField->getDefaultValue();
		}

		const tstring& getValue = pField->getter();
		if (getValue != value)
		{
			return pField->setter(value);
		}
		
		return true;
	}
};

template <typename T>
class SerializeOp<T, typename std::enable_if<std::is_enum<T>::value || std::is_same<T, bool>::value>::type> : public ISerializeOp
{
public:
	SerializeOp() :ISerializeOp(typeid(T)) {};

	virtual bool fromXml(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		auto pField = static_cast<SerializeField<T>*>(pFieldBase);

		int value;
		if (!XmlUtil::getAttribute<int>(value, pXmlElement, pField->getName().c_str())) return false;

		return callSetter(pField, value);
	}

	virtual bool toXml(ISerializeField* pFieldBase, tinyxml2::XMLElement* pXmlElement) override
	{
		auto pField = static_cast<SerializeField<T>*>(pFieldBase);

		T value = pField->getter();
		if (value != pField->getDefaultValue()) return XmlUtil::setAttribute<int>((int)value, pXmlElement, pField->getName().c_str());

		return true;
	}

	virtual bool fromXmlOrDefaultValue(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		auto pField = static_cast<SerializeField<T>*>(pFieldBase);

		int value;
		if (!XmlUtil::getAttribute<int>(value, pXmlElement, pField->getName().c_str()))
		{
			value = static_cast<int>(pField->getDefaultValue());
		}

		T getValue = pField->getter();
		if (static_cast<int>(getValue) != value)
		{
			return callSetter(pField, value);
		}

		return true;
	}

private:
	template<typename U>
	bool callSetter(SerializeField<U>* pField, int value) const
	{
		return pField ? pField->setter((U) value) : false;
	}
	bool callSetter(SerializeField<bool>* pField, int value) const
	{
		return pField ? pField->setter(value != 0) : false;
	}
};

class SerializeOpFlag : public ISerializeOp
{
public:
	SerializeOpFlag() :ISerializeOp(typeid(SerializeFieldFlag)) {};
	virtual ~SerializeOpFlag() {};

	virtual bool fromXml(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		SerializeFieldFlag* pField = static_cast<SerializeFieldFlag*>(pFieldBase);

		uint value;
		if (!XmlUtil::getAttribute(value, pXmlElement, pField->getName().c_str())) return false;

		return pField->setter(value);
	}

	virtual bool toXml(ISerializeField* pFieldBase, tinyxml2::XMLElement* pXmlElement) override
	{
		SerializeFieldFlag* pField = static_cast<SerializeFieldFlag*>(pFieldBase);

		uint value = pField->getter();
		if (value != pField->getDefaultValue()) return XmlUtil::setAttribute(value, pXmlElement, pField->getName().c_str());

		return true;
	}

	virtual bool fromXmlOrDefaultValue(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		SerializeFieldFlag* pField = static_cast<SerializeFieldFlag*>(pFieldBase);

		uint value;
		if (!XmlUtil::getAttribute(value, pXmlElement, pField->getName().c_str()))
		{
			value = pField->getDefaultValue();
		}

		uint getValue = pField->getter();
		if (getValue != value)
		{
			return pField->setter(value);
		}

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
template<typename T>
class SerializeOp<std::vector<T>> : public ISerializeOp
{
public:
	typedef std::vector<T> CONTAINER;
	SerializeOp() :ISerializeOp(typeid(CONTAINER)) {};

	virtual bool fromXml(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		auto pField = static_cast<SerializeField<CONTAINER>*>(pFieldBase);

		CONTAINER value;
		if (!XmlUtil::getAttribute(value, pXmlElement, pField->getName().c_str())) return false;

		return pField->setter(value);
	}

	virtual bool toXml(ISerializeField* pFieldBase, tinyxml2::XMLElement* pXmlElement) override
	{
		auto pField = static_cast<SerializeField<CONTAINER>*>(pFieldBase);

		const CONTAINER& value = pField->getter();
		return XmlUtil::setAttribute(value, pXmlElement, pField->getName().c_str());
	}

	virtual bool fromXmlOrDefaultValue(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		if (!fromXml(pXmlElement, pFieldBase))
		{
			auto pField = static_cast<SerializeField<CONTAINER>*>(pFieldBase);
			return pField->setter(pField->getDefaultValue());
		}

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
template<typename T>
class SerializeOp<std::vector<std::vector<T>>> : public ISerializeOp
{
public:
	typedef std::vector<T> ELEMENT_TYPE;
	typedef std::vector<std::vector<T>> CONTAINER;
	SerializeOp() :ISerializeOp(typeid(CONTAINER)) {};

	virtual bool fromXml(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		auto pField = static_cast<SerializeField<CONTAINER>*>(pFieldBase);

		std::vector<tstring> vecString;
		if (!XmlUtil::getAttribute(vecString, pXmlElement, pField->getName().c_str())) return false;
		CONTAINER value(vecString.size());
		for (std::size_t i = 0; i < vecString.size(); ++i)
		{
			StringUtil::splitString(value[i], vecString[i], "-");
		}

		return pField->setter(value);
	}

	virtual bool toXml(ISerializeField* pFieldBase, tinyxml2::XMLElement* pXmlElement) override
	{
		auto pField = static_cast<SerializeField<CONTAINER>*>(pFieldBase);

		const CONTAINER& value = pField->getter();
		std::vector<tstring> vecString;
		for (const auto& element : value)
		{
			if (element.empty()) { continue; }

			tstring strCombine;
			for (const auto& item : element)
			{
				if (!strCombine.empty())
				{
					strCombine += '-';
				}
				strCombine += fmt::format("{0}", item);
			}
			vecString.push_back(strCombine);
		}
		return XmlUtil::setAttribute(vecString, pXmlElement, pField->getName().c_str());
	}

	virtual bool fromXmlOrDefaultValue(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		if (!fromXml(pXmlElement, pFieldBase))
		{
			auto pField = static_cast<SerializeField<CONTAINER>*>(pFieldBase);
			return pField->setter(pField->getDefaultValue());
		}

		return true;
	}
};

//////////////////////////////////////////////////////////////
class SerializeOpSeperate : public ISerializeOp
{
public:
	SerializeOpSeperate() :ISerializeOp(typeid(SerializeFieldSeparate)) {};
	virtual ~SerializeOpSeperate() {};

	virtual bool fromXml(tinyxml2::XMLElement* pXmlElement, ISerializeField* pFieldBase) override
	{
		return true;
	}

	virtual bool toXml(ISerializeField* pFieldBase, tinyxml2::XMLElement* pXmlElement) override
	{
		return true;
	}
};

NS_END
