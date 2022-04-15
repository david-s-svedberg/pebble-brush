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

static Layer* nw_quad_background_layer;
static Layer* ne_quad_background_layer;
static Layer* sw_quad_background_layer;
static Layer* se_quad_background_layer;

static ActionBarLayer* main_window_action_bar_layer;

static int16_t m_quad_space_top;
static int16_t m_quad_space_bottom;
static int16_t m_quad_space_left;
static int16_t m_quad_space_right;
static int16_t m_quad_space_width;
static int16_t m_quad_space_height;
static int16_t m_quad_space_middle_x;
static int16_t m_quad_space_middle_y;
static uint16_t m_quad_space_text_padding;
static uint16_t m_quad_space_text_height;

static void main_window_click_config_provider(void* context)
{
    window_single_click_subscribe(BUTTON_ID_UP, toggle_time);
    window_single_click_subscribe(BUTTON_ID_SELECT, toggle_running);
    window_single_click_subscribe(BUTTON_ID_DOWN, goto_config_window);
}

static void setup_main_window_action_bar_layer(Layer *window_layer, GRect bounds)
{
    main_window_action_bar_layer = action_bar_layer_create();
    action_bar_layer_set_background_color(main_window_action_bar_layer, get_foreground_color());
    action_bar_layer_add_to_window(main_window_action_bar_layer, main_window);
    action_bar_layer_set_click_config_provider(main_window_action_bar_layer, main_window_click_config_provider);

    action_bar_layer_set_icon_animated(main_window_action_bar_layer, BUTTON_ID_UP, get_swap_icon(), true);
    action_bar_layer_set_icon_animated(main_window_action_bar_layer, BUTTON_ID_SELECT, get_play_icon(), true);
    action_bar_layer_set_icon_animated(main_window_action_bar_layer, BUTTON_ID_DOWN, get_config_icon(), true);
}

static void calculate_quad_space(GRect bounds)
{
    m_quad_space_top = STATUS_BAR_LAYER_HEIGHT;
    m_quad_space_bottom = bounds.size.h;
    m_quad_space_left = bounds.origin.x;
    m_quad_space_right = bounds.size.w - ACTION_BAR_WIDTH;
    m_quad_space_width = ((m_quad_space_right - m_quad_space_left) / 2);
    m_quad_space_height = ((m_quad_space_bottom - m_quad_space_top) / 2);
    m_quad_space_middle_x = m_quad_space_width + m_quad_space_left;
    m_quad_space_middle_y = m_quad_space_height + m_quad_space_top;

    m_quad_space_text_padding = ((m_quad_space_height - 42) / 2) - 4;
    m_quad_space_text_height = m_quad_space_height - (m_quad_space_text_padding * 2);
}

static void setup_quad_text_layers(Layer *window_layer, GRect bounds)
{
    nw_quad_layer = text_layer_create(GRect(m_quad_space_left, m_quad_space_top + m_quad_space_text_padding, m_quad_space_width, m_quad_space_text_height));
    ne_quad_layer = text_layer_create(GRect(m_quad_space_middle_x, m_quad_space_top + m_quad_space_text_padding, m_quad_space_width, m_quad_space_text_height));
    sw_quad_layer = text_layer_create(GRect(m_quad_space_left, m_quad_space_middle_y + m_quad_space_text_padding, m_quad_space_width, m_quad_space_text_height));
    se_quad_layer = text_layer_create(GRect(m_quad_space_middle_x, m_quad_space_middle_y + m_quad_space_text_padding, m_quad_space_width, m_quad_space_text_height));

    TextLayer* textLayers[] = {nw_quad_layer, ne_quad_layer, sw_quad_layer, se_quad_layer};

    GFont font = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
    for(uint8_t i = 0; i < (uint8_t)ARRAY_LENGTH(textLayers); i++)
    {
        text_layer_set_text_color(textLayers[i], get_foreground_color());
        text_layer_set_font(textLayers[i], font);
        text_layer_set_text_alignment(textLayers[i], GTextAlignmentCenter);
        layer_add_child(window_layer, text_layer_get_layer(textLayers[i]));
    }
}

static void setup_quad_background_layers(Layer *window_layer, GRect bounds)
{
    nw_quad_background_layer = layer_create(GRect(m_quad_space_left, m_quad_space_top, m_quad_space_width, m_quad_space_height));
    ne_quad_background_layer = layer_create(GRect(m_quad_space_middle_x, m_quad_space_top, m_quad_space_width, m_quad_space_height));
    sw_quad_background_layer = layer_create(GRect(m_quad_space_left, m_quad_space_middle_y, m_quad_space_width, m_quad_space_height));
    se_quad_background_layer = layer_create(GRect(m_quad_space_middle_x, m_quad_space_middle_y, m_quad_space_width, m_quad_space_height));

    Layer* layers[] = {nw_quad_background_layer, ne_quad_background_layer, sw_quad_background_layer, se_quad_background_layer};

    for(uint8_t i = 0; i < (uint8_t)ARRAY_LENGTH(layers); i++)
    {
        layer_set_update_proc(layers[i], update_background_layer);
        layer_add_child(window_layer, layers[i]);
    }
}

static void setup_status_bar(Layer *window_layer, GRect bounds)
{
    status_bar = status_bar_layer_create();

    status_bar_layer_set_colors(status_bar, get_background_color(), get_foreground_color());
    status_bar_layer_set_separator_mode(status_bar, StatusBarLayerSeparatorModeDotted);

    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
}

static void load_main_window(Window *window)
{
    window_set_background_color(window, get_background_color());
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    calculate_quad_space(bounds);
    setup_main_window_action_bar_layer(window_layer, bounds);
    setup_status_bar(window_layer, bounds);
    setup_quad_background_layers(window_layer, bounds);
    setup_quad_text_layers(window_layer, bounds);

    setup_layers(
        nw_quad_layer,
        ne_quad_layer,
        sw_quad_layer,
        se_quad_layer,
        nw_quad_background_layer,
        ne_quad_background_layer,
        sw_quad_background_layer,
        se_quad_background_layer,
        main_window_action_bar_layer,
        status_bar,
        main_window);

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