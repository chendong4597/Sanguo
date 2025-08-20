/*!
 * \file RenderableComponent.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 27/04/2016
 *
 * 
 */
#pragma once
#include "Component.h"

NS_BEGIN

class RenderableComponent;

class RenderableEditor
{
public:
	virtual ~RenderableEditor() = default;
protected:
	RenderableEditor() = default;

	void setRenderable(RenderableComponent* pRenderableComponent, bool renderable);
};

class RenderableComponent : public Component
{
public:
	RTTI_DEF(RenderableComponent);

	static const uint CULLTREE_ALWAYS_COLLECT = 1u << 0;
	static const uint CULLTREE_NEVER = 1u << 1;
	static const uint CULLTREE_BIN_COLLECT = 1u << 2;

public:
	RenderableComponent();
	virtual ~RenderableComponent();
	// prevent Component's copy assignment from being called
	RenderableComponent& operator=(const RenderableComponent& other);

	struct LightMapInfo
	{
		glm::vec4 v[4];
		glm::vec4 color;
		tstring texturePath;
	};
	virtual void renderLightMap(std::vector<LightMapInfo>& arr) {}

	virtual void render() = 0;
	virtual void renderShadowMap(const glm::mat4& viewMatrixL, const glm::mat4& projectionMatrixL) {}
	virtual void renderShadow(const glm::mat4& viewMatrixL, const glm::mat4& projectionMatrixL) {}
	virtual void renderDepthTexture(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {}

	bool METHOD(setRenderOrder)(int32 renderOrder) { m_renderOrder = renderOrder; return true; };
	int32 getRenderOrder() const { return m_renderOrder; };

	void setMaterialID(int32 materialID) { m_materialID = materialID; };
	int32 getMaterialID() const { return m_materialID; };

	bool getRenderable(){ return m_renderable && m_bIsRenderInContainer; }

	bool isCastSplash() { return m_bCastSplash; }
	bool setCastSplash(bool bCast) { m_bCastSplash = bCast; return true; }

	bool isMaskRain() { return m_bMaskRain; }
	bool setMaskRain(bool bMask) { m_bMaskRain = bMask; return true; }

	bool isStencilWrite() { return m_bStencilWrite; };
	bool setStencilWrite(bool bStencilWrite) { m_bStencilWrite = bStencilWrite; return true; };

	bool isStencilTest() { return m_bStencilTest; };
	bool setStencilTest(bool bStencilTest) { m_bStencilTest = bStencilTest; return true; };
	
	void setCullNodes(IntRect& cullNodes) { m_cullNodes = cullNodes; }
	IntRect& getCullNodes() { return m_cullNodes; }

	virtual void setCullTree(CullTree* pCullTree) override;

	void setRectExpand(FloatRect& rectExpand) { m_rectExpand = rectExpand; }
	FloatRect& getRectExpand() { return m_rectExpand; }

	void setState(uint state) { m_cullTreeMode = state; };
	bool checkState(uint state) const { return (m_cullTreeMode == state); };
	uint getState() const { return m_cullTreeMode; };

	void setShadowCaster(bool shadowCaster) { m_shadowCaster = shadowCaster; }
	bool isShadowCaster() { return m_shadowCaster; }
	void setShadowReceiver(bool shadowReceiver) { m_shadowReceiver = shadowReceiver; }
	bool isShadowReceiver() { return m_shadowReceiver; }
	friend class RenderableEditor;
	void METHOD(setRenderable)(bool renderable) { m_renderable = renderable; }

	virtual bool isInView(glm::vec3 worldPos, float fExtraRect);
	
	//add by xiaobowang 2018-12-15 (temporary)
	void setIsRenderInContainer(bool bIsRender)
	{
		m_bIsRenderInContainer = bIsRender;
	}
	bool getIsRenderInContainer()
	{
		return m_bIsRenderInContainer;
	}

protected:
	bool m_bCastSplash{ false };
	bool m_bMaskRain{ false };

	bool m_bStencilWrite{ false };
	bool m_bStencilTest{ false };

	uint m_cullTreeMode{ CULLTREE_ALWAYS_COLLECT };

	// for scene management
	IntRect m_cullNodes;
	FloatRect m_rectExpand;

private:
	int32 m_renderOrder{};
	int32 m_materialID{ 0 };
	bool m_renderable{ true };
	bool m_shadowCaster{false};
	bool m_shadowReceiver{false};

	//add by xiaobowang 2018-12-15 (temporary)
	bool m_bIsRenderInContainer{true};
};

NS_END
