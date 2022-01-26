/*
 * interrpt.c
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "globals.h"

char keyb[256];
char last_keys[50];

int addkey(unsigned int key)
{
	int c1;

	if (!(key & 0x8000)) {
		keyb[key & 0x7fff] = 1;
		for (c1 = 48; c1 > 0; c1--)
			last_keys[c1] = last_keys[c1 - 1];
		last_keys[0] = key & 0x7fff;
	} else
		keyb[key & 0x7fff] = 0;
	return 0;
}

void remove_keyb_handler(void)
{
}

int hook_keyb_handler(void)
{
	memset((void *) last_keys, 0, sizeof(last_keys));

	return 0;
}

int key_pressed(int key)
{
	return keyb[(unsigned char) key];
}

int intr_sysupdate()
{
	return 1;
}
