#include "globals.h"
#include <unistd.h>

int flies_enabled = 1;

struct {
	int x, y;
	int old_x, old_y;
	int old_draw_x, old_draw_y;
	int back[2];
	int back_defined[2];
} flies[NUM_FLIES];

int flip = 0;

gob_t rabbit_gobs = { 0 };
gob_t font_gobs = { 0 };
gob_t object_gobs = { 0 };
gob_t number_gobs = { 0 };

int endscore_reached;

main_info_t main_info;
player_t player[JNB_MAX_PLAYERS];
player_anim_t player_anims[7];
object_t objects[NUM_OBJECTS];

unsigned char *datafile_buffer = NULL;

unsigned char *background_pic;
unsigned char *mask_pic;
char pal[768];

int ai[JNB_MAX_PLAYERS];

unsigned int ban_map[17][22] = {
	{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0},
	{1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
	{2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 1, 1},
	{2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

#define GET_BAN_MAP_XY(x,y) ban_map[(y) >> 4][(x) >> 4]

struct {
	int num_frames;
	int restart_frame;
	struct {
		int image;
		int ticks;
	} frame[10];
} object_anims[8] = {
	{
		6, 0,
		{
			{0, 3},
			{1, 3},
			{2, 3},
			{3, 3},
			{4, 3},
			{5, 3},
			{0, 0},
			{0, 0},
			{0, 0},
			{0, 0}
		}
	}, {
		9, 0,
		{
			{6, 2},
			{7, 2},
			{8, 2},
			{9, 2},
			{10, 2},
			{11, 2},
			{12, 2},
			{13, 2},
			{14, 2},
			{0, 0}
		}
	}, {
		5, 0,
		{
			{15, 3},
			{16, 3},
			{16, 3},
			{17, 3},
			{18, 3},
			{19, 3},
			{0, 0},
			{0, 0},
			{0, 0},
			{0, 0}
		}
	}, {
		10, 0,
		{
			{20, 2},
			{21, 2},
			{22, 2},
			{23, 2},
			{24, 2},
			{25, 2},
			{24, 2},
			{23, 2},
			{22, 2},
			{21, 2}
		}
	}, {
		10, 0,
		{
			{26, 2},
			{27, 2},
			{28, 2},
			{29, 2},
			{30, 2},
			{31, 2},
			{30, 2},
			{29, 2},
			{28, 2},
			{27, 2}
		}
	}, {
		10, 0,
		{
			{32, 2},
			{33, 2},
			{34, 2},
			{35, 2},
			{36, 2},
			{37, 2},
			{36, 2},
			{35, 2},
			{34, 2},
			{33, 2}
		}
	}, {
		10, 0,
		{
			{38, 2},
			{39, 2},
			{40, 2},
			{41, 2},
			{42, 2},
			{43, 2},
			{42, 2},
			{41, 2},
			{40, 2},
			{39, 2}
		}
	}, {
		4, 0,
		{
			{76, 4},
			{77, 4},
			{78, 4},
			{79, 4},
			{0, 0},
			{0, 0},
			{0, 0},
			{0, 0},
			{0, 0},
			{0, 0}
		}
	}
};

struct {
	struct {
		short num_pobs;
		struct {
			int x, y;
			int image;
			gob_t *pob_data;
		} pobs[NUM_LEFTOVERS];
	} page[2];
} leftovers;

int client_player_num = -1;

int add_leftovers(int page, int x, int y, int image, gob_t *pob_data)
{

	if (leftovers.page[page].num_pobs >= NUM_LEFTOVERS)
		return 1;

	leftovers.page[page].pobs[leftovers.page[page].num_pobs].x = x;
	leftovers.page[page].pobs[leftovers.page[page].num_pobs].y = y;
	leftovers.page[page].pobs[leftovers.page[page].num_pobs].image = image;
	leftovers.page[page].pobs[leftovers.page[page].num_pobs].pob_data = pob_data;
	leftovers.page[page].num_pobs++;

	return 0;
}

void add_object(int type, int x, int y, int x_add, int y_add, int anim, int frame)
{
	int c1;

	for (c1 = 0; c1 < NUM_OBJECTS; c1++) {
		if (objects[c1].used == 0) {
			objects[c1].used = 1;
			objects[c1].type = type;
			objects[c1].x = (long) x << 16;
			objects[c1].y = (long) y << 16;
			objects[c1].x_add = x_add;
			objects[c1].y_add = y_add;
			objects[c1].x_acc = 0;
			objects[c1].y_acc = 0;
			objects[c1].anim = anim;
			objects[c1].frame = frame;
			objects[c1].ticks = object_anims[anim].frame[frame].ticks;
			objects[c1].image = object_anims[anim].frame[frame].image;
			break;
		}
	}
}

int init_program(const void *data, size_t size)
{
	unsigned char *handle = (unsigned char *) NULL;
	int c1 = 0, c2 = 0;
	sfx_data fly;
	int player_anim_data[] = {
		1, 0, 0, 0x7fff, 0, 0, 0, 0, 0, 0,
		4, 0, 0, 4, 1, 4, 2, 4, 3, 4,
		1, 0, 4, 0x7fff, 0, 0, 0, 0, 0, 0,
		4, 2, 5, 8, 6, 10, 7, 3, 6, 3,
		1, 0, 6, 0x7fff, 0, 0, 0, 0, 0, 0,
		2, 1, 5, 8, 4, 0x7fff, 0, 0, 0, 0,
		1, 0, 8, 5, 0, 0, 0, 0, 0, 0
	};

	srand(time(NULL));

	if (hook_keyb_handler() != 0)
		return 1;

	memset(&main_info, 0, sizeof(main_info));

    datafile_buffer = (unsigned char *) malloc(size);
    memcpy(datafile_buffer, data, size);

#if 0
/** It should not be necessary to assign a default player number here. The
server assigns one in init_server, the client gets one assigned by the server,
all provided the user didn't choose one on the commandline. */
	if (is_net) {
		if (client_player_num < 0)
		        client_player_num = 0;
		player[client_player_num].enabled = 1;
	}
#endif

	main_info.pob_backbuf[0] = malloc(screen_pitch*screen_height);
	main_info.pob_backbuf[1] = malloc(screen_pitch*screen_height);

	for (c1 = 0; c1 < 7; c1++) {
		player_anims[c1].num_frames = player_anim_data[c1 * 10];
		player_anims[c1].restart_frame = player_anim_data[c1 * 10 + 1];
		for (c2 = 0; c2 < 4; c2++) {
			player_anims[c1].frame[c2].image = player_anim_data[c1 * 10 + c2 * 2 + 2];
			player_anims[c1].frame[c2].ticks = player_anim_data[c1 * 10 + c2 * 2 + 3];
		}
	}

	if ((handle = dat_open("menu.pcx")) == 0) {
		strcpy(main_info.error_str, "Error loading 'menu.pcx', aborting...\n");
		return 1;
	}
	if (read_pcx(handle, background_pic, JNB_WIDTH*JNB_HEIGHT, pal) != 0) {
		strcpy(main_info.error_str, "Error loading 'menu.pcx', aborting...\n");
		return 1;
	}

	if ((handle = dat_open("rabbit.gob")) == 0) {
		strcpy(main_info.error_str, "Error loading 'rabbit.gob', aborting...\n");
		return 1;
	}
	if (register_gob(handle, &rabbit_gobs, dat_filelen("rabbit.gob"))) {
		/* error */
		return 1;
	}

	if ((handle = dat_open("objects.gob")) == 0) {
		strcpy(main_info.error_str, "Error loading 'objects.gob', aborting...\n");
		return 1;
	}
	if (register_gob(handle, &object_gobs, dat_filelen("objects.gob"))) {
		/* error */
		return 1;
	}

	if ((handle = dat_open("font.gob")) == 0) {
		strcpy(main_info.error_str, "Error loading 'font.gob', aborting...\n");
		return 1;
	}
	if (register_gob(handle, &font_gobs, dat_filelen("font.gob"))) {
		/* error */
		return 1;
	}

	if ((handle = dat_open("numbers.gob")) == 0) {
		strcpy(main_info.error_str, "Error loading 'numbers.gob', aborting...\n");
		return 1;
	}
	if (register_gob(handle, &number_gobs, dat_filelen("numbers.gob"))) {
		/* error */
		return 1;
	}

	if (read_level() != 0) {
		strcpy(main_info.error_str, "Error loading 'levelmap.txt', aborting...\n");
		return 1;
	}

	open_screen();
	dj_init();

	if (main_info.no_sound == 0) {
		dj_autodetect_sd();
		dj_set_mixing_freq(20000);
		dj_set_stereo(0);
		dj_set_auto_mix(0);
		dj_set_dma_time(8);
		dj_set_num_sfx_channels(5);
		dj_set_sfx_volume(64);
		dj_set_nosound(1);
		dj_start();

		if ((handle = dat_open("jump.mod")) == 0) {
			strcpy(main_info.error_str, "Error loading 'jump.mod', aborting...\n");
			return 1;
		}
		if (dj_load_mod(handle, 0, MOD_MENU) != 0) {
			strcpy(main_info.error_str, "Error loading 'jump.mod', aborting...\n");
			return 1;
		}

		if ((handle = dat_open("bump.mod")) == 0) {
			strcpy(main_info.error_str, "Error loading 'bump.mod', aborting...\n");
			return 1;
		}
		if (dj_load_mod(handle, 0, MOD_GAME) != 0) {
			strcpy(main_info.error_str, "Error loading 'bump.mod', aborting...\n");
			return 1;
		}

		if ((handle = dat_open("scores.mod")) == 0) {
			strcpy(main_info.error_str, "Error loading 'scores.mod', aborting...\n");
			return 1;
		}
		if (dj_load_mod(handle, 0, MOD_SCORES) != 0) {
			strcpy(main_info.error_str, "Error loading 'scores.mod', aborting...\n");
			return 1;
		}

		if ((handle = dat_open("jump.smp")) == 0) {
			strcpy(main_info.error_str, "Error loading 'jump.smp', aborting...\n");
			return 1;
		}
		if (dj_load_sfx(handle, 0, dat_filelen("jump.smp"), DJ_SFX_TYPE_SMP, SFX_JUMP) != 0) {
			strcpy(main_info.error_str, "Error loading 'jump.smp', aborting...\n");
			return 1;
		}

		if ((handle = dat_open("death.smp")) == 0) {
			strcpy(main_info.error_str, "Error loading 'death.smp', aborting...\n");
			return 1;
		}
		if (dj_load_sfx(handle, 0, dat_filelen("death.smp"), DJ_SFX_TYPE_SMP, SFX_DEATH) != 0) {
			strcpy(main_info.error_str, "Error loading 'death.smp', aborting...\n");
			return 1;
		}

		if ((handle = dat_open("spring.smp")) == 0) {
			strcpy(main_info.error_str, "Error loading 'spring.smp', aborting...\n");
			return 1;
		}
		if (dj_load_sfx(handle, 0, dat_filelen("spring.smp"), DJ_SFX_TYPE_SMP, SFX_SPRING) != 0) {
			strcpy(main_info.error_str, "Error loading 'spring.smp', aborting...\n");
			return 1;
		}

		if ((handle = dat_open("splash.smp")) == 0) {
			strcpy(main_info.error_str, "Error loading 'splash.smp', aborting...\n");
			return 1;
		}
		if (dj_load_sfx(handle, 0, dat_filelen("splash.smp"), DJ_SFX_TYPE_SMP, SFX_SPLASH) != 0) {
			strcpy(main_info.error_str, "Error loading 'splash.smp', aborting...\n");
			return 1;
		}

		if ((handle = dat_open("fly.smp")) == 0) {
			strcpy(main_info.error_str, "Error loading 'fly.smp', aborting...\n");
			return 1;
		}
		if (dj_load_sfx(handle, 0, dat_filelen("fly.smp"), DJ_SFX_TYPE_SMP, SFX_FLY) != 0) {
			strcpy(main_info.error_str, "Error loading 'fly.smp', aborting...\n");
			return 1;
		}

		dj_get_sfx_settings(SFX_FLY, &fly);
		fly.priority = 10;
		fly.default_freq = SFX_FLY_FREQ;
		fly.loop = 1;
		fly.loop_start = 0;
		fly.loop_length = fly.length;
		dj_set_sfx_settings(SFX_FLY, &fly);
	}

	if ((background_pic = (unsigned char*)malloc(JNB_WIDTH*JNB_HEIGHT)) == NULL)
		return 1;
	if ((mask_pic = (unsigned char*)malloc(JNB_WIDTH*JNB_HEIGHT)) == NULL)
		return 1;

	memset(mask_pic, 0, JNB_WIDTH*JNB_HEIGHT);
	register_mask(mask_pic);

	/* fix dark font */
	for (c1 = 0; c1 < 16; c1++) {
		pal[(240 + c1) * 3 + 0] = c1 << 2;
		pal[(240 + c1) * 3 + 1] = c1 << 2;
		pal[(240 + c1) * 3 + 2] = c1 << 2;
	}

	setpalette(0, 256, pal);

	for(c1 = 0; c1 < JNB_MAX_PLAYERS; c1++)		// reset player values
    {
		ai[c1] = 0;
    }

	return 0;
}

unsigned short rnd(unsigned short max)
{
#if (RAND_MAX < 0x7fff)
#error "rand returns too small values"
#elif (RAND_MAX == 0x7fff)
	return (unsigned short)((rand()*2) % (int)max);
#else
	return (unsigned short)(rand() % (int)max);
#endif
}

void update_objects(void)
{
	int c1;
	int s1 = 0;

	for (c1 = 0; c1 < NUM_OBJECTS; c1++) {
		if (objects[c1].used == 1) {
			switch (objects[c1].type) {
			case OBJ_SPRING:
				objects[c1].ticks--;
				if (objects[c1].ticks <= 0) {
					objects[c1].frame++;
					if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames) {
						objects[c1].frame--;
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
					} else {
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					}
				}
				if (objects[c1].used == 1)
					add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image, &object_gobs);
				break;
			case OBJ_SPLASH:
				objects[c1].ticks--;
				if (objects[c1].ticks <= 0) {
					objects[c1].frame++;
					if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames)
						objects[c1].used = 0;
					else {
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					}
				}
				if (objects[c1].used == 1)
					add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image, &object_gobs);
				break;
			case OBJ_SMOKE:
				objects[c1].x += objects[c1].x_add;
				objects[c1].y += objects[c1].y_add;
				objects[c1].ticks--;
				if (objects[c1].ticks <= 0) {
					objects[c1].frame++;
					if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames)
						objects[c1].used = 0;
					else {
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					}
				}
				if (objects[c1].used == 1)
					add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image, &object_gobs);
				break;
			case OBJ_YEL_BUTFLY:
			case OBJ_PINK_BUTFLY:
				objects[c1].x_acc += rnd(128) - 64;
				if (objects[c1].x_acc < -1024)
					objects[c1].x_acc = -1024;
				if (objects[c1].x_acc > 1024)
					objects[c1].x_acc = 1024;
				objects[c1].x_add += objects[c1].x_acc;
				if (objects[c1].x_add < -32768)
					objects[c1].x_add = -32768;
				if (objects[c1].x_add > 32768)
					objects[c1].x_add = 32768;
				objects[c1].x += objects[c1].x_add;
				if ((objects[c1].x >> 16) < 16) {
					objects[c1].x = 16 << 16;
					objects[c1].x_add = -objects[c1].x_add >> 2;
					objects[c1].x_acc = 0;
				} else if ((objects[c1].x >> 16) > 350) {
					objects[c1].x = 350 << 16;
					objects[c1].x_add = -objects[c1].x_add >> 2;
					objects[c1].x_acc = 0;
				}
				if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 0) {
					if (objects[c1].x_add < 0) {
						objects[c1].x = (((objects[c1].x >> 16) + 16) & 0xfff0) << 16;
					} else {
						objects[c1].x = ((((objects[c1].x >> 16) - 16) & 0xfff0) + 15) << 16;
					}
					objects[c1].x_add = -objects[c1].x_add >> 2;
					objects[c1].x_acc = 0;
				}
				objects[c1].y_acc += rnd(64) - 32;
				if (objects[c1].y_acc < -1024)
					objects[c1].y_acc = -1024;
				if (objects[c1].y_acc > 1024)
					objects[c1].y_acc = 1024;
				objects[c1].y_add += objects[c1].y_acc;
				if (objects[c1].y_add < -32768)
					objects[c1].y_add = -32768;
				if (objects[c1].y_add > 32768)
					objects[c1].y_add = 32768;
				objects[c1].y += objects[c1].y_add;
				if ((objects[c1].y >> 16) < 0) {
					objects[c1].y = 0;
					objects[c1].y_add = -objects[c1].y_add >> 2;
					objects[c1].y_acc = 0;
				} else if ((objects[c1].y >> 16) > 255) {
					objects[c1].y = 255 << 16;
					objects[c1].y_add = -objects[c1].y_add >> 2;
					objects[c1].y_acc = 0;
				}
				if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 0) {
					if (objects[c1].y_add < 0) {
						objects[c1].y = (((objects[c1].y >> 16) + 16) & 0xfff0) << 16;
					} else {
						objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
					}
					objects[c1].y_add = -objects[c1].y_add >> 2;
					objects[c1].y_acc = 0;
				}
				if (objects[c1].type == OBJ_YEL_BUTFLY) {
					if (objects[c1].x_add < 0 && objects[c1].anim != OBJ_ANIM_YEL_BUTFLY_LEFT) {
						objects[c1].anim = OBJ_ANIM_YEL_BUTFLY_LEFT;
						objects[c1].frame = 0;
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					} else if (objects[c1].x_add > 0 && objects[c1].anim != OBJ_ANIM_YEL_BUTFLY_RIGHT) {
						objects[c1].anim = OBJ_ANIM_YEL_BUTFLY_RIGHT;
						objects[c1].frame = 0;
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					}
				} else {
					if (objects[c1].x_add < 0 && objects[c1].anim != OBJ_ANIM_PINK_BUTFLY_LEFT) {
						objects[c1].anim = OBJ_ANIM_PINK_BUTFLY_LEFT;
						objects[c1].frame = 0;
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					} else if (objects[c1].x_add > 0 && objects[c1].anim != OBJ_ANIM_PINK_BUTFLY_RIGHT) {
						objects[c1].anim = OBJ_ANIM_PINK_BUTFLY_RIGHT;
						objects[c1].frame = 0;
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					}
				}
				objects[c1].ticks--;
				if (objects[c1].ticks <= 0) {
					objects[c1].frame++;
					if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames)
						objects[c1].frame = object_anims[objects[c1].anim].restart_frame;
					else {
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					}
				}
				if (objects[c1].used == 1)
					add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image, &object_gobs);
				break;
			case OBJ_FUR:
				if (rnd(100) < 30)
					add_object(OBJ_FLESH_TRACE, objects[c1].x >> 16, objects[c1].y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 0);
				if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 0) {
					objects[c1].y_add += 3072;
					if (objects[c1].y_add > 196608L)
						objects[c1].y_add = 196608L;
				} else if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 2) {
					if (objects[c1].x_add < 0) {
						if (objects[c1].x_add < -65536L)
							objects[c1].x_add = -65536L;
						objects[c1].x_add += 1024;
						if (objects[c1].x_add > 0)
							objects[c1].x_add = 0;
					} else {
						if (objects[c1].x_add > 65536L)
							objects[c1].x_add = 65536L;
						objects[c1].x_add -= 1024;
						if (objects[c1].x_add < 0)
							objects[c1].x_add = 0;
					}
					objects[c1].y_add += 1024;
					if (objects[c1].y_add < -65536L)
						objects[c1].y_add = -65536L;
					if (objects[c1].y_add > 65536L)
						objects[c1].y_add = 65536L;
				}
				objects[c1].x += objects[c1].x_add;
				if ((objects[c1].y >> 16) > 0 && (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 1 || ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 3)) {
					if (objects[c1].x_add < 0) {
						objects[c1].x = (((objects[c1].x >> 16) + 16) & 0xfff0) << 16;
						objects[c1].x_add = -objects[c1].x_add >> 2;
					} else {
						objects[c1].x = ((((objects[c1].x >> 16) - 16) & 0xfff0) + 15) << 16;
						objects[c1].x_add = -objects[c1].x_add >> 2;
					}
				}
				objects[c1].y += objects[c1].y_add;
				if ((objects[c1].x >> 16) < -5 || (objects[c1].x >> 16) > 405 || (objects[c1].y >> 16) > 260)
					objects[c1].used = 0;
				if ((objects[c1].y >> 16) > 0 && (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 0)) {
					if (objects[c1].y_add < 0) {
						if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 2) {
							objects[c1].y = (((objects[c1].y >> 16) + 16) & 0xfff0) << 16;
							objects[c1].x_add >>= 2;
							objects[c1].y_add = -objects[c1].y_add >> 2;
						}
					} else {
						if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 1) {
							if (objects[c1].y_add > 131072L) {
								objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
								objects[c1].x_add >>= 2;
								objects[c1].y_add = -objects[c1].y_add >> 2;
							} else
								objects[c1].used = 0;
						} else if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 3) {
							objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
							if (objects[c1].y_add > 131072L)
								objects[c1].y_add = -objects[c1].y_add >> 2;
							else
								objects[c1].y_add = 0;
						}
					}
				}
				if (objects[c1].x_add < 0 && objects[c1].x_add > -16384)
					objects[c1].x_add = -16384;
				if (objects[c1].x_add > 0 && objects[c1].x_add < 16384)
					objects[c1].x_add = 16384;
				if (objects[c1].used == 1) {
					s1 = (int)(atan2(objects[c1].y_add, objects[c1].x_add) * 4 / M_PI);
					if (s1 < 0)
						s1 += 8;
					if (s1 < 0)
						s1 = 0;
					if (s1 > 7)
						s1 = 7;
					add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].frame + s1, &object_gobs);
				}
				break;
			case OBJ_FLESH:
				if (rnd(100) < 30) {
					if (objects[c1].frame == 76)
						add_object(OBJ_FLESH_TRACE, objects[c1].x >> 16, objects[c1].y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 1);
					else if (objects[c1].frame == 77)
						add_object(OBJ_FLESH_TRACE, objects[c1].x >> 16, objects[c1].y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 2);
					else if (objects[c1].frame == 78)
						add_object(OBJ_FLESH_TRACE, objects[c1].x >> 16, objects[c1].y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 3);
				}
				if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 0) {
					objects[c1].y_add += 3072;
					if (objects[c1].y_add > 196608L)
						objects[c1].y_add = 196608L;
				} else if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 2) {
					if (objects[c1].x_add < 0) {
						if (objects[c1].x_add < -65536L)
							objects[c1].x_add = -65536L;
						objects[c1].x_add += 1024;
						if (objects[c1].x_add > 0)
							objects[c1].x_add = 0;
					} else {
						if (objects[c1].x_add > 65536L)
							objects[c1].x_add = 65536L;
						objects[c1].x_add -= 1024;
						if (objects[c1].x_add < 0)
							objects[c1].x_add = 0;
					}
					objects[c1].y_add += 1024;
					if (objects[c1].y_add < -65536L)
						objects[c1].y_add = -65536L;
					if (objects[c1].y_add > 65536L)
						objects[c1].y_add = 65536L;
				}
				objects[c1].x += objects[c1].x_add;
				if ((objects[c1].y >> 16) > 0 && (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 1 || ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 3)) {
					if (objects[c1].x_add < 0) {
						objects[c1].x = (((objects[c1].x >> 16) + 16) & 0xfff0) << 16;
						objects[c1].x_add = -objects[c1].x_add >> 2;
					} else {
						objects[c1].x = ((((objects[c1].x >> 16) - 16) & 0xfff0) + 15) << 16;
						objects[c1].x_add = -objects[c1].x_add >> 2;
					}
				}
				objects[c1].y += objects[c1].y_add;
				if ((objects[c1].x >> 16) < -5 || (objects[c1].x >> 16) > 405 || (objects[c1].y >> 16) > 260)
					objects[c1].used = 0;
				if ((objects[c1].y >> 16) > 0 && (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 0)) {
					if (objects[c1].y_add < 0) {
						if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] != 2) {
							objects[c1].y = (((objects[c1].y >> 16) + 16) & 0xfff0) << 16;
							objects[c1].x_add >>= 2;
							objects[c1].y_add = -objects[c1].y_add >> 2;
						}
					} else {
						if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 1) {
							if (objects[c1].y_add > 131072L) {
								objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
								objects[c1].x_add >>= 2;
								objects[c1].y_add = -objects[c1].y_add >> 2;
							} else {
								if (rnd(100) < 10) {
									s1 = rnd(4) - 2;
									add_leftovers(0, objects[c1].x >> 16, (objects[c1].y >> 16) + s1, objects[c1].frame, &object_gobs);
									add_leftovers(1, objects[c1].x >> 16, (objects[c1].y >> 16) + s1, objects[c1].frame, &object_gobs);
								}
								objects[c1].used = 0;
							}
						} else if (ban_map[objects[c1].y >> 20][objects[c1].x >> 20] == 3) {
							objects[c1].y = ((((objects[c1].y >> 16) - 16) & 0xfff0) + 15) << 16;
							if (objects[c1].y_add > 131072L)
								objects[c1].y_add = -objects[c1].y_add >> 2;
							else
								objects[c1].y_add = 0;
						}
					}
				}
				if (objects[c1].x_add < 0 && objects[c1].x_add > -16384)
					objects[c1].x_add = -16384;
				if (objects[c1].x_add > 0 && objects[c1].x_add < 16384)
					objects[c1].x_add = 16384;
				if (objects[c1].used == 1)
					add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].frame, &object_gobs);
				break;
			case OBJ_FLESH_TRACE:
				objects[c1].ticks--;
				if (objects[c1].ticks <= 0) {
					objects[c1].frame++;
					if (objects[c1].frame >= object_anims[objects[c1].anim].num_frames)
						objects[c1].used = 0;
					else {
						objects[c1].ticks = object_anims[objects[c1].anim].frame[objects[c1].frame].ticks;
						objects[c1].image = object_anims[objects[c1].anim].frame[objects[c1].frame].image;
					}
				}
				if (objects[c1].used == 1)
					add_pob(main_info.draw_page, objects[c1].x >> 16, objects[c1].y >> 16, objects[c1].image, &object_gobs);
				break;
			}
		}
	}
}


