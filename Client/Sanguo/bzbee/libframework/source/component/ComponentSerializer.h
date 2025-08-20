/*!
 * \file ComponentSerializer.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/11/2016
 *
 * 
 */
#pragma once

#include "Component.h"

NS_BEGIN

class ComponentSerializer : public Component
{
public:
	RTTI_DEF(ComponentSerializer);

	typedef std::function<Component*()> CallbackComponentCreater;

	class ComponentCreaterInfo
	{
	public:
		tstring m_name;
		tstring m_exportName;
		CallbackComponentCreater m_creater;
	};

	typedef std::unordered_map<tstring, ComponentCreaterInfo> ComponentCreaterInfoMap;
	typedef std::unordered_map<tstring, tstring> StringMap;

public:
	ComponentSerializer();
	virtual ~ComponentSerializer();

	bool registerComponentCreater(const bzbee::tstring& className, CallbackComponentCreater creater);
	bool registerComponentCreater(const bzbee::tstring& className, CallbackComponentCreater creater, const bzbee::tstring& modifyName);
	bool replaceComponentCreator(const bzbee::tstring& srcClassName, const bzbee::tstring& dstClassName);

	Component* loadFromXml(const tstring& filePath);
	Component* loadFromXml(const tstring& strXmlData, const tstring& dir);
	bool attachFromXml(Component* pComponent, const tstring& filePath);
	bool saveToXml(Component* pComponent, const tstring& filePath);

	virtual Component* createComponentRecrusive(const tstring& dir, tinyxml2::XMLElement* pXmlElement, Component* pComponent = nullptr);
	
	void reset();

	bool isRegisterComponentCreater(const tstring& className) const;
	Component* create(const tstring& className) const;

protected:
	virtual bool saveComponentRecrusive(const tstring& dir, Component* pComponent, tinyxml2::XMLNode* pXmlParent);
	Component* createComponent(const tstring& dir, tinyxml2::XMLElement* pXmlElement, Component* pComponent);

protected:
	ComponentCreaterInfoMap m_componentCreaterMap;

private:
	StringMap m_mappingComponent;

};

NS_END

