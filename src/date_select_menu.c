#include <pebble.h>
#include "date_select_menu.h"
#include "time_select_menu.h"
Window *s_main_window;
int weekDay = 0;

void date_select_menu_load(Window *window) {
  int num_a_items = 0;
  
  second_menu_items[num_a_items++] = (SimpleMenuItem){
    // You should give each menu item a title and callback
    .title = "Monday",
    .callback = menu_select_callback,
  };
  second_menu_items[num_a_items++] = (SimpleMenuItem){
    // You should give each menu item a title and callback
    .title = "Tuesday",
    .callback = menu_select_callback,
  };
  second_menu_items[num_a_items++] = (SimpleMenuItem){
    // You should give each menu item a title and callback
    .title = "Wednesday",
    .callback = menu_select_callback,
  };
  second_menu_items[num_a_items++] = (SimpleMenuItem){
    // You should give each menu item a title and callback
    .title = "Thursday",
    .callback = menu_select_callback,
  };
  second_menu_items[num_a_items++] = (SimpleMenuItem){
    // You should give each menu item a title and callback
    .title = "Friday",
    .callback = menu_select_callback,
  };
  second_menu_items[num_a_items++] = (SimpleMenuItem){
    // You should give each menu item a title and callback
    .title = "Saturday",
    .callback = menu_select_callback,
  };
  second_menu_items[num_a_items++] = (SimpleMenuItem){
    // You should give each menu item a title and callback
    .title = "Sunday",
    .callback = menu_select_callback,
  };
  
  menu_sections[0] = (SimpleMenuSection){
    .num_items = DAY_LIST_LENGTH,
    .items = second_menu_items,
  };
  Layer *menu_window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(menu_window_layer);
  
    // Initialize the simple menu layer
  simple_menu_layer2 = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);

  // Add it to the window for display
  layer_add_child(menu_window_layer, simple_menu_layer_get_layer(simple_menu_layer2));
}
void date_select_menu_unload(Window *window) {
  simple_menu_layer_destroy(simple_menu_layer2);

  // Cleanup the menu icon
  //gbitmap_destroy(menu_icon_image);
  
}

static void menu_select_callback(int index, void *ctx) {
  
    switch (index) {
      case 0:
        weekDay = 0;
        break;
      case 1:
        weekDay = 1;
        break;
      case 2:
        weekDay = 2;
        break;
      case 3:
        weekDay = 3;
        break;
      case 4:
        weekDay = 4;
        break;
      case 5:
        weekDay = 5;
        break;
      case 6:
        weekDay = 6;
        break;
    }
  
    time_select_window= window_create();
  
    window_set_window_handlers(time_select_window, (WindowHandlers) {
      .load = time_select_window_load,
      .unload = time_select_window_unload
    });
    
    
    window_stack_push(time_select_window, true);
  

}