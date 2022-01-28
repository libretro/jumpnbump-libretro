/*
 * gfx.c
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

#include "globals.h"

int dirty_block_shift=4;

static uint8_t screen_buffer[2][JNB_WIDTH*JNB_HEIGHT];
static uint8_t jnb_surface[JNB_WIDTH*JNB_HEIGHT];
static int drawing_enable = 0;
static void *background = NULL;
static int background_drawn;
static void *mask = NULL;
static int dirty_blocks[2][25*16*2];
static uint16_t current_palette[256];

extern uint16_t *frame_buf;

unsigned char *get_vgaptr(int page, int x, int y)
{
	assert(drawing_enable==1);

	return screen_buffer[page] + (y*JNB_WIDTH)+(x);
}

void open_screen(void)
{
	memset(dirty_blocks, 0, sizeof(dirty_blocks));
	memset(current_palette, 0, sizeof(current_palette));
}

void clear_page(int page, int color)
{
	int i,j;
	unsigned char *buf = get_vgaptr(page, 0, 0);

	assert(drawing_enable==1);

	for (i=0; i<(25*16); i++)
		dirty_blocks[page][i] = 1;

	for (i=0; i<JNB_HEIGHT; i++)
		for (j=0; j<JNB_WIDTH; j++)
			*buf++ = color;
}


void clear_lines(int page, int y, int count, int color)
{
	int i,j;

	assert(drawing_enable==1);

	for (i=0; i<count; i++) {
		if ((i+y)<JNB_HEIGHT) {
			unsigned char *buf = get_vgaptr(page, 0, i+y);
			for (j=0; j<JNB_WIDTH; j++)
				*buf++ = color;
		}
	}
	count = ((y+count)>>dirty_block_shift) - (y>>dirty_block_shift) + 1;
	y >>= dirty_block_shift;
	for (i=0; i<count; i++)
		for (j=0; j<25; j++)
			dirty_blocks[page][(y+i)*25+j] = 1;
}


int get_pixel(int page, int x, int y)
{
	assert(drawing_enable==1);

	assert(x<JNB_WIDTH);
	assert(y<JNB_HEIGHT);

	return *get_vgaptr(page, x, y);
}


void set_pixel(int page, int x, int y, int color)
{
	assert(drawing_enable==1);

	assert(x<JNB_WIDTH);
	assert(y<JNB_HEIGHT);

	dirty_blocks[page][(y>>dirty_block_shift)*25+(x>>dirty_block_shift)] = 1;

	*get_vgaptr(page, x, y) = color;
}


void flippage(int page)
{
	int x,y;
	unsigned char *src;

	assert(drawing_enable==0);

	src = screen_buffer[page];

	for (y=0; y<JNB_HEIGHT; y++) {
		for (x=0; x<25; x++) {
			int count;
			int test_x;

			count=0;
			test_x=x;
			while ( (test_x<25) && (dirty_blocks[page][(y>>dirty_block_shift)*25+test_x]) ) {
				count++;
				test_x++;
			}
			if (count) {
				memcpy(
					&jnb_surface[y*JNB_WIDTH+(x<<dirty_block_shift)],
					&src [y*JNB_WIDTH+(x<<dirty_block_shift)],
					((16<<dirty_block_shift)>>4)*count);
			}
			x = test_x;
		}
	}
	memset(&dirty_blocks[page], 0, sizeof(int)*25*16);

	// convert jnb_surface to RETRO_PIXEL_FORMAT_RGB565
	for(y=0; y<JNB_HEIGHT; y++) {
		for(x=0; x<JNB_WIDTH; x++) {
			int p = jnb_surface[y*JNB_WIDTH+x];
			frame_buf[y*JNB_WIDTH+x] = current_palette[p];
		}
	}
}


void draw_begin(void)
{
	assert(drawing_enable==0);

	drawing_enable = 1;
	if (background_drawn == 0) {
		if (background) {
			put_block(0, 0, 0, JNB_WIDTH, JNB_HEIGHT, background);
			put_block(1, 0, 0, JNB_WIDTH, JNB_HEIGHT, background);
		} else {
			clear_page(0, 0);
			clear_page(1, 0);
		}
		background_drawn = 1;
	}
}


void draw_end(void)
{
	assert(drawing_enable==1);

	drawing_enable = 0;
}


void setpalette(int index, int count, char *palette)
{
	assert(drawing_enable==0);

	uint8_t red, green, blue;

	for (int i = 0; i < count; i++) {
		red = (palette[i * 3 + 0] >> 1) & 0x1f;
		green = palette[i * 3 + 1] & 0x3f;
		blue = (palette[i * 3 + 2] >> 1) & 0x1f;
		current_palette[index + i] = (red << 11 | green << 5 | blue);
	}
}


void fillpalette(int red, int green, int blue)
{
	assert(drawing_enable==0);

	for (int i = 0; i < 256; i++)
		current_palette[i] = ((red >> 1) & 0x1f) << 11 | (green & 0x3f) << 5 | ((blue >> 1) & 0x1f);
}


void get_block(int page, int x, int y, int width, int height, void *buffer)
{
	unsigned char *buffer_ptr, *vga_ptr;
	int h;

	assert(drawing_enable==1);

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (y + height >= JNB_HEIGHT)
		height = JNB_HEIGHT - y;
	if (x + width >= JNB_WIDTH)
		width = JNB_WIDTH - x;
	if (width<=0)
		return;
	if(height<=0)
		return;

	vga_ptr = get_vgaptr(page, x, y);
	buffer_ptr = (unsigned char *)buffer;
	for (h = 0; h < height; h++) {
		memcpy(buffer_ptr, vga_ptr, width);
		vga_ptr += JNB_WIDTH;
		buffer_ptr += width;
	}
}


void put_block(int page, int x, int y, int width, int height, void *buffer)
{
	int h;
	unsigned char *vga_ptr, *buffer_ptr;

	assert(drawing_enable==1);

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (y + height >= JNB_HEIGHT)
		height = JNB_HEIGHT - y;
	if (x + width >= JNB_WIDTH)
		width = JNB_WIDTH - x;
	if (width<=0)
		return;
	if(height<=0)
		return;

	vga_ptr = get_vgaptr(page, x, y);
	buffer_ptr = (unsigned char *)buffer;
	for (h = 0; h < height; h++) {
		memcpy(vga_ptr, buffer_ptr, width);
		vga_ptr += JNB_WIDTH;
		buffer_ptr += width;
	}
	width = ((x+width)>>dirty_block_shift) - (x>>dirty_block_shift) + 1;
	height = ((y+height)>>dirty_block_shift) - (y>>dirty_block_shift) + 1;
	x >>= dirty_block_shift;
	y >>= dirty_block_shift;
	while (width--)
		for (h=0; h<height; h++)
			dirty_blocks[page][(y+h)*25+(x+width)] = 1;
}


void put_text(int page, int x, int y, char *text, int align)
{
	int c1;
	int t1;
	int width;
	int cur_x;
	int image;

	assert(drawing_enable==1);

	if (text == NULL || strlen(text) == 0)
		return;
	if (font_gobs.num_images == 0)
		return;

	width = 0;
	c1 = 0;
	while (text[c1] != 0) {
		t1 = text[c1];
		c1++;
		if (t1 == ' ') {
			width += 5;
			continue;
		}
		if (t1 >= 33 && t1 <= 34)
			image = t1 - 33;

		else if (t1 >= 39 && t1 <= 41)
			image = t1 - 37;

		else if (t1 >= 44 && t1 <= 59)
			image = t1 - 39;

		else if (t1 >= 64 && t1 <= 90)
			image = t1 - 43;

		else if (t1 >= 97 && t1 <= 122)
			image = t1 - 49;

		else if (t1 == '~')
			image = 74;

		else if (t1 == 0x84)
			image = 75;

		else if (t1 == 0x86)
			image = 76;

		else if (t1 == 0x8e)
			image = 77;

		else if (t1 == 0x8f)
			image = 78;

		else if (t1 == 0x94)
			image = 79;

		else if (t1 == 0x99)
			image = 80;

		else
			continue;
		width += pob_width(image, &font_gobs) + 1;
	}

	switch (align) {
	case 0:
		cur_x = x;
		break;
	case 1:
		cur_x = x - width;
		break;
	case 2:
		cur_x = x - width / 2;
		break;
	default:
		cur_x = 0;	/* this should cause error? -Chuck */
		break;
	}
	c1 = 0;

	while (text[c1] != 0) {
		t1 = text[c1];
		c1++;
		if (t1 == ' ') {
			cur_x += 5;
			continue;
		}
		if (t1 >= 33 && t1 <= 34)
			image = t1 - 33;

		else if (t1 >= 39 && t1 <= 41)
			image = t1 - 37;

		else if (t1 >= 44 && t1 <= 59)
			image = t1 - 39;

		else if (t1 >= 64 && t1 <= 90)
			image = t1 - 43;

		else if (t1 >= 97 && t1 <= 122)
			image = t1 - 49;

		else if (t1 == '~')
			image = 74;

		else if (t1 == 0x84)
			image = 75;

		else if (t1 == 0x86)
			image = 76;

		else if (t1 == 0x8e)
			image = 77;

		else if (t1 == 0x8f)
			image = 78;

		else if (t1 == 0x94)
			image = 79;

		else if (t1 == 0x99)
			image = 80;

		else
			continue;
		put_pob(page, cur_x, y, image, &font_gobs, 1, mask_pic);
		cur_x += pob_width(image, &font_gobs) + 1;
	}
}


