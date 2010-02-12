/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */

#include <stdio.h>
#include <string.h>

#include <game/collision.hpp>

#include <game/client/gameclient.hpp>
#include <game/client/components/menus.hpp>

extern "C"
{
	#include <engine/e_engine.h>
}

#include "race_demo.hpp"

enum
{
	RACE_NONE = 0,
	RACE_STARTED,
	RACE_FINISHED,
};

RACE::RACE()
{
	race_state = RACE_NONE;
	time = 0;
}

void RACE::on_render()
{
	if(gameclient.snap.gameobj)
	{
		vec2 player_pos = gameclient.local_character_pos;
		
		if(col_is_begin(player_pos.x, player_pos.y) && race_state != RACE_FINISHED && config.cl_auto_record)
		{
			if(race_state == RACE_STARTED)
				on_reset();
			
			map = client_demo_record("tmp");
			race_state = RACE_STARTED;
		}
		
		if(col_is_end(player_pos.x, player_pos.y) && race_state == RACE_STARTED)
		{
			record_time = client_tick() + client_tickspeed();
			race_state = RACE_FINISHED;
		}
		
		if(race_state == RACE_FINISHED && record_time < client_tick() && time > 0)
		{
			on_reset();
			
			char name[256];
			str_format(name, sizeof(name), "%s_best_", map);
			
			gameclient.menus->demolist_populate();
			for(int i = 0; i < gameclient.menus->num_demos; i++)
			{
				if(strncmp(gameclient.menus->demos[i].name, name, str_length(name))==0)
				{
					const char *demo = gameclient.menus->demos[i].name;
					
					int len = str_length(name);
					char buf[10];
					for(int t = 0; t < 10; t++)
					{
						buf[t] = demo[t+len];
						if((demo[t+len]=='.' && demo[t+len+1]=='d') || demo[t+len] == 0)
						{
							buf[t] = 0;
							break;
						}
					}
					
					float demo_time = atof(&buf[0]);
					
					if(time < demo_time)
					{
						char filename[512];
						char old_filename[512];
						char buffer[512];
						str_format(buffer, sizeof(buffer), "demos/%s%5.3f.demo", name, time);
						engine_savepath(buffer, filename, sizeof(filename));
						str_format(buffer, sizeof(buffer), "demos/%s_tmp.demo", map);
						engine_savepath(buffer, old_filename, sizeof(old_filename));
						
						remove(gameclient.menus->demos[i].filename);
						rename(old_filename, filename);
					}
					
					time = 0;
					
					return;
				}
			}
			
			char filename[512];
			char old_filename[512];
			char buffer[512];
			str_format(buffer, sizeof(buffer), "demos/%s%5.3f.demo", name, time);
			engine_savepath(buffer, filename, sizeof(filename));
			str_format(buffer, sizeof(buffer), "demos/%s_tmp.demo", map);
			engine_savepath(buffer, old_filename, sizeof(old_filename));
			
			rename(old_filename, filename);
			
			time = 0;
		}
	}
}

void RACE::on_reset()
{
	if(race_state > RACE_NONE)
		client_demo_record_stop();
	
	if(race_state != RACE_FINISHED)
		time = 0;
	
	race_state = RACE_NONE;
	record_time = 0;
}

void RACE::on_message(int msgtype, void *rawmsg)
{
	if(msgtype == NETMSGTYPE_SV_KILLMSG)
	{
		NETMSG_SV_KILLMSG *msg = (NETMSG_SV_KILLMSG *)rawmsg;
		if(msg->victim == gameclient.snap.local_cid && race_state == RACE_STARTED)
			on_reset();
	}
	else if(msgtype == NETMSGTYPE_SV_CHAT)
	{
		NETMSG_SV_CHAT *msg = (NETMSG_SV_CHAT *)rawmsg;

		if(msg->cid == -1 && race_state == RACE_FINISHED)
		{
			SERVER_INFO current_server_info;
			client_serverinfo(&current_server_info);
			
			const char *message = msg->message;
			int len;
			
			if(strcmp(current_server_info.gametype, "HpRACE") == 0)
			{
				while(strncmp(message, gameclient.clients[gameclient.snap.local_cid].name, str_length(gameclient.clients[gameclient.snap.local_cid].name)) != 0)
				{
					message++;
					if(!message[0])
						return;
				}
				
				while(strncmp(message,  " finished in: ", 14) != 0)
				{
					message++;
					if(!message[0])
						return;
				}
				
				len = 14;
			}
			else
			{
				char text[128];
				str_format(text, sizeof(text), "%s finished in: ", gameclient.clients[gameclient.snap.local_cid].name);
				len = str_length(text);
			
				if(strncmp(msg->message, text, len) != 0)
					return;
			}
			
			char buf[10];
			
			for(int i = 0; i < 5; i++)
			{
				buf[i] = message[i+len];
				if(message[i+len]==' ' || message[i+len] == 0)
				{
					buf[i] = 0;
					len += i;
					break;
				}
			}
			int min = atoi(&buf[0]);
			len += 11;
			
			mem_zero(buf, sizeof(buf));
			for(int i = 0; i < 10; i++)
			{
				buf[i] = message[i+len];
				if(message[i+len]==' ' || message[i+len] == 0)
				{
					buf[i] = 0;
					break;
				}
			}
			time = min*60 + atof(&buf[0]);
			dbg_msg("", "%d", time);
		}
	}
}
