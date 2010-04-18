/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */

#include <game/server/gamecontroller.hpp>
//#include <game/server/gamemodes/ctf.hpp>

class GAMECONTROLLER_KTF : public GAMECONTROLLER
{
public:
	GAMECONTROLLER_KTF();

	virtual void tick();
	virtual int on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon);
	virtual void on_character_spawn(class CHARACTER *chr);
	virtual void endround();

	void set_new_flagkeeper();
	void draw_dir_stars();
	void update_colors();
	int leader_score();

	class FLAG *flag;
	int flag_keeper_id;
	int point_counter;
};
