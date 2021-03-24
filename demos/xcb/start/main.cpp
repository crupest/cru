#include <unistd.h>
#include <xcb/xcb.h>

int main() {
  int screen_num;
  xcb_connection_t* connection = xcb_connect(NULL, &screen_num);
  const xcb_setup_t* setup = xcb_get_setup(connection);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  xcb_screen_t* screen = iter.data;

  xcb_window_t window = xcb_generate_id(connection);
  xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0,
                    0, 150, 150, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual, 0, NULL);
  xcb_map_window(connection, window);
  xcb_flush(connection);

  pause();

  xcb_disconnect(connection);

  return 0;
}
