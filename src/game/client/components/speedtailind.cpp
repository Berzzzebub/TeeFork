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

		i->r = 1.0f + (speed - smooth_table[last_index])/config.cl_speedtail_sens;
		i->g = 1.0f - (speed - smooth_table[last_index])/config.cl_speedtail_sens;
		i->b = 0.0f;
		i->a = 1.0f;


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
			draw_sprite(pos.x, pos.y, 48.0f);
			i++;
		}
	}
	gfx_quads_end();
}
