#include <string.h>

#include <engine/e_client_interface.h>

#include <game/generated/g_protocol.hpp>
#include <game/generated/gc_data.hpp>

#include <game/version.hpp>
#include <game/client/ui.hpp>
#include <game/client/render.hpp>
#include <game/client/gameclient.hpp>
#include <game/client/animstate.hpp>

#include "binds.hpp"
#include "menus.hpp"
#include "skins.hpp"
#include "items.hpp"

#include <game/client/teecomp.hpp>

void MENUS::render_rgb_sliders(RECT* main_view, RECT* button, int &r, int &g, int &b, bool enabled)
{
	const char *labels[] = {"R.", "G.", "B."};
	int *color_slider[3] = {&r, &g, &b};
	for(int i=0; i<3; i++)
	{
		RECT text;
		ui_hsplit_t(main_view, 19.0f, button, main_view);
		ui_vmargin(button, 15.0f, button);
		ui_vsplit_l(button, 30.0f, &text, button);
		ui_vsplit_r(button, 5.0f, button, 0);
		ui_hsplit_t(button, 4.0f, 0, button);

		if(enabled)
		{
			float k = (*color_slider[i]) / 255.0f;
			k = ui_do_scrollbar_h(color_slider[i], button, k);
			*color_slider[i] = (int)(k*255.0f);
		}
		else
			ui_do_scrollbar_h(color_slider[i], button, 0);
		ui_do_label(&text, labels[i], 15.0f, -1);
	}
}

void MENUS::ui_do_keybinder(KEYINFO& key, RECT* r)
{
	RECT label, button;
	ui_hsplit_t(r, 20.0f, &button, r);
	ui_vsplit_r(&button, 5.0f, &button, 0);
	ui_vsplit_l(&button, 130.0f, &label, &button);

	ui_do_label(&label, key.name, 14.0f, -1);
	int oldid = key.keyid;
	int newid = ui_do_key_reader((void *)key.name, &button, oldid);
	if(newid != oldid)
	{
		gameclient.binds->bind(oldid, "");
		gameclient.binds->bind(newid, key.command);
	}
	ui_hsplit_t(r, 5.0f, 0, r);
}

void MENUS::render_settings_teecomp(RECT main_view)
{
	RECT button;
	static int settings_page = 0;

	if(settings_page != 3)
	{
		ui_hsplit_b(&main_view, 20.0f, 0, &button);
		ui_vsplit_l(&button, main_view.w/3, &button, 0);
		static int default_button = 0;
		if(ui_do_button((void*)&default_button, "Reset to defaults", 0, &button, ui_draw_menu_button, 0))
			TeecompUtils::reset_config();

		ui_hsplit_b(&main_view, 10.0f, &main_view, &button);
		ui_do_label(&button, "http://spl0k.unreal-design.com/teeworlds/", 10.0f, 1);
		ui_hsplit_b(&main_view, 10.0f, &main_view, &button);
		char buf[64];
		str_format(buf, sizeof(buf), "Teeworlds %s with TeeComp %s by spl0k", GAME_VERSION, TEECOMP_VERSION);
		ui_do_label(&button, buf, 10.0f, 1);
		ui_hsplit_b(&main_view, 10.0f, &main_view, 0);
	}

	// render background
	RECT tabbar;
	ui_hsplit_t(&main_view, 24.0f, &tabbar, &main_view);
	//ui_draw_rect(&main_view, color_tabbar_active, CORNER_ALL, 10.0f);

	const char *tabs[] = {"Skins", "Stats", "Misc", "About"};
	int num_tabs = (int)(sizeof(tabs)/sizeof(*tabs));

	for(int i=0; i<num_tabs; i++)
	{
		ui_vsplit_l(&tabbar, 10.0f, &button, &tabbar);
		ui_vsplit_l(&tabbar, 80.0f, &button, &tabbar);
		int corners = CORNER_ALL;
		if(ui_do_button(tabs[i], tabs[i], settings_page == i, &button, ui_draw_menu_tab_button, &corners))
			settings_page = i;
	}

	if(settings_page != 3)
		ui_margin(&main_view, 10.0f, &main_view);
	
	if(settings_page == 0)
		render_settings_teecomp_skins(main_view);
	else if(settings_page == 1)
		render_settings_teecomp_stats(main_view);
	else if(settings_page == 2)
		render_settings_teecomp_misc(main_view);
	else if(settings_page == 3)
		render_settings_teecomp_about(main_view);
}

