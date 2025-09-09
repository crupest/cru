#include "cru/platform/gui/xcb/Window.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/Keyboard.h"
#include "cru/platform/gui/Window.h"
#include "cru/platform/gui/xcb/Keyboard.h"
#include "cru/platform/gui/xcb/UiApplication.h"

#include <xcb/xcb.h>
#include <cinttypes>
#include <cstdint>

namespace {
// TODO: Remove this demo function.
/* print names of modifiers present in mask */
void print_modifiers(uint32_t mask) {
  const char *MODIFIERS[] = {
      "Shift", "Lock",    "Ctrl",    "Alt",     "Mod2",    "Mod3",   "Mod4",
      "Mod5",  "Button1", "Button2", "Button3", "Button4", "Button5"};

  printf("Modifier mask: ");
  for (const char **modifier = MODIFIERS; mask; mask >>= 1, ++modifier) {
    if (mask & 1) {
      printf("%s", *modifier);
    }
  }
  printf("\n");
}

}  // namespace

namespace cru::platform::gui::xcb {

namespace {
MouseButton ConvertMouseButton(xcb_button_t button) {
  switch (button) {
    case 1:
      return MouseButtons::Left;
    case 2:
      return MouseButtons::Middle;
    case 3:
      return MouseButtons::Right;
    default:
      return MouseButtons::None;
  }
}

KeyModifier ConvertModifiers(uint32_t mask) {
  // const char *MODIFIERS[] = {
  //     "Shift", "Lock",    "Ctrl",    "Alt",     "Mod2",    "Mod3",   "Mod4",
  //     "Mod5",  "Button1", "Button2", "Button3", "Button4", "Button5"};
  constexpr KeyModifier MODIFIERS[] = {
      KeyModifiers::shift, KeyModifiers::none, KeyModifiers::ctrl,
      KeyModifiers::alt,   KeyModifiers::none, KeyModifiers::none,
      KeyModifiers::none,  KeyModifiers::none, KeyModifiers::none,
      KeyModifiers::none,  KeyModifiers::none, KeyModifiers::none,
      KeyModifiers::none,
  };

  KeyModifier result;
  for (auto iter = std::begin(MODIFIERS); mask; mask >>= 1, ++iter) {
    if (mask & 1) {
      result |= *iter;
    }
  }
  return result;
}
}  // namespace

XcbWindow::XcbWindow(XcbUiApplication *application)
    : application_(application) {
  application->RegisterWindow(this);
}

XcbWindow::~XcbWindow() { application_->UnregisterWindow(this); }

IEvent<MouseEnterLeaveType> *XcbWindow::MouseEnterLeaveEvent() {
  return &mouse_enter_leave_event_;
}

IEvent<Point> *XcbWindow::MouseMoveEvent() { return &mouse_move_event_; }

IEvent<NativeMouseButtonEventArgs> *XcbWindow::MouseDownEvent() {
  return &mouse_down_event_;
}

IEvent<NativeMouseButtonEventArgs> *XcbWindow::MouseUpEvent() {
  return &mouse_up_event_;
}

IEvent<NativeMouseWheelEventArgs> *XcbWindow::MouseWheelEvent() {
  return &mouse_wheel_event_;
}

std::optional<xcb_window_t> XcbWindow::GetXcbWindow() { return xcb_window_; }

xcb_window_t XcbWindow::DoCreateWindow() {
  auto connection = application_->GetXcbConnection();
  auto screen = application_->GetFirstXcbScreen();

  auto xcb_window = xcb_generate_id(connection);

  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t values[1] = {
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
      XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
      XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE};

  xcb_create_window(connection, XCB_COPY_FROM_PARENT, xcb_window, screen->root,
                    100, 100, 400, 200, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual, mask, values);

  return xcb_window;
}

void XcbWindow::HandleEvent(xcb_generic_event_t *event) {
  switch (event->response_type & ~0x80) {
    case XCB_EXPOSE: {
      xcb_expose_event_t *expose = (xcb_expose_event_t *)event;

      printf("Window %" PRIu32
             " exposed. Region to be redrawn at location (%" PRIu16 ",%" PRIu16
             "), with dimension (%" PRIu16 ",%" PRIu16 ")\n",
             expose->window, expose->x, expose->y, expose->width,
             expose->height);
      break;
    }
    case XCB_BUTTON_PRESS: {
      xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;

      if (bp->detail >= 4 && bp->detail <= 7) {
        NativeMouseWheelEventArgs args(30, Point(bp->event_x, bp->event_y),
                                       GetCurrentKeyModifiers(application_),
                                       false);
        if (bp->detail == 5 || bp->detail == 7) {
          args.delta = -args.delta;
        }
        if (bp->detail == 6 || bp->detail == 7) {
          args.horizontal = true;
        }
        mouse_wheel_event_.Raise(std::move(args));
        break;
      }

      NativeMouseButtonEventArgs args(ConvertMouseButton(bp->detail),
                                      Point(bp->event_x, bp->event_y),
                                      GetCurrentKeyModifiers(application_));
      mouse_down_event_.Raise(std::move(args));
      break;
    }
    case XCB_BUTTON_RELEASE: {
      xcb_button_release_event_t *br = (xcb_button_release_event_t *)event;
      NativeMouseButtonEventArgs args(ConvertMouseButton(br->detail),
                                      Point(br->event_x, br->event_y),
                                      GetCurrentKeyModifiers(application_));
      mouse_up_event_.Raise(std::move(args));
      break;
    }
    case XCB_MOTION_NOTIFY: {
      xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *)event;
      Point point(motion->event_x, motion->event_y);
      mouse_move_event_.Raise(point);
      break;
    }
    case XCB_ENTER_NOTIFY: {
      xcb_enter_notify_event_t *enter = (xcb_enter_notify_event_t *)event;
      mouse_enter_leave_event_.Raise(MouseEnterLeaveType::Enter);
      Point point(enter->event_x, enter->event_y);
      mouse_move_event_.Raise(point);
      break;
    }
    case XCB_LEAVE_NOTIFY: {
      xcb_leave_notify_event_t *leave = (xcb_leave_notify_event_t *)event;
      // Should we do this?
      // Point point(leave->event_x, leave->event_y);
      // mouse_move_event_.Raise(point);
      mouse_enter_leave_event_.Raise(MouseEnterLeaveType::Leave);
      break;
    }
    case XCB_KEY_PRESS: {
      xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;
      print_modifiers(kp->state);

      printf("Key pressed in window %" PRIu32 "\n", kp->event);
      break;
    }
    case XCB_KEY_RELEASE: {
      xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;
      print_modifiers(kr->state);

      printf("Key released in window %" PRIu32 "\n", kr->event);
      break;
    }
    default:
      /* Unknown event type, ignore it */
      printf("Unknown event: %" PRIu8 "\n", event->response_type);
      break;
  }
}

std::optional<xcb_window_t> XcbWindow::GetEventWindow(
    xcb_generic_event_t *event) {
  switch (event->response_type & ~0x80) {
    case XCB_EXPOSE: {
      xcb_expose_event_t *expose = (xcb_expose_event_t *)event;
      return expose->window;
    }
    case XCB_BUTTON_PRESS: {
      xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;
      return bp->event;
    }
    case XCB_BUTTON_RELEASE: {
      xcb_button_release_event_t *br = (xcb_button_release_event_t *)event;
      return br->event;
    }
    case XCB_MOTION_NOTIFY: {
      xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *)event;
      return motion->event;
    }
    case XCB_ENTER_NOTIFY: {
      xcb_enter_notify_event_t *enter = (xcb_enter_notify_event_t *)event;
      return enter->event;
    }
    case XCB_LEAVE_NOTIFY: {
      xcb_leave_notify_event_t *leave = (xcb_leave_notify_event_t *)event;
      return leave->event;
    }
    case XCB_KEY_PRESS: {
      xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;
      return kp->event;
    }
    case XCB_KEY_RELEASE: {
      xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;
      return kr->event;
    }
    default:
      return std::nullopt;
  }
}
}  // namespace cru::platform::gui::xcb
