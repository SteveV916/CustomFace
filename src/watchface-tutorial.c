#include <pebble.h>
#include <string.h>

#define KEY_DATA 0
  
static Window *s_main_window;
static TextLayer *s_date_layer;
static TextLayer *s_time_layer;
static TextLayer *s_data_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  BatteryChargeState charge_state;
  
  //date
  static char s_buffer2[15];
  strftime(s_buffer2, sizeof("mm/dd      "), "%m/%d     ", tick_time);
  //battery
  static char battery_text[] = "100%";
  charge_state = battery_state_service_peek();
  snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  strcat(s_buffer2, battery_text);
  text_layer_set_text(s_date_layer, s_buffer2);
  
  //hours minutes
  static char s_buffer1[8];
  strftime(s_buffer1, sizeof(s_buffer1), "%l:%M", tick_time);  
  text_layer_set_text(s_time_layer, s_buffer1);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(0, 0, bounds.size.w, 30));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "mm/dd");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 28, bounds.size.w, 60));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  // Create data Layer
  s_data_layer = text_layer_create(GRect(0, 85, bounds.size.w, 80));
  text_layer_set_background_color(s_data_layer, GColorClear);
  text_layer_set_text_color(s_data_layer, GColorWhite);
  text_layer_set_text_alignment(s_data_layer, GTextAlignmentCenter);
  text_layer_set_text(s_data_layer, "Loading...");
  text_layer_set_font(s_data_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_data_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}
 
static void main_window_unload(Window *window) {
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_data_layer);
}
 
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get data update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
 
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
 
    // Send the message!
    app_message_outbox_send();
  }
}
 
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char data_buffer[60];
  
  // Read tuples for data
  Tuple *data_tuple = dict_find(iterator, KEY_DATA);

  // If all data is available, use it
  if(data_tuple) {
    snprintf(data_buffer, sizeof(data_buffer), "%s", data_tuple->value->cstring);
    text_layer_set_text(s_data_layer, data_buffer);
  }
}
 
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
 
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
 
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}
 
static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}
 
int main(void) {
  init();
  app_event_loop();
  deinit();
}