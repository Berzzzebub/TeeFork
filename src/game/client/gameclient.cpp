#include <string.h>
#include <engine/e_client_interface.h>
#include <engine/e_demorec.h>

#include <game/generated/g_protocol.hpp>
#include <game/generated/gc_data.hpp>

#include <game/layers.hpp>
#include "render.hpp"

#include "gameclient.hpp"

#include "components/binds.hpp"
#include "components/broadcast.hpp"
#include "components/camera.hpp"
#include "components/chat.hpp"
#include "components/console.hpp"
#include "components/controls.hpp"
#include "components/damageind.hpp"
#include "components/debughud.hpp"
#include "components/effects.hpp"
#include "components/emoticon.hpp"
#include "components/flow.hpp"
#include "components/hud.hpp"
#include "components/items.hpp"
#include "components/killmessages.hpp"
#include "components/mapimages.hpp"
#include "components/maplayers.hpp"
#include "components/menus.hpp"
#include "components/motd.hpp"
#include "components/particles.hpp"
#include "components/players.hpp"
#include "components/nameplates.hpp"
#include "components/scoreboard.hpp"
#include "components/skins.hpp"
#include "components/sounds.hpp"
#include "components/voting.hpp"
#include "components/teecomp_stats.hpp"
#include "components/race_demo.hpp"
#include <game/client/teecomp.hpp>

GAMECLIENT gameclient;

// instanciate all systems
static KILLMESSAGES killmessages;
static CAMERA camera;
static CHAT chat;
static MOTD motd;
static BROADCAST broadcast;
static CONSOLE console;
static BINDS binds;
static PARTICLES particles;
static MENUS menus;
static SKINS skins;
static FLOW flow;
static HUD hud;
static DEBUGHUD debughud;
static CONTROLS controls;
static EFFECTS effects;
static SCOREBOARD scoreboard;
static SOUNDS sounds;
static EMOTICON emoticon;
static DAMAGEIND damageind;
static VOTING voting;
static TEECOMP_STATS teecomp_stats;
static RACE race;
static PLAYERS players;
static NAMEPLATES nameplates;
static ITEMS items;
static MAPIMAGES mapimages;

static MAPLAYERS maplayers_background(MAPLAYERS::TYPE_BACKGROUND);
static MAPLAYERS maplayers_foreground(MAPLAYERS::TYPE_FOREGROUND);

GAMECLIENT::STACK::STACK() { num = 0; }
void GAMECLIENT::STACK::add(class COMPONENT *component) { components[num++] = component; }

static int load_current;
static int load_total;

static void load_sounds_thread(void *do_render)
{
	// load sounds
	for(int s = 0; s < data->num_sounds; s++)
	{
		if(do_render)
			gameclient.menus->render_loading(load_current/(float)load_total);
		for(int i = 0; i < data->sounds[s].num_sounds; i++)
		{
			int id = snd_load_wv(data->sounds[s].sounds[i].filename);
			data->sounds[s].sounds[i].id = id;
		}

		if(do_render)
			load_current++;
	}
}

static void con_serverdummy(void *result, void *user_data)
{
	dbg_msg("client", "this command is not available on the client");
}

