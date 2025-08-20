/*!
 * \file RenderableComponent.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 27/04/2016
 *
 * 
 */
#include "RenderableComponent.h"
#include "CullTree.h"

NS_BEGIN

void RenderableEditor::setRenderable(RenderableComponent* pRenderableComponent, bool renderable)
{
	if (pRenderableComponent)
	{
		pRenderableComponent->setIsRenderInContainer(renderable);
	}
}

RenderableComponent::RenderableComponent()
{
	addField("render_order", &RenderableComponent::setRenderOrder, &RenderableComponent::getRenderOrder, &FFA_DEFAULT_INTEGER);
	addField("cast_splash", &RenderableComponent::setCastSplash, &RenderableComponent::isCastSplash);
	addField("mask_rain", &RenderableComponent::setMaskRain, &RenderableComponent::isMaskRain);
	addField("stencil_write", &RenderableComponent::setStencilWrite, &RenderableComponent::isStencilWrite);
	addField("stencil_test", &RenderableComponent::setStencilTest, &RenderableComponent::isStencilTest);
}

RenderableComponent::~RenderableComponent()
{

}

void RenderableComponent::setCullTree(CullTree* pCullTree)
{
	if (m_pCullTree == pCullTree)
	{
		return;
	}

	if (pCullTree == NULL)
	{
		if (m_pCullTree)
			m_pCullTree->RemoveRenderable(this);
	}
	else
	{
		pCullTree->AddRenderable(this);
	}

	m_pCullTree = pCullTree;

	for (auto&& pComponent : m_componentList)
	{
		pComponent->setCullTree(pCullTree);
	}
}

bool bzbee::RenderableComponent::isInView(glm::vec3 worldPos, float fExtraRect)
{
	return true;
}

RenderableComponent& RenderableComponent::operator=(const RenderableComponent& other)
{
	setName(other.getName());
	m_renderOrder = other.m_renderOrder;

	return *this;
}

NS_END
