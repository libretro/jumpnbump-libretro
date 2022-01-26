/*
 * network.c
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

#include "network.h"

int is_server = 1;
int is_net = 0;
int server_said_bye = 0;

void processMovePacket(NetPacket *pkt)
{
	int playerid = pkt->arg;
	int movetype = ((pkt->arg2 >> 16) & 0xFF);
	int newval   = ((pkt->arg2 >>  0) & 0xFF);

	if (movetype == MOVEMENT_LEFT) {
		player[playerid].action_left = newval;
	} else if (movetype == MOVEMENT_RIGHT) {
		player[playerid].action_right = newval;
	} else if (movetype == MOVEMENT_UP) {
		player[playerid].action_up = newval;
	} else {
		printf("bogus MOVE packet!\n");
	}

	player[playerid].x = pkt->arg3;
	player[playerid].y = pkt->arg4;
}

void tellServerPlayerMoved(int playerid, int movement_type, int newval)
{
	NetPacket pkt;

	pkt.cmd = NETCMD_MOVE;
	pkt.arg = playerid;
	pkt.arg2 = ( ((movement_type & 0xFF) << 16) | ((newval & 0xFF) << 0) );
	pkt.arg3 = player[playerid].x;
	pkt.arg4 = player[playerid].y;

	if (is_server) {
		processMovePacket(&pkt);
	}
}

void serverSendKillPacket(int killer, int victim)
{
	NetPacket pkt;

	assert(is_server);
	pkt.cmd = NETCMD_KILL;
	pkt.arg = killer;
	pkt.arg2 = victim;
	pkt.arg3 = player[victim].x;
	pkt.arg4 = player[victim].y;
	processKillPacket(&pkt);
}

void processKillPacket(NetPacket *pkt)
{
	int c1 = pkt->arg;
	int c2 = pkt->arg2;
	int x = pkt->arg3;
	int y = pkt->arg4;
	int c4 = 0;
	int s1 = 0;

	player[c1].y_add = -player[c1].y_add;
	if (player[c1].y_add > -262144L)
		player[c1].y_add = -262144L;
	player[c1].jump_abort = 1;
	player[c2].dead_flag = 1;
	if (player[c2].anim != 6) {
		player[c2].anim = 6;
		player[c2].frame = 0;
		player[c2].frame_tick = 0;
		player[c2].image = player_anims[player[c2].anim].frame[player[c2].frame].image + player[c2].direction * 9;
		if (main_info.no_gore == 0) {
			for (c4 = 0; c4 < 6; c4++)
				add_object(OBJ_FUR, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3, (rnd(65535) - 32768) * 3, 0, 44 + c2 * 8);
			for (c4 = 0; c4 < 6; c4++)
				add_object(OBJ_FLESH, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3, (rnd(65535) - 32768) * 3, 0, 76);
			for (c4 = 0; c4 < 6; c4++)
				add_object(OBJ_FLESH, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3, (rnd(65535) - 32768) * 3, 0, 77);
			for (c4 = 0; c4 < 8; c4++)
				add_object(OBJ_FLESH, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3, (rnd(65535) - 32768) * 3, 0, 78);
			for (c4 = 0; c4 < 10; c4++)
				add_object(OBJ_FLESH, (x >> 16) + 6 + rnd(5), (y >> 16) + 6 + rnd(5), (rnd(65535) - 32768) * 3, (rnd(65535) - 32768) * 3, 0, 79);
		}
		dj_play_sfx(SFX_DEATH, (unsigned short)(SFX_DEATH_FREQ + rnd(2000) - 1000), 64, 0, 0, -1);
		player[c1].bumps++;
		if (player[c1].bumps >= JNB_END_SCORE) {
			endscore_reached = 1;
		}
		player[c1].bumped[c2]++;
		s1 = player[c1].bumps % 100;
		add_leftovers(0, 360, 34 + c1 * 64, s1 / 10, &number_gobs);
		add_leftovers(1, 360, 34 + c1 * 64, s1 / 10, &number_gobs);
		add_leftovers(0, 376, 34 + c1 * 64, s1 - (s1 / 10) * 10, &number_gobs);
		add_leftovers(1, 376, 34 + c1 * 64, s1 - (s1 / 10) * 10, &number_gobs);
	}
}