int add_pob(int page, int x, int y, int image, gob_t *pob_data)
{
	if (main_info.page_info[page].num_pobs >= NUM_POBS)
		return 1;

	main_info.page_info[page].pobs[main_info.page_info[page].num_pobs].x = x;
	main_info.page_info[page].pobs[main_info.page_info[page].num_pobs].y = y;
	main_info.page_info[page].pobs[main_info.page_info[page].num_pobs].image = image;
	main_info.page_info[page].pobs[main_info.page_info[page].num_pobs].pob_data = pob_data;
	main_info.page_info[page].num_pobs++;

	return 0;
}


void draw_flies(int page)
{
	int c2;

	for (c2 = 0; c2 < NUM_FLIES; c2++) {
		flies[c2].back[main_info.draw_page] = get_pixel(main_info.draw_page, flies[c2].x, flies[c2].y);
		flies[c2].back_defined[main_info.draw_page] = 1;
		if (mask_pic[(flies[c2].y * JNB_WIDTH) + flies[c2].x] == 0)
			set_pixel(main_info.draw_page, flies[c2].x, flies[c2].y, 0);
	}
}

void draw_pobs(int page)
{
	int c1;
	int back_buf_ofs;

	back_buf_ofs = 0;

	for (c1 = main_info.page_info[page].num_pobs - 1; c1 >= 0; c1--) {
		main_info.page_info[page].pobs[c1].back_buf_ofs = back_buf_ofs;
		get_block(page, main_info.page_info[page].pobs[c1].x - pob_hs_x(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data), main_info.page_info[page].pobs[c1].y - pob_hs_y(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data), pob_width(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data), pob_height(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data), (unsigned char *)main_info.pob_backbuf[page] + back_buf_ofs);
		back_buf_ofs += pob_width(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data) * pob_height(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data);
		put_pob(page, main_info.page_info[page].pobs[c1].x, main_info.page_info[page].pobs[c1].y, main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data, 1, mask_pic);
	}
}


