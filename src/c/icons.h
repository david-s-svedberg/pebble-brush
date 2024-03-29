#pragma once

#include <pebble.h>

GBitmap* get_check_icon();
GBitmap* get_config_icon();
GBitmap* get_edit_icon();
GBitmap* get_up_icon();
GBitmap* get_down_icon();
GBitmap* get_play_icon();
GBitmap* get_brush_icon();
GBitmap* get_refresh_icon();
GBitmap* get_pause_icon();
GBitmap* get_swap_icon();

void destroy_all_icons();