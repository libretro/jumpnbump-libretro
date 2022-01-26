#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include <stdio.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#endif
#include "libretro.h"

#include "globals.h"

#define VIDEO_PIXELS JNB_WIDTH * JNB_HEIGHT

uint32_t *frame_buf;

static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static retro_environment_t environ_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

static int game_state = 0;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

void retro_init(void)
{
   frame_buf = calloc(VIDEO_PIXELS, sizeof(uint32_t));
}

void retro_deinit(void)
{
   free(frame_buf);
   frame_buf = NULL;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   if (port >= 4)
      return;

   log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "Jump 'n Bump";
   info->library_version  = "0.1";
   info->need_fullpath    = false;
   info->valid_extensions = "dat";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->geometry.base_width   = JNB_WIDTH;
   info->geometry.base_height  = JNB_HEIGHT;
   info->geometry.max_width    = JNB_WIDTH;
   info->geometry.max_height   = JNB_HEIGHT;
   info->geometry.aspect_ratio = (400.0f / 256.0f);
   info->timing.fps            = 60.0;
   info->timing.sample_rate    = 44100.0f;
}

void retro_set_environment(retro_environment_t cb)
{
   debug_print("cb=%p\n", cb);

   environ_cb = cb;

   if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
      log_cb = logging.log;
   else
      log_cb = fallback_log;
      
   static const struct retro_controller_description controllers[] = {
      { "Auto", RETRO_DEVICE_JOYPAD },
      { "Gamepad", RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0) },
      { NULL, 0 },
   };

   static const struct retro_controller_info ports[] = {
      { controllers, 2 },
      { controllers, 2 },
      { controllers, 2 },
      { controllers, 2 },
      { NULL, 0 },
   };

   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}

void retro_set_audio_sample(retro_audio_sample_t cb) { }

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_reset(void)
{
   debug_print("\n","");
}

static void update_input(void)
{
   input_poll_cb();

   addkey((KEY_PL1_LEFT & 0x7fff) | (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ? 0x0 : 0x8000));
   addkey((KEY_PL1_RIGHT & 0x7fff) | (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) ? 0x0 : 0x8000));
   addkey((KEY_PL1_JUMP & 0x7fff) | (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A) ? 0x0 : 0x8000));

   addkey((KEY_PL2_LEFT & 0x7fff) | (input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ? 0x0 : 0x8000));
   addkey((KEY_PL2_RIGHT & 0x7fff) | (input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) ? 0x0 : 0x8000));
   addkey((KEY_PL2_JUMP & 0x7fff) | (input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A) ? 0x0 : 0x8000));

   addkey((KEY_PL3_LEFT & 0x7fff) | (input_state_cb(2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ? 0x0 : 0x8000));
   addkey((KEY_PL3_RIGHT & 0x7fff) | (input_state_cb(2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) ? 0x0 : 0x8000));
   addkey((KEY_PL3_JUMP & 0x7fff) | (input_state_cb(2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A) ? 0x0 : 0x8000));

   addkey((KEY_PL4_LEFT & 0x7fff) | (input_state_cb(3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ? 0x0 : 0x8000));
   addkey((KEY_PL4_RIGHT & 0x7fff) | (input_state_cb(3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) ? 0x0 : 0x8000));
   addkey((KEY_PL4_JUMP & 0x7fff) | (input_state_cb(3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A) ? 0x0 : 0x8000));

	update_player_actions();
}

static void check_variables(void)
{

}

static void audio_callback(void)
{
   audio_batch_cb(0, 0);
}

static void render(void)
{
   if (game_state == 0)
      menu_frame();

   video_cb(frame_buf, JNB_WIDTH, JNB_HEIGHT, JNB_WIDTH << 2);
}

void retro_run(void)
{
   update_input();
   render();
   audio_callback();

   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();
}

bool retro_load_game(const struct retro_game_info *info)
{
   debug_print("\n","");

   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Jump" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },

      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Jump" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },

      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Jump" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },

      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Jump" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },

      { 0 },
   };

   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
      return false;
   }

   check_variables();

   if (init_program(info->data, info->size) != 0)
      return false;

   if (menu_init() != 0)
      return false;

   game_state = 0;

   return true;
}

void retro_unload_game(void)
{
   debug_print("\n","");
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_PAL;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void) { }

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}