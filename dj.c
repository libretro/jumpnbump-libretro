#include <limits.h>
#include "globals.h"
#include "micromod.h"

sfx_data sounds[NUM_SFX];

#define MAX_CHANNELS	32

typedef struct {
	/* loop flag */
	int loop;
	/* The channel step amount... */
	unsigned int step;
	/* ... and a 0.16 bit remainder of last step. */
	unsigned int stepremainder;
	unsigned int samplerate;
	/* The channel data pointers, start and end. */
	signed short* data;
	signed short* startdata;
	signed short* enddata;
	/* Hardware left and right channel volume lookup. */
	int leftvol;
	int rightvol;
} channel_info_t;

channel_info_t channelinfo[MAX_CHANNELS];

/* Sample rate in samples/second */
int audio_rate = 44100;
int global_sfx_volume = 0;
/*
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the given
//  mixing buffer, and clamping it to the allowed
//  range.
//
// This function currently supports only 16bit.
*/

static void stopchan(int i)
{
	if (channelinfo[i].data) {
		memset(&channelinfo[i], 0, sizeof(channel_info_t));
	}
}


/*
// This function adds a sound to the
//  list of currently active sounds,
//  which is maintained as a given number
//  (eight, usually) of internal channels.
// Returns a handle.
*/
int addsfx(signed short *data, int len, int loop, int samplerate, int channel)
{
	stopchan(channel);

	/* We will handle the new SFX. */
	/* Set pointer to raw data. */
	channelinfo[channel].data = data;
	channelinfo[channel].startdata = data;
      
	/* Set pointer to end of raw data. */
	channelinfo[channel].enddata = channelinfo[channel].data + len - 1;
	channelinfo[channel].samplerate = samplerate;

	channelinfo[channel].loop = loop;
	channelinfo[channel].stepremainder = 0;

	return channel;
}

static void updateSoundParams(int slot, int volume)
{
	int rightvol;
	int leftvol;

	/*
	// Set stepping
	// MWM 2000-12-24: Calculates proportion of channel samplerate
	// to global samplerate for mixing purposes.
	// Patched to shift left *then* divide, to minimize roundoff errors
	// as well as to use SAMPLERATE as defined above, not to assume 11025 Hz
	*/
	channelinfo[slot].step = ((channelinfo[slot].samplerate<<16)/audio_rate);

	leftvol = volume;
	rightvol= volume;  

	/* Sanity check, clamp volume. */
	if (rightvol < 0)
		rightvol = 0;
	if (rightvol > 127)
		rightvol = 127;
    
	if (leftvol < 0)
		leftvol = 0;
	if (leftvol > 127)
		leftvol = 127;
    
	channelinfo[slot].leftvol = leftvol;
	channelinfo[slot].rightvol = rightvol;
}


void mix_sound(signed short *stream, int len)
{
	/* Mix current sound data. */
	/* Data, from raw sound, for right and left. */
	register int sample;
	register int    dl;
	register int    dr;

	/* Pointers in audio stream, left, right, end. */
	signed short*   leftout;
	signed short*   rightout;
	signed short*   leftend;
	/* Step in stream, left and right, thus two. */
	int       step;

	/* Mixing channel index. */
	int       chan;

	/* Left and right channel */
	/*  are in audio stream, alternating. */
	leftout = (signed short *)stream;
	rightout = ((signed short *)stream)+1;
	step = 2;

	/* Determine end, for left channel only */
	/*  (right channel is implicit). */
	leftend = leftout + (len/4)*step;

	/* Mix sounds into the mixing buffer. */
	/* Loop over step*SAMPLECOUNT, */
	/*  that is 512 values for two channels. */
	while (leftout != leftend) {
		/* Reset left/right value. */
		dl = *leftout * 256;
		dr = *rightout * 256;

		/* Love thy L2 chache - made this a loop. */
		/* Now more channels could be set at compile time */
		/*  as well. Thus loop those  channels. */
		for ( chan = 0; chan < MAX_CHANNELS; chan++ ) {
			/* Check channel, if active. */
			if (channelinfo[chan].data) {
				/* Get the raw data from the channel. */
				/* no filtering */
				/* sample = *channelinfo[chan].data; */
				/* linear filtering */
				sample = (int)(((int)channelinfo[chan].data[0] * (int)(0x10000 - channelinfo[chan].stepremainder))
					+ ((int)channelinfo[chan].data[1] * (int)(channelinfo[chan].stepremainder))) >> 16;

				/* Add left and right part */
				/*  for this channel (sound) */
				/*  to the current data. */
				/* Adjust volume accordingly. */
				dl += sample * (channelinfo[chan].leftvol * global_sfx_volume) / 128;
				dr += sample * (channelinfo[chan].rightvol * global_sfx_volume) / 128;
				/* Increment index ??? */
				channelinfo[chan].stepremainder += channelinfo[chan].step;
				/* MSB is next sample??? */
				channelinfo[chan].data += channelinfo[chan].stepremainder >> 16;
				/* Limit to LSB??? */
				channelinfo[chan].stepremainder &= 0xffff;

				/* Check whether we are done. */
				if (channelinfo[chan].data >= channelinfo[chan].enddata) {
					if (channelinfo[chan].loop) {
						channelinfo[chan].data = channelinfo[chan].startdata;
					} else {
						stopchan(chan);
					}
				}
			}
		}
  
		/* Clamp to range. Left hardware channel. */
		/* Has been char instead of short. */
		/* if (dl > 127) *leftout = 127; */
		/* else if (dl < -128) *leftout = -128; */
		/* else *leftout = dl; */

		dl = dl / 256;
		dr = dr / 256;

		if (dl > SHRT_MAX)
			*leftout = SHRT_MAX;
		else if (dl < SHRT_MIN)
			*leftout = SHRT_MIN;
		else
			*leftout = (signed short)dl;

		/* Same for right hardware channel. */
		if (dr > SHRT_MAX)
			*rightout = SHRT_MAX;
		else if (dr < SHRT_MIN)
			*rightout = SHRT_MIN;
		else
			*rightout = (signed short)dr;

		/* Increment current pointers in stream */
		leftout += step;
		rightout += step;
	}
}

