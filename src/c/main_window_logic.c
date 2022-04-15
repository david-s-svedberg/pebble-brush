#include "main_window_logic.h"

#include "config_menu_window.h"
#include "persistance.h"
#include "icons.h"

#define MAX_TEXT_BUFFER_SIZE (4)

typedef struct {
    TextLayer* text_layer;
    Layer* background_layer;
    uint8_t time_remaining;
    char buffer[MAX_TEXT_BUFFER_SIZE];
} QuadData;

Window* m_main_window;
ActionBarLayer* m_action_bar;
StatusBarLayer* m_status_bar;

static QuadData m_quad_data[4];
static uint8_t m_current_quad_index;
static QuadData* m_current_quad;
static bool m_running;

static void update_quad_ui(QuadData* quad);
static void stop_brushing();

static const uint32_t const segments[] = { 50, 25, 50 };
static const VibePattern m_vibration_pattern =
{
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
};

static void on_time_tick(struct tm *tick_time, TimeUnits units_changed)
{
    m_current_quad->time_remaining--;
    update_quad_ui(m_current_quad);
    if(m_current_quad->time_remaining == 0)
    {
        vibes_enqueue_custom_pattern(m_vibration_pattern);
        m_current_quad_index++;
        if(m_current_quad_index < 4)
        {
            m_current_quad = &m_quad_data[m_current_quad_index];
            update_quad_ui(m_current_quad);
        } else {
            stop_brushing();
            if(use_auto_kill())
            {
                exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
                window_stack_remove(m_main_window, true);
            } else {
                reset_brushing();
            }
        }
    }
}

static void update_action_bar_icons()
{
    GBitmap* middle_icon = m_running ? get_pause_icon() : get_play_icon();

    action_bar_layer_set_icon_animated(m_action_bar, BUTTON_ID_UP, get_swap_icon(), true);
    action_bar_layer_set_icon_animated(m_action_bar, BUTTON_ID_SELECT, middle_icon, true);
    action_bar_layer_set_icon_animated(m_action_bar, BUTTON_ID_DOWN, get_config_icon(), true);
}

void start_brushing()
{
    m_running = true;
    tick_timer_service_subscribe(SECOND_UNIT, on_time_tick);
    update_action_bar_icons();
    light_enable(true);
}

static void stop_brushing()
{
    m_running = false;
    tick_timer_service_unsubscribe();
    update_action_bar_icons();
    light_enable(false);
}


void goto_config_window(ClickRecognizerRef recognizer, void* context)
{
    stop_brushing();
    setup_config_menu_window();
}

void toggle_running(ClickRecognizerRef recognizer, void* context)
{
    if(m_running)
    {
        stop_brushing();
    } else
    {
        start_brushing();
    }
}

void toggle_time(ClickRecognizerRef recognizer, void* context)
{
    toggle_quad_time();
    reset_brushing();
}

static GColor8 get_quad_background(QuadData* quad)
{
    return quad->time_remaining == 0 ? get_foreground_color() : get_background_color();
}

static void update_quad_ui(QuadData* quad)
{
    GColor8 background_color = get_quad_background(quad);
    text_layer_set_background_color(quad->text_layer, background_color);
    text_layer_set_text_color(quad->text_layer, get_foreground_color());

    layer_mark_dirty(quad->background_layer);
    if(quad == m_current_quad)
    {
        snprintf(quad->buffer, MAX_TEXT_BUFFER_SIZE, "%d", quad->time_remaining);
        text_layer_set_text(quad->text_layer, quad->buffer);
    }
}

static void update_all_quads_ui()
{
    for(uint8_t i = 0; i < (uint8_t)ARRAY_LENGTH(m_quad_data); i++)
    {
        update_quad_ui(&m_quad_data[i]);
    }
}

void reset_brushing()
{
    m_current_quad_index = 0;
    m_current_quad = &m_quad_data[m_current_quad_index];
    stop_brushing();
    GColor8 background_color = get_background_color();
    uint8_t quad_time = get_current_quad_time();

    for(uint8_t i = 0; i < (uint8_t)ARRAY_LENGTH(m_quad_data); i++)
    {
        m_quad_data[i].time_remaining = quad_time;
        snprintf(m_quad_data[i].buffer, MAX_TEXT_BUFFER_SIZE, "%d", quad_time);
        text_layer_set_background_color(m_quad_data[i].text_layer, background_color);
    }
}

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
    Window* main_window)
{
    m_quad_data[0].text_layer = nw_quad_layer;
    m_quad_data[0].background_layer = nw_quad_background_layer;

    m_quad_data[1].text_layer = ne_quad_layer;
    m_quad_data[1].background_layer = ne_quad_background_layer;

    m_quad_data[2].text_layer = sw_quad_layer;
    m_quad_data[2].background_layer = sw_quad_background_layer;

    m_quad_data[3].text_layer = se_quad_layer;
    m_quad_data[3].background_layer = se_quad_background_layer;

    m_action_bar = action_bar;
    m_status_bar = status_bar;
    m_main_window = main_window;
}

void update_main_window(Window *window)
{
    window_set_background_color(window, get_background_color());
    status_bar_layer_set_colors(m_status_bar, get_background_color(), get_foreground_color());
    action_bar_layer_set_background_color(m_action_bar, get_foreground_color());

    update_action_bar_icons();

    update_all_quads_ui();
}

static QuadData* get_quad_by_background_layer(struct Layer *layer)
{
    QuadData* ret = NULL;
    for(uint8_t i = 0; i < (uint8_t)ARRAY_LENGTH(m_quad_data); i++)
    {
        if(m_quad_data[i].background_layer == layer)
        {
            ret = &m_quad_data[i];
            break;
        }
    }
    return ret;
}

void update_background_layer(struct Layer *layer, GContext *ctx)
{
    QuadData* quad = get_quad_by_background_layer(layer);
    GColor8 background = get_quad_background(quad);
    graphics_context_set_fill_color(ctx, background);
    graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}