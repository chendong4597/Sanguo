/*!
 * \file Component.cpp
 *
 * \author zjhlogo
 * \date 06/03/2016
 *
 * 
 */
#include "Component.h"
#include "debugs/Debug.h"
#include <serialize/SerializeOp.h>
#include <LogUtil.h>

NS_BEGIN

uint Component::s_nextUniqueId = 10000;
Component::ComponentUniqueIdMap Component::s_componentUniqueIdMap;
Component::SerializeOperatorMap Component::m_serializeOpMap;

void Component::static_initialize()
{
	m_serializeOpMap.insert(std::make_pair(static_cast<std::type_index>(typeid(SerializeFieldFlag    )), new SerializeOpFlag    ));
	m_serializeOpMap.insert(std::make_pair(static_cast<std::type_index>(typeid(SerializeFieldSeparate)), new SerializeOpSeperate));
}

void Component::static_terminate()
{
	for (auto it : m_serializeOpMap) { SAFE_DELETE(it.second); }
	m_serializeOpMap.clear();
}

Component::Component()
{
	m_uniqueId = s_nextUniqueId++;
	s_componentUniqueIdMap.insert(std::make_pair(m_uniqueId, this));
}

Component::~Component()
{
	s_componentUniqueIdMap.erase(m_uniqueId);

    ComponentDestroyEvent evt;
    postEvent(evt);

    while (!m_componentList.empty())
    {
        Component* pComponent = m_componentList.back();
        pComponent->setCullTree(nullptr);
        SAFE_DELETE_AND_TERMINATE(pComponent);
        m_componentList.pop_back();
    }
    m_componentIdsDirty = true;
}

void Component::update(float dt)
{
	if (Debug::isFlagSet(Debug::disable_component_update))
	{
		return;
	}

    if (m_componentIdsDirty)
    {
        // sync component ids
        m_componentIds.clear();
        for (auto component : m_componentList)
        {
            m_componentIds.push_back(component->getUniqueId());
        }

        m_componentIdsDirty = false;
    }

    // using id to locate component
    for (auto id : m_componentIds)
    {
        auto it = s_componentUniqueIdMap.find(id);
        if (it == s_componentUniqueIdMap.end())
        {
            // the component is deleted from inside of update(), just skip it
            continue;
        }

        // TODO: sometimes component was removed but not deleted, we need double check
        // whether component still seat inside the m_componentList

        it->second->update(dt);
    }

	if (m_updateIvok) m_updateIvok(dt);
}

void Component::setCullTree(CullTree* pCullTree)
{
	if (m_pCullTree == pCullTree)
	{
		return;
	}

	for (auto&& pComponent : m_componentList)
	{
		pComponent->setCullTree(pCullTree);
	}

	m_pCullTree = pCullTree;
}

bool Component::addComponent(Component* pComponent, bool initialize /* = true */, uint index/*= UINT_MAX*/)
{
	if (pComponent->m_pEntity)
	{
		LOGE("addComponent {0} component failed", typeid(*pComponent).name());
		return false;
	}

	pComponent->m_pEntity = this;

	if (initialize && !pComponent->initialize(this))
	{
		LOGE("initialize {0} component failed", typeid(*pComponent).name());
		SAFE_DELETE_AND_TERMINATE(pComponent);
		return false;
	}

	if (index > m_componentList.size()) index = m_componentList.size();
	m_componentList.insert(m_componentList.begin() + index, pComponent);
    m_componentIdsDirty = true;
    pComponent->setCullTree(m_pCullTree);

	// notify parents entity that component added
	Component* pParentEntity = this;
	Component* pChild = pComponent;
	while (pParentEntity)
	{
		pParentEntity->notifyComponentTreeChanged(ComponentTreeUpdate::Added, pChild, pComponent);
		pChild = pParentEntity;
		pParentEntity = pParentEntity->m_pEntity;
	}

	return true;
}

bool Component::removeComponent(Component* pComponent, bool terminate /* = true */)
{
	const auto beginIter = m_componentList.begin();
	const auto endIter = m_componentList.end();

	const auto foundIter = std::find(beginIter, endIter, pComponent);
	if (foundIter == endIter) return false;

	// notify parents entity that component going to removed
	Component* pParentEntity = this;
	Component* pChild = pComponent;
	pComponent->setCullTree(nullptr);

	//LOGI("Removed Compoent Name: {0}", pComponent->getRtti()->getClassName());
	while (pParentEntity)
	{
		pParentEntity->notifyComponentTreeChanged(ComponentTreeUpdate::Removed, pChild, pComponent);
		pChild = pParentEntity;
		pParentEntity = pParentEntity->m_pEntity;
	}
	m_componentList.erase(foundIter);
    m_componentIdsDirty = true;

	if (terminate)
	{
		pComponent->terminate();
		pComponent->m_pEntity = nullptr;
		SAFE_DELETE(pComponent);
	}
	else
	{
		pComponent->m_pEntity = nullptr;
	}

	return true;
}