void GAMECLIENT::on_console_init()
{
	// setup pointers
	binds = &::binds;
	console = &::console;
	particles = &::particles;
	menus = &::menus;
	skins = &::skins;
	chat = &::chat;
	flow = &::flow;
	camera = &::camera;
	controls = &::controls;
	effects = &::effects;
	sounds = &::sounds;
	motd = &::motd;
	damageind = &::damageind;
	mapimages = &::mapimages;
	voting = &::voting;
	teecomp_stats = &::teecomp_stats;
	race = &::race;
	
	// make a list of all the systems, make sure to add them in the corrent render order
	all.add(skins);
	all.add(mapimages);
	all.add(effects); // doesn't render anything, just updates effects
	all.add(particles);
	all.add(binds);
	all.add(controls);
	all.add(camera);
	all.add(sounds);
	all.add(voting);
	all.add(particles); // doesn't render anything, just updates all the particles
	
	all.add(&maplayers_background); // first to render
	all.add(&particles->render_trail);
	all.add(&particles->render_explosions);
	all.add(&items);
	all.add(&players);
	all.add(&maplayers_foreground);
	all.add(&nameplates);
	all.add(&particles->render_general);
	all.add(damageind);
	all.add(&hud);
	all.add(&emoticon);
	all.add(&killmessages);
	all.add(chat);
	all.add(&broadcast);
	all.add(&debughud);
	all.add(&scoreboard);
	all.add(teecomp_stats);
	all.add(motd);
	all.add(menus);
	all.add(console);
	
	// build the input stack
	input.add(&menus->binder); // this will take over all input when we want to bind a key
	input.add(&binds->special_binds);
	input.add(console);
	input.add(chat); // chat has higher prio due to tha you can quit it by pressing esc
	input.add(motd); // for pressing esc to remove it
	input.add(menus);
	input.add(&emoticon);
	input.add(controls);
	input.add(binds);
	
	all.add(race);
		
	// add the some console commands
	MACRO_REGISTER_COMMAND("team", "i", CFGFLAG_CLIENT, con_team, this, "Switch team");
	MACRO_REGISTER_COMMAND("kill", "", CFGFLAG_CLIENT, con_kill, this, "Kill yourself");
	
	// register server dummy commands for tab completion
	MACRO_REGISTER_COMMAND("tune", "si", CFGFLAG_SERVER, con_serverdummy, 0, "Tune variable to value");
	MACRO_REGISTER_COMMAND("tune_reset", "", CFGFLAG_SERVER, con_serverdummy, 0, "Reset tuning");
	MACRO_REGISTER_COMMAND("tune_dump", "", CFGFLAG_SERVER, con_serverdummy, 0, "Dump tuning");
	MACRO_REGISTER_COMMAND("change_map", "r", CFGFLAG_SERVER, con_serverdummy, 0, "Change map");
	MACRO_REGISTER_COMMAND("restart", "?i", CFGFLAG_SERVER, con_serverdummy, 0, "Restart in x seconds");
	MACRO_REGISTER_COMMAND("broadcast", "r", CFGFLAG_SERVER, con_serverdummy, 0, "Broadcast message");
	/*MACRO_REGISTER_COMMAND("say", "r", CFGFLAG_SERVER, con_serverdummy, 0);*/
	MACRO_REGISTER_COMMAND("set_team", "ii", CFGFLAG_SERVER, con_serverdummy, 0, "Set team of player to team");
	MACRO_REGISTER_COMMAND("addvote", "r", CFGFLAG_SERVER, con_serverdummy, 0, "Add a voting option");
	/*MACRO_REGISTER_COMMAND("vote", "", CFGFLAG_SERVER, con_serverdummy, 0);*/
	
	// let all the other components register their console commands
	for(int i = 0; i < all.num; i++)
		all.components[i]->on_console_init();
		
	//
	suppress_events = false;
}

void GAMECLIENT::on_init()
{
	// init all components
	for(int i = 0; i < all.num; i++)
		all.components[i]->on_init();
	
	// setup item sizes
	for(int i = 0; i < NUM_NETOBJTYPES; i++)
		snap_set_staticsize(i, netobj_get_size(i));
	
	// load default font	
	static FONT_SET default_font;
	int64 start = time_get();
	
	int before = gfx_memory_usage();
	font_set_load(&default_font, "fonts/default_font%d.tfnt", "fonts/default_font%d.png", "fonts/default_font%d_b.png", 14, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 36);
	dbg_msg("font", "gfx memory used for font textures: %d", gfx_memory_usage()-before);
	
	gfx_text_set_default_font(&default_font);

	config.cl_threadsoundloading = 0;

	// setup load amount
	load_total = data->num_images;
	load_current = 0;
	if(!config.cl_threadsoundloading)
		load_total += data->num_sounds;
	
	// load textures
	for(int i = 0; i < data->num_images; i++)
	{
		gameclient.menus->render_loading(load_current/load_total);
		data->images[i].id = gfx_load_texture(data->images[i].filename, IMG_AUTO, 0);
		load_current++;
	}

	::skins.init();
	
	if(config.cl_threadsoundloading)
		thread_create(load_sounds_thread, 0);
	else
		load_sounds_thread((void*)1);

	for(int i = 0; i < all.num; i++)
		all.components[i]->on_reset();
	
	int64 end = time_get();
	dbg_msg("", "%f.2ms", ((end-start)*1000)/(float)time_freq());
	
	servermode = SERVERMODE_PURE;

	// Teecomp grayscale flags
	gfx_unload_texture(data->images[IMAGE_GAME_GRAY].id); // Already loaded with full color, unload
	data->images[IMAGE_GAME_GRAY].id = -1;

	IMAGE_INFO info;
	if(!gfx_load_png(&info, data->images[IMAGE_GAME_GRAY].filename))
		return;

	unsigned char *d = (unsigned char *)info.data;
	int step = info.format == IMG_RGBA ? 4 : 3;

	for(int i=0; i < info.width*info.height; i++)
	{
		int v = (d[i*step]+d[i*step+1]+d[i*step+2])/3;
		d[i*step] = v;
		d[i*step+1] = v;
		d[i*step+2] = v;
	}

	int freq[256];
	int org_weight;
	int new_weight;
	int flag_x = 384;
	int flag_y = 256;
	int flag_w = 128;
	int flag_h = 256;
	int pitch = info.width*4;

	for(int f=0; f<2; f++)
	{
		org_weight = 0;
		new_weight = 192;
		for(int i=0; i<256; i++)
			freq[i] = 0;

		// find most common frequence
		for(int y=flag_y; y<flag_y+flag_h; y++)
			for(int x=flag_x+flag_w*f; x<flag_x+flag_w*(1+f); x++)
			{
				if(d[y*pitch+x*4+3] > 128)
					freq[d[y*pitch+x*4]]++;
			}
		
		for(int i = 1; i < 256; i++)
		{
			if(freq[org_weight] < freq[i])
				org_weight = i;
		}

		// reorder
		int inv_org_weight = 255-org_weight;
		int inv_new_weight = 255-new_weight;
		for(int y=flag_y; y<flag_y+flag_h; y++)
			for(int x=flag_x+flag_w*f; x<flag_x+flag_w*(1+f); x++)
			{
				int v = d[y*pitch+x*4];
				if(v <= org_weight*1.25f) // modified for contrast
					v = (int)(((v/(float)org_weight) * new_weight));
				else
					v = (int)(((v-org_weight)/(float)inv_org_weight)*inv_new_weight + new_weight);
				d[y*pitch+x*4] = v;
				d[y*pitch+x*4+1] = v;
				d[y*pitch+x*4+2] = v;
			}
	}

	data->images[IMAGE_GAME_GRAY].id = gfx_load_texture_raw(info.width, info.height, info.format, info.data, info.format, 0);
	mem_free(info.data);
}