void redraw_flies_background(int page)
{
	int c2;

	for (c2 = NUM_FLIES - 1; c2 >= 0; c2--) {
		if (flies[c2].back_defined[page] == 1)
			set_pixel(page, flies[c2].old_draw_x, flies[c2].old_draw_y, flies[c2].back[page]);
		flies[c2].old_draw_x = flies[c2].x;
		flies[c2].old_draw_y = flies[c2].y;
	}
}


void redraw_pob_backgrounds(int page)
{
	int c1;

	for (c1 = 0; c1 < main_info.page_info[page].num_pobs; c1++)
		put_block(page, main_info.page_info[page].pobs[c1].x - pob_hs_x(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data), main_info.page_info[page].pobs[c1].y - pob_hs_y(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data), pob_width(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data), pob_height(main_info.page_info[page].pobs[c1].image, main_info.page_info[page].pobs[c1].pob_data), (unsigned char *)main_info.pob_backbuf[page] + main_info.page_info[page].pobs[c1].back_buf_ofs);
}

int read_level(void)
{
	unsigned char *handle;
	int c1, c2;
	int chr;

	if ((handle = dat_open("levelmap.txt")) == 0) {
		strcpy(main_info.error_str, "Error loading 'levelmap.txt', aborting...\n");
		return 1;
	}

	for (c1 = 0; c1 < 16; c1++) {
		for (c2 = 0; c2 < 22; c2++) {
			while (1) {
				chr = (int) *(handle++);
				if (chr >= '0' && chr <= '4')
					break;
			}
			if (flip)
				ban_map[c1][21-c2] = chr - '0';
			else
				ban_map[c1][c2] = chr - '0';
		}
	}

	for (c2 = 0; c2 < 22; c2++)
		ban_map[16][c2] = BAN_SOLID;

	return 0;

}

