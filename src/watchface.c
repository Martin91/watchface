#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;
static GFont s_time_font;
static GFont s_weather_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  text_layer_set_text(s_time_layer, s_buffer);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // 加载字体
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));

  // 添加位图
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // 创建文本层
  s_time_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // 天气信息层
  s_weather_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(125, 120), bounds.size.w, 25));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weather_layer, s_weather_font);
  text_layer_set_text(s_weather_layer, "Loading...");

  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  fonts_unload_custom_font(s_weather_font);
  fonts_unload_custom_font(s_time_font);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_time_layer);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorBlack);

  window_stack_push(s_main_window, true);
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}