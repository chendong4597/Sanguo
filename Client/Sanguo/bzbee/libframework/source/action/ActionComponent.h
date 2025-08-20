/*!
* \file ActionComponent.h
*
* \author zhichenxia
* \date 2017/8/16
*
*
*/
#pragma once
#include "../component/Component.h"
#include <functional>
#include <type_traits>

NS_BEGIN

class ActionComponent final : public Component	// Wrapper.
{
public:
	RTTI_DEF(ActionComponent);

	virtual void update(float dt) override;

public:
	std::function<void(float)> onUpdate;

	enum class EventType
	{
		End,
		Break,
		Error,
	};
	std::function<void(EventType)> onActionEvent;
	std::function<void(void)> onStop;

	void METHOD(stop)() { remove(EventType::Break); }
	bool METHOD(isActive)() const { return m_active; }

private:
	void remove(EventType eventType);

private:
	bool m_active{ true };
};
typedef ComponentPtr<ActionComponent> ActionPtr;


class Accumulation final
{
public:
	float operator() (float f) { return (m_accumulation += f); }

private:
	float m_accumulation{};
};


template <typename ExceptionT>
class Validator_Time final
{
public:
	Validator_Time(float maxValue) : m_maxValue(maxValue) {}

	float operator() (float f)
	{
		if (f >= m_maxValue)
		{
			if (!m_lastFrame)
			{
				m_lastFrame = true;
				return m_maxValue;
			}
			else
			{
				throw ExceptionT();
			}
		}

		return f;
	}

private:
	const float m_maxValue{};
	bool m_lastFrame{ false };
};


//////////////////////////////////////////////////////////////////////////


class Normalize final
{
public:
	Normalize(float maxValue) : m_maxValue(maxValue) {}

	float operator() (float f) const
	{
		if (m_maxValue <= 0.0f)
			return f;
		else
			return std::min(f / m_maxValue, 1.0f);
	}

private:
	const float m_maxValue{};
};


class Loop final
{
public:
	Loop(float cycle) : m_cycle(cycle) {}

	float operator() (float f) const { return std::fmod(f, m_cycle); }

private:
	const float m_cycle{};
};


class Pingpong final
{
public:
	Pingpong(float halfCycle) : m_halfCycle(halfCycle), m_cycle(halfCycle + halfCycle) {}

	float operator() (float f) const { return std::abs(std::fmod(f + m_halfCycle, m_cycle) - m_halfCycle); }

private:
	const float m_halfCycle{};
	const float m_cycle{};
};


template <typename T>
class ActionParameterRange final
{
public:
	ActionParameterRange(const T& v0, const T& v1) : m0(v0), m1(v1) {}

	T operator()(float f) const { return m0 * (1.0f - f) + m1 * f; }

private:
	T m0, m1;
};
template <typename T> ActionParameterRange<T> range(const T& v0, const T& v1)
{
	return ActionParameterRange<T>(v0, v1);
}


// Function type.
typedef std::function<glm::vec4(float)> Transition_vec4;
typedef std::function<glm::vec3(float)> Transition_vec3;
typedef std::function<glm::vec2(float)> Transition_vec2;
typedef std::function<float(float)> Transition_float;
typedef Transition_float Curve;

// Curve.
float linear(float f);
float accelerate(float f);
float decelerate(float f);
float adspeed(float f);

// Cycle.
Transition_float once      (float period   );
Transition_float loop_t    (float cycle    );
Transition_float loop      (float cycle    );
Transition_float pingpong_t(float halfCycle);
Transition_float pingpong  (float halfCycle);

// Transition.
Transition_vec4 transition_alpha(float alpha0, float alpha1, const Curve& curve, float period);
Transition_vec4 fadein(const Curve& curve, float period);
Transition_vec4 fadeout(const Curve& curve, float period);


template <typename T>
class EatReturnValue
{
public:
	EatReturnValue(const T& func) : m_func(func) {}
	template<typename... Args> void operator() (const Args&... args) { m_func(args...); }

private:
	T m_func;
};
template <typename T> EatReturnValue<T> eatReturnValue(const T& func) { return EatReturnValue<T>(func); }


template <int> struct NestT final	// For some stupid compiler just like Android Studio.
{
	template<typename T, typename... Args>
	static auto nest(const T& t, Args... args) -> decltype(std::bind(t, NestT<sizeof...(args)>::nest(args...)))
	{
		return std::bind(t, NestT<sizeof...(args)>::nest(args...));
	}
};
template <> struct NestT<1> final
{
	template <typename T>
	static auto nest(const T& t) -> decltype(std::bind(t, std::placeholders::_1))
	{
		return std::bind(t, std::placeholders::_1);
	}

	static auto nest(float f) -> decltype(nest(once(f))) { return nest(once(f)); }
};
template<typename... Args>
auto nest(Args... args) -> decltype(NestT<sizeof...(args)>::nest(args...))
{
	return NestT<sizeof...(args)>::nest(args...);
}


