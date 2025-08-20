/*!
 * \file CullTree.h
 *
 * \author kechen
 * \date 3/14/2019
 *
 *
 */
#pragma once
#include <baseutil.h>

NS_BEGIN

class RenderableComponent;

class CullNode
{
public:
	FloatRect area;
	std::vector<RenderableComponent*> renderables;
};

class CullTree
{
public:
	CullTree() {};
	virtual ~CullTree() {};

	bool initialize(float left, float right, float top, float bottom, unsigned rowBinNum, unsigned colBinNum);
	void reset(float left, float right, float top, float bottom, unsigned rowBinNum, unsigned colBinNum);
	void AddRenderable(RenderableComponent* pRenderable);
	void RemoveRenderable(RenderableComponent* pRenderable);
	void UpdateRenderable(RenderableComponent* pRenderable);

	void GetCoveredBins(FloatRect rectExpand, IntRect &outRect);
	const CullNode& GetCullNode(int row, int col) const;
	const CullNode& GetAlwaysCollectNode() const;
	const unsigned& GetRowBinNum() const { return m_rowBinNum; };
	const unsigned& GetColBinNum() const { return m_colBinNum; };

private:
	std::vector<CullNode> m_cullNodes;
	CullNode m_alwaysCollectNode;

	FloatRect m_rect;
	unsigned m_rowBinNum;
	unsigned m_colBinNum;
	float m_binHeight;
	float m_binWidth;
};

NS_END