void GAMECLIENT::on_save()
{
	for(int i = 0; i < all.num; i++)
		all.components[i]->on_save();
}

void GAMECLIENT::dispatch_input()
{
	// handle mouse movement
	int x=0, y=0;
	inp_mouse_relative(&x, &y);
	if(x || y || !freeview)
	{
		for(int h = 0; h < input.num; h++)
		{
			if(input.components[h]->on_mousemove(x, y))
				break;
		}
	}
	
	// handle key presses
	for(int i = 0; i < inp_num_events(); i++)
	{
		INPUT_EVENT e = inp_get_event(i);
		
		for(int h = 0; h < input.num; h++)
		{
			if(input.components[h]->on_input(e))
			{
				//dbg_msg("", "%d char=%d key=%d flags=%d", h, e.ch, e.key, e.flags);
				break;
			}
		}
	}
	
	// clear all events for this frame
	inp_clear_events();	
}


int GAMECLIENT::on_snapinput(int *data)
{
	int val = controls->snapinput(data);
	if(val && snap.spectate)
	{
		NETOBJ_PLAYER_INPUT *inp = (NETOBJ_PLAYER_INPUT *)data;
		static bool last_fire = false, last_hook = false;

		if(inp->fire&1 && !last_fire)
		{
			find_next_spectable_cid();
			last_fire = true;
		}
		else if(!(inp->fire&1) && last_fire)
			last_fire = false;

		if(inp->hook && !last_hook)
		{
			freeview = !freeview;
			if(!freeview)
				find_next_spectable_cid();
			last_hook = true;
		}
		else if(!inp->hook && last_hook)
			last_hook = false;
	}
	return val;
}

void GAMECLIENT::on_connected()
{
	layers_init();
	col_init();
	render_tilemap_generate_skip();

	for(int i = 0; i < all.num; i++)
	{
		all.components[i]->on_mapload();
		all.components[i]->on_reset();
	}
	
	SERVER_INFO current_server_info;
	client_serverinfo(&current_server_info);
	
	servermode = SERVERMODE_PURE;
	
	// send the inital info
	send_info(true);

	freeview = true;
	spectate_cid = -1;
	last_game_over = 0;
	last_warmup = false;
	last_flag_carrier[0] = -1;
	last_flag_carrier[1] = -1;
}

void GAMECLIENT::on_reset()
{
	// clear out the invalid pointers
	last_new_predicted_tick = -1;
	mem_zero(&gameclient.snap, sizeof(gameclient.snap));

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		clients[i].name[0] = 0;
		clients[i].skin_id = 0;
		clients[i].team = 0;
		clients[i].angle = 0;
		clients[i].emoticon = 0;
		clients[i].emoticon_start = -1;
		clients[i].skin_info.texture = gameclient.skins->get(0)->color_texture;
		clients[i].skin_info.color_body = vec4(1,1,1,1);
		clients[i].skin_info.color_feet = vec4(1,1,1,1);
		clients[i].update_render_info(i);

		// anti rainbow
		clients[i].color_change_count = 0;
		clients[i].prev_color_body = vec4(1,1,1,1);
		clients[i].prev_color_feet = vec4(1,1,1,1);

		stats[i].reset();
	}
	
	for(int i = 0; i < all.num; i++)
		all.components[i]->on_reset();
}


