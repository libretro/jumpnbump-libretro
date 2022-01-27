/*
 * globals.h
 * Copyright (C) 1998 Brainchild Design - http://brainchilddesign.com/
 *
 * Copyright (C) 2001 Chuck Mason <cemason@users.sourceforge.net>
 *
 * Copyright (C) 2002 Florian Schulze <crow@icculus.org>
 *
 * Copyright (C) 2015 Côme Chilliet <come@chilliet.eu>
 *
 * This file is part of Jump 'n Bump.
 *
 * Jump 'n Bump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Jump 'n Bump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "dj.h"

extern uint32_t *frame_buf;

#define JNB_MAX_PLAYERS 4

#define JNB_END_SCORE 100

#define JNB_INETPORT 11111

extern int client_player_num;
void tellServerPlayerMoved(int playerid, int movement_type, int newval);
#define MOVEMENT_LEFT  1
#define MOVEMENT_RIGHT 2
#define MOVEMENT_UP    3

#define JNB_VERSION "1.60"

#define JNB_WIDTH 400
#define JNB_HEIGHT 256

extern int screen_width;
extern int screen_height;
extern int screen_pitch;

extern int ai[JNB_MAX_PLAYERS];

#define KEY_PL1_LEFT	0xa0
#define KEY_PL1_RIGHT	0xa1
#define KEY_PL1_JUMP	0xa2
#define KEY_PL2_LEFT	0xb0
#define KEY_PL2_RIGHT	0xb1
#define KEY_PL2_JUMP	0xb2
#define KEY_PL3_LEFT	0xc0
#define KEY_PL3_RIGHT	0xc1
#define KEY_PL3_JUMP	0xc2
#define KEY_PL4_LEFT	0xd0
#define KEY_PL4_RIGHT	0xd1
#define KEY_PL4_JUMP	0xd2

#define NUM_POBS 200
#define NUM_OBJECTS 200
#define NUM_FLIES 20
#define NUM_LEFTOVERS 50

#define OBJ_SPRING 0
#define OBJ_SPLASH 1
#define OBJ_SMOKE 2
#define OBJ_YEL_BUTFLY 3
#define OBJ_PINK_BUTFLY 4
#define OBJ_FUR 5
#define OBJ_FLESH 6
#define OBJ_FLESH_TRACE 7

#define OBJ_ANIM_SPRING 0
#define OBJ_ANIM_SPLASH 1
#define OBJ_ANIM_SMOKE 2
#define OBJ_ANIM_YEL_BUTFLY_RIGHT 3
#define OBJ_ANIM_YEL_BUTFLY_LEFT 4
#define OBJ_ANIM_PINK_BUTFLY_RIGHT 5
#define OBJ_ANIM_PINK_BUTFLY_LEFT 6
#define OBJ_ANIM_FLESH_TRACE 7

#define MOD_MENU 0
#define MOD_GAME 1
#define MOD_SCORES 2

#define SFX_JUMP 0
#define SFX_LAND 1
#define SFX_DEATH 2
#define SFX_SPRING 3
#define SFX_SPLASH 4
#define SFX_FLY 5

#define NUM_SFX 6

#define SFX_JUMP_FREQ 15000
#define SFX_LAND_FREQ 15000
#define SFX_DEATH_FREQ 20000
#define SFX_SPRING_FREQ 15000
#define SFX_SPLASH_FREQ 12000
#define SFX_FLY_FREQ 12000

#define BAN_VOID	0
#define BAN_SOLID	1
#define BAN_WATER	2
#define BAN_ICE		3
#define BAN_SPRING	4

typedef struct {
	int num_images;
	int *width;
	int *height;
	int *hs_x;
	int *hs_y;
	void **data;
	void **orig_data;
} gob_t;

typedef struct {
	int joy_enabled;
	int no_sound, music_no_sound, no_gore;
	char error_str[256];
	int draw_page, view_page;
	struct {
		int num_pobs;
		struct {
			int x, y;
			int image;
			gob_t *pob_data;
			int back_buf_ofs;
		} pobs[NUM_POBS];
	} page_info[2];
	void *pob_backbuf[2];
} main_info_t;

typedef struct {
	int action_left,action_up,action_right;
	int enabled, dead_flag;
	int bumps;
	int bumped[JNB_MAX_PLAYERS];
	int x, y;
	int x_add, y_add;
	int direction, jump_ready, jump_abort, in_water;
	int anim, frame, frame_tick, image;
} player_t;

typedef struct {
	int num_frames;
	int restart_frame;
	struct {
		int image;
		int ticks;
	} frame[4];
} player_anim_t;

typedef struct {
	int used, type;
	int x, y;
	int x_add, y_add;
	int x_acc, y_acc;
	int anim;
	int frame, ticks;
	int image;
} object_t;

typedef struct {
	int x, y;
	int raw_x, raw_y;
	int but1, but2;
	struct {
		int x1, x2, x3;
		int y1, y2, y3;
	} calib_data;
} joy_t;

typedef struct {
	int but1, but2, but3;
} mouse_t;

extern main_info_t main_info;
extern player_t player[JNB_MAX_PLAYERS];
extern player_anim_t player_anims[7];
extern object_t objects[NUM_OBJECTS];
extern joy_t joy;
extern mouse_t mouse;

extern unsigned char *background_pic;
extern unsigned char *mask_pic;

extern gob_t rabbit_gobs;
extern gob_t font_gobs;
extern gob_t object_gobs;
extern gob_t number_gobs;

extern int endscore_reached;

/* main.c */

