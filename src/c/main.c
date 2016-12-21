#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static int toa = 60;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void draw_time_circle(GContext *ctx){
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  
  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_radial(ctx, GRect(30,30,10,10), GOvalScaleModeFillCircle, 3, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
}

static void layer_draw_flag(Layer *layer, GContext *ctx) {
  
   // draw horizon line
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_antialiased(ctx, true);
  
  //graphics_draw_line(ctx, GPoint(0, toa+30), GPoint(200, toa+30));
  
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, GRect(0,0,200,toa+30), 0, 0);
  
  // draw sinewave
  
  // Better sine wave
  // Background
  /*
  graphics_context_set_stroke_width(ctx, 4);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_arc(ctx, GRect(0,toa+17,40,40), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(135), DEG_TO_TRIGANGLE(180));
  graphics_draw_line(ctx, GPoint(35, toa+50), GPoint(72, toa+5));
  graphics_draw_arc(ctx, GRect(67,toa,40,40), GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(-45), DEG_TO_TRIGANGLE(45));
  graphics_draw_line(ctx, GPoint(100, toa+5), GPoint(145, toa+50));
  graphics_draw_arc(ctx, GRect(140,toa+17,40,40), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(-180), DEG_TO_TRIGANGLE(-135));
  */
  
  // forground
  
  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_stroke_color(ctx, GColorYellow);
  graphics_draw_arc(ctx, GRect(0,toa+17,40,40), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(135), DEG_TO_TRIGANGLE(180));
  graphics_draw_line(ctx, GPoint(35, toa+50), GPoint(72, toa+5));
  graphics_draw_arc(ctx, GRect(67,toa,40,40), GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(-45), DEG_TO_TRIGANGLE(45));
  graphics_draw_line(ctx, GPoint(100, toa+5), GPoint(145, toa+50));
  graphics_draw_arc(ctx, GRect(140,toa+17,40,40), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(-180), DEG_TO_TRIGANGLE(-135));

  draw_time_circle(ctx);
}

static void main_window_load(Window *window) {
  
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);

  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  
  s_time_layer = text_layer_create(
      GRect(100, PBL_IF_ROUND_ELSE(130, 132), bounds.size.w, 80));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  
  text_layer_set_text(s_time_layer, "00:00");
 
  //text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
  
   // Create GFont
  //s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PATRIOT_48));
  
  //s_time_font = fonts_load_custom_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  // Apply to TextLayer
  //text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  
  // Create a Layer and set the update_proc
  Layer *flag_layer = window_get_root_layer(window);
  flag_layer = layer_create(GRect(0, 0, 200, 200));
  
  layer_set_update_proc(flag_layer, layer_draw_flag);
  

  // Add it as a child layer to the Window's root layer
  
  layer_add_child(window_layer, flag_layer);

  
}

static void main_window_unload(Window *window) {
  // Unload GFont
  //fonts_unload_custom_font(s_time_font);

  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
}





static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_background_color(s_main_window, GColorBlack);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
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