void GAMECLIENT::update_local_character_pos()
{
	if(config.cl_predict && client_state() != CLIENTSTATE_DEMOPLAYBACK)
	{
		if(!snap.local_character || (snap.local_character->health < 0) || (snap.gameobj && snap.gameobj->game_over))
		{
			// don't use predicted
		}
		else
			local_character_pos = mix(predicted_prev_char.pos, predicted_char.pos, client_predintratick());
	}
	else if(snap.local_character && snap.local_prev_character)
	{
		local_character_pos = mix(
			vec2(snap.local_prev_character->x, snap.local_prev_character->y),
			vec2(snap.local_character->x, snap.local_character->y), client_intratick());
	}
	if(spectate_cid == -1)
		freeview = true;
	if(snap.spectate && !freeview)
	{
		if(!snap.characters[spectate_cid].active || clients[spectate_cid].team == -1)
		{
			find_next_spectable_cid();
			return;
		}
		spectate_pos = mix(
			vec2(snap.characters[spectate_cid].prev.x, snap.characters[spectate_cid].prev.y),
			vec2(snap.characters[spectate_cid].cur.x, snap.characters[spectate_cid].cur.y), client_intratick());
	}
}
 
void GAMECLIENT::find_next_spectable_cid()
{
	if(!freeview && (spectate_cid != killer_cid) && snap.characters[killer_cid].active && !(clients[killer_cid].team == -1))
	{
		spectate_cid = killer_cid;
		return;
	}
	int next = spectate_cid+1;
	next %= MAX_CLIENTS;
	int prev = next;
	while(!snap.characters[next].active || clients[next].team == -1)
	{
		next++;
		next %= MAX_CLIENTS;
		if(next == prev)
		{
			freeview = true;
			spectate_cid = -1;
			return;
		}
	}
	spectate_cid = next;
	if(freeview)
		freeview = false;
}

static void evolve(NETOBJ_CHARACTER *character, int tick)
{
	WORLD_CORE tempworld;
	CHARACTER_CORE tempcore;
	mem_zero(&tempcore, sizeof(tempcore));
	tempcore.world = &tempworld;
	tempcore.read(character);
	
	while(character->tick < tick)
	{
		character->tick++;
		tempcore.tick(false);
		tempcore.move();
		tempcore.quantize();
	}

	tempcore.write(character);
}


void GAMECLIENT::on_render()
{
	// update the local character position
	update_local_character_pos();
	
	// dispatch all input to systems
	dispatch_input();
	
	// render all systems
	for(int i = 0; i < all.num; i++)
		all.components[i]->on_render();
		
	// clear new tick flags
	new_tick = false;
	new_predicted_tick = false;
}

void GAMECLIENT::on_message(int msgtype)
{
	
	// special messages
	if(msgtype == NETMSGTYPE_SV_EXTRAPROJECTILE)
	{
		/*
		int num = msg_unpack_int();
		
		for(int k = 0; k < num; k++)
		{
			NETOBJ_PROJECTILE proj;
			for(unsigned i = 0; i < sizeof(NETOBJ_PROJECTILE)/sizeof(int); i++)
				((int *)&proj)[i] = msg_unpack_int();
				
			if(msg_unpack_error())
				return;
				
			if(extraproj_num != MAX_EXTRA_PROJECTILES)
			{
				extraproj_projectiles[extraproj_num] = proj;
				extraproj_num++;
			}
		}
		
		return;*/
	}
	else if(msgtype == NETMSGTYPE_SV_TUNEPARAMS)
	{
		// unpack the new tuning
		TUNING_PARAMS new_tuning;
		int *params = (int *)&new_tuning;
		for(unsigned i = 0; i < sizeof(TUNING_PARAMS)/sizeof(int); i++)
			params[i] = msg_unpack_int();

		// check for unpacking errors
		if(msg_unpack_error())
			return;
		
		servermode = SERVERMODE_PURE;
			
		// apply new tuning
		tuning = new_tuning;
		return;
	}
	
	void *rawmsg = netmsg_secure_unpack(msgtype);
	if(!rawmsg)
	{
		dbg_msg("client", "dropped weird message '%s' (%d), failed on '%s'", netmsg_get_name(msgtype), msgtype, netmsg_failed_on());
		return;
	}

	// TODO: this should be done smarter
	for(int i = 0; i < all.num; i++)
		all.components[i]->on_message(msgtype, rawmsg);
	
	if(msgtype == NETMSGTYPE_SV_READYTOENTER)
	{
		client_entergame();
	}
	else if (msgtype == NETMSGTYPE_SV_EMOTICON)
	{
		NETMSG_SV_EMOTICON *msg = (NETMSG_SV_EMOTICON *)rawmsg;

		// apply
		clients[msg->cid].emoticon = msg->emoticon;
		clients[msg->cid].emoticon_start = client_tick();
	}
	else if(msgtype == NETMSGTYPE_SV_SOUNDGLOBAL)
	{
		if(suppress_events)
			return;
			
		NETMSG_SV_SOUNDGLOBAL *msg = (NETMSG_SV_SOUNDGLOBAL *)rawmsg;
		gameclient.sounds->play(SOUNDS::CHN_GLOBAL, msg->soundid, 1.0f, vec2(0,0));
	}		
}

