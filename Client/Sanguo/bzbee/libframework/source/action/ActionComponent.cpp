/*!
* \file ActionComponent.cpp
*
* \author zhichenxia
* \date 2017/8/16
*
*
*/

#include "ActionComponent.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <GarbageCollection.h>

using namespace NS;

const glm::mat4& matrix_left()
{
	static const auto v = glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	return v;
}


const glm::quat& quat_left()
{
	static const auto v = glm::rotate(QUAT_ZERO, glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	return v;
}


const glm::mat4& matrix_right()
{
	static const auto v = glm::rotate(MAT4_IDENTITY, -glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	return v;
}


const glm::quat& quat_right()
{
	static const auto v = glm::rotate(QUAT_ZERO, -glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	return v;
}

const glm::quat& quat_reverse()
{
    static const auto v = glm::rotate(QUAT_ZERO, glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    return v;
}

NS_BEGIN

void ActionComponent::update(float dt)
{
	Component::update(dt);
	if (!m_active) return;

	try
	{
		onUpdate(dt);
	}
	catch (const Exception_Natural&)
	{
		remove(EventType::End);
	}
	catch (const Exception_Error&)
	{
		//LOGE("Action error.");
		remove(EventType::Error);
	}
}


void ActionComponent::remove(EventType eventType)
{
	if (!m_active) return;
	m_active = false;

	if (onActionEvent) onActionEvent(eventType);
	if (onStop) onStop();

	removeFromParent(false);

	_GC[this]
	{
		terminate();
		delete this;
	};
}


float linear(float f) { return f; }
float accelerate(float f) { return 1.0f - std::cos(f * glm::half_pi<float>()); }
float decelerate(float f) { return std::cos((1.0f - f) * glm::half_pi<float>()); }
float adspeed(float time)
{
	time = time * 2;
	if (time < 1)
		return 0.5f * time * time * time * time;
	time -= 2;
	return -0.5f * (time * time * time * time - 2);
}

Transition_float once(float period)
{
	return nest(Normalize(period), Validator_Time<Exception_Natural>(period), Accumulation());
}


Transition_float loop_t(float cycle)
{
	return nest(Normalize(cycle), Loop(cycle));
}


Transition_float loop(float cycle)
{
	return nest(Normalize(cycle), Loop(cycle), Accumulation());
}


Transition_float pingpong_t(float halfCycle)
{
	return nest(Normalize(halfCycle), Pingpong(halfCycle));
}


Transition_float pingpong(float halfCycle)
{
	return nest(Normalize(halfCycle), Pingpong(halfCycle), Accumulation());
}


Transition_vec4 transition_alpha(float alpha0, float alpha1, const Curve& curve, float period)
{
	return nest(range(glm::vec4(alpha0), glm::vec4(alpha1)), curve, period);
}


Transition_vec4 fadein(const Curve& curve, float period)
{
	return transition_alpha(0.0f, 1.0f, curve, period);
}


Transition_vec4 fadeout(const Curve& curve, float period)
{
	return transition_alpha(1.0f, 0.0f, curve, period);
}


glm::quat rotateBy_XY(glm::vec3 dir)
{
	dir.z = 0.0f;

	bool leftOrRight = dir.x <= 0.0f;
	dir = glm::normalize(dir);

	glm::quat ret;
	if (dir.x != 1.0f && dir.x != -1.0f)
	{
		glm::vec3 lr = glm::vec3((leftOrRight ? -1.0f : 1.0f), 0.0f, 0.0f);
		ret = glm::rotate(QUAT_ZERO, acosf(glm::dot(lr, dir)), glm::cross(lr, dir));
	}
	else
	{
		ret = QUAT_ZERO;
	}
	if (leftOrRight) ret *= quat_reverse();

	return ret;
}


glm::quat RotateBy_2D::operator()(glm::vec3 dir) const
{
	dir.z = 0.0f;
	if (glm::length2(dir) < 1.e-9f) return m_previous;

	m_previous = rotateBy_XY(dir);
	return m_previous;
}


RotateBy_2D rotateBy2D()
{
	return RotateBy_2D();
}


RotateBy_2D rotateBy2D(glm::vec3 dir)
{
	return RotateBy_2D(dir);
}


glm::quat RotateBy_3D::operator() (glm::vec3 dir) const
{
	glm::quat ret = QUAT_ZERO;
	bool valid = false;

	if (glm::length2(glm::vec2(dir)) > 1.e-9f)
	{
		valid = true;
		ret = rotateBy_XY(dir);
	}

	auto length = glm::length(dir);
	if (length > 1.e-9f)
	{
		valid = true;
		ret *= glm::rotate(QUAT_ZERO, std::asin(dir.z / length), glm::vec3(0.0f, (dir.x < 0.0f ? 1.0f : -1.0f), 0.0f));
	}

	if (valid) m_previous = ret;
	return m_previous;
}


RotateBy_3D rotateBy3D()
{
	return RotateBy_3D();
}


RotateBy_3D rotateBy3D(glm::vec3 dir)
{
	return RotateBy_3D(dir);
}


NS_END

float LUA(sine_in)(float f)
{
    return (1.0f - std::cos(f * glm::half_pi<float>()));
}

float LUA(sine_out)(float f)
{
    return std::sin(f * glm::half_pi<float>());
}

float LUA(sine)(float f)
{
    return 0.5f * (1.0f - std::cos(f * glm::pi<float>()));
}

float LUA(quadratic_in )(float f)
{
    return f * f;
}

float LUA(quadratic_out)(float f)
{
    return 1.0f - quadratic_in(1.0f - f);
}

float LUA(quadratic)(float f)
{
    return f < 0.5f ? quadratic_in(f + f) * 0.5f : quadratic_out(f + f - 1.0f) * 0.5f + 0.5f;
}

float LUA(cubic_in )(float f)
{
    return f * f * f;
}

float LUA(cubic_out)(float f)
{
    return 1.0f - cubic_in(1.0f - f);
}

float LUA(cubic)(float f)
{
    return f < 0.5f ? cubic_in(f + f) * 0.5f : cubic_out(f + f - 1.0f) * 0.5f + 0.5f;
}

float LUA(quartic_in )(float f)
{
    return f * f * f * f;
}

float LUA(quartic_out)(float f)
{
    return 1.0f - quartic_in(1.0f - f);
}

float LUA(quartic)(float f)
{
    return f < 0.5f ? quartic_in(f + f) * 0.5f : quartic_out(f + f - 1.0f) * 0.5f + 0.5f;
}

float LUA(quintic_in )(float f)
{
    return f * f * f * f * f;
}

float LUA(quintic_out)(float f)
{
    return 1.0f - quintic_in(1.0f - f);
}

float LUA(quintic)(float f)
{
    return f < 0.5f ? quintic_in(f + f) * 0.5f : quintic_out(f + f - 1.0f) * 0.5f + 0.5f;
}

float LUA(circular_in)(float f)
{
    return 1.0f - circular_out(1.0f - f);
}

float LUA(circular_out)(float f)
{
    return std::sqrt((2.0f - f) * f);
}

float LUA(circular)(float f)
{
    return f < 0.5f ? circular_in(f + f) * 0.5f : circular_out(f + f - 1.0f) * 0.5f + 0.5f;
}

float LUA(exponential_in )(float f)
{
    return std::pow(2.0f, 10.0f * (f - 1.0f));
}

float LUA(exponential_out)(float f)
{
    return 1.0f - exponential_in(1.0f - f);
}

float LUA(exponential)(float f)
{
    return f < 0.5f ? exponential_in(f + f) * 0.5f : exponential_out(f + f - 1.0f) * 0.5f + 0.5f;
}

float LUA(ease_back_in )(float f)
{
    return f * f * (2.70158f * f - 1.70158f);
}

float LUA(ease_back_out)(float f)
{
    return 1.0f - ease_back_in(1.0f - f);
}

float LUA(ease_back)(float f)
{
    if (f < 0.5f)
        return f * f * (14.379636f * f - 5.189818f);
    else
    {
        f -= 1.0f;
        return f * f * (14.379636f * f + 5.189818f) + 1.0f;
    }
}

float LUA(elastic_in)(float f)
{
    return -std::pow(2.0f, 10.0f * f - 10.0f) * std::sin((3.33f * f - 3.58f) * glm::two_pi<float>());
}

float LUA(elastic_out)(float f)
{
    return 1.0f - elastic_in(1.0f - f);
}

float LUA(elastic)(float f)
{
    if (f < 0.5f)
        return -0.5f * std::pow(2.0f, 20.0f * f - 10.0f) * std::sin((4.45f * f - 2.475f) * glm::two_pi<float>());
    else
        return  0.5f * std::pow(2.0f, -20.0f * f + 10.0f) * std::sin((4.45f * f - 2.475f) * glm::two_pi<float>()) + 1.0f;
}

float LUA(bounce_in)(float f)
{
    return 1.0f - bounce_out(1.0f - f);
}

float LUA(bounce_out)(float f)
{
    if (f < 0.363636f) return 7.5625f * f * f;

    else if (f < 0.72727f)
    {
        f -= 0.545454f;
        return 7.5625f * f * f + 0.75f;
    }
    else if (f < 0.909091f)
    {
        f -= 0.818182f;
        return 7.5625f * f * f + 0.9375f;
    }
    else
    {
        f -= 0.954545f;
        return 7.5625f * f * f + 0.984375f;
    }
}

float LUA(bounce)(float f)
{
    return f < 0.5f ? bounce_in(f + f) * 0.5f : bounce_out(f + f - 1.0f) * 0.5f + 0.5f;
}