void put_pob(int page, int x, int y, int image, gob_t *gob, int use_mask, void *mask_pic)
{
	int c1, c2;
	int pob_x, pob_y;
	int width, height;
	int draw_width, draw_height;
	int colour;
	unsigned char *vga_ptr;
	unsigned char *pob_ptr;
	unsigned char *mask_ptr;

	assert(drawing_enable==1);
	assert(gob);
	assert(image>=0);
	assert(image<gob->num_images);

	width = draw_width = gob->width[image];
	height = draw_height = gob->height[image];
	x -= gob->hs_x[image];
	y -= gob->hs_y[image];

	if ((x + width) <= 0 || x >= JNB_WIDTH)
		return;
	if ((y + height) <= 0 || y >= JNB_HEIGHT)
		return;

	pob_x = 0;
	pob_y = 0;
	if (x < 0) {
		pob_x -= x;
		draw_width += x;
		x = 0;
	}
	if ((x + width) > JNB_WIDTH)
		draw_width -= x + width - JNB_WIDTH;
	if (y < 0) {
		pob_y -= y;
		draw_height += y;
		y = 0;
	}
	if ((y + height) > JNB_HEIGHT)
		draw_height -= y + height - JNB_HEIGHT;

	vga_ptr = get_vgaptr(page, x, y);
	pob_ptr = ((unsigned char *)gob->data[image]) + ((pob_y * width) + pob_x);
	mask_ptr = ((unsigned char *)mask) + ((y * JNB_WIDTH) + (x));
	for (c1 = 0; c1 < draw_height; c1++) {
		for (c2 = 0; c2 < draw_width; c2++) {
			colour = *mask_ptr;
			if (use_mask == 0 || (use_mask == 1 && colour == 0)) {
				colour = *pob_ptr;
				if (colour != 0) {
					*vga_ptr = colour;
				}
			}
			vga_ptr++;
			pob_ptr++;
			mask_ptr++;
		}
		pob_ptr += width - c2;
		vga_ptr += (JNB_WIDTH - c2);
		mask_ptr += (JNB_WIDTH - c2);
	}
	draw_width = ((x+draw_width)>>dirty_block_shift) - (x>>dirty_block_shift) + 1;
	draw_height = ((y+draw_height)>>dirty_block_shift) - (y>>dirty_block_shift) + 1;
	x >>= dirty_block_shift;
	y >>= dirty_block_shift;
	while (draw_width--)
		for (c1=0; c1<draw_height; c1++)
			dirty_blocks[page][(y+c1)*25+(x+draw_width)] = 1;
}


