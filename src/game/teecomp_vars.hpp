MACRO_CONFIG_INT(tc_nameplate_shadow, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Enable name plate shadow")
MACRO_CONFIG_INT(tc_nameplate_score, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Display score on name plates")
MACRO_CONFIG_INT(tc_colored_nameplates, 0, 0, 3, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Enable colored name plates")
MACRO_CONFIG_INT(tc_colored_nameplates_team1, 16739179, 0, 16777215, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Red team/team mates name plate color")
MACRO_CONFIG_INT(tc_colored_nameplates_team2, 7053311, 0, 16777215, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Blue team/enemies name plate color")

MACRO_CONFIG_INT(tc_colored_tees_method, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Enable enemy based skin colors")
MACRO_CONFIG_INT(tc_dm_colors_team1, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Use DM colors for red team/team mates")
MACRO_CONFIG_INT(tc_dm_colors_team2, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Use DM colors for blue team/enemies")
MACRO_CONFIG_INT(tc_colored_tees_team1, 16739179, 0, 16777215, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Red team/team mates color")
MACRO_CONFIG_INT(tc_colored_tees_team2, 7053311, 0, 16777215, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Blue team/enemies color")

MACRO_CONFIG_INT(tc_forced_skins_method, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Enable enemy based forced skins")
MACRO_CONFIG_INT(tc_force_skin_team1, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Force a skin for red team/your team/DM matchs")
MACRO_CONFIG_INT(tc_force_skin_team2, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Force a skin for blue team/opponents")
MACRO_CONFIG_STR(tc_forced_skin1, 64, "default", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Forced skin for red/mates/DM matchs")
MACRO_CONFIG_STR(tc_forced_skin2, 64, "default", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Forced skin for blue/opponents")

MACRO_CONFIG_INT(tc_hud_match, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Make HUD match tees' colors")
MACRO_CONFIG_INT(tc_speedmeter, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Display speed meter")
MACRO_CONFIG_INT(tc_speedmeter_accel, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Speed meter shows acceleration")

MACRO_CONFIG_INT(tc_autodemo, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Enable auto demo recording")
MACRO_CONFIG_INT(tc_autoscreen, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Enable auto screenshot")

MACRO_CONFIG_INT(tc_colored_flags, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Make flags colors match tees colors")
MACRO_CONFIG_INT(tc_hide_carrying, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Hide the flag if you're carrying it")

MACRO_CONFIG_INT(tc_statboard_infos, 235, 1, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Mask of infos to display on the global statboard")
MACRO_CONFIG_INT(tc_stat_id, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show player id in statboards")
