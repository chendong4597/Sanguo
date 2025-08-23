// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <algorithm>
#include <cmath>

#define CHANNEL_TPY_WINDOWS 1
#define CHANNEL_TPY_ANDROID 2
#define CHANNEL_TPY_ANDROID_TAP 3
#define CHANNEL_TPY_IOS		4
#define CHANNEL_TPY_IOS_TAP		5

#if PLATFORM_ANDROID
#define CHANNEL_PLAT CHANNEL_TPY_ANDROID_TAP
#elif PLATFORM_IOS
#define CHANNEL_PLAT CHANNEL_TPY_IOS_TAP
#else
#define CHANNEL_PLAT CHANNEL_TPY_WINDOWS
#endif

enum Weapon_Tpy {
	Weapon_Tpy_None			= 0 ,              //没有武器
	Weapon_Tpy_Sword		= 1,              //剑
	Weapon_Tpy_TwinSword	= 2,			  //双手剑
	Weapon_Tpy_Knife		= 3,              //刀
	Weapon_Tpy_Axe			= 4,              //斧子
	Weapon_Tpy_Blade		= 5,              //大刀
	Weapon_Tpy_Claw 		= 6,              //爪子
	Weapon_Tpy_Darts		= 7,              //回旋镖
	Weapon_Tpy_Staff		= 8,              //法杖
	Weapon_Tpy_Ball			= 9,              //法球
	Weapon_Tpy_Book			= 10,             //法书
	Weapon_Tpy_Gun			= 11,             //枪
	Weapon_Tpy_Cannon		= 12,             //大炮
	Weapon_Tpy_Bow  		= 13,             //弓
	Weapon_Tpy_Hammer       = 14,             //大锤子
	Weapon_Tpy_Bomb			= 15,             //炸弹
	Weapon_Tpy_Spear		= 16,             //古代枪
	Weapon_Tpy_Shield		= 17,             //盾牌
	Weapon_Tpy_Quiver		= 18,             //箭匣
	Weapon_Tpy_Horse		= 19,             //马
};


enum ENU_TOUCH_UITPY {  //touch区域
	ENU_TOUCH_UITPY_NONE = 0,  //touch 无 控制玩家
	ENU_TOUCH_UITPY_BALL = 1,  //touch区域方向球
	ENU_TOUCH_UITPY_ATK = 2,  //touch攻击选项
	ENU_TOUCH_UITPY_MAP = 3,  //touch地图
};

enum ENU_ATK_ANI_TPY {  //技能动画类型
	ENU_ATK_ANI_TPY_NORMAL = 0,  //正常攻击
	ENU_ATK_ANI_TPY_CONTINUE = 1,  //持续攻击
};


// 浮点数最小精度
#define EPSILON 0.000001

class DamageRange
{
private:
	double MY_PI{ 3.141592627f };  // 定义圆周率为PI
public:
	//定义一个点类
	class Point {
	public:
		double x, y;
		Point() { x = y = 0; }  // 默认构造函数，将点赋值为(0, 0)
		Point(double _x, double _y) : x(_x), y(_y) {}
	};

	// 计算两点之间的距离
	double getDistance(Point A, Point B) {
		double dx = A.x - B.x, dy = A.y - B.y;
		return sqrt(dx * dx + dy * dy);
	}

//****************************************************************************************
//扇形区域判断
//****************************************************************************************
public:
	double calTheta(Point A, Point B, Point C)
	{
		// 计算向量AB和AC
		double AB_x = B.x - A.x;
		double AB_y = B.y - A.y;
		double AC_x = C.x - A.x;
		double AC_y = C.y - A.y;

		// 计算点积
		double dotProduct = AB_x * AC_x + AB_y * AC_y;

		// 计算向量AB和AC的模长
		double magnitudeAB = std::sqrt(AB_x * AB_x + AB_y * AB_y);
		double magnitudeAC = std::sqrt(AC_x * AC_x + AC_y * AC_y);

		// 计算夹角的余弦值
		double cosTheta = dotProduct / (magnitudeAB * magnitudeAC);

		// 计算夹角（弧度转角度）
		return std::acos(cosTheta) * 180.0 / MY_PI;
	}

