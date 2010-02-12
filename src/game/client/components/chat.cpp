#include <string.h> // strcmp

#include <engine/e_client_interface.h>
#include <game/generated/g_protocol.hpp>
#include <game/generated/gc_data.hpp>

#include <game/client/gameclient.hpp>
#include <game/client/teecomp.hpp>

#include <game/client/components/sounds.hpp>

#include "chat.hpp"

void CHAT::on_statechange(int new_state, int old_state)
{
	if(old_state <= CLIENTSTATE_CONNECTING)
	{
		mode = MODE_NONE;
		for(int i = 0; i < MAX_LINES; i++)
			lines[i].time = 0;
		current_line = 0;
	}
}

void CHAT::con_say(void *result, void *user_data)
{
	((CHAT*)user_data)->say(0, console_arg_string(result, 0));
}

void CHAT::con_sayteam(void *result, void *user_data)
{
	((CHAT*)user_data)->say(1, console_arg_string(result, 0));
}

void CHAT::con_chat(void *result, void *user_data)
{
	const char *mode = console_arg_string(result, 0);
	if(strcmp(mode, "all") == 0)
		((CHAT*)user_data)->enable_mode(0);
	else if(strcmp(mode, "team") == 0)
		((CHAT*)user_data)->enable_mode(1);
	else
		dbg_msg("console", "expected all or team as mode");
}

void CHAT::on_console_init()
{
	MACRO_REGISTER_COMMAND("say", "r", CFGFLAG_CLIENT, con_say, this, "Say in chat");
	MACRO_REGISTER_COMMAND("say_team", "r", CFGFLAG_CLIENT, con_sayteam, this, "Say in team chat");
	MACRO_REGISTER_COMMAND("chat", "s", CFGFLAG_CLIENT, con_chat, this, "Enable chat with all/team mode");
}

bool CHAT::on_input(INPUT_EVENT e)
{
	if(mode == MODE_NONE)
		return false;

	if(e.flags&INPFLAG_PRESS && e.key == KEY_ESCAPE)
		mode = MODE_NONE;
	else if(e.flags&INPFLAG_PRESS && (e.key == KEY_RETURN || e.key == KEY_KP_ENTER))
	{
		if(input.get_string()[0])
			gameclient.chat->say(mode == MODE_ALL ? 0 : 1, input.get_string());
		mode = MODE_NONE;
	}
	else
		input.process_input(e);
	
	return true;
}


void CHAT::enable_mode(int team)
{
	if(mode == MODE_NONE)
	{
		if(team)
			mode = MODE_TEAM;
		else
			mode = MODE_ALL;
		
		input.clear();
		inp_clear_events();
	}
}

void CHAT::on_message(int msgtype, void *rawmsg)
{
	if(msgtype == NETMSGTYPE_SV_CHAT)
	{
		NETMSG_SV_CHAT *msg = (NETMSG_SV_CHAT *)rawmsg;
		const char *message = msg->message;
		
		// save last message for each player
		spam = false;
		
		if(!strcmp(last_msg[msg->cid], message) != 0)
			spam = true;
			
		strcpy(last_msg[msg->cid], message);
			
		// check if player is ignored
		char buf[64];
		strcpy(buf, config.cl_spammer_name);

		struct split sp = split(buf, ' '); 
		
		ignore_player = false;
		
		if(config.cl_block_spammer)
		{
			int i = 0;
			while (i < sp.count)
			{
				if(str_find_nocase(gameclient.clients[msg->cid].name, sp.pointers[i]) != 0)
				{
					ignore_player = true;
					break;
				}
				else
					i++;
			}
		}
		
		// check if message should be marked
		strcpy(buf, config.cl_search_name);

		struct split sp2 = split(buf, ' '); 
		
		contains_name = false;
		
		if(config.cl_change_color || config.cl_change_sound)
		{
			int i = 0;
			while (i < sp2.count)
			{
				if(str_find_nocase(message, sp2.pointers[i]) != 0)
				{
					contains_name = true;
					break;
				}
				else
					i++;
			}
		}
			
 		add_line(msg->cid, msg->team, msg->message);

		if(!spam && !ignore_player)
		{
			if((msg->cid >= 0) && config.cl_change_sound && contains_name)	
			{
				if(config.cl_chatsound)
					gameclient.sounds->play(SOUNDS::CHN_GUI, SOUND_TEE_CRY, 0, vec2(0,0));
			}
			else if(msg->cid >= 0)
			{
				if(config.cl_chatsound)
					gameclient.sounds->play(SOUNDS::CHN_GUI, SOUND_CHAT_CLIENT, 0, vec2(0,0));
			}
			else
			{
				if(config.cl_servermsgsound)
					gameclient.sounds->play(SOUNDS::CHN_GUI, SOUND_CHAT_SERVER, 0, vec2(0,0));
			}
		}
	}
}

