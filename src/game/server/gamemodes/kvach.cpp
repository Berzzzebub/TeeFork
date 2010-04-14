/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#include "kvach.hpp"
#include <new>
#include <engine/e_server_interface.h>
#include <engine/e_config.h>
#include <game/server/gamecontext.hpp>
#include <game/mapitems.hpp>

GAMECONTROLLER_KVACH::GAMECONTROLLER_KVACH()
{
	// Exchange this to a string that identifies your game mode.
	// DM, TDM and CTF are reserved for teeworlds original modes.
	gametype = "Kvach";
	kvacher_client_id = -100;
	last_kvacher_client_id = -100;
	leader_client_id = -1;
	invinsible_tick = 0; //Dark tee tick

	//game_flags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode
}

void GAMECONTROLLER_KVACH::tick()
{
	// this is the main part of the gamemode, this function is run every tick
	do_player_score_wincheck(); // checks for winners, no teams version
	//do_team_score_wincheck(); // checks for winners, two teams version
	
	if(!timer_going(invinsible_tick, config.sv_immune_to_kvach) && last_kvacher_client_id >= 0 && game.players[last_kvacher_client_id])
	{
		update_colors();
	}

	
	GAMECONTROLLER::tick();
}

bool GAMECONTROLLER_KVACH::on_entity(int index, vec2 pos)
{
	if(index == ENTITY_SPAWN || index == ENTITY_SPAWN_RED || index == ENTITY_SPAWN_BLUE)
		return GAMECONTROLLER::on_entity(index, pos);
	else
		return false;
}

void GAMECONTROLLER_KVACH::on_character_spawn(class CHARACTER *chr)
{
	// default health
	chr->health = 10;
	chr->armor = 10;

	if(kvacher_client_id  == -100)
		choose_new_kvach();

	chr->damage_bonus = 0;
	chr->player->use_custom_color = 1;

	chr->weapons[WEAPON_GRENADE].got = 1;
	chr->weapons[WEAPON_GRENADE].ammo = -1;
	chr->active_weapon = WEAPON_GRENADE;

	update_equipment();
	//if(kvacher_client_id == chr->player->client_id)
	//{
	//	chr->weapons[WEAPON_HAMMER].ammo = -1;
	//	chr->weapons[WEAPON_HAMMER].got = 1;
	//	chr->active_weapon = WEAPON_HAMMER;
	//	chr->player->color_body = 65387;
	//	chr->player->color_feet = 65387;
	//}
	//else
	//{
	//	chr->player->color_feet = 10223467;
	//	chr->player->color_body = 10223467;
	//}

}

void GAMECONTROLLER_KVACH::on_player_info_change(class PLAYER *p)
{
	const int team_colors[2] = {65387, 10223467};
	if(p->client_id == kvacher_client_id && p->team < 0)
		choose_new_kvach();



	if(is_teamplay())
	{
		if(p->team >= 0 || p->team <= 1)
		{
			p->use_custom_color = 1;
			p->color_body = team_colors[p->team];
			p->color_feet = team_colors[p->team];
		}
	}
}

void GAMECONTROLLER_KVACH::update_score_leader()
{
	int leader_score = -1;
	//Find leader score to make him kvacher
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(game.players[i] && game.players[i]->team > -1 && game.players[i]->client_id != kvacher_client_id)
		{				
			if(game.players[i]->score > leader_score)
			{
				leader_score = game.players[i]->score;
			}
		}
	}

	if(leader_score >= 0)
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(game.players[i] && game.players[i]->team > -1 && game.players[i]->client_id != kvacher_client_id)
			{				
				if(game.players[i]->score == leader_score)
				{
					leader_client_id = game.players[i]->client_id;
					return;
				}
			}
		}
	else
		leader_client_id = -1;
}

void GAMECONTROLLER_KVACH::update_equipment()
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(game.players[i] && game.players[i]->get_character() && game.players[i]->team != -1)
		{				
			if(game.players[i]->client_id == kvacher_client_id)
			{
				game.players[i]->get_character()->weapons[WEAPON_HAMMER].ammo = -1;
				game.players[i]->get_character()->weapons[WEAPON_HAMMER].got = 1;
				game.players[i]->get_character()->active_weapon = WEAPON_HAMMER;
			}
			else
			{
				game.players[i]->get_character()->weapons[WEAPON_HAMMER].got = 0;
				game.players[i]->get_character()->weapons[WEAPON_GRENADE].got = 1;
				game.players[i]->get_character()->weapons[WEAPON_GRENADE].ammo = -1;
				game.players[i]->get_character()->active_weapon = WEAPON_GRENADE;
			}
		}
	}
	update_colors();
}

void GAMECONTROLLER_KVACH::update_colors()
{
	update_score_leader();
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		PLAYER* player = game.players[i];
		if(player && player->team != -1)
		{				
			player->use_custom_color = 1;
			if(player->client_id == kvacher_client_id)
			{//Set kvach color
				player->color_body = 65387;
				player->color_feet = 65387;
			}
			else if((player->client_id == last_kvacher_client_id) && timer_going(invinsible_tick, config.sv_immune_to_kvach))
			{//Set BlackTee (last kvach) color
				player->color_body = 0;
				player->color_feet = 0;
			}
			else if(player->client_id == leader_client_id)
			{//Set scoreleader color
				player->color_body = 5096023;
				player->color_feet = 5096023;
			}
			else if(player->score <= game.players[kvacher_client_id]->score)
			{//Set noob tee color (tee wich has less points then kvach)
				player->color_body = 16777215;
				player->color_feet = 16777215;
			}
			else
			{//Set regular tee color
				player->color_body = 10223467;
				player->color_feet = 10223467;
			}
		}
	}
}

