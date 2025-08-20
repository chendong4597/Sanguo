/*!
 * \file CullTree.cpp
 *
 * \author kechen
 * \date 3/14/2019
 *
 *
 */
#include "CullTree.h"
#include "RenderableComponent.h"

NS_BEGIN

bool CullTree::initialize(float left, float right, float top, float bottom, unsigned rowBinNum, unsigned colBinNum)
{
	m_rect.min.x = left;
	m_rect.max.x = right;
	m_rect.max.y = top;
	m_rect.min.y = bottom;

	m_rowBinNum = rowBinNum;
	m_colBinNum = colBinNum;
	m_binHeight = m_rect.getHeight() / rowBinNum;
	m_binWidth = m_rect.getWidth() / colBinNum;

	m_cullNodes.resize(rowBinNum * colBinNum);

	for (unsigned int x=0; x < colBinNum; x++)
	{
		for (unsigned int y=0; y < rowBinNum; y++)
		{
			m_cullNodes[y * m_colBinNum + x].area.min.x = m_rect.min.x + (float)x * m_binWidth;
			m_cullNodes[y * m_colBinNum + x].area.max.x = m_rect.min.x + (float)(x+1) * m_binWidth;
			m_cullNodes[y * m_colBinNum + x].area.min.y = m_rect.min.y + (float)y * m_binHeight;
			m_cullNodes[y * m_colBinNum + x].area.max.y = m_rect.min.y + (float)(y+1) * m_binHeight;
		}
	}  
	        
	return true;
}

void CullTree::reset(float left, float right, float top, float bottom, unsigned rowBinNum, unsigned colBinNum)
{
	m_rect.min.x = left;
	m_rect.max.x = right;
	m_rect.max.y = top;
	m_rect.min.y = bottom;

	m_rowBinNum = rowBinNum;
	m_colBinNum = colBinNum;
	m_binHeight = m_rect.getHeight() / rowBinNum;
	m_binWidth = m_rect.getWidth() / colBinNum;

	m_cullNodes.clear();
	m_cullNodes.resize(rowBinNum * colBinNum);

	for (unsigned int x = 0; x < colBinNum; x++)
	{
		for (unsigned int y = 0; y < rowBinNum; y++)
		{
			m_cullNodes[y * m_colBinNum + x].area.min.x = m_rect.min.x + (float)x * m_binWidth;
			m_cullNodes[y * m_colBinNum + x].area.max.x = m_rect.min.x + (float)(x + 1) * m_binWidth;
			m_cullNodes[y * m_colBinNum + x].area.min.y = m_rect.min.y + (float)y * m_binHeight;
			m_cullNodes[y * m_colBinNum + x].area.max.y = m_rect.min.y + (float)(y + 1) * m_binHeight;
		}
	}
}

void CullTree::AddRenderable(RenderableComponent* pRenderable)
{
	if (pRenderable->checkState(RenderableComponent::CULLTREE_NEVER))
	{
		return;
	}

	if(pRenderable->checkState(RenderableComponent::CULLTREE_ALWAYS_COLLECT))
	{
		m_alwaysCollectNode.renderables.push_back(pRenderable);
		return;
	}

	FloatRect& rectExpand = pRenderable->getRectExpand();
	IntRect& cullNodes = pRenderable->getCullNodes();

	// left
	cullNodes.min.x = std::min((unsigned)(std::max(rectExpand.min.x - m_rect.min.x, 0.0f) / m_binWidth), m_colBinNum - 1);

	// right
	cullNodes.max.x = std::min((unsigned)(std::max(rectExpand.max.x - m_rect.min.x, 0.0f) / m_binWidth), m_colBinNum - 1);

	// bottom
	cullNodes.min.y = std::min((unsigned)(std::max(rectExpand.min.y - m_rect.min.y, 0.0f) / m_binHeight), m_rowBinNum - 1);

	// top
	cullNodes.max.y = std::min((unsigned)(std::max(rectExpand.max.y - m_rect.min.y, 0.0f) / m_binHeight), m_rowBinNum - 1);

	for (int x = cullNodes.min.x; x <= cullNodes.max.x; x++)
	{
		for (int y = cullNodes.min.y; y <= cullNodes.max.y; y++)
		{
			m_cullNodes[y * m_colBinNum + x].renderables.push_back(pRenderable);
		}
	}
}

void CullTree::RemoveRenderable(RenderableComponent* pRenderable)
{
	if (pRenderable->checkState(RenderableComponent::CULLTREE_ALWAYS_COLLECT))
	{
		std::vector<RenderableComponent*>& renderables = m_alwaysCollectNode.renderables;
		renderables.erase(std::remove(renderables.begin(), renderables.end(), pRenderable), renderables.end());
		return;
	}

	if (m_cullNodes.size()==0)
	{
		return;
	}

	IntRect& cullNodes = pRenderable->getCullNodes();
	for (int x = cullNodes.min.x; x <= cullNodes.max.x; x++)
	{
		for (int y = cullNodes.min.y; y <= cullNodes.max.y; y++)
		{
			std::vector<RenderableComponent*>& renderables = m_cullNodes[y * m_colBinNum + x].renderables;
			renderables.erase(std::remove(renderables.begin(), renderables.end(), pRenderable), renderables.end());
		}
	}
}

void CullTree::UpdateRenderable(RenderableComponent* pRenderable)
{
	RemoveRenderable(pRenderable);
	AddRenderable(pRenderable);
}

void CullTree::GetCoveredBins(FloatRect rectExpand, IntRect &outRect)
{
	// left
	outRect.min.x = std::min((unsigned)(std::max(rectExpand.min.x - m_rect.min.x, 0.0f) / m_binWidth), m_colBinNum - 1);

	// right
	outRect.max.x = std::min((unsigned)(std::max(rectExpand.max.x - m_rect.min.x, 0.0f) / m_binWidth), m_colBinNum - 1);

	// bottom
	outRect.min.y = std::min((unsigned)(std::max(rectExpand.min.y - m_rect.min.y, 0.0f) / m_binHeight), m_rowBinNum - 1);

	// top
	outRect.max.y = std::min((unsigned)(std::max(rectExpand.max.y - m_rect.min.y, 0.0f) / m_binHeight), m_rowBinNum - 1);
}

const CullNode& CullTree::GetAlwaysCollectNode() const
{
	return m_alwaysCollectNode;
}

const CullNode& CullTree::GetCullNode(int row, int col) const
{
	return m_cullNodes[row * m_colBinNum + col];
}

NS_END