void CHAT::add_line(int client_id, int team, const char *line)
{
	current_line = (current_line+1)%MAX_LINES;
	lines[current_line].time = time_get();
	lines[current_line].client_id = client_id;
	lines[current_line].team = team;
	lines[current_line].name_color = -2;
	lines[current_line].contains_name = 0;
	lines[current_line].ignore = 0;
	lines[current_line].spam = 0;
	
	if(config.cl_block_spammer && ignore_player)
		lines[current_line].ignore = 1;
	if(config.cl_anti_spam && spam)
		lines[current_line].spam = 1;
		
	if(client_id == -1) // server message
	{
		str_copy(lines[current_line].name, "*** ", sizeof(lines[current_line].name));
		str_format(lines[current_line].text, sizeof(lines[current_line].text), "%s", line);
	}
	else
	{
		if((gameclient.clients[client_id].team == -1) && contains_name)
		{
			lines[current_line].contains_name = 1;
 			lines[current_line].name_color = -1;
		}
		else if(gameclient.clients[client_id].team == -1)
			lines[current_line].name_color = -1;

 		if(gameclient.snap.gameobj && gameclient.snap.gameobj->flags&GAMEFLAG_TEAMS)
 		{
			if((gameclient.clients[client_id].team == 0) && contains_name)
			{
				lines[current_line].contains_name = 1;
 				lines[current_line].name_color = 0;
			}
			else if(gameclient.clients[client_id].team == 0)
				lines[current_line].name_color = 0;
			else if((gameclient.clients[client_id].team == 1) && contains_name)
			{
				lines[current_line].contains_name = 1;
				lines[current_line].name_color = 1;
			}
 			else if(gameclient.clients[client_id].team == 1)
 				lines[current_line].name_color = 1;
		}
		
		str_copy(lines[current_line].name, gameclient.clients[client_id].name, sizeof(lines[current_line].name));
		str_format(lines[current_line].text, sizeof(lines[current_line].text), ": %s", line);
	}
	
	dbg_msg("chat", "%s%s", lines[current_line].name, lines[current_line].text);
}

