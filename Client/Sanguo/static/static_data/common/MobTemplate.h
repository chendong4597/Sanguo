#pragma once
#include <cstdint>

// MOB陣營
enum MOB_CAMP : int32_t
{
	MOB_CAMP_NEUTRAL = 0,		// 0 : 中立 
	MOB_CAMP_FRIENDLY = 1,		// 1 : 友好
	MOB_CAMP_HOSTILITY = 2,		// 2 : 敵對
};

// MOB品質
enum MOB_RARE : int32_t
{
	MOB_RARE_NONE = 0,		// 未設定 
	MOB_RARE_ORDINARY = 1,	// 普通怪物
	MOB_RARE_ELITE = 3,		// 精英怪物
	MOB_RARE_BOSS = 5,		// BOSS级怪物
};

// MOB種類
enum MOB_KIND : int32_t
{
	MOB_KIND_MOB = 0,	// 怪物
	MOB_KIND_ANIMAL,	// 小動物
	MOB_KIND_NPC		// NPC
};
