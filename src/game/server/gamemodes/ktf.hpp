/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */

#include <game/server/gamecontroller.hpp>
//#include <game/server/gamemodes/ctf.hpp>

enum
{
	STAND_SPAWN,
	STAND_NINJA,
	STAND_FLAGSTAND,
};

class GAMECONTROLLER_KTF : public GAMECONTROLLER
{
public:
	GAMECONTROLLER_KTF();

	virtual void tick();
	//virtual bool on_entity(int index, vec2 pos);
	virtual int on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon);
	virtual void change_flag_stand(vec2 pos, int type);
	virtual void on_character_spawn(class CHARACTER *chr);
	void set_new_flagkeeper();
	void draw_dir_stars();
	float shift;
	class FLAG *flag;
	int flag_keeper_id;
	int point_counter;
	int stand_type;
};
