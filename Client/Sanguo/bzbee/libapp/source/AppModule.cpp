/*!
 * \file AppModule.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 27/04/2016
 *
 * 
 */
#include "AppModule.h"
#include <controls/RichLabelElementParsers.h>
#include <FontModule.h>
#include <Framework.h>
#include <PostProcessModule.h>
#include <RenderModule.h>
#include <StyleModule.h>
#include <SystemUtil.h>
#include <utils/CoordinateUtil.h>
#include <utils/PendingCleanup.h>

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
AppModule* AppModule::s_pInstance{};

AppModule::AppModule(const glm::ivec2& deviceSize, const glm::ivec2& designSize, const tstring& appId, const tstring& appTitle)
	: m_deviceSize(deviceSize)
	, m_designSize(designSize)
	, m_appId(appId)
	, m_appTitle(appTitle)
{
	s_pInstance = this;
}

AppModule::~AppModule()
{
}

bool AppModule::initialize(Component* pEntity)
{
	RenderModule* pRenderModule = new RenderModule();
	pEntity->addComponent(pRenderModule);

	StyleModule* pStyleModule = new StyleModule(pRenderModule);
	pEntity->addComponent(pStyleModule);

	PostProcessModule* pPostProcessModule = new PostProcessModule();
	pEntity->addComponent(pPostProcessModule);

	FontModule* pFontModule = new FontModule(pRenderModule);
	pEntity->addComponent(pFontModule);

	RichLabelElementParserModule* pRichLabelElementParserModule = new RichLabelElementParserModule(pRenderModule, pFontModule, pStyleModule);
	pEntity->addComponent(pRichLabelElementParserModule);

	Framework::getInstance().resize(getDeviceSize());
	REGISTER_EVENT(&Framework::getInstance(), FrameStepEvent, &AppModule::onStep);

	return true;
}

void AppModule::terminate()
{
	// NOTE: Framework will destroy all the module components attached to it during its termination
	Super::terminate();
}

void AppModule::update(float dt)
{
	RenderableComponent::update(dt);
}

void AppModule::onStep(FrameStepEvent& evt)
{
	render();

	{
		// Delete the objects which were enqueued for deferred cleanup
		PendingCleanupObjects::flush();
	}
}
//////////////////////////////////////////////////////////////////////////

NS_END