void GAMECLIENT::on_statechange(int new_state, int old_state)
{
	if(demorec_isrecording())
		demorec_record_stop();

	// reset everything
	on_reset();
	
	// then change the state
	for(int i = 0; i < all.num; i++)
		all.components[i]->on_statechange(new_state, old_state);

	if(new_state == CLIENTSTATE_ONLINE && config.tc_autodemo)
		teecomp_demo_start();
}



void GAMECLIENT::process_events()
{
	if(suppress_events)
		return;
	
	int snaptype = SNAP_CURRENT;
	int num = snap_num_items(snaptype);
	for(int index = 0; index < num; index++)
	{
		SNAP_ITEM item;
		const void *data = snap_get_item(snaptype, index, &item);

		if(item.type == NETEVENTTYPE_DAMAGEIND)
		{
			NETEVENT_DAMAGEIND *ev = (NETEVENT_DAMAGEIND *)data;
			gameclient.effects->damage_indicator(vec2(ev->x, ev->y), get_direction(ev->angle));
		}
		else if(item.type == NETEVENTTYPE_EXPLOSION)
		{
			NETEVENT_EXPLOSION *ev = (NETEVENT_EXPLOSION *)data;
			gameclient.effects->explosion(vec2(ev->x, ev->y));
		}
		else if(item.type == NETEVENTTYPE_HAMMERHIT)
		{
			NETEVENT_HAMMERHIT *ev = (NETEVENT_HAMMERHIT *)data;
			gameclient.effects->hammerhit(vec2(ev->x, ev->y));
		}
		else if(item.type == NETEVENTTYPE_SPAWN)
		{
			NETEVENT_SPAWN *ev = (NETEVENT_SPAWN *)data;
			gameclient.effects->playerspawn(vec2(ev->x, ev->y));
		}
		else if(item.type == NETEVENTTYPE_DEATH)
		{
			NETEVENT_DEATH *ev = (NETEVENT_DEATH *)data;
			gameclient.effects->playerdeath(vec2(ev->x, ev->y), ev->cid);
		}
		else if(item.type == NETEVENTTYPE_SOUNDWORLD)
		{
			NETEVENT_SOUNDWORLD *ev = (NETEVENT_SOUNDWORLD *)data;
			gameclient.sounds->play(SOUNDS::CHN_WORLD, ev->soundid, 1.0f, vec2(ev->x, ev->y));
		}
	}
}