void MENUS::render_settings_teecomp_skins(RECT main_view)
{
	RECT button, left_view, right_view;
	ui_vsplit_l(&main_view, main_view.w/2, &left_view, &right_view);

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_colored_tees_method, "Enemy based colors", config.tc_colored_tees_method, &button, ui_draw_checkbox, 0))
		config.tc_colored_tees_method ^= 1;

	// Colors team 1

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	char buf[128];
	str_format(buf, sizeof(buf), "Use DM colors for team %s", (config.tc_colored_tees_method)?"mates":"1");
	if(ui_do_button(&config.tc_dm_colors_team1, buf, config.tc_dm_colors_team1, &button, ui_draw_checkbox, 0))
		config.tc_dm_colors_team1 ^= 1;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	ui_do_label(&button, (config.tc_colored_tees_method)?"Team mates":"Team 1", 14.0f, -1);
	int r1, g1, b1, r2, g2, b2;
	r1 = config.tc_colored_tees_team1>>16;
	g1 = (config.tc_colored_tees_team1>>8)&0xff;
	b1 = config.tc_colored_tees_team1&0xff;
	render_rgb_sliders(&left_view, &button, r1, g1, b1, !config.tc_dm_colors_team1);
	config.tc_colored_tees_team1 = (r1<<16) + (g1<<8) + b1;

	const SKINS::SKIN *s = gameclient.skins->get(max(0, gameclient.skins->find(config.tc_forced_skin1)));
	TEE_RENDER_INFO info;
	if(!config.tc_dm_colors_team1)
	{
		info.texture = s->color_texture;
		info.color_body = vec4(r1/255.0f, g1/255.0f, b1/255.0f, 1.0f);
		info.color_feet = vec4(r1/255.0f, g1/255.0f, b1/255.0f, 1.0f);
	}
	else
	{
		info.texture = s->org_texture;
		info.color_body = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		info.color_feet = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	info.size = ui_scale()*50.f;

	ui_hsplit_t(&button, 70.0f, 0, &button);
	render_tee(ANIMSTATE::get_idle(), &info, 0, vec2(1, 0), vec2(button.x, button.y+button.h/2));
	ui_hsplit_t(&left_view, 50.0f, 0, &left_view);

	// Colors team 2

	ui_hsplit_t(&right_view, 20.0f, 0, &right_view);
	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	str_format(buf, sizeof(buf), "Use DM colors for %s", (config.tc_colored_tees_method)?"enemies":"team 2");
	if(ui_do_button(&config.tc_dm_colors_team2, buf, config.tc_dm_colors_team2, &button, ui_draw_checkbox, 0))
		config.tc_dm_colors_team2 ^= 1;

	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	ui_do_label(&button, (config.tc_colored_tees_method)?"Enemies":"Team 2", 14.0f, -1);
	r2 = config.tc_colored_tees_team2>>16;
	g2 = (config.tc_colored_tees_team2>>8)&0xff;
	b2 = config.tc_colored_tees_team2&0xff;
	render_rgb_sliders(&right_view, &button, r2, g2, b2, !config.tc_dm_colors_team2);
	config.tc_colored_tees_team2 = (r2<<16) + (g2<<8) + b2;

	s = gameclient.skins->get(max(0, gameclient.skins->find(config.tc_forced_skin2)));
	if(!config.tc_dm_colors_team2)
	{
		info.texture = s->color_texture;
		info.color_body = vec4(r2/255.0f, g2/255.0f, b2/255.0f, 1.0f);
		info.color_feet = vec4(r2/255.0f, g2/255.0f, b2/255.0f, 1.0f);
	}
	else
	{
		info.texture = s->org_texture;
		info.color_body = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		info.color_feet = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	ui_hsplit_t(&button, 70.0f, 0, &button);
	render_tee(ANIMSTATE::get_idle(), &info, 0, vec2(1, 0), vec2(button.x, button.y+button.h/2));
	ui_hsplit_t(&right_view, 50.0f, 0, &right_view);

	// Force skins team 1

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_forced_skins_method, "Enemy based skins", config.tc_forced_skins_method, &button, ui_draw_checkbox, 0))
		config.tc_forced_skins_method ^= 1;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	str_format(buf, sizeof(buf), "Force team %s/FFA skins", (config.tc_forced_skins_method)?"mates":"1");
	if(ui_do_button(&config.tc_force_skin_team1, buf, config.tc_force_skin_team1, &button, ui_draw_checkbox, 0))
		config.tc_force_skin_team1 ^= 1;

	RECT skinselection, scroll;
	ui_margin(&left_view, 10.0f, &skinselection);

	ui_hsplit_t(&skinselection, 20, &button, &skinselection);
	ui_draw_rect(&button, vec4(1,1,1,0.25f), CORNER_T, 5.0f); 
	ui_do_label(&button, "Forced skin", 14.0f, 0);

	ui_draw_rect(&skinselection, vec4(0,0,0,0.15f), 0, 0);
	ui_vsplit_r(&skinselection, 15, &skinselection, &scroll);

	RECT list = skinselection;
	ui_hsplit_t(&list, 20, &button, &list);
	
	int num = (int)(skinselection.h/button.h);
	static float scrollvalue = 0;
	static int scrollbar = 0;
	ui_hmargin(&scroll, 5.0f, &scroll);
	scrollvalue = ui_do_scrollbar_v(&scrollbar, &scroll, scrollvalue);

	int start = (int)((gameclient.skins->num()-num)*scrollvalue);
	if(start < 0)
		start = 0;

	for(int i=start; i<start+num && i<gameclient.skins->num(); i++)
	{
		const SKINS::SKIN *s = gameclient.skins->get(i);

		str_format(buf, sizeof(buf), "%s", s->name);
		int selected = 0;
		if(strcmp(s->name, config.tc_forced_skin1) == 0)
			selected = 1;

		if(ui_do_button(s, "", selected, &button, ui_draw_list_row, 0))
			str_copy(config.tc_forced_skin1, s->name, sizeof(config.tc_forced_skin1));

		ui_vmargin(&button, 5.0f, &button);
		ui_hsplit_t(&button, 1.0f, 0, &button);
		ui_do_label(&button, buf, 14.0f, -1);

		ui_hsplit_t(&list, 20.0f, &button, &list);
	}

	// Forced skin team 2

	ui_hsplit_t(&right_view, 20.0f, 0, &right_view);
	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	str_format(buf, sizeof(buf), "Force %s skins", (config.tc_forced_skins_method)?"enemies":"team 2");
	if(ui_do_button(&config.tc_force_skin_team2, buf, config.tc_force_skin_team2, &button, ui_draw_checkbox, 0))
		config.tc_force_skin_team2 ^= 1;

	ui_margin(&right_view, 10.0f, &skinselection);

	ui_hsplit_t(&skinselection, 20, &button, &skinselection);
	ui_draw_rect(&button, vec4(1,1,1,0.25f), CORNER_T, 5.0f); 
	ui_do_label(&button, "Forced skin", 14.0f, 0);

	ui_draw_rect(&skinselection, vec4(0,0,0,0.15f), 0, 0);
	ui_vsplit_r(&skinselection, 15, &skinselection, &scroll);

	list = skinselection;
	ui_hsplit_t(&list, 20, &button, &list);
	
	num = (int)(skinselection.h/button.h);
	static float scrollvalue2 = 0;
	static int scrollbar2 = 0;
	ui_hmargin(&scroll, 5.0f, &scroll);
	scrollvalue2 = ui_do_scrollbar_v(&scrollbar2, &scroll, scrollvalue2);

	start = (int)((gameclient.skins->num()-num)*scrollvalue2);
	if(start < 0)
		start = 0;

	for(int i=start; i<start+num && i<gameclient.skins->num(); i++)
	{
		const SKINS::SKIN *s = gameclient.skins->get(i);

		str_format(buf, sizeof(buf), "%s", s->name);
		int selected = 0;
		if(strcmp(s->name, config.tc_forced_skin2) == 0)
			selected = 1;

		if(ui_do_button(s+gameclient.skins->num(), "", selected, &button, ui_draw_list_row, 0))
			str_copy(config.tc_forced_skin2, s->name, sizeof(config.tc_forced_skin2));

		ui_vmargin(&button, 5.0f, &button);
		ui_hsplit_t(&button, 1.0f, 0, &button);
		ui_do_label(&button, buf, 14.0f, -1);

		ui_hsplit_t(&list, 20.0f, &button, &list);
	}
}

