#include "cru/platform/gui/xcb/Window.h"
#include "cru/platform/Check.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/graphics/cairo/CairoPainter.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/Keyboard.h"
#include "cru/platform/gui/Window.h"
#include "cru/platform/gui/xcb/Keyboard.h"
#include "cru/platform/gui/xcb/UiApplication.h"

#include <cairo-xcb.h>
#include <cairo.h>
#include <xcb/xcb.h>
#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <memory>
#include <optional>

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
      KeyModifiers::Shift, KeyModifiers::none, KeyModifiers::Ctrl,
      KeyModifiers::Alt,   KeyModifiers::none, KeyModifiers::none,
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
    : application_(application),
      xcb_window_(std::nullopt),
      cairo_surface_(nullptr),
      parent_(nullptr) {
  application->RegisterWindow(this);
}

XcbWindow::~XcbWindow() { application_->UnregisterWindow(this); }

void XcbWindow::Close() {
  if (xcb_window_) {
    xcb_destroy_window(application_->GetXcbConnection(), *xcb_window_);
  }
}

INativeWindow *XcbWindow::GetParent() { return parent_; }

void XcbWindow::SetParent(INativeWindow *parent) {
  parent_ = CheckPlatform<XcbWindow>(parent, GetPlatformIdUtf8());
  if (xcb_window_) {
    auto real_parent = application_->GetFirstXcbScreen()->root;
    if (parent_ && parent_->xcb_window_) {
      real_parent = *parent_->xcb_window_;
    }
    xcb_reparent_window(application_->GetXcbConnection(), *xcb_window_,
                        real_parent, 0, 0);
    // TODO: Maybe restore position?
  }
}

std::unique_ptr<graphics::IPainter> XcbWindow::BeginPaint() {
  assert(cairo_surface_);

  auto factory = application_->GetCairoFactory();
  cairo_t *cairo = cairo_create(cairo_surface_);
  auto painter =
      new graphics::cairo::CairoPainter(factory, cairo, true, cairo_surface_);
  return std::unique_ptr<graphics::IPainter>(painter);
}

IEvent<std::nullptr_t> *XcbWindow::CreateEvent() { return &create_event_; }

IEvent<std::nullptr_t> *XcbWindow::DestroyEvent() { return &destroy_event_; }

IEvent<std::nullptr_t> *XcbWindow::PaintEvent() { return &paint_event_; }

IEvent<WindowVisibilityType> *XcbWindow::VisibilityChangeEvent() {
  return &visibility_change_event_;
}

IEvent<Size> *XcbWindow::ResizeEvent() { return &resize_event_; }

IEvent<FocusChangeType> *XcbWindow::FocusEvent() { return &focus_event_; }

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

IEvent<NativeKeyEventArgs> *XcbWindow::KeyDownEvent() {
  return &key_down_event_;
}

IEvent<NativeKeyEventArgs> *XcbWindow::KeyUpEvent() { return &key_up_event_; }

std::optional<xcb_window_t> XcbWindow::GetXcbWindow() { return xcb_window_; }

xcb_window_t XcbWindow::DoCreateWindow() {
  assert(xcb_window_ == std::nullopt);
  assert(cairo_surface_ == nullptr);

  auto connection = application_->GetXcbConnection();
  auto screen = application_->GetFirstXcbScreen();

  auto window = xcb_generate_id(connection);

  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t values[1] = {
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
      XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
      XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
      XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE};

  int width = 400, height = 200;

  xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 100,
                    100, width, height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual, mask, values);
  current_size_ = Size(width, height);

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

  cairo_surface_ =
      cairo_xcb_surface_create(connection, window, visual_type, width, height);

  create_event_.Raise(nullptr);

  return window;
}

