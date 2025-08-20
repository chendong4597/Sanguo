/*!
* \file MathCommon.h
*
* \author kechen
* \date 2018/11/22
*
*
*/
#pragma once

#include "../BaseType.h"
#include <limits>

typedef float Real;

NS_BEGIN

class Math
{
public:

	/** Compare 2 reals, using tolerance for inaccuracies.
	*/
	static bool RealEqual(Real a, Real b, Real tolerance = std::numeric_limits<Real>::epsilon());
};


/** Structure used to define a box in a 3-D integer space.
Note that the left, top, and front edges are included but the right,
bottom and back ones are not.
*/
struct Box3D
{
	uint32 left, top, right, bottom, front, back;
	/// Parameterless constructor for setting the members manually
	Box3D()
		: left(0), top(0), right(1), bottom(1), front(0), back(1)
	{
	}
	/** Define a box from left, top, right and bottom coordinates
	This box will have depth one (front=0 and back=1).
	@param	l	x value of left edge
	@param	t	y value of top edge
	@param	r	x value of right edge
	@param	b	y value of bottom edge
	@note Note that the left, top, and front edges are included
	but the right, bottom and back ones are not.
	*/
	Box3D(uint32 l, uint32 t, uint32 r, uint32 b) :
		left(l),
		top(t),
		right(r),
		bottom(b),
		front(0),
		back(1)
	{
		assert(right >= left && bottom >= top && back >= front);
	}
	/** Define a box from left, top, front, right, bottom and back
	coordinates.
	@param	l	x value of left edge
	@param	t	y value of top edge
	@param  ff  z value of front edge
	@param	r	x value of right edge
	@param	b	y value of bottom edge
	@param  bb  z value of back edge
	@note Note that the left, top, and front edges are included
	but the right, bottom and back ones are not.
	*/
	Box3D(uint32 l, uint32 t, uint32 ff, uint32 r, uint32 b, uint32 bb) :
		left(l),
		top(t),
		right(r),
		bottom(b),
		front(ff),
		back(bb)
	{
		assert(right >= left && bottom >= top && back >= front);
	}

	/// Return true if the other box is a part of this one
	bool contains(const Box3D &def) const
	{
		return (def.left >= left && def.top >= top && def.front >= front &&
			def.right <= right && def.bottom <= bottom && def.back <= back);
	}

	/// Get the width of this box
	uint32 getWidth() const { return right - left; }
	/// Get the height of this box
	uint32 getHeight() const { return bottom - top; }
	/// Get the depth of this box
	uint32 getDepth() const { return back - front; }
};

NS_END