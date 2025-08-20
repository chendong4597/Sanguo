/*!
 * \file GameAppModule.h
 *
 * \author mattho
 * \date 2017/12/20
 *
 *
 */
#pragma once
#include <common/Timer.h>
#include "component/Component.h"



class GameAppModule : public NS::Component
{
	//DECLARE_TIMER_HOST();
	RTTI_DEF(GameAppModule);

public:
	GameAppModule();
	virtual ~GameAppModule();

	virtual bool initialize(Component* pEntity) override;
	virtual void terminate() override;

	virtual void update(float dt) override;

	void StartModule();
	void StopModule();
protected:

};