int pob_width(int image, gob_t *gob)
{
	assert(gob);
	assert(image>=0);
	assert(image<gob->num_images);
	return gob->width[image];
}


int pob_height(int image, gob_t *gob)
{
	assert(gob);
	assert(image>=0);
	assert(image<gob->num_images);
	return gob->height[image];
}


int pob_hs_x(int image, gob_t *gob)
{
	assert(gob);
	assert(image>=0);
	assert(image<gob->num_images);
	return gob->hs_x[image];
}


int pob_hs_y(int image, gob_t *gob)
{
	assert(gob);
	assert(image>=0);
	assert(image<gob->num_images);
	return gob->hs_y[image];
}


int read_pcx(unsigned char * handle, void *buf, int buf_len, char *pal)
{
	unsigned char *buffer=(unsigned char *)buf;
	short c1;
	short a, b;
	long ofs1;
	if (buffer != 0) {
		handle += 128;
		ofs1 = 0;
		while (ofs1 < buf_len) {
			a = *(handle++);
			if ((a & 0xc0) == 0xc0) {
				b = *(handle++);
				a &= 0x3f;
				for (c1 = 0; c1 < a && ofs1 < buf_len; c1++)
					buffer[ofs1++] = (char) b;
			} else
				buffer[ofs1++] = (char) a;
		}
		if (pal != 0) {
			handle++;
			for (c1 = 0; c1 < 768; c1++)
				pal[c1] = *(handle++) /*fgetc(handle)*/ >> 2;
		}
	}
	return 0;
}


