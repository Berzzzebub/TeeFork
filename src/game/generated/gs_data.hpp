#ifndef SERVER_CONTENT_HEADER
#define SERVER_CONTENT_HEADER
struct SOUND
{
	int id;
	const char* filename;
};
struct SOUNDSET
{
	const char* name;
	int num_sounds;
	SOUND *sounds;
	int last;
};
struct IMAGE
{
	const char* name;
	const char* filename;
	int id;
};
struct SPRITESET
{
	IMAGE* image;
	int gridx;
	int gridy;
};
struct SPRITE
{
	const char* name;
	SPRITESET* set;
	int x;
	int y;
	int w;
	int h;
};
struct PICKUPSPEC
{
	const char* name;
	int respawntime;
	int spawndelay;
};
struct ANIM_KEYFRAME
{
	float time;
	float x;
	float y;
	float angle;
};
struct ANIM_SEQUENCE
{
	int num_frames;
	ANIM_KEYFRAME *frames;
};
struct ANIMATION
{
	const char* name;
	ANIM_SEQUENCE body;
	ANIM_SEQUENCE back_foot;
	ANIM_SEQUENCE front_foot;
	ANIM_SEQUENCE attach;
};
struct WEAPONSPEC
{
	const char* name;
	SPRITE* sprite_body;
	SPRITE* sprite_cursor;
	SPRITE* sprite_proj;
	int num_sprite_muzzles;
	SPRITE* *sprite_muzzles;
	int visual_size;
	int firedelay;
	int maxammo;
	int ammoregentime;
	int damage;
	float offsetx;
	float offsety;
	float muzzleoffsetx;
	float muzzleoffsety;
	float muzzleduration;
};
struct WEAPONSPEC_HAMMER
{
	WEAPONSPEC* base;
};
struct WEAPONSPEC_GUN
{
	WEAPONSPEC* base;
	float curvature;
	float speed;
	float lifetime;
};
struct WEAPONSPEC_SHOTGUN
{
	WEAPONSPEC* base;
	float curvature;
	float speed;
	float speeddiff;
	float lifetime;
};
struct WEAPONSPEC_GRENADE
{
	WEAPONSPEC* base;
	float curvature;
	float speed;
	float lifetime;
};
struct WEAPONSPEC_RIFLE
{
	WEAPONSPEC* base;
	float reach;
	int bounce_delay;
	int bounce_num;
	float bounce_cost;
};
struct WEAPONSPEC_NINJA
{
	WEAPONSPEC* base;
	int duration;
	int movetime;
	int velocity;
};
struct WEAPONSPECS
{
	WEAPONSPEC_HAMMER hammer;
	WEAPONSPEC_HAMMER gun;
	WEAPONSPEC_SHOTGUN shotgun;
	WEAPONSPEC_GRENADE grenade;
	WEAPONSPEC_RIFLE rifle;
	WEAPONSPEC_NINJA ninja;
	int num_id;
	WEAPONSPEC *id;
};
struct DATACONTAINER
{
	int num_sounds;
	SOUNDSET *sounds;
	int num_images;
	IMAGE *images;
	int num_pickups;
	PICKUPSPEC *pickups;
	int num_spritesets;
	SPRITESET *spritesets;
	int num_sprites;
	SPRITE *sprites;
	int num_animations;
	ANIMATION *animations;
	WEAPONSPECS weapons;
};
extern DATACONTAINER *data;
enum
{
	IMAGE_NULL=0,
	IMAGE_GAME,
	IMAGE_GAME_GRAY,
	IMAGE_PARTICLES,
	IMAGE_CURSOR,
	IMAGE_BANNER,
	IMAGE_EMOTICONS,
	IMAGE_BROWSEICONS,
	IMAGE_CONSOLE_BG,
	IMAGE_CONSOLE_BAR,
	NUM_IMAGES
};
enum
{
	ANIM_BASE=0,
	ANIM_IDLE,
	ANIM_INAIR,
	ANIM_WALK,
	ANIM_HAMMER_SWING,
	ANIM_NINJA_SWING,
	NUM_ANIMS
};
enum
{
	SPRITE_PART_SLICE=0,
	SPRITE_PART_BALL,
	SPRITE_PART_SPLAT01,
	SPRITE_PART_SPLAT02,
	SPRITE_PART_SPLAT03,
	SPRITE_PART_SMOKE,
	SPRITE_PART_SHELL,
	SPRITE_PART_EXPL01,
	SPRITE_PART_AIRJUMP,
	SPRITE_HEALTH_FULL,
	SPRITE_HEALTH_EMPTY,
	SPRITE_ARMOR_FULL,
	SPRITE_ARMOR_EMPTY,
	SPRITE_STAR1,
	SPRITE_STAR2,
	SPRITE_STAR3,
	SPRITE_PART1,
	SPRITE_PART2,
	SPRITE_PART3,
	SPRITE_PART4,
	SPRITE_PART5,
	SPRITE_PART6,
	SPRITE_PART7,
	SPRITE_PART8,
	SPRITE_PART9,
	SPRITE_WEAPON_GUN_BODY,
	SPRITE_WEAPON_GUN_CURSOR,
	SPRITE_WEAPON_GUN_PROJ,
	SPRITE_WEAPON_GUN_MUZZLE1,
	SPRITE_WEAPON_GUN_MUZZLE2,
	SPRITE_WEAPON_GUN_MUZZLE3,
	SPRITE_WEAPON_SHOTGUN_BODY,
	SPRITE_WEAPON_SHOTGUN_CURSOR,
	SPRITE_WEAPON_SHOTGUN_PROJ,
	SPRITE_WEAPON_SHOTGUN_MUZZLE1,
	SPRITE_WEAPON_SHOTGUN_MUZZLE2,
	SPRITE_WEAPON_SHOTGUN_MUZZLE3,
	SPRITE_WEAPON_GRENADE_BODY,
	SPRITE_WEAPON_GRENADE_CURSOR,
	SPRITE_WEAPON_GRENADE_PROJ,
	SPRITE_WEAPON_HAMMER_BODY,
	SPRITE_WEAPON_HAMMER_CURSOR,
	SPRITE_WEAPON_HAMMER_PROJ,
	SPRITE_WEAPON_NINJA_BODY,
	SPRITE_WEAPON_NINJA_CURSOR,
	SPRITE_WEAPON_NINJA_PROJ,
	SPRITE_WEAPON_RIFLE_BODY,
	SPRITE_WEAPON_RIFLE_CURSOR,
	SPRITE_WEAPON_RIFLE_PROJ,
	SPRITE_HOOK_CHAIN,
	SPRITE_HOOK_HEAD,
	SPRITE_WEAPON_NINJA_MUZZLE1,
	SPRITE_WEAPON_NINJA_MUZZLE2,
	SPRITE_WEAPON_NINJA_MUZZLE3,
	SPRITE_PICKUP_HEALTH,
	SPRITE_PICKUP_ARMOR,
	SPRITE_PICKUP_WEAPON,
	SPRITE_PICKUP_NINJA,
	SPRITE_FLAG_BLUE,
	SPRITE_FLAG_RED,
	SPRITE_TEE_BODY,
	SPRITE_TEE_BODY_OUTLINE,
	SPRITE_TEE_FOOT,
	SPRITE_TEE_FOOT_OUTLINE,
	SPRITE_TEE_HAND,
	SPRITE_TEE_HAND_OUTLINE,
	SPRITE_TEE_EYE_NORMAL,
	SPRITE_TEE_EYE_ANGRY,
	SPRITE_TEE_EYE_PAIN,
	SPRITE_TEE_EYE_HAPPY,
	SPRITE_TEE_EYE_DEAD,
	SPRITE_TEE_EYE_SURPRISE,
	SPRITE_OOP,
	SPRITE_EXCLAMATION,
	SPRITE_HEARTS,
	SPRITE_DROP,
	SPRITE_DOTDOT,
	SPRITE_MUSIC1,
	SPRITE_MUSIC2,
	SPRITE_GHOST,
	SPRITE_SUSHI,
	SPRITE_SPLATTEE,
	SPRITE_DEVILTEE,
	SPRITE_ZOMG,
	SPRITE_ZZZ,
	SPRITE_BLANK1,
	SPRITE_DEADTEE,
	SPRITE_BLANK2,
	SPRITE_BROWSE_LOCK,
	SPRITE_BROWSE_HEART,
	SPRITE_BROWSE_UNPURE,
	NUM_SPRITES
};
#endif