void CHAT::on_render()
{
	gfx_mapscreen(0,0,300*gfx_screenaspect(),300);
	float x = 10.0f;
	float y = 300.0f-20.0f;
	if(mode != MODE_NONE)
	{
		// render chat input
		TEXT_CURSOR cursor;
		gfx_text_set_cursor(&cursor, x, y, 8.0f, TEXTFLAG_RENDER);
		cursor.line_width = 200.0f;
		
		if(mode == MODE_ALL)
			gfx_text_ex(&cursor, "All: ", -1);
		else if(mode == MODE_TEAM)
			gfx_text_ex(&cursor, "Team: ", -1);
		else
			gfx_text_ex(&cursor, "Chat: ", -1);
			
		gfx_text_ex(&cursor, input.get_string(), input.cursor_offset());
		TEXT_CURSOR marker = cursor;
		gfx_text_ex(&marker, "|", -1);
		gfx_text_ex(&cursor, input.get_string()+input.cursor_offset(), -1);
	}

	y -= 8;

	int i;
	for(i = 0; i < MAX_LINES; i++)
	{
		int r = ((current_line-i)+MAX_LINES)%MAX_LINES;
		if(time_get() > lines[r].time+15*time_freq())
			break;

		float begin = x;
		float fontsize = 7.0f;
		
		// get the y offset
		TEXT_CURSOR cursor;
		gfx_text_set_cursor(&cursor, begin, 0, fontsize, 0);
		cursor.line_width = 200.0f;
		gfx_text_ex(&cursor, lines[r].name, -1);
		gfx_text_ex(&cursor, lines[r].text, -1);
		if(!lines[r].spam && !lines[r].ignore)
		{
			if(config.cl_render_chat && !config.cl_render_servermsg && !(lines[r].client_id == -1))
				y -= cursor.y + cursor.font_size;
			else if(!config.cl_render_chat && config.cl_render_servermsg && (lines[r].client_id == -1))
				y -= cursor.y + cursor.font_size;
			else if(config.cl_render_chat && config.cl_render_servermsg)
				y -= cursor.y + cursor.font_size;
		}

		// cut off if msgs waste too much space
		if(y < 200.0f)
			break;
		
		// reset the cursor
		gfx_text_set_cursor(&cursor, begin, y, fontsize, TEXTFLAG_RENDER);
		cursor.line_width = 200.0f;

		// render name
		if(!config.cl_clear_all)
		{
			vec3 tcolor;
			gfx_text_color(0.8f,0.8f,0.8f,1);
			if(lines[r].client_id == -1)
				gfx_text_color(1,1,0.5f,1); // system
			else if(lines[r].team)
				gfx_text_color(0.45f,0.9f,0.45f,1); // team message
			else if(lines[r].name_color == 0)
			{
				if(!gameclient.snap.local_info)
					gfx_text_color(1.0f,0.5f,0.5f,1); // red
				else
				{
					tcolor = TeecompUtils::getTeamColor(0, gameclient.snap.local_info->team, config.tc_colored_tees_team1,
							config.tc_colored_tees_team2, config.tc_colored_tees_method);
					gfx_text_color(tcolor.r, tcolor.g, tcolor.b, 1);
				}
			}
			else if(lines[r].name_color == 1)
			{
				if(!gameclient.snap.local_info)
					gfx_text_color(0.7f,0.7f,1.0f,1); // blue
				else
				{
					tcolor = TeecompUtils::getTeamColor(1, gameclient.snap.local_info->team, config.tc_colored_tees_team1,
							config.tc_colored_tees_team2, config.tc_colored_tees_method);
					gfx_text_color(tcolor.r, tcolor.g, tcolor.b, 1);
				}	
			}
			else if(lines[r].name_color == -1)
				gfx_text_color(0.75f,0.5f,0.75f, 1); // spectator
			
			// render name
			if(!lines[r].spam && !lines[r].ignore)
			{
				if(config.cl_render_chat && !config.cl_render_servermsg && !(lines[r].client_id == -1))
					gfx_text_ex(&cursor, lines[r].name, -1);
				else if(!config.cl_render_chat && config.cl_render_servermsg && (lines[r].client_id == -1))
					gfx_text_ex(&cursor, lines[r].name, -1);
				else if(config.cl_render_chat && config.cl_render_servermsg)
					gfx_text_ex(&cursor, lines[r].name, -1);
			}

			// render line
			if(lines[r].contains_name && config.cl_change_color)
				gfx_text_color(0.6f,0.6f,0.6f,1); // standard color if name
			else
				gfx_text_color(1,1,1,1);
			if(lines[r].client_id == -1)
				gfx_text_color(1,1,0.5f,1); // system
			else if(lines[r].team && lines[r].contains_name && config.cl_change_color)
				gfx_text_color(0.3f,1,0.3f,1); // team color if name
			else if(lines[r].team)
				gfx_text_color(0.65f,1,0.65f,1); // team message
			
			if(!lines[r].spam && !lines[r].ignore)
			{
				if(config.cl_render_chat && !config.cl_render_servermsg && !(lines[r].client_id == -1))
					gfx_text_ex(&cursor, lines[r].text, -1);
				else if(!config.cl_render_chat && config.cl_render_servermsg && (lines[r].client_id == -1))
					gfx_text_ex(&cursor, lines[r].text, -1);
				else if(config.cl_render_chat && config.cl_render_servermsg)
					gfx_text_ex(&cursor, lines[r].text, -1);
			}
		}
	}

	gfx_text_color(1,1,1,1);
}

void CHAT::say(int team, const char *line)
{
	// send chat message
	NETMSG_CL_SAY msg;
	msg.team = team;
	msg.message = line;
	msg.pack(MSGFLAG_VITAL);
	client_send_msg();
}