void steer_players(void);
void position_player(int player_num);
void add_object(int type, int x, int y, int x_add, int y_add, int anim, int frame);
void update_objects(void);
int add_pob(int page, int x, int y, int image, gob_t *pob_data);
void draw_flies(int page);
void draw_pobs(int page);
void redraw_flies_background(int page);
void redraw_pob_backgrounds(int page);
int add_leftovers(int page, int x, int y, int image, gob_t *pob_data);
void draw_leftovers(int page);
int init_level(int level, char *pal);
void deinit_level(void);
int init_program(const void *data, size_t size);
void deinit_program(void);
unsigned short rnd(unsigned short max);
int read_level(void);
unsigned char *dat_open(char *file_name);
int dat_filelen(char *file_name);


/* input.c */

void update_player_actions(void);

/* menu.c */

int menu_frame(void);
int menu_init(void);
void menu_deinit(void);


/* gfx.c */

void open_screen(void);
void draw_begin(void);
void draw_end(void);
void flippage(int page);
void draw_begin(void);
void draw_end(void);
void clear_lines(int page, int y, int count, int color);
int get_color(int color, char pal[768]);
int get_pixel(int page, int x, int y);
void set_pixel(int page, int x, int y, int color);
void setpalette(int index, int count, char *palette);
void fillpalette(int red, int green, int blue);
void get_block(int page, int x, int y, int width, int height, void *buffer);
void put_block(int page, int x, int y, int width, int height, void *buffer);
void put_text(int page, int x, int y, char *text, int align);
void put_pob(int page, int x, int y, int image, gob_t *gob, int mask, void *mask_pic);
int pob_width(int image, gob_t *gob);
int pob_height(int image, gob_t *gob);
int pob_hs_x(int image, gob_t *gob);
int pob_hs_y(int image, gob_t *gob);
int read_pcx(unsigned char * handle, void *buffer, int buf_len, char *pal);
void register_background(unsigned char *pixels, char pal[768]);
int register_gob(unsigned char *handle, gob_t *gob, int len);
void register_mask(void *pixels);

/* interrpt.c */

extern char last_keys[50];

int hook_keyb_handler(void);
void remove_keyb_handler(void);
int key_pressed(int key);
int addkey(unsigned int key);


#ifdef __cplusplus
}
#endif

#endif
