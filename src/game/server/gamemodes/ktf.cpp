/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#include <engine/e_server_interface.h>
#include <game/server/gamecontext.hpp>
#include <game/mapitems.hpp>
#include <engine/e_config.h>
#include "ktf.hpp"
#include "ctf.hpp"

GAMECONTROLLER_KTF::GAMECONTROLLER_KTF()
{
	flag = 0;
	flag_keeper_id = -100;
	//flag = new FLAG(0);
	stand_type = 0;
	shift = 0;
	
	gametype = "KTF";
	game_flags = GAMEFLAG_FLAGS;
	
	point_counter = 0;
}

void GAMECONTROLLER_KTF::set_new_flagkeeper()
{
	int minscore = config.sv_scorelimit;
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(game.players[i] &&
			game.players[i]->team == 0 && 
			game.players[i]->client_id != flag_keeper_id &&
			game.players[i]->get_character() && 
			game.players[i]->get_character()->alive)
		{
			if(game.players[i]->score < minscore)
			minscore = game.players[i]->score;
		}
	}

	if(minscore != config.sv_scorelimit)
	{
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(game.players[i] && game.players[i]->team == 0)
			{
				if(game.players[i]->score == minscore)
				{
					flag_keeper_id = game.players[i]->client_id;
					return;
				}
			}
		}
	}
	flag_keeper_id = -100;
}

void GAMECONTROLLER_KTF::draw_dir_stars()
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(game.players[i] && game.players[i]->team == 0)
		{
			if(game.players[i]->get_character() && game.players[i]->client_id != flag_keeper_id && game.players[i]->get_character()->core.vel.y == 0.0f && game.players[i]->get_character()->core.vel.v == 0.0f)
			{
				if(distance(flag->carrying_character->core.pos, game.players[i]->get_character()->pos) < 800.0f)
					return;
				vec2 vect = normalize(flag->carrying_character->core.pos - game.players[i]->get_character()->pos);
				float angle = get_angle(vect);
				game.create_damageind(game.players[i]->get_character()->pos, angle + 5.1f, 1);
			}
		}
	}	
}

void GAMECONTROLLER_KTF::tick()
{
	// game still running?
	if ( flag && game_over_tick == -1 )
	{
		// reset the flag if it's dropped on some death tile
		if ( col_get((int)flag->pos.x, (int)flag->pos.y)&COLFLAG_DEATH )
		{
			set_new_flagkeeper();
			if(flag_keeper_id != -100)
			{
				game.create_sound_global(SOUND_CTF_RETURN);
				flag->reset();
				flag->pos = game.players[flag_keeper_id]->get_character()->pos;
				flag->stand_pos = flag->pos;
			}
			else
			{
				flag->destroy();
				flag = 0;
				return;
			}
		}
		
		// does someone carry the flag?
		if ( flag->carrying_character )
		{
			// update flag position
			flag->pos = flag->carrying_character->pos;
			
			// add 1 point per second for carrier
			point_counter++;
			if ( point_counter == 60 )
			{		
				//draw_dir_stars();
				// is there more than one player?
				//int num_players = 0;
				CHARACTER *ents[64];
				float radius = 800.0f;
				int num_players = game.world.find_entities(flag->pos, radius, (ENTITY**)ents, 64, NETOBJTYPE_CHARACTER);

				/*for(int i = 0; i < MAX_CLIENTS; i++)
				{
					if(game.players[i] )
					{
						num_players++;
					}
				}*/
				
				// only add score if there's more than one player
				if ( num_players > 1 )
				{
					// add score
					flag->carrying_character->player->score += num_players - 1;
				}

				point_counter = 0;
			}
		}
		// check if someone could get the flag
		else
		{
			CHARACTER *close_characters[MAX_CLIENTS];
			int num = game.world.find_entities(flag->pos, 32.0f, (ENTITY**)close_characters, MAX_CLIENTS, NETOBJTYPE_CHARACTER);
			for(int i = 0; i < num; i++)
			{
				if(!close_characters[i]->alive || col_intersect_line(flag->pos, close_characters[i]->pos, NULL, NULL))
					continue;
					
				// take the flag
				flag->grab_tick = server_tick();
				flag->at_stand = 0;
				flag->carrying_character = close_characters[i];

				// tell everyone
				char buf[512];
				str_format(buf, sizeof(buf), "%s got the flag!", server_clientname(flag->carrying_character->player->client_id));
				game.send_broadcast(buf, -1);
				
				dbg_msg("game", "flag_grab player='%d:%s'",
					flag->carrying_character->player->client_id,
					server_clientname(flag->carrying_character->player->client_id));
					
				for(int c = 0; c < MAX_CLIENTS; c++)
				{
					if(!game.players[c])
						continue;

					if( c != flag->carrying_character->player->client_id )
						game.create_sound_global(SOUND_CTF_GRAB_EN, game.players[c]->client_id);
					else
						game.create_sound_global(SOUND_CTF_GRAB_PL, game.players[c]->client_id);
				}
				
				break;
			}
		}
		
		// flag not in posession and not at stand
		if(!flag->carrying_character && !flag->at_stand)
		{
			if(server_tick() > flag->drop_tick + server_tickspeed()*30)
			{
				set_new_flagkeeper();
				if(flag_keeper_id != -100)
				{
					game.create_sound_global(SOUND_CTF_RETURN);
					flag->reset();
					flag->pos = game.players[flag_keeper_id]->get_character()->pos;
					flag->stand_pos = flag->pos;
				}
				else
				{
					flag->destroy();
					flag = 0;
					return;
				}

			}
			else
			{
				flag->vel.y += game.world.core.tuning.gravity;
				move_box(&flag->pos, &flag->vel, vec2(flag->phys_size, flag->phys_size), 0.5f);
			}
		}
	}

	// check for winner
	do_player_score_wincheck();
	
	// do default stuff
	GAMECONTROLLER::tick();
}

