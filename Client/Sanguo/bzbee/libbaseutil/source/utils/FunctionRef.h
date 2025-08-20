/*!
 * \file FunctionRef.h
 *
 * \author mattho
 * \date 2018/01/02
 *
 *
 */
#pragma once

#include <BaseType.h>
#include <type_traits>
#include <functional>

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
namespace detail
{
	/** Tests if a From* is convertible to a To*. */
	template <typename From, typename To>
	struct PointerIsConvertible
	{
	private:
		static uint8  test(...);
		static uint16 test(To*);

	public:
		enum { value = sizeof(test((From*)nullptr)) - 1 };
	};

	template <typename BaseType, typename CallableType>
	FORCEINLINE auto dereferenceIfNecessary(CallableType&& inCallable)
		-> typename std::enable_if<PointerIsConvertible<typename std::decay<CallableType>::type, typename std::decay<BaseType>::type>::value, decltype((CallableType&&)inCallable)>::type
	{
		return (CallableType&&)inCallable;
	}
	template <typename BaseType, typename CallableType>
	FORCEINLINE auto dereferenceIfNecessary(CallableType&& inCallable)
		-> typename std::enable_if<!PointerIsConvertible<typename std::decay<CallableType>::type, typename std::decay<BaseType>::type>::value, decltype(*(CallableType&&)inCallable)>::type
	{
		return *(CallableType&&)inCallable;
	}

	/** Calling a functor object given a set of arguments. */
	template<typename Functor, typename... ParamTypes>
	FORCEINLINE auto invoke(Functor&& inFunc, ParamTypes&&... inArgs)
		-> decltype(std::forward<Functor>(inFunc)(std::forward<ParamTypes>(inArgs)...))
	{
		return std::forward<Functor>(inFunc)(std::forward<ParamTypes>(inArgs)...);
	}
	/** Calling a function pointer given a set of arguments. */
	template<typename ReturnType, typename ObjectType, typename CallableType>
	FORCEINLINE auto invoke(ReturnType ObjectType::*inFunc, CallableType&& inCallable)
		-> decltype(dereferenceIfNecessary<ObjectType>(std::forward<CallableType>(inCallable)).*inFunc)
	{
		return dereferenceIfNecessary<ObjectType>(std::forward<CallableType>(inCallable)).*inFunc;
	}

	/** Calling a member function given a reference to an object and a set of arguments. */
	template <typename ReturnType, typename ObjectType, typename... ArgTypes, typename CallableType, typename... ParamTypes>
	FORCEINLINE auto invoke(ReturnType (ObjectType::*inFunc)(ArgTypes...), CallableType&& inCallable, ParamTypes&&... inArgs)
		-> decltype((dereferenceIfNecessary<ObjectType>(std::forward<CallableType>(inCallable)).*inFunc)(std::forward<ParamTypes>(inArgs)...))
	{
		return (dereferenceIfNecessary<ObjectType>(std::forward<CallableType>(inCallable)).*inFunc)(std::forward<ParamTypes>(inArgs)...);
	}
	/** Calling a member function given a pointer (including smart pointers) to an object and a set of arguments. */
	template <typename ReturnType, typename ObjectType, typename... ArgTypes, typename CallableType, typename... ParamTypes>
	FORCEINLINE auto invoke(ReturnType (ObjectType::*inFunc)(ArgTypes...) const, CallableType&& inCallable, ParamTypes&&... inArgs)
		-> decltype((dereferenceIfNecessary<ObjectType>(std::forward<CallableType>(inCallable)).*inFunc)(std::forward<ParamTypes>(inArgs)...))
	{
		return (dereferenceIfNecessary<ObjectType>(std::forward<CallableType>(inCallable)).*inFunc)(std::forward<ParamTypes>(inArgs)...);
	}

	template<typename Functor, typename TSignature>
	class FunctionRefCaller;

	template<typename Functor, typename ReturnType, typename... ParamTypes>
	class FunctionRefCaller<Functor, ReturnType(ParamTypes...)>
	{
	public:
		static ReturnType call(void* inTarget, ParamTypes&&... inArgs)
		{
			return detail::invoke(*(Functor*)inTarget, std::forward<ParamTypes>(inArgs)...);
		}
	};

	template<typename Functor, typename... ParamTypes>
	class FunctionRefCaller<Functor, void(ParamTypes...)>
	{
	public:
		static void call(void* inTarget, ParamTypes&&... inArgs)
		{
			detail::invoke(*(Functor*)inTarget, std::forward<ParamTypes>(inArgs)...);
		}
	};

	template<typename DerivedType, typename TSignature>
	class FunctionRefBase;

	template<typename DerivedType, typename ReturnType, typename... ParamTypes>
	class FunctionRefBase<DerivedType, ReturnType(ParamTypes...)>
	{
	public:
		FORCEINLINE explicit FunctionRefBase()
		{ }

