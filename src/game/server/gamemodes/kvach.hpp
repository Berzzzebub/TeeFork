/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */

#include <game/server/gamecontroller.hpp>

// you can subclass GAMECONTROLLER_CTF, GAMECONTROLLER_TDM etc if you want
// todo a modification with their base as well.
class GAMECONTROLLER_KVACH : public GAMECONTROLLER
{
public:
	int kvacher_client_id;
	int leader_client_id;
	int last_kvacher_client_id;
	int invinsible_tick;


	GAMECONTROLLER_KVACH();
	virtual void tick();
	virtual bool on_entity(int index, vec2 pos);
	virtual void on_character_spawn(class CHARACTER *chr);
	virtual void on_player_info_change(class PLAYER *p);
	virtual int on_character_death(class CHARACTER *victim, class PLAYER *killer, int weapon);
	// add more virtual functions here if you wish
	void explosion_calculate(vec2 p, int owner, int weapon, bool bnodamage);
	void damage_calculate(class CHARACTER *chrTo, int dmg, class CHARACTER *chrFrom, int weapon);

	void choose_new_kvach();
	void update_score_leader();
	void update_equipment();
	bool timer_going(int timerTick, int timerLife);
	void update_colors();
};
