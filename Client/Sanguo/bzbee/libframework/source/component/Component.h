/*!
 * \file Component.h
 *
 * \author zjhlogo
 * \date 06/03/2016
 *
 * 
 */
#pragma once
#include <baseutil.h>
#include <Cpp11EventSystem.h>
#include "SerializeFieldList.h"
#include <lua/LuaCallback.h>

NS_BEGIN

class ComponentSerializer;
class ComponentDestroyEvent;
class CullTree;

class Component : public EventHandler
{
	RTTI_ROOT(Component);
	friend ComponentSerializer;

public:
	typedef std::vector<Component*> ComponentList;
	typedef std::unordered_map<uint, Component*> ComponentUniqueIdMap;

    typedef std::unordered_map<std::type_index, ISerializeOp*> SerializeOperatorMap;

	enum class ComponentTreeUpdate
	{
		Added,
		Removed,
	};

public:
	static void static_initialize(void);
	static void static_terminate (void);

	Component();
	virtual ~Component();

	template<typename T> inline bool isA() const { return this->getRtti()->isDerivedFrom(typeid(T)); }

	Component* METHOD(getEntity)(void) const { return m_pEntity; };
	tstring METHOD(getClassName)(void) const { return getRtti()->getClassName(); }

	virtual bool initialize(Component* pEntity) { return true; };
	virtual void terminate() {};
	virtual void update(float dt);

	virtual void setCullTree(CullTree* pCullTree);
	virtual CullTree* getCullTree() const { return m_pCullTree; }

	uint METHOD(getUniqueId)() const { return m_uniqueId; };

	virtual bool METHOD(swapComponent)(Component* pComponent1, Component* pComponent2);
	virtual bool METHOD(addComponent)(Component* pComponent, bool initialize = true, uint index = UINT_MAX);
	virtual bool METHOD(removeComponent)(Component* pComponent, bool terminate = true);
	virtual bool METHOD(removeComponentByUniqueId)(uint unUniqueId, bool terminate = true);
	virtual bool METHOD(removeAllComponent)(bool bTerminate = true);
	bool METHOD(removeFromParent)(bool bTerminate = true);

	int METHOD(getComponentIndex)(Component* pComponent);
	Component* METHOD(getComponentByIndex)(int index) const;
	Component* METHOD(getComponentByUniqueId)(uint unUniqueId) const;
	
	int METHOD(getNumComponent)() const { return static_cast<int>(m_componentList.size()); };

	Component* METHOD(findComp)(const tstring& baseTypeName) const;
	template <typename T> T* findComponent() const
	{
		std::type_index baseTypeIndex = typeid(T);

		for (auto component : m_componentList)
		{
			if (component && component->getRtti()->isDerivedFrom(baseTypeIndex)) return (T*)component;
		}

		return nullptr;
	}

	template <typename T> T* findComponentRecrusive() const
	{
		std::type_index baseTypeIndex = typeid(T);

		for (auto component : m_componentList)
		{			
			if (component->getRtti()->isDerivedFrom(baseTypeIndex))
			{
				return (T*)component;
			}

			T* childComp = component->findComponentRecrusive<T>();
			if (childComp && childComp->getRtti()->isDerivedFrom(baseTypeIndex))
			{
				return (T*)childComp;
			}
		}

		return nullptr;
	}

	template<class VectorType>
	int queryComponents(std::type_index baseTypeIndex, VectorType& componentsOut, bool recrusive) const
	{
		int totalCount = 0;
		for (auto component : m_componentList)
		{
			if (component->getRtti()->isDerivedFrom(baseTypeIndex))
			{				
				ASSERT(dynamic_cast<typename VectorType::value_type>(component));
				componentsOut.emplace_back((typename VectorType::value_type)(component));
				++totalCount;
			}

			if (recrusive)
			{
				totalCount += component->queryComponents<VectorType>(baseTypeIndex, componentsOut, recrusive);
			}
		}

		return totalCount;
	}

