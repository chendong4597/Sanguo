/*!
 * \file Framework.h
 * \date 10-10-2014 11:44:30
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once

#include "component/Component.h"

NS_BEGIN

class Framework : public Component
{
public:
	RTTI_DEF(Framework);

public:
	Framework();
	virtual ~Framework();

	static Framework& getInstance();

	virtual bool initialize(Component* pEntity) override;
	virtual void terminate() override;

	void step();
	void resize(const glm::ivec2& deviceSize);
	void reload(bool freeOld);

	float getRealFrameTime() { return m_currTime - m_lastTime; }
    float getFrameTime() const { return m_deltaTime; };
    const UI64& getFrameNumber() const { return m_frameNumber; };

    void setSafeArea(const int top, const int left, const int bottom, const int right);

	//FPS related
	void	SetLimitFPS(bool bLimitFPS, uint32 nFPSNum = 60)	{ m_bLimitFPS = bLimitFPS; SetLimitFPSNum(nFPSNum); }
	bool	GetLimitFPS() const									{ return m_bLimitFPS; }
	uint32	GetLimitFPSNum() const								{ return m_nLimitFPSNum; }
	void	SetLimitFPSNum(uint32 val)							{ m_nLimitFPSNum = val; SetLimitFPSTick(1000/m_nLimitFPSNum); }
	uint32	GetLimitFPSTick() const								{ return m_fLimitFPSTick; }
	void	SetLimitFPSTick(uint32 ms)							{ m_fLimitFPSTick = ms; }

	void setDisableTouch(bool bYes) { m_disableTouch = bYes; };
	bool getDisableTouch() const { return m_disableTouch; };

	void setDisableKeyboard(bool bYes) { m_disableKeyboard = bYes; };
	bool getDisableKeyboard() const { return m_disableKeyboard; };

	// 下雨效果开关
	void setEnableWeather(bool bYes) { m_enableWeather = bYes; };
	bool getEnableWeather() const { return m_enableWeather; };

private:
	float m_lastTime{};
	float m_currTime{};
	float m_deltaTime{};
	bool m_bLimitFPS;
	uint32 m_nLimitFPSNum;
	uint32 m_fLimitFPSTick;
	bool m_disableTouch{};
	bool m_disableKeyboard{};
	bool m_enableWeather{};

    UI64 m_frameNumber{};

};

class BeginFrameEvent final : public Event
{
	RTTI_DEF(BeginFrameEvent);
};

class EndFrameEvent final : public Event
{
	RTTI_DEF(EndFrameEvent);
};

class FrameStepEvent final : public Event
{
	RTTI_DEF(FrameStepEvent);

public:
	FrameStepEvent(float dt) :m_dt(dt) {};
	~FrameStepEvent() {};

public:
	/** The adjusted delta time of current frame. */
	float m_dt{};

	/** The real time elapsed since last frame. */
	float m_frameTime{};
};

class ResizeEvent final : public Event
{
	RTTI_DEF(ResizeEvent);

public:
	ResizeEvent(const glm::ivec2& deviceSize) :m_deviceSize(deviceSize) {};

public:
	glm::ivec2 m_deviceSize;

};

class SetSafeAreaEvent final : public Event
{
    RTTI_DEF(SetSafeAreaEvent);
    
public:
    SetSafeAreaEvent(const int top, const int left, const int bottom, const int right):m_top(top), m_left(left), m_bottom(bottom), m_right(right)  {};
    
public:
    int m_top{0};
    int m_left{0};
    int m_bottom{0};
    int m_right{0};
    
};

class ReloadEvent final : public Event
{
	RTTI_DEF(ReloadEvent);

public:
	ReloadEvent(bool freeOld) :m_freeOld(freeOld) {};

public:
	bool m_freeOld{};

};

class TouchEvent final : public Event
{
	RTTI_DEF(TouchEvent);

public:
	enum class ActionTypeId
	{
		Unknown = 0,
		Begin,
		End,
		Move,
		PressedBegin,  // begin frame
		PressedHold,   // per frame  hold pressed not move
		PressedMove,   // per frame  pressed and move
		PressedEnd,    // end frame
	};

	class TouchPointer
	{
	public:
		int m_pointId{};
		ActionTypeId m_actionType{ ActionTypeId::Unknown };
		glm::vec2 m_touchPos;
	};

	typedef std::vector<TouchPointer> TouchPointerList;

public:
	void addPoint(int pointId, ActionTypeId actionType, const glm::vec2& pos);
	void removePoint(int pointId);

public:
	TouchPointerList m_touchPointerList;

};

class KeyboardEvent final : public Event
{
	RTTI_DEF(KeyboardEvent);

public:
	enum class ActionTypeId
	{
		KeyDown,
		KeyUp,
	};

public:
	ActionTypeId m_actionType{ ActionTypeId::KeyDown };
	uint8 m_keyCode{};
};

class OnCharEvent final : public Event
{
	RTTI_DEF(OnCharEvent);

public:
	tstring strText;
};

class ZoomEvent final : public Event
{
	RTTI_DEF(ZoomEvent);

public:
	float m_zoom{};
};

NS_END
