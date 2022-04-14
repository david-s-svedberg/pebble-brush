#include "main_window.h"

#include <pebble.h>

#include "main_window_logic.h"
#include "icons.h"
#include "persistance.h"

static Window *main_window;

static StatusBarLayer* status_bar;

static TextLayer* nw_quad_layer;
static TextLayer* ne_quad_layer;
static TextLayer* sw_quad_layer;
static TextLayer* se_quad_layer;

static ActionBarLayer* main_window_action_bar_layer;

static GColor8 m_background_color;
static GColor8 m_foreground_color;

static void main_window_click_config_provider(void* context)
{
    window_single_click_subscribe(BUTTON_ID_UP, toggle_time);
    window_single_click_subscribe(BUTTON_ID_SELECT, toggle_running);
    window_single_click_subscribe(BUTTON_ID_DOWN, goto_config_window);
}

static void setup_main_window_action_bar_layer(Layer *window_layer, GRect bounds)
{
    main_window_action_bar_layer = action_bar_layer_create();
    action_bar_layer_set_background_color(main_window_action_bar_layer, m_foreground_color);
    action_bar_layer_add_to_window(main_window_action_bar_layer, main_window);
    action_bar_layer_set_click_config_provider(main_window_action_bar_layer, main_window_click_config_provider);

    action_bar_layer_set_icon_animated(main_window_action_bar_layer, BUTTON_ID_UP, get_swap_icon(), true);
    action_bar_layer_set_icon_animated(main_window_action_bar_layer, BUTTON_ID_SELECT, get_play_icon(), true);
    action_bar_layer_set_icon_animated(main_window_action_bar_layer, BUTTON_ID_DOWN, get_config_icon(), true);
}

static void setup_quad_text_layers(Layer *window_layer, GRect bounds)
{
    int16_t top = STATUS_BAR_LAYER_HEIGHT;
    int16_t bottom = bounds.size.h;
    int16_t left = 0;
    int16_t right = bounds.size.w - ACTION_BAR_WIDTH;

    int16_t quad_width = ((right - left) / 2);
    int16_t quad_height = ((bottom - top) / 2);

    int16_t middle_x = quad_width + left;
    int16_t middle_y = quad_height + top;

    nw_quad_layer = text_layer_create(GRect(left, top, quad_width, quad_height));
    ne_quad_layer = text_layer_create(GRect(middle_x, top, quad_width, quad_height));
    sw_quad_layer = text_layer_create(GRect(left, middle_y, quad_width, quad_height));
    se_quad_layer = text_layer_create(GRect(middle_x, middle_y, quad_width, quad_height));

    text_layer_set_text_color(nw_quad_layer, m_foreground_color);
    text_layer_set_text_color(ne_quad_layer, m_foreground_color);
    text_layer_set_text_color(sw_quad_layer, m_foreground_color);
    text_layer_set_text_color(se_quad_layer, m_foreground_color);

    GFont font = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);

    text_layer_set_font(nw_quad_layer, font);
    text_layer_set_font(ne_quad_layer, font);
    text_layer_set_font(sw_quad_layer, font);
    text_layer_set_font(se_quad_layer, font);

    text_layer_set_text_alignment(nw_quad_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(ne_quad_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(sw_quad_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(se_quad_layer, GTextAlignmentCenter);

    layer_add_child(window_layer, text_layer_get_layer(nw_quad_layer));
    layer_add_child(window_layer, text_layer_get_layer(ne_quad_layer));
    layer_add_child(window_layer, text_layer_get_layer(sw_quad_layer));
    layer_add_child(window_layer, text_layer_get_layer(se_quad_layer));
}

static void setup_status_bar(Layer *window_layer, GRect bounds)
{
    status_bar = status_bar_layer_create();

    status_bar_layer_set_colors(status_bar, m_background_color, m_foreground_color);
    status_bar_layer_set_separator_mode(status_bar, StatusBarLayerSeparatorModeDotted);

    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
}

static void load_main_window(Window *window)
{
    window_set_background_color(window, m_background_color);
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    setup_main_window_action_bar_layer(window_layer, bounds);
    setup_status_bar(window_layer, bounds);
    setup_quad_text_layers(window_layer, bounds);

    setup_layers(nw_quad_layer, ne_quad_layer, sw_quad_layer, se_quad_layer, main_window_action_bar_layer, main_window);

    reset_brushing();

    if(use_auto_start())
    {
        start_brushing();
    }
}

static void unload_main_window(Window *window)
{
    action_bar_layer_remove_from_window(main_window_action_bar_layer);
    action_bar_layer_destroy(main_window_action_bar_layer);
    status_bar_layer_destroy(status_bar);
    text_layer_destroy(nw_quad_layer);
    text_layer_destroy(ne_quad_layer);
    text_layer_destroy(sw_quad_layer);
    text_layer_destroy(se_quad_layer);
}

void setup_main_window(GColor8 background_color, GColor8 foreground_color)
{
    m_background_color = background_color;
    m_foreground_color = foreground_color;

    main_window = window_create();

    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = load_main_window,
        .unload = unload_main_window,
        .appear = update_main_window
    });

    window_stack_push(main_window, true);
}

void tear_down_main_window()
{
    window_destroy(main_window);
}