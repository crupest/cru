#include <cairo/cairo-xcb.h>
#include <cairo/cairo.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <cstdlib>

int main() {
  int screen_num;
  xcb_connection_t *connection = xcb_connect(NULL, &screen_num);
  const xcb_setup_t *setup = xcb_get_setup(connection);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  xcb_screen_t *screen = iter.data;

  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t data[] = {XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};

  int width = 500;
  int height = 500;

  xcb_window_t window = xcb_generate_id(connection);
  xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0,
                    0, width, height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual, mask, data);
  xcb_map_window(connection, window);
  xcb_flush(connection);

  xcb_visualtype_t *visual_type;

  for (xcb_depth_iterator_t depth_iter =
           xcb_screen_allowed_depths_iterator(screen);
       depth_iter.rem; xcb_depth_next(&depth_iter)) {
    for (xcb_visualtype_iterator_t visual_iter =
             xcb_depth_visuals_iterator(depth_iter.data);
         visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
      if (screen->root_visual == visual_iter.data->visual_id) {
        visual_type = visual_iter.data;
        break;
      }
    }
  }

  cairo_surface_t *surface =
      cairo_xcb_surface_create(connection, window, visual_type, width, height);

  cairo_t *cairo = cairo_create(surface);

  xcb_generic_event_t *e;

  auto paint = [](cairo_t *cairo, int width, int height) {
    cairo_set_source_rgb(cairo, 1, 1, 1);
    cairo_paint(cairo);
    cairo_set_source_rgb(cairo, 1, 1, 0);
    cairo_set_line_width(cairo, 5);
    cairo_move_to(cairo, width / 2.0, 0);
    cairo_curve_to(cairo, width, 0, width, 0, width, height / 2.0);
    cairo_curve_to(cairo, width, height, width, height, width / 2.0, height);
    cairo_curve_to(cairo, 0, height, 0, height, 0, height / 2.0);
    cairo_curve_to(cairo, 0, 0, 0, 0, width / 2.0, 0);
    cairo_stroke(cairo);
  };

  while ((e = xcb_wait_for_event(connection))) {
    switch (e->response_type & ~0x80) {
      case XCB_EXPOSE: {
        paint(cairo, width, height);
        break;
      }
      case XCB_CONFIGURE_NOTIFY: {
        xcb_configure_notify_event_t *event =
            reinterpret_cast<xcb_configure_notify_event_t *>(e);

        width = event->width;
        height = event->height;

        cairo_xcb_surface_set_size(surface, width, height);

        paint(cairo, width, height);

        break;
      }
      default: {
        /* Unknown event type, ignore it */
        break;
      }
    }
    /* Free the Generic Event */
    free(e);
  }

  pause();

  cairo_destroy(cairo);
  cairo_surface_destroy(surface);
  xcb_disconnect(connection);

  return 0;
}
