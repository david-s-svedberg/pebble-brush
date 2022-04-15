#pragma once

#include <pebble.h>

void goto_config_window(ClickRecognizerRef recognizer, void* context);
void toggle_running(ClickRecognizerRef recognizer, void* context);
void toggle_time(ClickRecognizerRef recognizer, void* context);
void setup_layers(
    TextLayer* nw_quad_layer,
    TextLayer* ne_quad_layer,
    TextLayer* sw_quad_layer,
    TextLayer* se_quad_layer,
    Layer* nw_quad_background_layer,
    Layer* ne_quad_background_layer,
    Layer* sw_quad_background_layer,
    Layer* se_quad_background_layer,
    ActionBarLayer* action_bar,
    StatusBarLayer* status_bar,
    Window* main_window);
void update_main_window(Window *window);
void start_brushing();
void reset_brushing();

void update_background_layer(struct Layer *layer, GContext *ctx);