void GAMECLIENT::on_snapshot()
{
	new_tick = true;
	
	// clear out the invalid pointers
	mem_zero(&gameclient.snap, sizeof(gameclient.snap));
	snap.local_cid = -1;

	// secure snapshot
	{
		int num = snap_num_items(SNAP_CURRENT);
		for(int index = 0; index < num; index++)
		{
			SNAP_ITEM item;
			void *data = snap_get_item(SNAP_CURRENT, index, &item);
			if(netobj_validate(item.type, data, item.datasize) != 0)
			{
				if(config.debug)
					dbg_msg("game", "invalidated index=%d type=%d (%s) size=%d id=%d", index, item.type, netobj_get_name(item.type), item.datasize, item.id);
				snap_invalidate_item(SNAP_CURRENT, index);
			}
		}
	}
		
	process_events();

	if(config.dbg_stress)
	{
		if((client_tick()%100) == 0)
		{
			char message[64];
			int msglen = rand()%(sizeof(message)-1);
			for(int i = 0; i < msglen; i++)
				message[i] = 'a'+(rand()%('z'-'a'));
			message[msglen] = 0;
				
			NETMSG_CL_SAY msg;
			msg.team = rand()&1;
			msg.message = message;
			msg.pack(MSGFLAG_VITAL);
			client_send_msg();
		}
	}

	// go trough all the items in the snapshot and gather the info we want
	{
		snap.team_size[0] = snap.team_size[1] = 0;
		
		// TeeComp.
		for(int i=0; i<MAX_CLIENTS; i++)
			stats[i].active = false;

		int num = snap_num_items(SNAP_CURRENT);
		for(int i = 0; i < num; i++)
		{
			SNAP_ITEM item;
			const void *data = snap_get_item(SNAP_CURRENT, i, &item);

			if(item.type == NETOBJTYPE_CLIENT_INFO)
			{
				const NETOBJ_CLIENT_INFO *info = (const NETOBJ_CLIENT_INFO *)data;
				int cid = item.id;
				ints_to_str(&info->name0, 6, clients[cid].name);
				ints_to_str(&info->skin0, 6, clients[cid].skin_name);
				
				clients[cid].use_custom_color = info->use_custom_color;
				clients[cid].color_body = info->color_body;
				clients[cid].color_feet = info->color_feet;
				
				// prepare the info
				if(clients[cid].skin_name[0] == 'x' || clients[cid].skin_name[1] == '_')
					str_copy(clients[cid].skin_name, "default", 64);
					
				clients[cid].skin_info.color_body = skins->get_color(clients[cid].color_body);
				clients[cid].skin_info.color_feet = skins->get_color(clients[cid].color_feet);
				clients[cid].skin_info.size = 64;
				
				// find new skin
				clients[cid].skin_id = gameclient.skins->find(clients[cid].skin_name);
				if(clients[cid].skin_id < 0)
				{
					clients[cid].skin_id = gameclient.skins->find("default");
					if(clients[cid].skin_id < 0)
						clients[cid].skin_id = 0;
				}
				
				if(clients[cid].use_custom_color)
					clients[cid].skin_info.texture = gameclient.skins->get(clients[cid].skin_id)->color_texture;
				else
				{
					clients[cid].skin_info.texture = gameclient.skins->get(clients[cid].skin_id)->org_texture;
					clients[cid].skin_info.color_body = vec4(1,1,1,1);
					clients[cid].skin_info.color_feet = vec4(1,1,1,1);
				}

				clients[cid].update_render_info(cid);
				gameclient.snap.num_players++;
			}
			else if(item.type == NETOBJTYPE_PLAYER_INFO)
			{
				const NETOBJ_PLAYER_INFO *info = (const NETOBJ_PLAYER_INFO *)data;
				
				clients[info->cid].team = info->team;
				snap.player_infos[info->cid] = info;
				
				if(info->local)
				{
					snap.local_cid = item.id;
					snap.local_info = info;
					
					if (info->team == -1)
						snap.spectate = true;
				}
				
				// calculate team-balance
				if(info->team != -1)
				{
					snap.team_size[info->team]++;
					stats[info->cid].active = true;
				}
				
			}
			else if(item.type == NETOBJTYPE_CHARACTER)
			{
				const void *old = snap_find_item(SNAP_PREV, NETOBJTYPE_CHARACTER, item.id);
				if(old)
				{
					snap.characters[item.id].active = true;
					snap.characters[item.id].prev = *((const NETOBJ_CHARACTER *)old);
					snap.characters[item.id].cur = *((const NETOBJ_CHARACTER *)data);

					if(snap.characters[item.id].prev.tick)
						evolve(&snap.characters[item.id].prev, client_prevtick());
					if(snap.characters[item.id].cur.tick)
						evolve(&snap.characters[item.id].cur, client_tick());
				}
			}
			else if(item.type == NETOBJTYPE_GAME)
			{
				snap.gameobj = (NETOBJ_GAME *)data;
				if(snap.gameobj->game_over != last_game_over)
				{
					if(!last_game_over)
						on_game_over();
					else
						on_game_restart();
					last_game_over = snap.gameobj->game_over;
				}
				if((snap.gameobj->warmup > 0) != last_warmup)
				{
					if(last_warmup)
						on_warmup_end();
					last_warmup = snap.gameobj->warmup > 0;
				}
			}
			else if(item.type == NETOBJTYPE_FLAG)
			{
				int fid = item.id%2;
				snap.flags[fid] = (const NETOBJ_FLAG *)data;
				if(snap.flags[fid]->carried_by != last_flag_carrier[fid])
				{
					if(snap.flags[fid]->carried_by >= 0)
						on_flag_grab(fid);
					last_flag_carrier[fid] = snap.flags[fid]->carried_by;
				}
			}
		}

		// TeeComp
		for(int i=0; i<MAX_CLIENTS; i++)
		{
			if(stats[i].active && !stats[i].was_active)
			{
				stats[i].reset(); // Client connected, reset stats.
				stats[i].active = true;
				stats[i].join_date = client_tick();
			}
			stats[i].was_active = stats[i].active;
		}
	}
	
	// setup local pointers
	if(snap.local_cid >= 0)
	{
		SNAPSTATE::CHARACTERINFO *c = &snap.characters[snap.local_cid];
		if(c->active)
		{
			snap.local_character = &c->cur;
			snap.local_prev_character = &c->prev;
			local_character_pos = vec2(snap.local_character->x, snap.local_character->y);
		}
	}
	else
		snap.spectate = true;
	
	TUNING_PARAMS standard_tuning;
	SERVER_INFO current_server_info;
	client_serverinfo(&current_server_info);
	if(current_server_info.gametype[0] != '0')
	{
		if(strcmp(current_server_info.gametype, "DM") != 0 && strcmp(current_server_info.gametype, "TDM") != 0 && strcmp(current_server_info.gametype, "CTF") != 0)
			servermode = SERVERMODE_MOD;
		else if(memcmp(&standard_tuning, &tuning, sizeof(TUNING_PARAMS)) == 0)
			servermode = SERVERMODE_PURE;
		else
			servermode = SERVERMODE_PUREMOD;
	}
	

	// update render info
	for(int i = 0; i < MAX_CLIENTS; i++)
		clients[i].update_render_info(i);
}