	// 计算点P是否在以点O为中心的扇形范围内  S角度  R半径
	bool isInSector(Point A, Point B, Point C, double S, double R) { 
		if (getDistance(A, C) > R) return false;  // 如果点P距离中心点O的距离大于半径R，肯定不在扇形范围内

		// 判断夹角是否小于S
		return calTheta(A, B, C) < S;
	}

//****************************************************************************************
//圆形区域判断
//****************************************************************************************
public:
	// 计算点P是否在以点O为中心的圆形范围内
	bool IsInCircle(Point O, Point P, double R)
	{
		return getDistance(O, P) < R;
	}

//****************************************************************************************
//矩形区域判断
//****************************************************************************************
public:
	// 计算向量的长度
	double length(const Point& v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	// 归一化向量
	Point normalize(const Point& v) {
		double len = length(v);
		return { v.x / len, v.y / len };
	}

	// 计算两个向量的点积
	double dot(const Point& a, const Point& b) {
		return a.x * b.x + a.y * b.y;
	}

	// 计算两个向量的叉积
	double cross(const Point& a, const Point& b) {
		return a.x * b.y - a.y * b.x;
	}

	// 判断点C是否在矩形内
	bool isPointInsideRectangle(const Point& A, const Point& B, double W, double H, const Point& C) {
		// 计算AB的方向向量
		Point dir = { B.x - A.x, B.y - A.y };
		dir = normalize(dir);

		// 计算垂直于AB的向量
		Point perp = { -dir.y, dir.x };

		// 计算矩形的四个顶点
		Point rect[4];
		rect[0] = { A.x - perp.x * (H / 2), A.y - perp.y * (H / 2) };
		rect[1] = { rect[0].x + dir.x * W, rect[0].y + dir.y * W };
		rect[2] = { rect[1].x + perp.x * H, rect[1].y + perp.y * H };
		rect[3] = { rect[2].x - dir.x * W, rect[2].y - dir.y * W };

		// 判断点C是否在矩形内
		double cross1 = cross({ C.x - rect[0].x, C.y - rect[0].y }, { rect[1].x - rect[0].x, rect[1].y - rect[0].y });
		double cross2 = cross({ C.x - rect[1].x, C.y - rect[1].y }, { rect[2].x - rect[1].x, rect[2].y - rect[1].y });
		double cross3 = cross({ C.x - rect[2].x, C.y - rect[2].y }, { rect[3].x - rect[2].x, rect[3].y - rect[2].y });
		double cross4 = cross({ C.x - rect[3].x, C.y - rect[3].y }, { rect[0].x - rect[3].x, rect[0].y - rect[3].y });

		// 如果所有叉积的符号相同，则点C在矩形内
		if ((cross1 >= 0 && cross2 >= 0 && cross3 >= 0 && cross4 >= 0) ||
			(cross1 <= 0 && cross2 <= 0 && cross3 <= 0 && cross4 <= 0)) {
			return true;
		}

		return false;
	}


public:
	struct Vec2d
	{
		double x, y;

		Vec2d()
		{
			x = 0.0;
			y = 0.0;
		}
		Vec2d(double dx, double dy)
		{
			x = dx;
			y = dy;
		}
		void Set(double dx, double dy)
		{
			x = dx;
			y = dy;
		}
	};

	static bool IsPointOnLine(double px0, double py0, double px1, double py1, double px2, double py2)
	{
		bool flag = false;
		double d1 = (px1 - px0) * (py2 - py0) - (px2 - px0) * (py1 - py0);
		if ((fabs(d1) < EPSILON) && ((px0 - px1) * (px0 - px2) <= 0) && ((py0 - py1) * (py0 - py2) <= 0))
		{
			flag = true;
		}
		return flag;
	}

	static bool IsIntersect(double px1, double py1, double px2, double py2, double px3, double py3, double px4, double py4)
	{
		bool flag = false;
		double d = (px2 - px1) * (py4 - py3) - (py2 - py1) * (px4 - px3);
		if (d != 0)
		{
			double r = ((py1 - py3) * (px4 - px3) - (px1 - px3) * (py4 - py3)) / d;
			double s = ((py1 - py3) * (px2 - px1) - (px1 - px3) * (py2 - py1)) / d;
			if ((r >= 0) && (r <= 1) && (s >= 0) && (s <= 1))
			{
				flag = true;
			}
		}
		return flag;
	}

	static bool Point_In_Polygon_2D(double x, double y, const std::vector<Vec2d>& POL)
	{
		bool isInside = false;
		int  count = 0;

		double minX = DBL_MAX;
		for (int i = 0; i < POL.size(); i++)
		{
			minX = std::min(minX, POL[i].x);
		}

		double px = x;
		double py = y;
		double linePoint1x = x;
		double linePoint1y = y;
		double linePoint2x = minX - 10;
		double linePoint2y = y;
		for (int i = 0; i < POL.size() - 1; i++)
		{
			double cx1 = POL[i].x;
			double cy1 = POL[i].y;
			double cx2 = POL[i + 1].x;
			double cy2 = POL[i + 1].y;
			if (IsPointOnLine(px, py, cx1, cy1, cx2, cy2))
			{
				return true;
			}
			if (fabs(cy2 - cy1) < EPSILON)
			{
				continue;
			}
			if (IsPointOnLine(cx1, cy1, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
			{
				if (cy1 > cy2)
				{
					count++;
				}
			}
			else if (IsPointOnLine(cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
			{
				if (cy2 > cy1)
				{
					count++;
				}
			}
			else if (IsIntersect(cx1, cy1, cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
			{
				count++;
			}
		}
		if (count % 2 == 1)
		{
			isInside = true;
		}

		return isInside;
	}
};