unsigned char *dat_open(char *file_name)
{
	int num;
	int c1;
	char name[21];
	int ofs;
	unsigned char *ptr;

	if (datafile_buffer == NULL)
		return 0;

	memset(name, 0, sizeof(name));

	num = ( (datafile_buffer[0] <<  0) +
	        (datafile_buffer[1] <<  8) +
	        (datafile_buffer[2] << 16) +
	        (datafile_buffer[3] << 24) );

	ptr = datafile_buffer + 4;

	for (c1 = 0; c1 < num; c1++) {

		memcpy(name, ptr, 12);
		ptr += 12;

		if (strnicmp(name, file_name, strlen(file_name)) == 0) {
			ofs = ( (ptr[0] <<  0) +
				(ptr[1] <<  8) +
				(ptr[2] << 16) +
				(ptr[3] << 24) );

			return (datafile_buffer + ofs);
		}
		ptr += 8;
	}

	return 0;
}


int dat_filelen(char *file_name)
{
	unsigned char *ptr;
	int num;
	int c1;
	char name[21];
	int len;

	memset(name, 0, sizeof(name));

	num = ( (datafile_buffer[0] <<  0) +
	        (datafile_buffer[1] <<  8) +
	        (datafile_buffer[2] << 16) +
	        (datafile_buffer[3] << 24) );

	ptr = datafile_buffer + 4;

	for (c1 = 0; c1 < num; c1++) {

	        memcpy(name, ptr, 12);
		ptr += 12;

		if (strnicmp(name, file_name, strlen(file_name)) == 0) {

			ptr += 4;
			len = ( (ptr[0] <<  0) +
				(ptr[1] <<  8) +
				(ptr[2] << 16) +
				(ptr[3] << 24) );

			return len;
		}
		ptr += 8;
	}

	return 0;
}