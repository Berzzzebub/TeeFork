#ifndef __TEECOMP_HPP_
#define __TEECOMP_HPP_

#include <base/vmath.hpp>

enum {
	TC_STATS_FRAGS=1,
	TC_STATS_DEATHS=2,
	TC_STATS_SUICIDES=4,
	TC_STATS_RATIO=8,
	TC_STATS_NET=16,
	TC_STATS_FPM=32,
	TC_STATS_WEAPS=64,
	TC_STATS_FLAGS=128
};

class TeecompUtils
{
public:
	static vec3 getTeamColor(int for_team, int local_team, int color1, int color2, int method);
	static bool get_forced_skin_name(int for_team, int local_team, const char*& skin_name);
	static bool get_force_dm_colors(int for_team, int local_team);
	static void reset_config();
	static const char* rgb_to_name(int rgb);
};

#endif
