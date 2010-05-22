#include <engine/e_client_interface.h>
#include <game/generated/g_protocol.hpp>
#include <game/generated/gc_data.hpp>
#include <game/client/gameclient.hpp>
#include <game/gamecore.hpp> // get_angle
#include <game/client/ui.hpp>
#include <game/client/render.hpp>
#include "speedtailind.hpp"

SPEEDTAILIND::SPEEDTAILIND()
{
	lastupdate = 0;
	num_items = 0;
}

SPEEDTAILIND::ITEM *SPEEDTAILIND::create_i()
{
	if (num_items < MAX_ITEMS)
	{
		ITEM *p = &items[num_items];
		num_items++;
		return p;
	}
	return 0;
}

void SPEEDTAILIND::destroy_i(SPEEDTAILIND::ITEM *i)
{
	num_items--;
	*i = items[num_items];
}

void SPEEDTAILIND::create(vec2 pos)
{
	static float speed;
	static vec2 oldpos;
	static const int SMOOTH_TABLE_SIZE = 24;
	static float smooth_table[SMOOTH_TABLE_SIZE];
	static int smooth_index = 0;
	static float last_speed = 0;
	static float last_acceleration = 0;

	smooth_table[smooth_index] = distance(pos, oldpos)/client_frametime();

	smooth_index = (smooth_index + 1) % SMOOTH_TABLE_SIZE;
	oldpos = pos;
	speed = 0;
	for(int j=0; j<SMOOTH_TABLE_SIZE; j++)
		speed += smooth_table[j];

	speed /= SMOOTH_TABLE_SIZE;

	int last_index = smooth_index - 1;
	if(last_index < 0)
		last_index = SMOOTH_TABLE_SIZE - 1;

	ITEM *i = create_i();
	if (i)
	{
		i->pos = pos;
		i->life = 1.5f;
		i->speed = speed;

		float velspeed = length(vec2(gameclient.snap.local_character->vx/256.0f, gameclient.snap.local_character->vy/256.0f))*50;
		float acceleration =  velspeed - last_speed;
		last_speed = velspeed;
		if(acceleration)
		{
			char buf[64];
			str_format(buf, sizeof(buf), "%f", acceleration);


			float w = gfx_text_width(0, 14, buf, -1);
			gfx_text(0, 150*gfx_screenaspect()-w/2, 35, 14, buf, -1);
			dbg_msg("SpeedtailDebug", "acceleration = '%f'", acceleration);
		}
		else
			if(last_acceleration)
				acceleration = last_acceleration;
		i->r = 1.0f + acceleration / 100;
		i->g = 1.0f - acceleration / 100;
		i->b = 0.0f;
		i->a = 1.0f;

		last_acceleration = acceleration;
	}
}

void SPEEDTAILIND::on_render()
{


	gfx_texture_set(data->images[IMAGE_GAME].id);
	gfx_quads_begin();

	for(int i = 0; i < num_items;)
	{
		vec2 pos = mix(items[i].pos, items[i].pos, clamp(1.0f, 0.0f, 1.0f));

		items[i].life -= client_frametime();
		if(items[i].life < 0.0f)
			destroy_i(&items[i]);
		else
		{
			gfx_setcolor(items[i].r, items[i].g, items[i].b, items[i].a);
			select_sprite(SPRITE_PART4);
			float factor = (items[i].speed/15);
			if (factor < 0.5f)
				factor = 0.5f;
			draw_sprite(pos.x, pos.y, 0.5f * factor);
			i++;
		}
	}
	gfx_quads_end();
}