//bool GAMECONTROLLER_KTF::on_entity(int index, vec2 pos)
//{
//	//// first, look for real flag stands
//	//if ( (index == ENTITY_FLAGSTAND_RED || index == ENTITY_FLAGSTAND_BLUE) && stand_type != STAND_FLAGSTAND )
//	//	change_flag_stand(pos, STAND_FLAGSTAND);
//
//	//// then look for ninja as flag stand
//	//if ( index == ENTITY_POWERUP_NINJA && stand_type < STAND_NINJA )
//	//	change_flag_stand(pos, STAND_NINJA);
//	//
//	//// no flagstand and no ninja? use spawn point so HTF is playable
//	//if ( (index == ENTITY_SPAWN || index == ENTITY_SPAWN_RED || index == ENTITY_SPAWN_BLUE) &&  stand_type <= STAND_SPAWN )
//	//	change_flag_stand(pos, STAND_SPAWN);
//
//	// check default stuff
//	if ( GAMECONTROLLER::on_entity(index, pos) )
//		return true;
//	
//	return false;
//}

int GAMECONTROLLER_KTF::on_character_death(class CHARACTER *victim, class PLAYER *killer, int weaponid)
{
	CHARACTER *kill_char = killer->get_character();

	if ( flag && flag->carrying_character == victim )
	{
		// flag dropped
		game.create_sound_global(SOUND_CTF_DROP);
		flag->drop_tick				= server_tick();
		flag->carrying_character	= 0;
		flag->vel					= vec2(0,0);
		
		// reset counter so new carrier has also to wait for a second to get first point
		point_counter				= 0;
		
		char buf[512];
		if ( victim != kill_char )
		{
			str_format(buf, sizeof(buf), "%s killed the flag carrier!", server_clientname(killer->client_id));
			killer->score += 5;
		}
		else
		{
			str_format(buf, sizeof(buf), "%s lost the flag!", server_clientname(victim->player->client_id));
		}
		game.send_broadcast(buf, -1);
		
		return 1;
	}
	
	return 0;
}

void GAMECONTROLLER_KTF::change_flag_stand(vec2 pos, int type)
{
	flag->pos = pos;
	flag->stand_pos = pos;
	stand_type = type;
}

void GAMECONTROLLER_KTF::on_character_spawn(class CHARACTER *chr)
{
	// do default stuff
	GAMECONTROLLER::on_character_spawn(chr);
	if(flag_keeper_id == -100 || !flag)
	{
		flag_keeper_id = chr->player->client_id;
		if(!flag)
			flag = new FLAG(0);
		flag->pos = chr->pos;
	}
}