		FORCEINLINE ReturnType operator()(ParamTypes... inArgs) const
		{
			const DerivedType* inDerived = static_cast<const DerivedType*>(this);
			return m_callable(inDerived->get(), std::forward<ParamTypes>(inArgs)...);
		}

	protected:
		FORCEINLINE void copyAndReseat(const FunctionRefBase& inOther, void* inFunctor)
		{
			m_callable = inOther.m_callable;

#ifndef NDEBUG
#endif
		}

		template<typename TFunctor>
		FORCEINLINE void set(TFunctor* inFunctor)
		{
			m_callable = &FunctionRefCaller<TFunctor, ReturnType(ParamTypes...)>::call;

#ifndef NDEBUG
#endif
		}

	private:
		/** A pointer to a function which invokes the call operator on the callable object */
		ReturnType (*m_callable)(void*, ParamTypes&&...);
	};
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/**
 * A class which represents a reference to something callable.  The important part here is *reference* - if
 * you bind it to a lambda and the lambda goes out of scope, you will be left with an invalid reference.
 *
 * If you also want to take ownership of the callable thing, e.g. you want to return a lambda from a
 * function, you should use std::function or Delegate.  FunctionRef does not concern itself with
 * ownership because it's intended to be FAST.
 *
 * FunctionRef is most useful when you want to parameterize a function with some caller-defined code
 * without making it a template.
 *
 * Example:
 * // Something.h
 * void doSomethingWithConvertingStringsToInts(FunctionRef<int32(const tstring&)> inConvert);
 *
 * // Something.cpp
 * void doSomethingWithConvertingStringsToInts(FunctionRef<int32(const tstring&)> inConvert)
 * {
 *     for (const tstring& str : SomeBunchOfStrings)
 *     {
 *         int32 value = inConvert(str);
 *         doSomething(value);
 *     }
 * }
 *
 * // SomewhereElse.cpp
 * #include "Something.h"
 *
 * void main()
 * {
 *     // First do something using string length
 *     doSomethingWithConvertingStringsToInts([](const tstring& inStr) {
 *         return inStr.Len();
 *     });
 *
 *     // Then do something using string conversion
 *     doSomethingWithConvertingStringsToInts([](const tstring& inStr) {
 *         int32 result;
 *         TypeFromString<int32>::fromString(result, inStr);
 *         return result;
 *     });
 * }
 *
 */
template<typename TSignature>
class FunctionRef : public detail::FunctionRefBase<FunctionRef<TSignature>, TSignature>
{
	typedef detail::FunctionRefBase<FunctionRef<TSignature>, TSignature>	Super;
	friend class detail::FunctionRefBase<FunctionRef<TSignature>, TSignature>;

public:
	/** Constructor which binds a FunctionRef to a non-const lvalue function object. */
	template<typename TFunctor,
		typename = typename std::enable_if<!std::is_function<TFunctor>::value && !std::is_same<FunctionRef, TFunctor>::value>::type>
	FORCEINLINE FunctionRef(TFunctor& inFunctor) : Super()
	{
		set(&inFunctor);
	}

	/** Constructor which binds a FunctionRef to an rvalue or const lvalue function object. */
	template<typename TFunctor,
		typename = typename std::enable_if<!std::is_function<TFunctor>::value && !std::is_same<FunctionRef, TFunctor>::value>::type>
	FORCEINLINE FunctionRef(const TFunctor& inFunctor) : Super()
	{
		set(&inFunctor);
	}

	/** Constructor which binds a FunctionRef to a function pointer. */
	template<typename TFunctor,
		typename = typename std::enable_if<std::is_function<TFunctor>::value>::type>
	FORCEINLINE FunctionRef(TFunctor* inFunctor) : Super()
	{
		set(inFunctor);
	}

	// Does compiler has default functions?
#if !defined(_MSC_VER) || _MSC_VER >= 1900
	FunctionRef(const FunctionRef&) = default;
	~FunctionRef() = default;

	FunctionRef& operator=(const FunctionRef&) const = delete;

#else
private:
	FunctionRef& operator=(const FunctionRef&) const;
#endif

private:
	FORCEINLINE void copyAndReseat(const FunctionRef& inOther, void* inFunctor)
	{
		m_ptr = inFunctor;
		Super::copyAndReseat(inOther, inFunctor);
	}

	template<typename TFunctor>
	FORCEINLINE void set(TFunctor* inFunctor)
	{
		m_ptr = (void*)inFunctor;
		Super::set(inFunctor);
	}

	FORCEINLINE void* get() const
	{
		return m_ptr;
	}

private:
	/** A pointer to the callable object */
	void*	m_ptr;
};
//////////////////////////////////////////////////////////////////////////

NS_END