template <typename EntityT, typename ParameterT>
class Nest2
{
public:
	template <typename FuncT, typename... Args>
	Nest2(const FuncT& func, Args... args)
		: m_func(eatReturnValue(std::bind(func, std::placeholders::_1, std::bind(nest(args...), std::placeholders::_2))))
	{}

public:
	inline void operator() (EntityT* pEntity, ParameterT v) { m_func(pEntity, v); }

private:
	std::function<void(EntityT*, ParameterT)> m_func;
};
template <typename FuncT, typename... Args>
auto nest2(const FuncT& func, Args... args) -> decltype(std::bind(func, std::placeholders::_1, std::bind(nest(args...), std::placeholders::_2)))
{
	return std::bind(func, std::placeholders::_1, std::bind(nest(args...), std::placeholders::_2));
}


template <typename T, typename... Args>
class Merge : Merge<Args...>
{
public:
	Merge(const T& t, const Args&... args)
		: Merge<Args...>(args...)
		, m_func(t)
	{}

	template <typename X, typename F>
	void operator() (X* pEntity, F f)
	{
		m_func(pEntity, f);
		Merge<Args...>::operator ()(pEntity, f);
	}

private:
	T m_func;
};

template <typename T0, typename T1>
class Merge<T0, T1>
{
public:
	Merge(const T0& t0, const T1& t1) : m_func0(t0), m_func1(t1) {}

	template <typename X, typename F>
	void operator() (X* pEntity, F f)
	{
		m_func0(pEntity, f);
		m_func1(pEntity, f);
	}

private:
	T0 m_func0;
	T1 m_func1;
};

template <typename T, typename... Args>
Merge<T, Args...> merge(const T& t, const Args&... args)
{
	return Merge<T, Args...>(t, args...);
}

template <typename EntityT, typename ParameterT>
class MergeEx
{
public:
	typedef const Nest2<EntityT, ParameterT>& Fn;

	MergeEx(Fn f0, Fn f1                                   ) : m_func(merge(f0, f1                    )) {}
	MergeEx(Fn f0, Fn f1, Fn f2                            ) : m_func(merge(f0, f1, f2                )) {}
	MergeEx(Fn f0, Fn f1, Fn f2, Fn f3                     ) : m_func(merge(f0, f1, f2, f3            )) {}
	MergeEx(Fn f0, Fn f1, Fn f2, Fn f3, Fn f4              ) : m_func(merge(f0, f1, f2, f3, f4        )) {}
	MergeEx(Fn f0, Fn f1, Fn f2, Fn f3, Fn f4, Fn f5       ) : m_func(merge(f0, f1, f2, f3, f4, f5    )) {}
	MergeEx(Fn f0, Fn f1, Fn f2, Fn f3, Fn f4, Fn f5, Fn f6) : m_func(merge(f0, f1, f2, f3, f4, f5, f6)) {}

public:
	inline void operator() (EntityT* pEntity, ParameterT v) { m_func(pEntity, v); }

private:
	std::function<void(EntityT*, ParameterT)> m_func;
};


template <typename EntityT, typename UpdaterT>
ActionPtr action(EntityT* pEntity, const UpdaterT& updater)
{
	if (pEntity == nullptr)
		return nullptr;
	
	auto pAction = new ActionComponent;
	pAction->onUpdate = eatReturnValue(std::bind(updater, pEntity, std::placeholders::_1));
	pEntity->addComponent(pAction);
	pAction->update(0.0f);
	return pAction;
}

template <typename EntityT, typename... Args>
ActionPtr action(EntityT* pEntity, Args... args)
{
	return action(pEntity, nest2(args...));
}

template <typename EntityT, typename... Args>
ActionPtr action(EntityT* pEntity, const MergeEx<typename std::remove_pointer<decltype(pEntity)>::type, float>& m, Args... args)
{
	return action(pEntity, nest2(m, args...));
}

template <typename EntityT>
ActionPtr action(EntityT* pEntity, const MergeEx<typename std::remove_pointer<decltype(pEntity)>::type, float>& m)
{
	return action<EntityT, MergeEx<EntityT, float>>(pEntity, m);
}

#define ACTION(p, method, ...) action(p, &std::remove_pointer<decltype(p)>::type::method, __VA_ARGS__)


template <typename T>
class DataProvider
{
public:
	typedef std::function<T()> Func;
	DataProvider(const Func& func) : m_func(func) {}

	T get() const { return m_func(); }

private:
	Func m_func;
};
template <typename FuncT>
auto source(const FuncT& func) -> DataProvider<decltype(func())>
{
	return DataProvider<decltype(func())>(func);
}


template <typename T> struct ValueType { typedef T Type; };
template <typename T> struct ValueType<DataProvider<T>> { typedef T Type; };

template <typename T> T val(const T& v) { return v; }
template <typename T> T val(const DataProvider<T>& v) { return v.get(); }