	template<typename T, typename AllocatorType, template<typename, typename> class VectorType>
	int queryComponents(VectorType<T*, AllocatorType>& componentsOut, bool recrusive) const
	{
		return queryComponents<VectorType<T*, AllocatorType>>(typeid(T), componentsOut, recrusive);
	}

    template<typename T> static T* getComponentFromUniqueId(uint uniqueId)
	{
		auto it = s_componentUniqueIdMap.find(uniqueId);
		if (it == s_componentUniqueIdMap.end()) return nullptr; 
		return dynamic_cast<T*>(it->second);
	}

	const ComponentList& METHOD(getComponentList)() const { return m_componentList; }

	inline SerializeFieldList getSerializeFieldList(void) const { return { const_cast<Component*>(getSerializeSource()) }; }
	virtual const Component* getSerializeSource(void) const noexcept { return this; }

	template<typename ClassT, typename SetterValueT, typename GetterT>
	void addField(const tstring& name, bool (ClassT::*setter)(SetterValueT), GetterT getter, const IFieldAttribute* attr = nullptr)
	{
		typedef FieldDefine<ClassT, SetterValueT, GetterT> Field;

		std::type_index classId(typeid(typename Field::Type));
		auto it = m_serializeOpMap.find(classId);
		if (it == m_serializeOpMap.end()) it = m_serializeOpMap.insert(std::make_pair(classId, new SerializeOp<typename Field::Type>)).first;

		auto&& pField = getSerializeSource()->getRtti()->addField<Field>(it->second, name, setter, getter, attr);
		if (pField)
		{
			pField->setDefaultValue(this);
		}
	}

	template<typename ClassT>
	void addFieldFlag(const tstring& name, bool (ClassT::*setter)(uint), bool (ClassT::*cleaner)(uint), uint(ClassT::*getter)(void) const, const FlagFieldAttribute* attr = nullptr)
	{
		auto&& pField = getSerializeSource()->getRtti()->addField<FieldFlagDefine<ClassT>>(m_serializeOpMap[typeid(SerializeFieldFlag)], name, setter, cleaner, getter, attr);
		if (pField)
		{
			pField->setDefaultValue(this);
		}
	}

	void addFieldSeperate(const tstring& name, const FlagFieldAttribute* attr = nullptr);

	bool fromXml(tinyxml2::XMLElement* pXmlElement) const;
	bool toXml(tinyxml2::XMLElement* pXmlElement) const;
	virtual bool onFromXmlFinalize(const tstring& dir, tinyxml2::XMLElement* pXmlElement) { return true; }
	virtual bool onToXmlFinalize  (const tstring& dir) { return true; }

	const tstring& METHOD(getName)() const { return m_name; }
	void METHOD(setName)(const tstring& name) { m_name = name; }

	void METHOD(register_update)(const LuaCallback& invoker) {m_updateIvok = invoker;}

protected:
	virtual void notifyComponentTreeChanged(ComponentTreeUpdate updateType, Component* pChild, Component* pInstance) {};

protected:
	ComponentList m_componentList;
	CullTree* m_pCullTree{};

private:
	uint m_uniqueId{};
	Component* m_pEntity{};

	tstring m_name;

    std::vector<uint> m_componentIds;
    bool m_componentIdsDirty{ true };

	static uint s_nextUniqueId;
	static ComponentUniqueIdMap s_componentUniqueIdMap;

	LuaCallback m_updateIvok{};

    static SerializeOperatorMap m_serializeOpMap;

};


