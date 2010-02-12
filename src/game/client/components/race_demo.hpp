#include <game/client/component.hpp>

class RACE : public COMPONENT
{
	int race_state;
	int record_time;
	float time;
	const char *map;
	
public:
	RACE();
	
	virtual void on_render();
	virtual void on_reset();
	virtual void on_message(int msgtype, void *rawmsg);
};
