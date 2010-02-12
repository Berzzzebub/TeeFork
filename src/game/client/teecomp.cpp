#include "teecomp.hpp"
#include <engine/e_config.h>
#include <base/math.hpp>
#include <base/system.h>

vec3 TeecompUtils::getTeamColor(int for_team, int local_team, int color1, int color2, int method)
{
	vec3 c1((color1>>16)/255.0f, ((color1>>8)&0xff)/255.0f, (color1&0xff)/255.0f);
	vec3 c2((color2>>16)/255.0f, ((color2>>8)&0xff)/255.0f, (color2&0xff)/255.0f);

	// Team based colors or spectating
	if(!method || local_team == -1)
	{
		if(for_team == 0)
			return c1;
		return c2;
	}

	// Enemy based colors
	if(for_team == local_team)
		return c1;
	return c2;
}

bool TeecompUtils::get_forced_skin_name(int for_team, int local_team, const char*& skin_name)
{
	// Team based colors or spectating
	if(!config.tc_forced_skins_method || local_team == -1)
	{
		if(for_team == 0)
		{
			skin_name = config.tc_forced_skin1;
			return config.tc_force_skin_team1;
		}
		skin_name = config.tc_forced_skin2;
		return config.tc_force_skin_team2;
	}

	// Enemy based colors
	if(for_team == local_team)
	{
		skin_name = config.tc_forced_skin1;
		return config.tc_force_skin_team1;
	}
	skin_name = config.tc_forced_skin2;
	return config.tc_force_skin_team2;
}

bool TeecompUtils::get_force_dm_colors(int for_team, int local_team)
{
	if(!config.tc_colored_tees_method || local_team == -1)
	{
		if(for_team == 0)
			return config.tc_dm_colors_team1;
		return config.tc_dm_colors_team2;
	}

	if(for_team == local_team)
		return config.tc_dm_colors_team1;
	return config.tc_dm_colors_team2;
}

void TeecompUtils::reset_config()
{
	#define MACRO_CONFIG_INT(name, def, min, max, flags, desc) config.name = def;
	#define MACRO_CONFIG_STR(name, len, def, flags, desc) str_copy(config.name, def, len);
	#include "../teecomp_vars.hpp"
	#undef MACRO_CONFIG_INT
	#undef MACRO_CONFIG_STR
}

static vec3 rgb_to_hsl(vec3 rgb)
{
	float r = rgb.r;
	float g = rgb.g;
	float b = rgb.b;

	float vMin = min(min(r, g), b);
	float vMax = max(max(r, g), b);
	float dMax = vMax - vMin;

	float h;
	float s;
	float l = (vMax + vMin) / 2.0f;

	if(dMax == 0.0f)
	{
		h = 0.0f;
		s = 0.0f;
	}
	else
	{
		if(l < 0.5f)
			s = dMax / (vMax + vMin);
		else
			s = dMax / (2 - vMax - vMin);

		float dR = (((vMax - r) / 6.0f) + (dMax / 2.0f)) / dMax;
		float dG = (((vMax - g) / 6.0f) + (dMax / 2.0f)) / dMax;
		float dB = (((vMax - b) / 6.0f) + (dMax / 2.0f)) / dMax;

		if(r == vMax)
			h = dB - dG;
		else if(g == vMax)
			h = (1.0f/3.0f) + dR - dB;
		else if(b == vMax)
			h = (2.0f/3.0f) + dG - dR;

		if(h < 0.0f)
			h += 1.0f;
		if(h > 1.0f)
			h -= 1.0f;
	}

	return vec3(h*360, s, l);
}

const char* TeecompUtils::rgb_to_name(int rgb)
{
	vec3 rgb_v((rgb>>16)/255.0f, ((rgb>>8)&0xff)/255.0f, (rgb&0xff)/255.0f);
	vec3 hsl = rgb_to_hsl(rgb_v);

	if(hsl.l < 0.2f)
		return "Black";
	if(hsl.l > 0.9f)
		return "White";
	if(hsl.s < 0.1f)
		return "Gray";
	if(hsl.h < 20)
		return "Red";
	if(hsl.h < 45)
		return "Orange";
	if(hsl.h < 70)
		return "Yellow";
	if(hsl.h < 155)
		return "Green";
	if(hsl.h < 260)
		return "Blue";
	if(hsl.h < 335)
		return "Purple";
	return "Red";
}