void XcbWindow::HandleEvent(xcb_generic_event_t *event) {
  switch (event->response_type & ~0x80) {
    case XCB_EXPOSE: {
      paint_event_.Raise(nullptr);
      break;
    }
    case XCB_DESTROY_NOTIFY: {
      destroy_event_.Raise(nullptr);

      cairo_surface_destroy(cairo_surface_);
      cairo_surface_ = nullptr;
      xcb_window_ = std::nullopt;
      break;
    }
    case XCB_CONFIGURE_NOTIFY: {
      xcb_configure_notify_event_t *configure =
          (xcb_configure_notify_event_t *)event;
      auto width = configure->width, height = configure->height;
      if (width != current_size_.width || height != current_size_.height) {
        current_size_ = Size(width, height);
        assert(cairo_surface_);
        cairo_xcb_surface_set_size(cairo_surface_, width, height);
        resize_event_.Raise(current_size_);
      }
      break;
    }
    case XCB_MAP_NOTIFY: {
      visibility_change_event_.Raise(WindowVisibilityType::Show);
      break;
    }
    case XCB_UNMAP_NOTIFY: {
      visibility_change_event_.Raise(WindowVisibilityType::Hide);
      break;
    }
    case XCB_FOCUS_IN: {
      focus_event_.Raise(FocusChangeType::Gain);
      break;
    }
    case XCB_FOCUS_OUT: {
      focus_event_.Raise(FocusChangeType::Lose);
      break;
    }
    case XCB_BUTTON_PRESS: {
      xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;

      if (bp->detail >= 4 && bp->detail <= 7) {
        NativeMouseWheelEventArgs args(30, Point(bp->event_x, bp->event_y),
                                       ConvertModifiers(bp->state), false);
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
                                      ConvertModifiers(bp->state));
      mouse_down_event_.Raise(std::move(args));
      break;
    }
    case XCB_BUTTON_RELEASE: {
      xcb_button_release_event_t *br = (xcb_button_release_event_t *)event;
      NativeMouseButtonEventArgs args(ConvertMouseButton(br->detail),
                                      Point(br->event_x, br->event_y),
                                      ConvertModifiers(br->state));
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
      // Should we do this?
      // xcb_leave_notify_event_t *leave = (xcb_leave_notify_event_t *)event;
      // Point point(leave->event_x, leave->event_y);
      // mouse_move_event_.Raise(point);
      mouse_enter_leave_event_.Raise(MouseEnterLeaveType::Leave);
      break;
    }
    case XCB_KEY_PRESS: {
      xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;
      NativeKeyEventArgs args(XorgKeycodeToCruKeyCode(application_, kp->detail),
                              ConvertModifiers(kp->state));
      key_down_event_.Raise(std::move(args));
      break;
    }
    case XCB_KEY_RELEASE: {
      xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;
      NativeKeyEventArgs args(XorgKeycodeToCruKeyCode(application_, kr->detail),
                              ConvertModifiers(kr->state));
      key_up_event_.Raise(std::move(args));
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
    case XCB_DESTROY_NOTIFY: {
      xcb_destroy_notify_event_t *destroy = (xcb_destroy_notify_event_t *)event;
      return destroy->event;
    }
    case XCB_CONFIGURE_NOTIFY: {
      xcb_configure_notify_event_t *configure =
          (xcb_configure_notify_event_t *)event;
      return configure->event;
    }
    case XCB_MAP_NOTIFY: {
      xcb_map_notify_event_t *map = (xcb_map_notify_event_t *)event;
      return map->event;
    }
    case XCB_UNMAP_NOTIFY: {
      xcb_unmap_notify_event_t *unmap = (xcb_unmap_notify_event_t *)event;
      return unmap->event;
    }
    case XCB_FOCUS_IN: {
      xcb_focus_in_event_t *fi = (xcb_focus_in_event_t *)event;
      return fi->event;
    }
    case XCB_FOCUS_OUT: {
      xcb_focus_out_event_t *fo = (xcb_focus_out_event_t *)event;
      return fo->event;
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
