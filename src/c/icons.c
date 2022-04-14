#include "icons.h"

static GBitmap *check_icon;
static GBitmap *config_icon;
static GBitmap *edit_icon;
static GBitmap *up_icon;
static GBitmap *down_icon;
static GBitmap *play_icon;
static GBitmap *brush_icon;
static GBitmap *refresh_icon;
static GBitmap *pause_icon;
static GBitmap *swap_icon;

static GBitmap* get_icon(uint32_t id, GBitmap* icon)
{
    if(icon == NULL)
    {
        icon = gbitmap_create_with_resource(id);
    }

    return icon;
}

static void destroy_icon(GBitmap* icon)
{
    if(icon == NULL)
    {
        gbitmap_destroy(icon);
    }
}

GBitmap* get_check_icon()
{
    return get_icon(RESOURCE_ID_CHECK_ICON, check_icon);
}

GBitmap* get_config_icon()
{
    return get_icon(RESOURCE_ID_CONFIG_ICON, config_icon);
}

GBitmap* get_edit_icon()
{
    return get_icon(RESOURCE_ID_EDIT_ICON, edit_icon);
}

GBitmap* get_up_icon()
{
    return get_icon(RESOURCE_ID_UP_ICON, up_icon);
}

GBitmap* get_down_icon()
{
    return get_icon(RESOURCE_ID_DOWN_ICON, down_icon);
}

GBitmap* get_play_icon()
{
    return get_icon(RESOURCE_ID_PLAY_ICON, play_icon);
}

GBitmap* get_brush_icon()
{
    return get_icon(RESOURCE_ID_BRUSH_ICON, brush_icon);
}

GBitmap* get_refresh_icon()
{
    return get_icon(RESOURCE_ID_REFRESH_ICON, refresh_icon);
}

GBitmap* get_pause_icon()
{
    return get_icon(RESOURCE_ID_PAUSE_ICON, pause_icon);
}

GBitmap* get_swap_icon()
{
    return get_icon(RESOURCE_ID_SWAP_ICON, swap_icon);
}

void destroy_all_icons()
{
    destroy_icon(check_icon);
    destroy_icon(config_icon);
    destroy_icon(edit_icon);
    destroy_icon(up_icon);
    destroy_icon(down_icon);
    destroy_icon(play_icon);
    destroy_icon(brush_icon);
    destroy_icon(refresh_icon);
    destroy_icon(pause_icon);
    destroy_icon(swap_icon);
}
