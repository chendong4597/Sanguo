#include "stdafx.h"
#include "CommonDataTypeDef.h"


//===========================================================================
//
// Implement of CIntRect
//
//===========================================================================
const CIntRect CIntRect::ZERO = CIntRect(0, 0, 0, 0);

CIntRect::CIntRect() :
	left(0),
	top(0),
	right(0),
	bottom(0)
{

}

//---------------------------------------------------------------------------
CIntRect::CIntRect(int32 inLeft, int32 inTop, int32 inRight, int32 inBottom) :
	left(inLeft),
	top(inTop),
	right(inRight),
	bottom(inBottom)
{

}

//---------------------------------------------------------------------------
void CIntRect::reset(int32 inLeft, int32 inTop, int32 inRight, int32 inBottom)
{
	left = inLeft;
	top = inTop;
	right = inRight;
	bottom = inBottom;
}

//---------------------------------------------------------------------------
bool CIntRect::operator ==(const CIntRect& rect) const
{
	return (this->left == rect.left && this->top == rect.top && this->right == rect.right && this->bottom == rect.bottom) ? true : false;
}

//---------------------------------------------------------------------------
CIntRect& CIntRect::operator =(const CIntRect& rect)
{
	left = rect.left;
	top = rect.top;
	right = rect.right;
	bottom = rect.bottom;
	return *this;
}


const CIntPoint CIntPoint::ZERO = CIntPoint(0, 0);

CIntPoint::CIntPoint() :
	left(0),
	top(0)
{

}

//---------------------------------------------------------------------------
CIntPoint::CIntPoint(int32 inLeft, int32 inTop) :
	left(inLeft),
	top(inTop)
{

}

////---------------------------------------------------------------------------
//CIntPoint::CIntPoint(const Vector2& v) :
//	left(static_cast<int32>(v.x)),
//	top(static_cast<int32>(v.y))
//{
//
//}

//---------------------------------------------------------------------------
CIntPoint::CIntPoint(const char* str)
{
	ReadFromString(str);
}

//---------------------------------------------------------------------------
void CIntPoint::reset(int32 inLeft, int32 inTop)
{
	left = inLeft;
	top = inTop;
}

//---------------------------------------------------------------------------
void CIntPoint::ReadFromString(const char* str)
{
	sscanf(str, "%d,%d", &left, &top);
}

////---------------------------------------------------------------------------
//Vector2 CIntPoint::toVector2() const
//{
//	return Vector2(static_cast<float>(left), static_cast<float>(top));
//}

//---------------------------------------------------------------------------
bool CIntPoint::operator ==(const CIntPoint& point) const
{
	return (this->left == point.left && this->top == point.top) ? true : false;
}

//---------------------------------------------------------------------------
bool CIntPoint::operator !=(const CIntPoint& point) const
{
	return (this->left != point.left || this->top != point.top) ? true : false;
}

//---------------------------------------------------------------------------
CIntPoint& CIntPoint::operator =(const CIntPoint& point)
{
	left = point.left;
	top = point.top;
	return *this;
}

//---------------------------------------------------------------------------
CIntPoint CIntPoint::operator +(const CIntPoint& point) const
{
	return CIntPoint(left + point.left, top + point.top);
}

//---------------------------------------------------------------------------
CIntPoint CIntPoint::operator -(const CIntPoint& point) const
{
	return CIntPoint(left - point.left, top - point.top);
}

//---------------------------------------------------------------------------
CIntPoint& CIntPoint::operator +=(const CIntPoint& point)
{
	left += point.left;
	top += point.top;
	return *this;
}

//---------------------------------------------------------------------------
CIntPoint& CIntPoint::operator -=(const CIntPoint& point)
{
	left -= point.left;
	top -= point.top;
	return *this;
}

//---------------------------------------------------------------------------
CIntPoint CIntPoint::operator -() const
{
	return CIntPoint(-left, -top);
}

//---------------------------------------------------------------------------
bool CIntPoint::operator < (const CIntPoint& r)const
{
	if (this->x < r.x)
		return true;
	else if (this->x > r.x)
		return false;
	if (this->y < r.y)
		return true;
	return false;
}




//===========================================================================
//
// Implement of CIntSize
//
//===========================================================================
const CIntSize CIntSize::ZERO = CIntSize(0, 0);

CIntSize::CIntSize() :
	width(0),
	height(0)
{

}

//---------------------------------------------------------------------------
CIntSize::CIntSize(int32 inWidth, int32 inHeight) :
	width(inWidth),
	height(inHeight)
{

}

//---------------------------------------------------------------------------
void CIntSize::reset(int32 inWidth, int32 inHeight)
{
	width = inWidth;
	height = inHeight;
}

////---------------------------------------------------------------------------
//void CIntSize::ReadFromString(const char* str)
//{
//	sscanf_s(str, "%d,%d", &width, &height);
//}

//---------------------------------------------------------------------------
bool CIntSize::operator ==(const CIntSize& size) const
{
	return (this->width == size.width && this->height == size.height) ? true : false;
}

//---------------------------------------------------------------------------
CIntSize& CIntSize::operator =(const CIntSize& size)
{
	width = size.width;
	height = size.height;
	return *this;
}