#define OBJ(x) (*reinterpret_cast<x*>(0))
#define VAL(x) OBJ(typename ValueType<x>::Type)


class RotateBy_2D
{
public:
	RotateBy_2D() {}
	RotateBy_2D(glm::vec3 dir) : m_previous(operator()(dir)) {}

	glm::quat operator() (glm::vec3 dir) const;

private:
	mutable glm::quat m_previous{ NS::QUAT_ZERO };
};
RotateBy_2D rotateBy2D();
RotateBy_2D rotateBy2D(glm::vec3 dir);


class RotateBy_3D
{
public:
	RotateBy_3D() {}
	RotateBy_3D(glm::vec3 dir) : m_previous(operator()(dir)) {}

	glm::quat operator() (glm::vec3 dir) const;

private:
	mutable glm::quat m_previous{ NS::QUAT_ZERO };
};
RotateBy_3D rotateBy3D();
RotateBy_3D rotateBy3D(glm::vec3 dir);


template <typename CurrentT, typename TargetT, typename SpeedT>
class MoveTo
{
public:
	MoveTo(const CurrentT& current, const TargetT& target, const SpeedT& speed)
		: m_current(current)
		, m_target(target)
		, m_speed(speed)
	{}

	glm::vec3 operator() (float dt) const
	{
		auto current = val(m_current);
		auto target  = val(m_target );

		auto dir = target - current;
		auto d = glm::length(dir);
		auto delta = val(m_speed) * dt;

		if (d <= delta) throw Exception_Natural();
		return dir * delta / d;
	}

private:
	CurrentT m_current;
	TargetT m_target;
	SpeedT m_speed;
};
template <typename CurrentT, typename TargetT, typename SpeedT>
MoveTo<CurrentT, TargetT, SpeedT> moveTo(const CurrentT& current, const TargetT& target, const SpeedT& speed)
{
	return MoveTo<CurrentT, TargetT, SpeedT>(current, target, speed);
}


template <typename CurrentT, typename DirT, typename SpeedT>
class MoveBy
{
public:
	MoveBy(const CurrentT& current, const DirT& dir, const SpeedT& speed)
		: m_current(current)
		, m_dir(dir)
		, m_speed(speed)
	{}

	auto operator() (float dt) const -> decltype(VAL(CurrentT) + VAL(DirT) * (VAL(SpeedT) * dt))
	{
		return val(m_current) + val(m_dir) * (val(m_speed) * dt);
	}

private:
	CurrentT m_current;
	DirT m_dir;
	SpeedT m_speed;
};
template <typename CurrentT, typename DirT, typename SpeedT>
MoveBy<CurrentT, DirT, SpeedT> moveby(const CurrentT& current, const DirT& dir, const SpeedT& speed)
{
	return MoveBy<CurrentT, DirT, SpeedT>(current, dir, speed);
}


template <typename DirT, typename SpeedT>
class MoveBy<void, DirT, SpeedT>
{
public:
	MoveBy(const DirT& dir, const SpeedT& speed)
		: m_dir(dir)
		, m_speed(speed)
	{}

	auto operator() (float dt) const -> decltype(VAL(DirT) * (VAL(SpeedT) * dt))
	{
		return val(m_dir) * (val(m_speed) * dt);
	}

private:
	DirT m_dir;
	SpeedT m_speed;
};
template <typename DirT, typename SpeedT>
MoveBy<void, DirT, SpeedT> moveby(const DirT& dir, const SpeedT& speed)
{
	return MoveBy<void, DirT, SpeedT>(dir, speed);
}


template <typename CoefficientT>
class Coefficient
{
public:
	Coefficient(const CoefficientT& c) : m_coefficient(c) {}

	template <typename ValueT>
	auto operator() (const ValueT& value) -> decltype(VAL(CoefficientT) * VAL(ValueT))
	{
		return val(m_coefficient) * value;
	}

private:
	CoefficientT m_coefficient;
};
template <typename CoefficientT>
Coefficient<CoefficientT> coefficient(const CoefficientT& c)
{
	return Coefficient<CoefficientT>(c);
}

NS_END


float sine_in        (float f);
float sine_out       (float f);
float sine           (float f);

float quadratic_in   (float f);
float quadratic_out  (float f);
float quadratic      (float f);

float cubic_in       (float f);
float cubic_out      (float f);
float cubic          (float f);

float quartic_in     (float f);
float quartic_out    (float f);
float quartic        (float f);

float quintic_in     (float f);
float quintic_out    (float f);
float quintic        (float f);

float circular_in    (float f);
float circular_out   (float f);
float circular       (float f);

float exponential_in (float f);
float exponential_out(float f);
float exponential    (float f);

float ease_back_in   (float f);
float ease_back_out  (float f);
float ease_back      (float f);

float elastic_in     (float f);
float elastic_out    (float f);
float elastic        (float f);

float bounce_in      (float f);
float bounce_out     (float f);
float bounce         (float f);