void GAMECLIENT::on_predict()
{
	// store the previous values so we can detect prediction errors
	CHARACTER_CORE before_prev_char = predicted_prev_char;
	CHARACTER_CORE before_char = predicted_char;

	// we can't predict without our own id or own character
	if(snap.local_cid == -1 || !snap.characters[snap.local_cid].active)
		return;
	
	// don't predict anything if we are paused
	if(snap.gameobj && snap.gameobj->paused)
	{
		if(snap.local_character)
			predicted_char.read(snap.local_character);
		if(snap.local_prev_character)
			predicted_prev_char.read(snap.local_prev_character);
		return;
	}

	// repredict character
	WORLD_CORE world;
	world.tuning = tuning;

	// search for players
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(!snap.characters[i].active)
			continue;
			
		gameclient.clients[i].predicted.world = &world;
		world.characters[i] = &gameclient.clients[i].predicted;
		gameclient.clients[i].predicted.read(&snap.characters[i].cur);
	}
	
	// predict
	for(int tick = client_tick()+1; tick <= client_predtick(); tick++)
	{
		// fetch the local
		if(tick == client_predtick() && world.characters[snap.local_cid])
			predicted_prev_char = *world.characters[snap.local_cid];
		
		// first calculate where everyone should move
		for(int c = 0; c < MAX_CLIENTS; c++)
		{
			if(!world.characters[c])
				continue;

			mem_zero(&world.characters[c]->input, sizeof(world.characters[c]->input));
			if(snap.local_cid == c)
			{
				// apply player input
				int *input = client_get_input(tick);
				if(input)
					world.characters[c]->input = *((NETOBJ_PLAYER_INPUT*)input);
				world.characters[c]->tick(true);
			}
			else
				world.characters[c]->tick(false);

		}

		// move all players and quantize their data
		for(int c = 0; c < MAX_CLIENTS; c++)
		{
			if(!world.characters[c])
				continue;

			world.characters[c]->move();
			world.characters[c]->quantize();
		}
		
		// check if we want to trigger effects
		if(tick > last_new_predicted_tick)
		{
			last_new_predicted_tick = tick;
			new_predicted_tick = true;
			
			if(snap.local_cid != -1 && world.characters[snap.local_cid])
			{
				vec2 pos = world.characters[snap.local_cid]->pos;
				int events = world.characters[snap.local_cid]->triggered_events;
				if(events&COREEVENT_GROUND_JUMP) gameclient.sounds->play_and_record(SOUNDS::CHN_WORLD, SOUND_PLAYER_JUMP, 1.0f, pos);
				
				/*if(events&COREEVENT_AIR_JUMP)
				{
					gameclient.effects->air_jump(pos);
					gameclient.sounds->play_and_record(SOUNDS::CHN_WORLD, SOUND_PLAYER_AIRJUMP, 1.0f, pos);
				}*/
				
				//if(events&COREEVENT_HOOK_LAUNCH) snd_play_random(CHN_WORLD, SOUND_HOOK_LOOP, 1.0f, pos);
				//if(events&COREEVENT_HOOK_ATTACH_PLAYER) snd_play_random(CHN_WORLD, SOUND_HOOK_ATTACH_PLAYER, 1.0f, pos);
				if(events&COREEVENT_HOOK_ATTACH_GROUND) gameclient.sounds->play_and_record(SOUNDS::CHN_WORLD, SOUND_HOOK_ATTACH_GROUND, 1.0f, pos);
				if(events&COREEVENT_HOOK_HIT_NOHOOK) gameclient.sounds->play_and_record(SOUNDS::CHN_WORLD, SOUND_HOOK_NOATTACH, 1.0f, pos);
				//if(events&COREEVENT_HOOK_RETRACT) snd_play_random(CHN_WORLD, SOUND_PLAYER_JUMP, 1.0f, pos);
			}
		}
		
		if(tick == client_predtick() && world.characters[snap.local_cid])
			predicted_char = *world.characters[snap.local_cid];
	}
	
	if(config.debug && config.cl_predict && predicted_tick == client_predtick())
	{
		NETOBJ_CHARACTER_CORE before = {0}, now = {0}, before_prev = {0}, now_prev = {0};
		before_char.write(&before);
		before_prev_char.write(&before_prev);
		predicted_char.write(&now);
		predicted_prev_char.write(&now_prev);

		if(mem_comp(&before, &now, sizeof(NETOBJ_CHARACTER_CORE)) != 0)
		{
			dbg_msg("client", "prediction error");
			for(unsigned i = 0; i < sizeof(NETOBJ_CHARACTER_CORE)/sizeof(int); i++)
				if(((int *)&before)[i] != ((int *)&now)[i])
				{
					dbg_msg("", "\t%d %d %d  (%d %d)", i, ((int *)&before)[i], ((int *)&now)[i], ((int *)&before_prev)[i], ((int *)&now_prev)[i]);
				}
		}
	}
	
	predicted_tick = client_predtick();
}

