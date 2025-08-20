/*!
 * \file ComponentSerializer.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/11/2016
 *
 * 
 */
#include "ComponentSerializer.h"
#include <XmlUtil.h>
#include <LogUtil.h>

NS_BEGIN

ComponentSerializer::ComponentSerializer()
{

}

ComponentSerializer::~ComponentSerializer()
{

}

bool ComponentSerializer::registerComponentCreater(const tstring& className, CallbackComponentCreater creater)
{
	return registerComponentCreater(className, creater, className);
}

bool ComponentSerializer::registerComponentCreater(const bzbee::tstring& className, CallbackComponentCreater creater, const bzbee::tstring& exportName)
{
	if (m_componentCreaterMap.find(className) != m_componentCreaterMap.end()) return false;

	ComponentCreaterInfo info{ className, exportName, creater };
	m_componentCreaterMap.insert(std::make_pair(className, info));

	return true;
}

bool ComponentSerializer::replaceComponentCreator(const tstring& srcClassName, const tstring& dstClassName)
{
	m_mappingComponent[srcClassName] = dstClassName;
	return true;
}

Component* ComponentSerializer::loadFromXml(const tstring& filePath)
{
	tinyxml2::XMLDocument doc;
	if (!XmlUtil::loadXmlFile(doc, filePath)) return nullptr;

	tstring dir = FileUtil::getFileDirectory(filePath);
	tinyxml2::XMLElement* pXmlElement = doc.RootElement();
	return createComponentRecrusive(dir, pXmlElement);
}

Component* ComponentSerializer::loadFromXml(const tstring& strXmlData, const tstring& dir)
{
	tinyxml2::XMLDocument doc;

	if (doc.Parse(strXmlData.c_str()) != tinyxml2::XML_SUCCESS)
	{
		LOGE(("parse xml failed, data={0}, error={1}"), strXmlData, doc.ErrorName());
		return nullptr;
	}

	tinyxml2::XMLElement* pXmlElement = doc.RootElement();
	return createComponentRecrusive(dir, pXmlElement);
}

bool ComponentSerializer::attachFromXml(Component* pComponent, const tstring& filePath)
{
	tinyxml2::XMLDocument doc;
	if (!XmlUtil::loadXmlFile(doc, filePath)) return false;

	tstring dir = FileUtil::getFileDirectory(filePath);
	tinyxml2::XMLElement* pXmlElement = doc.RootElement();
	if (!createComponentRecrusive(dir, pXmlElement, pComponent)) return false;

	return true;
}

bool ComponentSerializer::saveToXml(Component* pComponent, const tstring& filePath)
{
	tinyxml2::XMLDocument doc;

	tstring dir = FileUtil::getFileDirectory(filePath);
	if (!saveComponentRecrusive(dir, pComponent, &doc)) return false;

	return (XmlUtil::saveXmlFile(doc, filePath));
}

void ComponentSerializer::reset()
{
	m_componentCreaterMap.clear();
	m_mappingComponent.clear();
}

Component* ComponentSerializer::createComponentRecrusive(const tstring& dir, tinyxml2::XMLElement* pXmlElement, Component* pComponent /* = nullptr */)
{
	pComponent = createComponent(dir, pXmlElement, pComponent);
	if (!pComponent) return nullptr;

	for (tinyxml2::XMLElement* pXmlSubElement = pXmlElement->FirstChildElement(); pXmlSubElement != nullptr; pXmlSubElement = pXmlSubElement->NextSiblingElement())
	{
		Component* pSubComponent = createComponentRecrusive(dir, pXmlSubElement, nullptr);
		if (!pSubComponent)
		{
			continue;
		}

		if (!pComponent->addComponent(pSubComponent))
		{
			// do not delete pSubComponent, because it is deleted in the pComponent->addComponent
			SAFE_DELETE_AND_TERMINATE(pComponent);
			return nullptr;
		}
	}

	return pComponent;
}

Component* ComponentSerializer::createComponent(const tstring& dir, tinyxml2::XMLElement* pXmlElement, Component* pComponent)
{
	tstring className = pXmlElement->Value();
	bool freeComponent = false;

	if (!pComponent)
	{
		StringMap::iterator itMapping = m_mappingComponent.find(className);
		if (itMapping != m_mappingComponent.end())
		{
			LOGW("mapping component {0} to {1}", className, itMapping->second);
			className = itMapping->second;
		}

		ComponentCreaterInfoMap::iterator it = m_componentCreaterMap.find(className);
		if (it == m_componentCreaterMap.end())
		{
			LOGE("can not parse component of {0}", className);
			return nullptr;
		}

		pComponent = it->second.m_creater();
		if (!pComponent) return nullptr;
		freeComponent = true;
	}

	// read component name
	tstring compName;
	if (XmlUtil::getAttribute(compName, pXmlElement, "comp_name"))
	{
		pComponent->setName(compName);
	}

	if (!pComponent->fromXml(pXmlElement))
	{
		LOGE("parse component attributes failed {0}", className);
		if (freeComponent) SAFE_DELETE(pComponent);
		return nullptr;
	}

	pComponent->onFromXmlFinalize(dir, pXmlElement);

	return pComponent;
}

bool ComponentSerializer::isRegisterComponentCreater(const tstring& className) const
{
	auto it = m_componentCreaterMap.find(className);
	if (it == m_componentCreaterMap.end())
	{
		return false;
	}

	return true;
}

Component* ComponentSerializer::create(const tstring& className) const
{
	auto it = m_componentCreaterMap.find(className);
	if (it == m_componentCreaterMap.end())
	{
		LOGE("can not parse component of {0}", className);
		return nullptr;
	}

	return it->second.m_creater();
}

bool ComponentSerializer::saveComponentRecrusive(const tstring& dir, Component* pComponent, tinyxml2::XMLNode* pXmlParent)
{
	const tstring& className = pComponent->getRtti()->getClassName();
	const auto& found = m_componentCreaterMap.find(className);
	if (found == m_componentCreaterMap.end())
	{
		LOGW("the component {0} not registed, the component loader may not load correctly for unregisted components", className);
		return false;
	}

	const auto& createInfo = found->second;
	tinyxml2::XMLElement* pXmlElement = pXmlParent->GetDocument()->NewElement(createInfo.m_exportName.c_str());

	// save component name
	auto&& compName = pComponent->getName();
	if (!compName.empty())
	{
		XmlUtil::setAttribute(compName, pXmlElement, "comp_name");
	}

	if (!pComponent->toXml(pXmlElement)) return false;
	pXmlParent->LinkEndChild(pXmlElement);

	const Component::ComponentList& componentList = pComponent->getComponentList();
	for (auto pSubComponent : componentList)
	{
		saveComponentRecrusive(dir, pSubComponent, pXmlElement);
	}

	pComponent->onToXmlFinalize(dir);

	return true;
}

NS_END