void GAMECONTROLLER_KVACH::choose_new_kvach()
{
	update_score_leader();
	if(leader_client_id >= 0)
		kvacher_client_id = leader_client_id;
	else
		kvacher_client_id = -100;

	update_equipment();
}

int GAMECONTROLLER_KVACH::on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon)
{
	// do scoreing
	if(!killer)
		return 0;
	if(killer == victim->player)
	{
		victim->player->score -= config.sv_scorelimit*10/100; // suicide
		if(victim->player->score < 0)
			victim->player->score = 0;
	}
	//else
	//{
	//	if(is_teamplay() && victim->team == killer->team)
	//		killer->score--; // teamkill
	//	else
	//		killer->score++; // normal kill
	//}
	if(victim->player->client_id == kvacher_client_id && weapon == WEAPON_GAME)
		choose_new_kvach();

	if(weapon == WEAPON_HAMMER)
	{
		last_kvacher_client_id = kvacher_client_id;
		invinsible_tick = server_tick();

		game.players[last_kvacher_client_id]->use_custom_color = 1;
		game.players[last_kvacher_client_id]->color_body = 0;
		game.players[last_kvacher_client_id]->color_feet = 0;

		kvacher_client_id = victim->player->client_id;
		update_equipment();

		for(int i = 0; i < 5; i++)
		{
			game.create_death(victim->pos, victim->player->client_id);
		}
		return 1;
	}

	return 0;
}

void GAMECONTROLLER_KVACH::damage_calculate(class CHARACTER *chrTo, int dmg, class CHARACTER *chrFrom, int weapon)
{
	if(!chrFrom || !chrTo)
		return;

	int from = chrFrom->player->client_id;
	int to = chrTo->player->client_id;
	PLAYER* player = chrTo->player;
	if(from == kvacher_client_id && to != kvacher_client_id && weapon == WEAPON_GRENADE)
		chrFrom->weapons[WEAPON_HAMMER].got = 1;
		

	if(!(player->client_id == kvacher_client_id || (chrFrom->player->client_id == kvacher_client_id && weapon == WEAPON_HAMMER)) || chrFrom->player->client_id == player->client_id || !chrFrom->player)
		return; 

	// check for hammer hit.
	if(weapon == WEAPON_HAMMER)
	{
		if ((player->client_id == last_kvacher_client_id) && timer_going(invinsible_tick, config.sv_immune_to_kvach))
		{
			//if kvacher kick last kvacher while that immune to kvach, he get 10% of score limit.
			dbg_msg("###::KvachCheckPoint::###","BlackTeeHit!");
			dmg = config.sv_scorelimit*10/100;
			game.players[from]->score += dmg;
		}
		else
		{
			if(player->score > game.players[from]->score)
			{
				dmg = (player->score - game.players[from]->score)/2;
				game.players[from]->score += dmg; //Last kvacher gain points
				game.players[to]->score -= config.sv_scorelimit*10/100; //New kvacher loose points
				if(game.players[to]->score < 0)
					game.players[to]->score = 0;
			}
			chrTo->die(from, weapon);
		}
		
		// set attacker's face to happy (taunt!)
		if (from >= 0 && from != player->client_id && game.players[from])
		{
			CHARACTER *chr = game.players[from]->get_character();
			if (chr)
			{
				chr->emote_type = EMOTE_HAPPY;
				chr->emote_stop = server_tick() + server_tickspeed();
			}
		}
	
		update_colors();
		return;
	}

	float factor = 1;
	if(player->score > game.players[from]->score)
	{
		//Make damage greater if player has fewer score point than kvacher.
		factor = (game.players[player->client_id]->score - game.players[from]->score)/(config.sv_scorelimit*10/100) + 1;
		dmg = (dmg + chrFrom->damage_bonus) * factor;
	}

	if(kvacher_client_id == player->client_id)// If this is tee is kvacher..
	{
		dmg +=  game.players[from]->get_character()->damage_bonus;
		dbg_msg("###::KvachCheckPoint::###","dmg = dmg + damage_bonus='%d'", dmg);
		game.players[from]->score += dmg;
	}
	game.players[from]->get_character()->damage_bonus++;

	chrTo->damage_taken++;

	// create healthmod indicator
	if(server_tick() < chrTo->damage_taken_tick+25)
	{
		// make sure that the damage indicators doesn't group together
		game.create_damageind(chrTo->pos, chrTo->damage_taken*0.25f, dmg);
	}
	else
	{
		chrTo->damage_taken = 0;
		game.create_damageind(chrTo->pos, 0, dmg);
	}

	chrTo->damage_taken_tick = server_tick();

	// do damage hit sound
	if(from >= 0 && from != player->client_id && game.players[from])
		game.create_sound(game.players[from]->view_pos, SOUND_HIT, cmask_one(from));

	if (dmg > 2)
		game.create_sound(chrTo->pos, SOUND_PLAYER_PAIN_LONG);
	else
		game.create_sound(chrTo->pos, SOUND_PLAYER_PAIN_SHORT);

	chrTo->emote_type = EMOTE_PAIN;
	chrTo->emote_stop = server_tick() + 500 * server_tickspeed() / 1000;

	// spawn blood?

	return;

}

bool GAMECONTROLLER_KVACH::timer_going(int timerTick, int timerLife)
{
	if((server_tick() - timerTick) < (timerLife * server_tickspeed()/1000))
		return true;

	return false;
}