void GAMECLIENT::on_game_over()
{
	if(config.tc_autoscreen && !demorec_isplaying())
		gfx_screenshot();
}

void GAMECLIENT::on_game_restart()
{
	if(!demorec_isplaying() && config.tc_autodemo && demorec_isrecording())
	{
		demorec_record_stop();
		teecomp_demo_start();
	}
	for(int i=0; i<MAX_CLIENTS; i++)
		stats[i].reset();
}

void GAMECLIENT::on_warmup_end()
{
	for(int i=0; i<MAX_CLIENTS; i++)
		stats[i].reset();
}

void GAMECLIENT::on_flag_grab(int id)
{
	stats[snap.flags[id]->carried_by].flag_grabs++;
}

void GAMECLIENT::CLIENT_STATS::reset()
{
	join_date  = 0;
	active     = false;
	was_active = false;
	frags      = 0;
	deaths     = 0;
	suicides   = 0;
	for(int j=0; j<NUM_WEAPONS; j++)
	{
		frags_with[j]  = 0;
		deaths_from[j] = 0;
	}
	flag_grabs      = 0;
	flag_captures   = 0;
	carriers_killed = 0;
	kills_carrying  = 0;
	deaths_carrying = 0;
}

void GAMECLIENT::CLIENT_DATA::update_render_info(int cid)
{
	render_info = skin_info;

	// force team colors
	if(gameclient.snap.gameobj && gameclient.snap.gameobj->flags&GAMEFLAG_TEAMS)
	{
		int local_team;
		if(gameclient.snap.local_info)
			local_team = gameclient.snap.local_info->team;
		else // local_info null when joining a server
			local_team = 0;

		if(team != -1)
		{
			const char* forced_skin;
			int sid = skin_id;
			if(cid != gameclient.snap.local_cid && TeecompUtils::get_forced_skin_name(team, local_team, forced_skin))
				sid = max(0, gameclient.skins->find(forced_skin));

			if(TeecompUtils::get_force_dm_colors(team, local_team))
			{
				render_info.texture = gameclient.skins->get(sid)->org_texture;
				render_info.color_body = vec4(1,1,1,1);
				render_info.color_feet = vec4(1,1,1,1);
			}
			else
			{
				render_info.texture = gameclient.skins->get(sid)->color_texture;
				vec3 col = TeecompUtils::getTeamColor(team, local_team, config.tc_colored_tees_team1,
					config.tc_colored_tees_team2, config.tc_colored_tees_method);
				render_info.color_body = vec4(col.r, col.g, col.b, 1.0f);
				render_info.color_feet = vec4(col.r, col.g, col.b, 1.0f);
			}
		}
		else
		{
			render_info.color_body = vec4(1,1,1,1);
			render_info.color_feet = vec4(1,1,1,1);
		}
	}
	else if(config.tc_force_skin_team1 && cid != gameclient.snap.local_cid) // Force DM skin
	{
		const SKINS::SKIN* skin;
		skin = gameclient.skins->get(max(0, gameclient.skins->find(config.tc_forced_skin1)));
		if(use_custom_color)
			render_info.texture = skin->color_texture;
		else
			render_info.texture = skin->org_texture;
	}	
}

void GAMECLIENT::send_switch_team(int team)
{
	NETMSG_CL_SETTEAM msg;
	msg.team = team;
	msg.pack(MSGFLAG_VITAL);
	client_send_msg();	
}

void GAMECLIENT::send_info(bool start)
{
	if(start)
	{
		NETMSG_CL_STARTINFO msg;
		msg.name = config.player_name;
		msg.skin = config.player_skin;
		msg.use_custom_color = config.player_use_custom_color;
		msg.color_body = config.player_color_body;
		msg.color_feet = config.player_color_feet;
		msg.pack(MSGFLAG_VITAL);
	}
	else
	{
		NETMSG_CL_CHANGEINFO msg;
		msg.name = config.player_name;
		msg.skin = config.player_skin;
		msg.use_custom_color = config.player_use_custom_color;
		msg.color_body = config.player_color_body;
		msg.color_feet = config.player_color_feet;
		msg.pack(MSGFLAG_VITAL);
	}
	client_send_msg();
}

void GAMECLIENT::send_kill(int client_id)
{
	NETMSG_CL_KILL msg;
	msg.pack(MSGFLAG_VITAL);
	client_send_msg();
}

void GAMECLIENT::con_team(void *result, void *user_data)
{
	((GAMECLIENT*)user_data)->send_switch_team(console_arg_int(result, 0));
}

void GAMECLIENT::con_kill(void *result, void *user_data)
{
	((GAMECLIENT*)user_data)->send_kill(-1);
}