void MENUS::render_settings_teecomp_stats(RECT main_view)
{
	RECT button, left_view;

	ui_vsplit_l(&main_view, main_view.w/2, &left_view, &main_view);

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	ui_do_label(&button, "Show in global statboard:", 16.0f, -1);

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_statboard_infos, "Frags", config.tc_statboard_infos & TC_STATS_FRAGS, &button, ui_draw_checkbox, 0))
		config.tc_statboard_infos ^= TC_STATS_FRAGS;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_statboard_infos+1, "Deaths", config.tc_statboard_infos & TC_STATS_DEATHS, &button, ui_draw_checkbox, 0))
		config.tc_statboard_infos ^= TC_STATS_DEATHS;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_statboard_infos+2, "Suicides", config.tc_statboard_infos & TC_STATS_SUICIDES, &button, ui_draw_checkbox, 0))
		config.tc_statboard_infos ^= TC_STATS_SUICIDES;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_statboard_infos+3, "Ratio", config.tc_statboard_infos & TC_STATS_RATIO, &button, ui_draw_checkbox, 0))
		config.tc_statboard_infos ^= TC_STATS_RATIO;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_statboard_infos+4, "Net score", config.tc_statboard_infos & TC_STATS_NET, &button, ui_draw_checkbox, 0))
		config.tc_statboard_infos ^= TC_STATS_NET;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_statboard_infos+5, "Frags per minute", config.tc_statboard_infos & TC_STATS_FPM, &button, ui_draw_checkbox, 0))
		config.tc_statboard_infos ^= TC_STATS_FPM;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_statboard_infos+6, "Weapons stats", config.tc_statboard_infos & TC_STATS_WEAPS, &button, ui_draw_checkbox, 0))
		config.tc_statboard_infos ^= TC_STATS_WEAPS;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_statboard_infos+7, "Flag captures", config.tc_statboard_infos & TC_STATS_FLAGS, &button, ui_draw_checkbox, 0))
		config.tc_statboard_infos ^= TC_STATS_FLAGS;

	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "Key bindings:", 16.0f, -1);
	KEYINFO keys[] = {{ "Global statboard:", "+stats 1", 0},
		{ "Player board:", "+stats 2", 0},
		{ "Next player:", "+next_stats", 0}};

	for(int keyid=0; keyid<KEY_LAST; keyid++)
	{
		const char *bind = gameclient.binds->get(keyid);
		if(!bind[0])
			continue;

		for(unsigned int i=0; i<sizeof(keys)/sizeof(KEYINFO); i++)
			if(strcmp(bind, keys[i].command) == 0)
			{
				keys[i].keyid = keyid;
				break;
			}
	}

	for(unsigned int i=0; i<sizeof(keys)/sizeof(KEYINFO); i++)
		ui_do_keybinder(keys[i], &main_view);
}

