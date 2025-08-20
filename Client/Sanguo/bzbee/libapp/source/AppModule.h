/*!
 * \file AppModule.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 27/04/2016
 *
 * 
 */
#pragma once
#include <component/RenderableComponent.h>

NS_BEGIN

class FrameStepEvent;

class AppModule : public RenderableComponent
{
	DECLARE_EVENT_HOST();
	RTTI_DEF(AppModule);

public:
	virtual bool initialize(Component* pEntity) override;
	virtual void terminate() override;

	virtual void update(float dt) override;

	const glm::ivec2& getDeviceSize() const { return m_deviceSize; };
	const glm::ivec2& getDesignSize() const { return m_designSize; };
	const tstring& getAppId() const { return m_appId; };
	const tstring& getAppTitle() const { return m_appTitle; };

	// need a better place to handle them
	virtual tstring getContentText() { return EMPTY_STRING; };
	virtual int getEditBoxHeight() { return 0; };
	virtual int getEditBoxState() { return 0; }

	static inline AppModule* getInstance(void) { return s_pInstance; }

protected:
	AppModule(const glm::ivec2& deviceSize, const glm::ivec2& designSize, const tstring& appId, const tstring& appTitle);
	virtual ~AppModule();

private:
	void onStep(FrameStepEvent& evt);

protected:
	glm::ivec2 m_deviceSize;
	glm::ivec2 m_designSize;
	tstring m_appId;
	tstring m_appTitle;

private:
	static AppModule* s_pInstance;
};

NS_END
