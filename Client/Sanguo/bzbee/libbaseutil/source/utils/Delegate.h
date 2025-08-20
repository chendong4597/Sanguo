/*!
 * \file Delegate.h
 *
 * \author mattho
 * \date 2017/11/29
 *
 *
 */
#pragma once

#include <BaseType.h>
#include <functional>

/** Implements lightweight delegate/ multicast-delegate mechanism by wrapping std::function. */
NS_BEGIN

//////////////////////////////////////////////////////////////////////////
class IDelegateInstance
{
protected:
	template<typename TSignature>
	class FunctionPointerTrait;

	template<typename TReturn, typename... TParamTypes>
	class FunctionPointerTrait<TReturn(TParamTypes...)>
	{
	public:
		typedef TReturn TReturnType;
		typedef TReturnType(*Type)(TParamTypes...);
	};

	template<bool bConstant, class TClass, typename TSignature>
	class MemberFunctionPointerTrait;

	template<class TClass, typename TReturn, typename... TParamTypes>
	class MemberFunctionPointerTrait<false, TClass, TReturn(TParamTypes...)>
	{
	public:
		typedef TReturn(TClass::*Type)(TParamTypes...);
	};

	template<class TClass, typename TReturn, typename... TParamTypes>
	class MemberFunctionPointerTrait<true, TClass, TReturn(TParamTypes...)>
	{
	public:
		typedef TReturn(TClass::*Type)(TParamTypes...) const;
	};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename TScope, typename THandle = uint32>
class HandleGenerator
{
public:
	HandleGenerator() = delete;

	static FORCEINLINE THandle create()
	{
		return s_generator++;
	}

private:
	static THandle s_generator;
};

template<typename TScope, typename THandle>
THandle		HandleGenerator<TScope, THandle>::s_generator;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename TSignature> class Delegate;
template<typename TSignature> class EventDelegate;
template<typename TDelegate> class MulticastDelegate;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename TReturn, typename... TParamTypes>
class Delegate<TReturn(TParamTypes...)> : public IDelegateInstance
{
public:
	Delegate() = default;
	Delegate(Delegate&& inOther)
		: m_method(std::move(inOther.m_method))
	{
		//inOther.m_method = nullptr;
	}
	Delegate(const Delegate& inOther)
		: m_method(inOther.m_method)
	{ }

	Delegate(std::function<TReturn(TParamTypes...)>&& inFunction)
		: m_method(std::move(inFunction))
	{ }
	Delegate(const std::function<TReturn(TParamTypes...)>& inFunction)
		: m_method(inFunction)
	{ }

	FORCEINLINE void swap(Delegate& inOther)
	{
		m_method.swap(inOther.m_method);
	}

	/** Invokes the stored callable function target with the parameters args. */
	FORCEINLINE TReturn operator()(TParamTypes... inArgs) const
	{
		return m_method(std::forward<TParamTypes>(inArgs)...);
	}

	FORCEINLINE explicit operator bool() const
	{
		return !!m_method;
	}

	FORCEINLINE Delegate& operator=(Delegate&& inOther)
	{
		m_method = std::move(inOther.m_method);
		inOther.m_method = nullptr;
		return *this;
	}

	FORCEINLINE Delegate& operator=(const Delegate& inOther)
	{
		m_method = inOther.m_method;
		return *this;
	}

public:
	template<typename... TPayloadTypes>
	FORCEINLINE void bindStatic(TReturn(*inFunction)(TParamTypes..., TPayloadTypes...), TPayloadTypes... inPayloads)
	{
		m_method = [=](TParamTypes... inArgs)->TReturn
		{
			return inFunction(std::forward<TParamTypes>(inArgs)..., inPayloads...);
		};
	}

	template<typename... TPayloadTypes, class TClass>
	FORCEINLINE void bindMethod(TClass* inObject, TReturn(TClass::*inMethod)(TParamTypes..., TPayloadTypes...), TPayloadTypes... inPayloads)
	{
		ASSERT(inObject != nullptr);
		m_method = [=](TParamTypes... inArgs)->TReturn
		{
			return (inObject->*inMethod)(std::forward<TParamTypes>(inArgs)..., inPayloads...);
		};
	}

