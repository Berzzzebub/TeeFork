#include <base/vmath.hpp>
#include <game/client/component.hpp>

class SPEEDTAILIND : public COMPONENT
{
	int64 lastupdate;
	struct ITEM
	{
		vec2 pos;
		float life;

		float r, g, b, a;
	};

	enum
	{
		MAX_ITEMS=128,
	};

	ITEM items[MAX_ITEMS];
	int num_items;

	ITEM *create_i();
	void destroy_i(ITEM *i);

public:	
	SPEEDTAILIND();

	void create(vec2 pos);
	virtual void on_render();
};