void MENUS::render_settings_teecomp_misc(RECT main_view)
{
	RECT left_view, right_view, button;

	ui_vsplit_l(&main_view, main_view.w/2, &left_view, &right_view);

	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "Name plates", 16.0f, 0);
	ui_hsplit_t(&left_view, 20.0f, 0, &left_view);
	ui_hsplit_t(&right_view, 20.0f, 0, &right_view);

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_colored_nameplates, "Colored name plates", config.tc_colored_nameplates&1, &button, ui_draw_checkbox, 0))
		config.tc_colored_nameplates ^= 1;

	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	if(ui_do_button(&config.tc_colored_nameplates+1, "Enemy based colors", config.tc_colored_nameplates&2, &button, ui_draw_checkbox, 0))
		config.tc_colored_nameplates ^= 2;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	ui_do_label(&button, (config.tc_colored_nameplates&2)?"Team mates":"Team 1", 14.0f, -1);
	int r1, g1, b1, r2, g2, b2;
	r1 = config.tc_colored_nameplates_team1>>16;
	g1 = (config.tc_colored_nameplates_team1>>8)&0xff;
	b1 = config.tc_colored_nameplates_team1&0xff;
	render_rgb_sliders(&left_view, &button, r1, g1, b1, true);
	config.tc_colored_nameplates_team1 = (r1<<16) + (g1<<8) + b1;

	ui_hsplit_t(&left_view, 25.0f, &button, &left_view);
	ui_vsplit_l(&button, 30.0f, 0, &button);
	gfx_text_color(r1/255.0f, g1/255.0f, b1/255.0f, 1);
	ui_do_label(&button, (config.tc_colored_nameplates&2)?"Team mates":"Team 1", 15.0f, -1);
	gfx_text_color(1,1,1,1);

	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	ui_do_label(&button, (config.tc_colored_nameplates&2)?"Enemies":"Team 2", 14.0f, -1);
	r2 = config.tc_colored_nameplates_team2>>16;
	g2 = (config.tc_colored_nameplates_team2>>8)&0xff;
	b2 = config.tc_colored_nameplates_team2&0xff;
	render_rgb_sliders(&right_view, &button, r2, g2, b2, true);
	config.tc_colored_nameplates_team2 = (r2<<16) + (g2<<8) + b2;

	ui_hsplit_t(&right_view, 25.0f, &button, &right_view);
	ui_vsplit_l(&button, 30.0f, 0, &button);
	gfx_text_color(r2/255.0f, g2/255.0f, b2/255.0f, 1);
	ui_do_label(&button, (config.tc_colored_nameplates&2)?"Enemies":"Team 2", 15.0f, -1);
	gfx_text_color(1,1,1,1);

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_nameplate_shadow, "Show name plate shadow", config.tc_nameplate_shadow, &button, ui_draw_checkbox, 0))
		config.tc_nameplate_shadow ^= 1;

	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	if(ui_do_button(&config.tc_nameplate_score, "Show score in name plate", config.tc_nameplate_score, &button, ui_draw_checkbox, 0))
		config.tc_nameplate_score ^= 1;

	ui_hsplit_t(&left_view, 20.0f, 0, &left_view);
	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	ui_do_label(&button, "HUD/Flag", 16.0f, -1);

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_hud_match, "Make HUD match tees colors", config.tc_hud_match, &button, ui_draw_checkbox, 0))
		config.tc_hud_match ^= 1;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_colored_flags, "Make flags match tees colors", config.tc_colored_flags, &button, ui_draw_checkbox, 0))
		config.tc_colored_flags ^= 1;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	if(ui_do_button(&config.tc_speedmeter, "Display speed meter", config.tc_speedmeter, &button, ui_draw_checkbox, 0))
		config.tc_speedmeter ^= 1;

	ui_hsplit_t(&left_view, 20.0f, &button, &left_view);
	ui_vsplit_l(&button, 15.0f, 0, &button);
	if(ui_do_button(&config.tc_speedmeter_accel, "Speed meter show acceleration", config.tc_speedmeter_accel, &button, ui_draw_checkbox, 0))
		config.tc_speedmeter_accel ^= 1;

	ui_hsplit_t(&right_view, 20.0f, 0, &right_view);
	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	ui_do_label(&button, "Others", 16.0f, -1);

	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	if(ui_do_button(&config.tc_hide_carrying, "Hide flag while carrying it", config.tc_hide_carrying, &button, ui_draw_checkbox, 0))
		config.tc_hide_carrying ^= 1;

	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	if(ui_do_button(&config.tc_autodemo, "Enable automatic demo recording", config.tc_autodemo, &button, ui_draw_checkbox, 0))
		config.tc_autodemo ^= 1;

	ui_hsplit_t(&right_view, 20.0f, &button, &right_view);
	if(ui_do_button(&config.tc_autoscreen, "Enable end game automatic screenshot", config.tc_autoscreen, &button, ui_draw_checkbox, 0))
		config.tc_autoscreen ^= 1;
}