char dj_init(void)
{
	if (main_info.no_sound)
		return 0;

	memset(channelinfo, 0, sizeof(channelinfo));
	memset(sounds, 0, sizeof(sounds));

	return 0;
}

void dj_deinit(void)
{
	if (main_info.no_sound)
		return;
}

void dj_start(void)
{
}

void dj_stop(void)
{
}

char dj_autodetect_sd(void)
{
	return 0;
}

char dj_set_stereo(char flag)
{
	return 0;
}

void dj_set_auto_mix(char flag)
{
}

unsigned short dj_set_mixing_freq(unsigned short freq)
{
	return freq;
}

void dj_set_dma_time(unsigned short time)
{
}

void dj_set_nosound(char flag)
{
}

/* sfx handling */

char dj_set_num_sfx_channels(char num_channels)
{
	return num_channels;
}

void dj_set_sfx_volume(char volume)
{
	if (main_info.no_sound)
		return;

	global_sfx_volume = volume*2;
}

void dj_play_sfx(unsigned char sfx_num, unsigned short freq, char volume, char panning, unsigned short delay, int channel)
{
	int slot;

	if (main_info.music_no_sound || main_info.no_sound)
		return;

	if (channel<0) {
		for (slot=0; slot<MAX_CHANNELS; slot++)
			if (channelinfo[slot].data==NULL)
				break;
		if (slot>=MAX_CHANNELS)
			return;
	} else
		slot = channel;

	addsfx((short *)sounds[sfx_num].buf, sounds[sfx_num].length, sounds[sfx_num].loop, freq, slot);
	updateSoundParams(slot, volume*2);
}

char dj_get_sfx_settings(unsigned char sfx_num, sfx_data *data)
{
	if (main_info.no_sound)
		return 0;

	memcpy(data, &sounds[sfx_num], sizeof(sfx_data));
	return 0;
}

char dj_set_sfx_settings(unsigned char sfx_num, sfx_data *data)
{
	if (main_info.no_sound)
		return 0;

	memcpy(&sounds[sfx_num], data, sizeof(sfx_data));
	return 0;
}

void dj_set_sfx_channel_volume(char channel_num, char volume)
{
	if (main_info.no_sound)
		return;

    updateSoundParams(channel_num, volume*2);
}

void dj_stop_sfx_channel(char channel_num)
{
	if (main_info.no_sound)
		return;

    stopchan(channel_num);
}

char dj_load_sfx(unsigned char * file_handle, char *filename, int file_length, char sfx_type, unsigned char sfx_num)
{
	unsigned int i;
	unsigned char *src;
	unsigned short *dest;
	
	if (main_info.no_sound)
		return 0;

	sounds[sfx_num].buf = (unsigned char*) malloc(file_length);

	memcpy(sounds[sfx_num].buf, file_handle, file_length);

	sounds[sfx_num].length = file_length / 2;
	src = sounds[sfx_num].buf;
	dest = (unsigned short *)sounds[sfx_num].buf;
	for (i=0; i<sounds[sfx_num].length; i++)
	{
		unsigned short temp;
		temp = src[0] + (src[1] << 8);
		*dest = temp;
		src += 2;
		dest++;
	}
	return 0;
}

void dj_free_sfx(unsigned char sfx_num)
{
	if (main_info.no_sound)
		return;

	free(sounds[sfx_num].buf);
	memset(&sounds[sfx_num], 0, sizeof(sfx_data));
}

/* mod handling */

char dj_ready_mod(char mod_num)
{
	unsigned char *fp;

    if (main_info.no_sound)
		return 0;

	switch (mod_num) {
	case MOD_MENU:
		fp = dat_open("jump.mod");
		break;
	case MOD_GAME:
		fp = dat_open("bump.mod");
		break;
	case MOD_SCORES:
		fp = dat_open("scores.mod");
		break;
	default:
		fprintf(stderr, "bogus parameter to dj_ready_mod()\n");
		fp = NULL;
		break;
	}

	if (fp == NULL) {
		return 0;
	}
    
	return micromod_initialise((signed char*) fp, audio_rate);
}

char dj_start_mod(void)
{
    if (main_info.no_sound)
		return 0;
    
	return 0;
}

void dj_stop_mod(void)
{
	if (main_info.no_sound)
		return;
}

void dj_set_mod_volume(char volume)
{
	if (main_info.no_sound)
		return;

	micromod_set_gain(volume);
}

char dj_load_mod(unsigned char * file_handle, char *filename, char mod_num)
{
	return 0;
}

void dj_free_mod(char mod_num)
{
}