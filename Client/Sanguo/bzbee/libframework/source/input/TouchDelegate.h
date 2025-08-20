/*!
 * \file TouchDelegate.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 08/31/2016
 *
 * 
 */
#pragma once
#include <baseutil.h>

NS_BEGIN

class Component;
class TouchEvent;

class TouchDelegate
{
	RTTI_ROOT(TouchDelegate);

public:
	virtual ~TouchDelegate();
	virtual bool processTouchEvent(Component* pEntity, TouchEvent& evt) = 0;
	virtual void onUpdate(float dt){};
};

NS_END