void MENUS::render_settings_teecomp_about(RECT main_view)
{
	RECT button;

	ui_hsplit_t(&main_view, 52.0f, &button, &main_view);
	ui_do_label(&button, "TeeComp", 48.0f, 0);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_vsplit_r(&button, button.w/3, 0, &button);
	ui_do_label(&button, "Version "TEECOMP_VERSION, 14.0f, -1);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_vsplit_r(&button, button.w/3, 0, &button);
	ui_do_label(&button, "For Teeworlds "GAME_VERSION, 14.0f, -1);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_vsplit_r(&button, button.w/3, 0, &button);
	ui_do_label(&button, "Compiled "__DATE__" "__TIME__, 14.0f, -1);

	ui_hsplit_t(&main_view, 40.0f, 0, &main_view);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "By Alban 'spl0k' FERON", 14.0f, 0);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "http://spl0k.unreal-design.com/", 14.0f, 0);

	ui_hsplit_t(&main_view, 20.0f, 0, &main_view);
	ui_hsplit_t(&main_view, 22.0f, &button, &main_view);
	ui_do_label(&button, "Special thanks to:", 16.0f, 0);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "Sd`", 14.0f, 0);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "Tho", 14.0f, 0);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "Eve", 14.0f, 0);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "some other MonkeyStyle members", 14.0f, 0);
	ui_hsplit_t(&main_view, 20.0f, &button, &main_view);
	ui_do_label(&button, "and the Teeworlds.com community", 14.0f, 0);

	ui_hsplit_b(&main_view, 10.0f, &main_view, &button);
	ui_do_label(&button, "so you can set while u set while u set options", 10.0f, -1);
	ui_hsplit_b(&main_view, 10.0f, &main_view, &button);
	ui_do_label(&button, "Yo dawg I herd you like tabs so we put tabs in yo tabs in yo tabs", 10.0f, -1);
}
