#include "main_window_logic.h"

#include "config_menu_window.h"
#include "persistance.h"
#include "icons.h"

#define MAX_TEXT_BUFFER_SIZE (4)

typedef struct {
    TextLayer* text_layer;
    uint8_t time_remaining;
    char buffer[MAX_TEXT_BUFFER_SIZE];
} QuadData;

Window* m_main_window;
ActionBarLayer* m_action_bar;

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

void start_brushing()
{
    m_running = true;
    tick_timer_service_subscribe(SECOND_UNIT, on_time_tick);
    action_bar_layer_set_icon_animated(m_action_bar, BUTTON_ID_SELECT, get_pause_icon(), true);
    light_enable(true);
}

static void stop_brushing()
{
    m_running = false;
    tick_timer_service_unsubscribe();
    action_bar_layer_set_icon_animated(m_action_bar, BUTTON_ID_SELECT, get_play_icon(), true);
    light_enable(false);
}


void goto_config_window(ClickRecognizerRef recognizer, void* context)
{
    stop_brushing();
    setup_config_menu_window(get_background_color(), get_foreground_color());
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

static void update_quad_ui(QuadData* quad)
{
    GColor8 background_color = get_background_color();
    GColor8 foreground_color = get_foreground_color();
    GColor8 color_to_use = quad->time_remaining == 0 ? foreground_color : background_color;

    snprintf(quad->buffer, MAX_TEXT_BUFFER_SIZE, "%d", quad->time_remaining);
    text_layer_set_text(quad->text_layer, quad->buffer);
    text_layer_set_background_color(quad->text_layer, color_to_use);
}

static void update_all_quads_ui()
{
    for(int i = 0; i < 4; i++)
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

    for(int i = 0; i < 4; i++)
    {
        m_quad_data[i].time_remaining = quad_time;
        snprintf(m_quad_data[i].buffer, MAX_TEXT_BUFFER_SIZE, "%d", quad_time);
        text_layer_set_background_color(m_quad_data[i].text_layer, background_color);
    }

    text_layer_set_text(m_current_quad->text_layer, m_current_quad->buffer);
}

void setup_layers(
    TextLayer* nw_quad_layer,
    TextLayer* ne_quad_layer,
    TextLayer* sw_quad_layer,
    TextLayer* se_quad_layer,
    ActionBarLayer* action_bar,
    Window* main_window)
{
    m_quad_data[0].text_layer = nw_quad_layer;
    m_quad_data[1].text_layer = ne_quad_layer;
    m_quad_data[2].text_layer = sw_quad_layer;
    m_quad_data[3].text_layer = se_quad_layer;

    m_action_bar = action_bar;
    m_main_window = main_window;
}

void update_main_window(Window *window)
{
    update_all_quads_ui();
}