	template<typename... TPayloadTypes, class TClass>
	FORCEINLINE void bindMethod(TClass const* inObject, TReturn(TClass::*inMethod)(TParamTypes..., TPayloadTypes...) const, TPayloadTypes... inPayloads)
	{
		ASSERT(inObject != nullptr);
		m_method = [=](TParamTypes... inArgs)->TReturn
		{
			return (inObject->*inMethod)(std::forward<TParamTypes>(inArgs)..., inPayloads...);
		};
	}

	template<typename TFunctor>
	FORCEINLINE void bindFunctor(TFunctor&& inFunctor)
	{
		m_method = std::forward<TFunctor>(inFunctor);
	}

public:
	template<typename... TPayloadTypes>
	static FORCEINLINE Delegate createStatic(TReturn(*inFunction)(TParamTypes..., TPayloadTypes...), TPayloadTypes... inPayloads)
	{
		Delegate result;
		result.bindStatic(inFunction, std::forward<TPayloadTypes>(inPayloads)...);
		return result;
	}

	template<typename... TPayloadTypes, class TClass>
	static FORCEINLINE Delegate createMethod(TClass* inObject, TReturn(TClass::*inMethod)(TParamTypes..., TPayloadTypes...), TPayloadTypes... inPayloads)
	{
		ASSERT(inObject != nullptr);
		Delegate result;
		result.bindMethod(inObject, inMethod, std::forward<TPayloadTypes>(inPayloads)...);
		return result;
	}

	template<typename... TPayloadTypes, class TClass>
	static FORCEINLINE Delegate createMethod(TClass const* inObject, TReturn(TClass::*inMethod)(TParamTypes..., TPayloadTypes...) const, TPayloadTypes... inPayloads)
	{
		ASSERT(inObject != nullptr);
		Delegate result;
		result.bindMethod(inObject, inMethod, std::forward<TPayloadTypes>(inPayloads)...);
		return result;
	}

	template<typename TFunctor>
	static FORCEINLINE Delegate createFunctor(TFunctor&& inFunctor)
	{
		Delegate result;
		result.bindFunctor(std::forward<TFunctor>(inFunctor));
		return result;
	}

protected:
	friend class MulticastDelegate<Delegate<TReturn(TParamTypes...)>>;

	enum class EStaticType { Static };
	enum class EMethodType { Method };
	enum class EFunctorType { Functor };

	template<typename... TPayloadTypes>
	Delegate(EStaticType, TReturn(*inFunction)(TParamTypes..., TPayloadTypes...), TPayloadTypes... inPayloads)
	{
		bindStatic(inFunction, std::forward<TPayloadTypes>(inPayloads)...);
	}

	template<typename... TPayloadTypes, class TClass>
	Delegate(EMethodType, TClass* inObject, TReturn(TClass::*inMethod)(TParamTypes..., TPayloadTypes...), TPayloadTypes... inPayloads)
	{
		bindMethod(inObject, inMethod, std::forward<TPayloadTypes>(inPayloads)...);
	}

	template<typename... TPayloadTypes, class TClass>
	Delegate(EMethodType, TClass const* inObject, TReturn(TClass::*inMethod)(TParamTypes..., TPayloadTypes...) const, TPayloadTypes... inPayloads)
	{
		bindMethod(inObject, inMethod, std::forward<TPayloadTypes>(inPayloads)...);
	}

	template<typename TFunctor>
	Delegate(EFunctorType, TFunctor&& inFunctor)
	{
		bindFunctor(std::forward<TFunctor>(inFunctor));
	}

private:
	std::function<TReturn(TParamTypes...)>		m_method;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename... TParamTypes>
class MulticastDelegate<Delegate<void(TParamTypes...)> > : public IDelegateInstance
{
public:
	typedef Delegate<void(TParamTypes...)> DelegateType;
	typedef uint32 IndexerType;

public:
	MulticastDelegate() = default;
	MulticastDelegate(MulticastDelegate&& inOther)
	{}

