#include <game/client/component.hpp>

class TEECOMP_STATS: public COMPONENT
{
private:
	int mode;
	int stats_cid;
	static void con_key_stats(void *result, void *user_data);
	static void con_key_next(void *result, void *user_data);
	void render_global_stats();
	void render_individual_stats();
	void check_stats_cid();

public:
	TEECOMP_STATS();
	virtual void on_reset();
	virtual void on_console_init();
	virtual void on_render();
	virtual void on_message(int msgtype, void *rawmsg);
	bool is_active();
};