void register_background(unsigned char *pixels, char pal[768])
{
	if (background) {
		free(background);
		background = NULL;
	}
	background_drawn = 0;
	if (!pixels)
		return;
	assert(pal);

	background = malloc(JNB_WIDTH*JNB_HEIGHT);
	assert(background);
	memcpy(background, pixels, JNB_WIDTH*JNB_HEIGHT);
}

int register_gob(unsigned char *handle, gob_t *gob, int len)
{
	unsigned char *gob_data;
	int i;

	gob_data = (unsigned char *)malloc(len);
	memcpy(gob_data, handle, len);

	gob->num_images = (short)((gob_data[0]) + (gob_data[1] << 8));

	gob->width = (int*)malloc(gob->num_images*sizeof(int));
	gob->height = (int*)malloc(gob->num_images*sizeof(int));
	gob->hs_x = (int*)malloc(gob->num_images*sizeof(int));
	gob->hs_y = (int*)malloc(gob->num_images*sizeof(int));
	gob->data = (void**)malloc(gob->num_images*sizeof(void *));
	gob->orig_data = (void**)malloc(gob->num_images*sizeof(void *));
	for (i=0; i<gob->num_images; i++) {
		int image_size;
		int offset;

		offset = (gob_data[i*4+2]) + (gob_data[i*4+3] << 8) + (gob_data[i*4+4] << 16) + (gob_data[i*4+5] << 24);

		gob->width[i]  = (short)((gob_data[offset]) + (gob_data[offset+1] << 8)); offset += 2;
		gob->height[i] = (short)((gob_data[offset]) + (gob_data[offset+1] << 8)); offset += 2;
		gob->hs_x[i]   = (short)((gob_data[offset]) + (gob_data[offset+1] << 8)); offset += 2;
		gob->hs_y[i]   = (short)((gob_data[offset]) + (gob_data[offset+1] << 8)); offset += 2;

		image_size = gob->width[i] * gob->height[i];
		gob->orig_data[i] = malloc(image_size);
		memcpy(gob->orig_data[i], &gob_data[offset], image_size);
		gob->data[i] = (unsigned short *)gob->orig_data[i];
	}
	free(gob_data);
	return 0;
}

void register_mask(void *pixels)
{
	if (mask) {
		free(mask);
		mask = NULL;
	}
	assert(pixels);
	mask = malloc(JNB_WIDTH*JNB_HEIGHT);
	assert(mask);
	memcpy(mask, pixels, JNB_WIDTH*JNB_HEIGHT);
}