	MulticastDelegate(const MulticastDelegate&) = delete;
	MulticastDelegate& operator=(const MulticastDelegate&) = delete;

	FORCEINLINE explicit operator bool() const
	{
		return !m_listeners.empty();
	}

	void operator()(TParamTypes... inArgs) const
	{
		for (uint32 i = 0; i < m_listeners.size(); ++i)
		{
			m_listeners[i].second(std::forward<TParamTypes>(inArgs)...);
		}
	}

public:
	template<typename... TPayloadTypes>
	FORCEINLINE IndexerType addStatic(void(*inFunction)(TParamTypes..., TPayloadTypes...), TPayloadTypes... inPayloads)
	{
		return addDelegate(DelegateType(DelegateType::EStaticType::Static, std::forward(inFunction), std::forward<TPayloadTypes>(inPayloads)...));
	}

	template<typename... TPayloadTypes, class TClass>
	FORCEINLINE IndexerType addMethod(TClass* inObject, void(TClass::*inMethod)(TParamTypes..., TPayloadTypes...), TPayloadTypes... inPayloads)
	{
		return addDelegate(DelegateType(DelegateType::EMethodType::Method, inObject, inMethod, std::forward<TPayloadTypes>(inPayloads)...));
	}

	template<typename... TPayloadTypes, class TClass>
	FORCEINLINE IndexerType addMethod(TClass const* inObject, void(TClass::*inMethod)(TParamTypes..., TPayloadTypes...) const, TPayloadTypes... inPayloads)
	{
		return addDelegate(DelegateType(DelegateType::EMethodType::Method, inObject, inMethod, std::forward<TPayloadTypes>(inPayloads)...));
	}

	template<typename TFunctor>
	FORCEINLINE IndexerType addFunctor(TFunctor&& inFunctor)
	{
		return addDelegate(DelegateType(DelegateType::EFunctorType::Functor, std::forward<TFunctor>(inFunctor)));
	}

	FORCEINLINE void remove(IndexerType inUniqueId)
	{
		const auto end_iter = m_listeners.end();
		auto found_iter = std::find_if(m_listeners.begin(), end_iter,
			[=](typename decltype(m_listeners)::value_type const& item) { return (item.first == inUniqueId); });
		if (found_iter != end_iter)
		{
			m_listeners.erase(found_iter);
		}
	}

private:
	FORCEINLINE uint32 addDelegate(DelegateType&& inDelegate)
	{
		const IndexerType uniqueId = HandleGenerator<IDelegateInstance, uint32>::create();
		{
			m_listeners.emplace_back(typename decltype(m_listeners)::value_type(uniqueId, std::move(inDelegate)));
		}
		return uniqueId;
	}

protected:
	std::vector<std::pair<IndexerType, DelegateType> > m_listeners;
};

template<typename TReturn, typename... TParamTypes>
class EventDelegate<TReturn(TParamTypes...)> : public MulticastDelegate<Delegate<void(TParamTypes...)>>
{
	typedef MulticastDelegate<Delegate<void(TParamTypes...)>>	Super;

public:
	EventDelegate() = default;
	EventDelegate(EventDelegate&& inOther)
		: Super(std::move(inOther))
	{}

	EventDelegate(const EventDelegate&) = delete;
	EventDelegate& operator=(const EventDelegate&) = delete;

	FORCEINLINE void broadcast(TParamTypes... inArgs) const
	{
		(*this)(std::forward<TParamTypes>(inArgs)...);
	}
};
//////////////////////////////////////////////////////////////////////////

NS_END

//////////////////////////////////////////////////////////////////////////
namespace std
{
	template<typename TReturn, typename... TParamTypes>
	FORCEINLINE void swap(bzbee::Delegate<TReturn(TParamTypes...)>& lhs, bzbee::Delegate<TReturn(TParamTypes...)>& rhs)
	{
		lhs.swap(rhs);
	}
}
//////////////////////////////////////////////////////////////////////////
