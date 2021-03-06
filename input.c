/*
 * input.c
 * Copyright (C) 1998 Brainchild Design - http://brainchilddesign.com/
 * 
 * Copyright (C) 2001 Chuck Mason <cemason@users.sourceforge.net>
 *
 * Copyright (C) 2002 Florian Schulze <crow@icculus.org>
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

#include "globals.h"


void update_player_actions(void)
{
	int tmp;

	if (client_player_num < 0) {
		tmp = key_pressed(KEY_PL1_LEFT) == 1;
		if (tmp != player[0].action_left)
			tellServerPlayerMoved(0, MOVEMENT_LEFT, tmp);
		tmp = key_pressed(KEY_PL1_RIGHT) == 1;
		if (tmp != player[0].action_right)
			tellServerPlayerMoved(0, MOVEMENT_RIGHT, tmp);
		tmp = key_pressed(KEY_PL1_JUMP) == 1;
		if (tmp != player[0].action_up)
			tellServerPlayerMoved(0, MOVEMENT_UP, tmp);

		tmp = key_pressed(KEY_PL2_LEFT);
		if (tmp != player[1].action_left)
			tellServerPlayerMoved(1, MOVEMENT_LEFT, tmp);
		tmp = key_pressed(KEY_PL2_RIGHT) == 1;
		if (tmp != player[1].action_right)
			tellServerPlayerMoved(1, MOVEMENT_RIGHT, tmp);
		tmp = key_pressed(KEY_PL2_JUMP) == 1;
		if (tmp != player[1].action_up)
			tellServerPlayerMoved(1, MOVEMENT_UP, tmp);

		tmp = key_pressed(KEY_PL3_LEFT) == 1;
		if (tmp != player[2].action_left)
			tellServerPlayerMoved(2, MOVEMENT_LEFT, tmp);
		tmp = key_pressed(KEY_PL3_RIGHT) == 1;
		if (tmp != player[2].action_right)
			tellServerPlayerMoved(2, MOVEMENT_RIGHT, tmp);
		tmp = key_pressed(KEY_PL3_JUMP) == 1;
		if (tmp != player[2].action_up)
			tellServerPlayerMoved(2, MOVEMENT_UP, tmp);

		tmp = key_pressed(KEY_PL4_LEFT) == 1;
		if (tmp != player[3].action_left)
	    	tellServerPlayerMoved(3, MOVEMENT_LEFT, tmp);
		tmp = key_pressed(KEY_PL4_RIGHT) == 1;
		if (tmp != player[3].action_right)
		    tellServerPlayerMoved(3, MOVEMENT_RIGHT, tmp);
		tmp = key_pressed(KEY_PL4_JUMP) == 1;
		if (tmp != player[3].action_up)
		    tellServerPlayerMoved(3, MOVEMENT_UP, tmp);
	} else {
		tmp = key_pressed(KEY_PL1_LEFT) == 1;
		if (tmp != player[client_player_num].action_left)
			tellServerPlayerMoved(client_player_num, MOVEMENT_LEFT, tmp);
		tmp = key_pressed(KEY_PL1_RIGHT) == 1;
		if (tmp != player[client_player_num].action_right)
			tellServerPlayerMoved(client_player_num, MOVEMENT_RIGHT, tmp);
		tmp = key_pressed(KEY_PL1_JUMP) == 1;
		if (tmp != player[client_player_num].action_up)
			tellServerPlayerMoved(client_player_num, MOVEMENT_UP, tmp);
	}
}