class ComponentDestroyEvent final : public Event { RTTI_DEF(ComponentDestroyEvent); };
template<typename T> class ComponentPtr final
{
	DECLARE_EVENT_HOST();

public:
	ComponentPtr(void) = default;
	ComponentPtr(const ComponentPtr& p) : ComponentPtr(p.m_p) {}
	ComponentPtr(T* p) { operator= (p); }

	inline ComponentPtr& operator= (const ComponentPtr& p) { return operator= (p.m_p); }
	inline ComponentPtr& operator= (T* p)
	{
		if (m_p != p)
		{
            if (m_p)
            {
                unreg((NS::EventHandler*)m_p);
            }

            m_p = p;

            if (m_p)
            {
                reg((NS::EventHandler*)m_p);
            }
		}
		return *this;
	}

public:
	inline T* get        (void) const noexcept { return  m_p; }
	inline operator T*   (void) const noexcept { return  m_p; }
	inline T* operator-> (void) const noexcept { return  m_p; }
	inline T& operator*  (void) const noexcept { return *m_p; }

private:
	void   reg(EventHandler* p) {   REG_EVENT(p) [this](const ComponentDestroyEvent&) { m_p = nullptr; }; }
	void unreg(EventHandler* p) { UNREG_EVENT(p, ComponentDestroyEvent); }

private:
	T* m_p{};
};
template<typename T> inline ComponentPtr<T> make_ptr(T* ptr) { return ComponentPtr<T>(ptr); }


//////////////////////////////////////////////////////////////////////////


class Exception_Natural {};
class Exception_Error   {};


//////////////////////////////////////////////////////////////////////////


template <typename T, typename ExceptionT>
class Validator_Component
{
public:
	Validator_Component(T* ptr) : m_ptr(ptr) {}

	inline float operator() (float f) const
	{
		validate();
		return f;
	}

protected:
	inline void validate(void) const { if (!m_ptr) throw ExceptionT(); }

protected:
	const ComponentPtr<T> m_ptr;
};


template <typename T, typename ExceptionT = Exception_Error>
inline Validator_Component<T, ExceptionT> check_component(T* ptr)
{
	return Validator_Component<T, ExceptionT>(ptr);
}

template <typename T, typename ExceptionT = Exception_Error>
inline Validator_Component<T, ExceptionT> check_component(const ComponentPtr<T>& ptr)
{
	return Validator_Component<T, ExceptionT>(ptr.get());
}

////////////////////////////////////////
template<typename T> class ComponentPtrUseId final
{
public:
	ComponentPtrUseId(void) = default;
	ComponentPtrUseId(const ComponentPtrUseId& p) : ComponentPtrUseId(p.get()) {}
	ComponentPtrUseId(T* p) { operator= (p); }

	inline ComponentPtrUseId& operator= (const ComponentPtrUseId& p) { return operator= (p.get()); }
	inline ComponentPtrUseId& operator= (T* p)
	{
		m_p = p;
		if (m_p)
		{
			m_compId = m_p->getUniqueId();
		}
		else
		{
			m_compId = 0;
		}

		return *this;
	}

public:
	inline T* get(void) const noexcept 
	{
		if (m_compId != 0)
		{
			return  NS::Component::getComponentFromUniqueId<T>(m_compId);
		}
		
		return nullptr;
	}

	inline operator T* const (void) noexcept 
	{
		return  get();
	}

	inline T* operator-> (void) const noexcept { return  m_p; }
	inline T& operator*  (void) const noexcept { return *m_p; }

private:

	NS::uint m_compId{0};
	T* m_p{};
};


NS_END


namespace LPCD
{
	// ComponentPtr
	template<typename T> struct Type<NS::ComponentPtr<T>>
	{
		static void Push(lua_State* L, const NS::ComponentPtr<T>& value)
		{
			Type<T*>::Push(L, value.get());
		}

		static bool Match(lua_State* L, int idx)
		{
			int b = lua_istable(L, idx);
			if (!b) return false;

			lua_getfield(L, idx, "__object");
			b = lua_islightuserdata(L, -1);
			lua_pop(L, 1);
			if (!b) return false;

			return true;
		}

		static T* Get(lua_State* L, int idx)
		{
			lua_getfield(L, idx, "__object");
			auto ret = make_ptr(reinterpret_cast<T*>(lua_touserdata(L, -1)));
			lua_pop(L, 1);

			return ret;
		}
	};
	template<typename T> struct Type<      NS::ComponentPtr<T>&> : public Type<NS::ComponentPtr<T>> {};
	template<typename T> struct Type<const NS::ComponentPtr<T>&> : public Type<NS::ComponentPtr<T>> {};
}

