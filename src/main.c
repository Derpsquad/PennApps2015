#include <pebble.h>
  
#define SLEEP_AFTER_MOVE_TIME 5000

enum App_State {
  awake,
  level1,
  level2,
  level3,
  ringing    
};
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static int secondCount = 0; //Keeps track of how many seconds since user has moved
static int x_accel = 0, y_accel = 0, z_accel = 0; //Keeps track of most recent accelerometer reading
static int x_prev = 0, y_prev = 0, z_prev = 0;    //2nd most recent accelerometer reading
static int sleep_threshold[4] = {5, 10, 20, 30};




//Tracks whether the watch is moving or not
//Sensitivity should be updated after testing
bool moving(int sensitivity) { 
  
  //Checks to see if watch has moved by 50 units on any axis
  if ((x_accel >= x_prev + sensitivity) || (x_accel <= x_prev - sensitivity) ||
        (y_accel >= y_prev + sensitivity) || (y_accel <= y_prev - sensitivity) ||
        (z_accel >= z_prev + sensitivity) || (z_accel <= z_prev - sensitivity)) {
    //If yes, we know the watch was moving
    return true;
  }
  else {
    //If not, it was stationary
    return false;
  }

}

enum App_State get_state(int seconds){
  if (seconds>=sleep_threshold[3]){
    return ringing;
  }
  else if (seconds>=sleep_threshold[2]){
    return level3;
  }
  else if (seconds>=sleep_threshold[1]){
    return level2;
  }
  else if (seconds>=sleep_threshold[0]){
    return level1;
  }
  else return awake;
}

static void data_handler(AccelData *data, uint32_t num_samples) {
  /*Needed to get the data from the accelerometer
  The 'data' variable has everything we need and has attributes x y and z
  'num_samples' is how many sample we want for each cycle of sampling ("We're at 10Hz")
  Only using one to conserve memory/battery*/
  
  //This block stops the pebble from vibrating if shaken
  enum App_State sleep_state = get_state(secondCount);
  if (sleep_state == ringing && moving(200)) {
    secondCount = 0;
    sleep_state = awake;
  }
  
  if (x_accel == 0 && y_accel == 0 && z_accel == 0) {
    x_accel = data[0].x;    //If this is the first recording, then just record the first values
    y_accel = data[0].y;
    z_accel = data[0].z;
  }
  else {
    
    x_prev = x_accel;    //Holding these values to compare to new ones in the timeticker
    y_prev = y_accel;
    z_prev = z_accel;
    
    x_accel = data[0].x;
    y_accel = data[0].y;
    z_accel = data[0].z;
  }
  
}

//Calls the function every x where x is units_changed variable in the init() function (using seconds for now)
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  static char buff[128];
  enum App_State sleep_state = get_state(secondCount);
  
  if (moving(50) && (sleep_state != ringing)){
    secondCount = 0;
    sleep_state = awake;
  }
  
  switch (sleep_state) {
    case awake:
      text_layer_set_text(s_time_layer, "Good job!");
      secondCount++;//note:this is a special case because it sleeps for the entire interval, essentially waiting for level1
      break;
    case level1:
      text_layer_set_text(s_time_layer, "Feeling tired?");
      secondCount++;
      break;
    case level2:
      text_layer_set_text(s_time_layer, "Asleep?");
      secondCount++;
      break;
    case level3:
      snprintf(buff, sizeof(buff), "Buzzer coming in %d...", 30-secondCount);
      text_layer_set_text(s_time_layer, buff);
      secondCount++;
      break;
    case ringing:
      text_layer_set_text(s_time_layer, "Wakey wakey!");
      vibes_long_pulse();
      secondCount++;
      break;
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  enum App_State sleep_state = get_state(secondCount);
  if (sleep_state == ringing) {
    secondCount = 0;
    text_layer_set_text(s_time_layer, "Try to stay awake!");
  }
}

static void wakeup_handler(WakeupId id, int32_t reason){
  return;
}

static void click_config_provider(void *context) {
  
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  
}




static void main_window_load(Window *window) {
  
  //Sets up the text layer in a rectangle
  s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  
  //Sets up the colors and content of the layer
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text(s_time_layer, "Try to stay awake!");
  
  //Sets up the font for the layer
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  //Adds the layer to the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer); //Destroys the layer to free up memory
}

static void init() {
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler); //Sets the pebble to call the handler every second
  //window_set_click_config_provider(s_main_window, click_config_provider);
  
  /*Sets up the accelerometer
    -Takes ten samples (num_samples) every time data is collected
    -Gives the data to the data_handler function
    -Sets sampling rate to 10Hz
  */
  int num_samples = 10;
  accel_data_service_subscribe(num_samples, data_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_click_config_provider(s_main_window, click_config_provider);
  
  window_stack_push(s_main_window, true);
  
  wakeup_service_subscribe(wakeup_handler);
  
  time_t future_time = time(NULL) + 30;
  
  wakeup_schedule(future_time, 1, false);
  
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {

  init();
  app_event_loop();
  deinit();
  
}