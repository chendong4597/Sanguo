/*!
 * \file Rect.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 07/22/2016
 *
 * 
 */
#pragma once
#include "baseutil.h"

class Rect
{
public:
	Rect() {};
	Rect(int left, int top, int right, int bottom) :m_left(left), m_top(top), m_right(right), m_bottom(bottom) {};

	bool operator==(const Rect& rect) const
	{
		return (m_left == rect.m_left && m_top == rect.m_top && m_right == rect.m_right && m_bottom == rect.m_bottom);
	}

	bool operator!=(const Rect& rect) const
	{
		return (m_left != rect.m_left || m_top != rect.m_top || m_right != rect.m_right || m_bottom != rect.m_bottom);
	}

	bool contain(const glm::ivec2& coord) const
	{
		return (m_left <= coord.x && m_right >= coord.x && m_top <= coord.y && m_bottom >= coord.y);
	}

	bool contain(const Rect& rect) const
	{
		return (m_left <= rect.m_left && m_right >= rect.m_right && m_top <= rect.m_top && m_bottom >= rect.m_bottom);
	}

	bool intersect(const Rect& rect) const
	{
		return !(rect.m_left > m_right || rect.m_right < m_left || rect.m_top > m_bottom || rect.m_bottom < m_top);
	}
	
	void merge(const glm::ivec2& coord)
	{
		m_left = std::min(m_left, coord.x);
		m_right = std::max(m_right, coord.x);
		m_top = std::min(m_top, coord.y);
		m_bottom = std::max(m_bottom, coord.y);
	}

	void merge(const Rect& other)
	{
		m_left = std::min(m_left, other.m_left);
		m_right = std::max(m_right, other.m_right);
		m_top = std::min(m_top, other.m_top);
		m_bottom = std::max(m_bottom, other.m_bottom);
	}

	void ShrinkTo(int x, int y)
	{
		m_left = m_right = x;
		m_top = m_bottom = y;
	}

	void Expand(int size, const Rect& limit)
	{
		m_left	 = std::max(m_left - size, limit.m_left);
		m_right  = std::min(m_right + size, limit.m_right);
		m_top    = std::max(m_top - size, limit.m_top);
		m_bottom = std::min(m_bottom + size, limit.m_bottom);
	}

	int size()
	{
		return (m_bottom - m_top + 1) * (m_right - m_left + 1);
	}

	int width()
	{
		return m_right - m_left + 1;
	}

	int height()
	{
		return m_bottom - m_top + 1;
	}

public:
	int m_left{};
	int m_top{};
	int m_right{};
	int m_bottom{};
};
