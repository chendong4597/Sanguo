
/*!
 * \file Debug.h
 * \date 5-2-2017 17:57:18
 * 
 * 
 * \author cs (04nycs@gmail.com)
 */

#pragma once

#include "../BaseType.h"

class Debug
{
public:
	enum Flag
	{
		none = 0,
		disable_player_update = 1 << 0,
		disable_player_render = 1 << 1,
		disable_monster_update = 1 << 2,
		disable_monster_render = 1 << 3,
		disable_npc_update = 1 << 4,
		disable_npc_render = 1 << 5,
		disable_map_update = 1 << 6,
		disable_map_render = 1 << 7,
		disable_movement = 1 << 8,
		disable_spine_update = 1 << 9,
		disable_spine_render =  1 << 10,
		disable_foreground_update = 1 << 11,
		disable_foreground_render = 1 << 12,
		disable_plant_update =  1 << 13,
		disable_plant_render = 1 << 14,
		disable_model_update = 1 << 15,
		disable_model_render = 1 << 16,
		disable_gameobject_update = 1 << 17,
		disable_component_update = 1 << 18,
		disable_liquid_update = 1 << 19,
		disable_particle_update = 1 << 20,
		disable_particle_render = 1 << 21,
		disable_hp_update = 1 << 22,
		disable_hp_render =  1 << 23,
		disable_ui_render =  1 << 24,
		disable_ui_3drender = 1 << 25,
		disable_delay_show = 1 << 26,
		disable_ui_notify = 1 << 27,
		disable_ui_Toast = 1 << 28,

		debugger_visible = 1 << 31,
	};

	static bool isFlagSet(bzbee::uint flags);
	static void setFlag(bzbee::uint flags, bool set);
	static bzbee::uint getFlag() { return s_flags; };

	static float getTimeScale();
	static void setTimeScale(float scale);

	static bool playControl();
	static bool isPause() { return s_bPause; };
	static void setStepInto() { s_bPause = true; s_bStepInto = true; };
	static void setPause(bool bPause) { s_bPause = bPause; };

	static void setRenderOrderMax(int order) { m_nRenderOrderMax = order; };
	static int getRenderOrderMax() { return m_nRenderOrderMax; };

	static void setRenderOrderMin(int order) { m_nRenderOrderMin = order; }; 
	static int getRenderOrderMin() { return m_nRenderOrderMin; };

private:
	static bzbee::uint s_flags;
	static float s_timeScale;

	static bool s_bPause;
	static bool s_bStepInto;

	static int m_nRenderOrderMax;
	static int m_nRenderOrderMin;
};