bool Component::removeAllComponent(bool bTerminate /*= true*/)
{
    if (m_componentIdsDirty)
    {
        // sync component ids
        m_componentIds.clear();
        for (auto component : m_componentList)
        {
            m_componentIds.push_back(component->getUniqueId());
        }

        m_componentIdsDirty = false;
    }

    // using id to locate component
    for (auto id : m_componentIds)
    {
        auto it = s_componentUniqueIdMap.find(id);
        if (it == s_componentUniqueIdMap.end())
        {
            continue;
        }

        auto pComponent = it->second;

        // notify parents entity that component going to removed
        Component* pParentEntity = this;
        Component* pChild = pComponent;
        pComponent->setCullTree(nullptr);
        while (pParentEntity)
        {
            pParentEntity->notifyComponentTreeChanged(ComponentTreeUpdate::Removed, pChild, pComponent);
            pChild = pParentEntity;
            pParentEntity = pParentEntity->m_pEntity;
        }

        if (bTerminate)
        {
            pComponent->terminate();
            pComponent->m_pEntity = nullptr;
            SAFE_DELETE(pComponent);
        }
        else
        {
            pComponent->m_pEntity = nullptr;
        }
    }

	m_componentList.clear();
    m_componentIds.clear();
    m_componentIdsDirty = false;

	return true;
}

bool Component::removeFromParent(bool bTerminate)
{
	auto* pParent = getEntity();
	if (!pParent) return false;
	return pParent->removeComponent(this, bTerminate);
}

bool Component::swapComponent(Component* pComponent1, Component* pComponent2)
{
	if (!pComponent1 || !pComponent2) return false;

	ComponentList::iterator it1 = m_componentList.end();
	ComponentList::iterator it2 = m_componentList.end();

	for (ComponentList::iterator it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		if (*it == pComponent1)
		{
			it1 = it;
		}
		else if (*it == pComponent2)
		{
			it2 = it;
		}

		if (it1 != m_componentList.end() && it2 != m_componentList.end())
		{
			*it1 = pComponent2;
			*it2 = pComponent1;

			return true;
		}
	}

    m_componentIdsDirty = true;

    return false;
}

int Component::getComponentIndex(Component* pComponent)
{
	for (int i = 0; i < (int)m_componentList.size(); ++i)
	{
		if (m_componentList[i] == pComponent)
		{
			return i;
		}
	}
	return -1;
}

Component* Component::getComponentByIndex(int index) const
{
	if (index < 0 || index >= (int)m_componentList.size()) return nullptr;
	return m_componentList[index];
}

Component* Component::getComponentByUniqueId(uint unUniqueId) const
{
	for (auto component : m_componentList)
	{
		if (component->getUniqueId() == unUniqueId)
		{
			return component;
		}
	}
	return nullptr;
}

bool Component::removeComponentByUniqueId(uint unUniqueId, bool terminate /*= true*/)
{
	Component* pComponent = getComponentByUniqueId(unUniqueId);
	if (pComponent)
	{
		return removeComponent(pComponent, terminate);
	}
	return false;
}

void Component::addFieldSeperate(const tstring & name, const FlagFieldAttribute * attr)
{
	getSerializeSource()->getRtti()->addField<FieldSeparateDefine>(m_serializeOpMap[typeid(SerializeFieldSeparate)], name, attr);
}

bool Component::fromXml(tinyxml2::XMLElement* pXmlElement) const
{
	for (auto field : getSerializeFieldList())
	{
		field.getOperator()->fromXml(pXmlElement, &field);
	}

	return true;
}

bool Component::toXml(tinyxml2::XMLElement* pXmlElement) const
{
	for (auto field : getSerializeFieldList())
	{
		if (!field.getOperator()->toXml(&field, pXmlElement)) return false;
	}

	return true;
}

Component* Component::findComp(const tstring& baseTypeName) const
{
	for (auto p : m_componentList)
	{
		if (p->getRtti()->isDerivedFrom(baseTypeName))
			return p;
	}
	return nullptr;
}

NS_END
