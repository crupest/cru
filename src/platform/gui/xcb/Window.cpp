#include "cru/platform/gui/xcb/Window.h"
#include "cru/base/Base.h"
#include "cru/base/Guard.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/Check.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/graphics/NullPainter.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/graphics/cairo/CairoPainter.h"
#include "cru/platform/gui/xcb/Cursor.h"
#include "cru/platform/gui/xcb/Input.h"
#include "cru/platform/gui/xcb/InputMethod.h"
#include "cru/platform/gui/xcb/UiApplication.h"

#include <cairo-xcb.h>
#include <cairo.h>
#include <xcb/xcb.h>
#include <algorithm>
#include <cassert>
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
}  // namespace

XcbWindow::XcbWindow(XcbUiApplication *application)
    : application_(application),
      xcb_window_(std::nullopt),
      cairo_surface_(nullptr),
      parent_(nullptr) {
  application->RegisterWindow(this);
  input_method_ = new XcbXimInputMethodContext(
      application->GetXcbXimInputMethodManager(), this);

  paint_event_.AddSpyOnlyHandler([this] {
    if (xcb_window_)
      CRU_LOG_TAG_DEBUG("{:#x} Paint event triggered.", *xcb_window_);
  });
}

XcbWindow::~XcbWindow() {
  delete input_method_;
  application_->UnregisterWindow(this);
}

bool XcbWindow::IsCreated() { return xcb_window_.has_value(); }

void XcbWindow::Close() {
  if (xcb_window_) {
    xcb_destroy_window(application_->GetXcbConnection(), *xcb_window_);
    application_->XcbFlush();
  }
}

INativeWindow *XcbWindow::GetParent() { return parent_; }

void XcbWindow::SetParent(INativeWindow *parent) {
  parent_ = CheckPlatform<XcbWindow>(parent, GetPlatformId());
  if (xcb_window_) {
    DoSetParent(*xcb_window_);
  }
  application_->XcbFlush();
}

WindowStyleFlag XcbWindow::GetStyleFlag() { return style_; }

void XcbWindow::SetStyleFlag(WindowStyleFlag flag) {
  style_ = flag;
  if (xcb_window_) {
    DoSetStyleFlags(*xcb_window_);
  }
  application_->XcbFlush();
}

std::string XcbWindow::GetTitle() { return title_; }

void XcbWindow::SetTitle(std::string title) {
  title_ = std::move(title);
  if (xcb_window_) {
    DoSetTitle(*xcb_window_);
  }
  application_->XcbFlush();
}

namespace {
constexpr int WithdrawnState = 0;
constexpr int NormalState = 1;
constexpr int IconicState = 3;
}  // namespace

WindowVisibilityType XcbWindow::GetVisibility() {
  if (!xcb_window_) return WindowVisibilityType::Hide;
  auto value = static_cast<std::uint32_t *>(
      XcbGetProperty(*xcb_window_, application_->GetXcbAtomWM_STATE(),
                     application_->GetXcbAtomWM_STATE(), 0, 1));
  if (value != nullptr && *value == IconicState) {
    return WindowVisibilityType::Minimize;
  }
  if (!mapped_) return WindowVisibilityType::Hide;
  return WindowVisibilityType::Show;
}

void XcbWindow::SetVisibility(WindowVisibilityType visibility) {
  auto update_wm_state = [this, visibility] {
    auto atom = application_->GetXcbAtomWM_STATE();
    auto window = *xcb_window_;

    std::uint32_t value[2];
    switch (visibility) {
      case WindowVisibilityType::Show:
        value[0] = NormalState;
        break;
      case WindowVisibilityType::Minimize:
        value[0] = IconicState;
        break;
      case WindowVisibilityType::Hide:
        value[0] = WithdrawnState;
        break;
      default:
        UnreachableCode();
    }

    auto old_value = static_cast<std::uint32_t *>(
        XcbGetProperty(*xcb_window_, atom, atom, 0, 2));
    if (old_value) value[1] = old_value[1];

    xcb_change_property(application_->GetXcbConnection(), XCB_PROP_MODE_REPLACE,
                        window, atom, atom, 32, sizeof(value) / sizeof(*value),
                        value);
  };

  switch (visibility) {
    case WindowVisibilityType::Show: {
      if (!xcb_window_) {
        DoCreateWindow();
      }
      update_wm_state();
      xcb_map_window(application_->GetXcbConnection(), *xcb_window_);
      break;
    }
    case WindowVisibilityType::Minimize: {
      if (!xcb_window_) {
        DoCreateWindow();
      }
      update_wm_state();
      xcb_unmap_window(application_->GetXcbConnection(), *xcb_window_);
      break;
    }
    case WindowVisibilityType::Hide: {
      if (!xcb_window_) {
        return;
      }
      update_wm_state();
      xcb_unmap_window(application_->GetXcbConnection(), *xcb_window_);
      break;
    }
    default:
      UnreachableCode();
  }

  application_->XcbFlush();
}

Size XcbWindow::GetClientSize() { return GetClientRect().GetSize(); }

void XcbWindow::SetClientSize(const Size &size) {
  auto rect = GetClientRect();
  SetClientRect(Rect(rect.GetLeftTop(), size));
}

Rect XcbWindow::GetClientRect() {
  if (!xcb_window_) {
    return Rect{};
  }

  auto window = *xcb_window_;

  auto cookie = xcb_get_geometry(application_->GetXcbConnection(), window);
  auto reply = FreeLater(xcb_get_geometry_reply(
      application_->GetXcbConnection(), cookie, nullptr));
  auto position = GetXcbWindowPosition(window);

  return Rect(position.x, position.y, reply->width, reply->height);
}

void XcbWindow::SetClientRect(const Rect &rect) {
  if (!xcb_window_) return;
  DoSetClientRect(*xcb_window_, rect);
  application_->XcbFlush();
}

Rect XcbWindow::GetWindowRect() {
  if (!xcb_window_) return {};

  auto client_rect = GetClientRect();
  auto frame_properties = Get_NET_FRAME_EXTENTS(*xcb_window_);

  if (frame_properties.has_value()) {
    return client_rect.Expand(*frame_properties);
  }

  return client_rect;
}

void XcbWindow::SetWindowRect(const Rect &rect) {
  if (!xcb_window_) return;

  auto real_rect = rect;
  auto frame_properties = Get_NET_FRAME_EXTENTS(*xcb_window_);

  if (frame_properties.has_value()) {
    real_rect = real_rect.Shrink(*frame_properties, true);
  }

  SetClientRect(real_rect);
}

bool XcbWindow::RequestFocus() {
  if (!xcb_window_) return false;
  xcb_set_input_focus(application_->GetXcbConnection(), XCB_NONE, *xcb_window_,
                      XCB_CURRENT_TIME);
  application_->XcbFlush();
  return true;
}

Point XcbWindow::GetMousePosition() {
  auto window = xcb_window_.value_or(application_->GetFirstXcbScreen()->root);
  auto cookie = xcb_query_pointer(application_->GetXcbConnection(), window);
  auto reply = FreeLater(xcb_query_pointer_reply(
      application_->GetXcbConnection(), cookie, nullptr));
  return Point(reply->win_x, reply->win_y);
}

bool XcbWindow::CaptureMouse() {
  if (!xcb_window_) return false;
  // KDE handles grabbing automatically.
  //
  // xcb_grab_pointer(application_->GetXcbConnection(), TRUE, *xcb_window_, 0,
  //                  XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_WINDOW_NONE,
  //                  XCB_CURSOR_NONE, XCB_CURRENT_TIME);
  // application_->XcbFlush();
  return true;
}

bool XcbWindow::ReleaseMouse() {
  if (!xcb_window_) return false;
  // KDE handles grabbing automatically.
  //
  // xcb_ungrab_pointer(application_->xcb_connection_, XCB_CURRENT_TIME);
  // application_->XcbFlush();
  return true;
}

void XcbWindow::SetCursor(std::shared_ptr<ICursor> cursor) {
  if (!xcb_window_) return;
  auto xcb_cursor = CheckPlatform<XcbCursor>(cursor, GetPlatformId());
  cursor_ = xcb_cursor;
  DoSetCursor(*xcb_window_, xcb_cursor.get());
}

void XcbWindow::SetToForeground() {
  SetVisibility(WindowVisibilityType::Show);
  assert(xcb_window_.has_value());
  const static uint32_t values[] = {XCB_STACK_MODE_ABOVE};
  xcb_configure_window(application_->GetXcbConnection(), *xcb_window_,
                       XCB_CONFIG_WINDOW_STACK_MODE, values);
  application_->XcbFlush();
}

void XcbWindow::RequestRepaint() {
  if (!xcb_window_.has_value()) return;
  CRU_LOG_TAG_DEBUG("{:#x} Repaint requested.", *xcb_window_);
  // TODO: true throttle
  repaint_canceler_.Reset(
      application_->SetImmediate([this] { paint_event_.Raise(nullptr); }));
}

std::unique_ptr<graphics::IPainter> XcbWindow::BeginPaint() {
  if (!xcb_window_.has_value()) {
    return std::make_unique<graphics::NullPainter>();
  }

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

IInputMethodContext *XcbWindow::GetInputMethodContext() {
  return input_method_;
}

std::optional<xcb_window_t> XcbWindow::GetXcbWindow() { return xcb_window_; }

XcbUiApplication *XcbWindow::GetXcbUiApplication() { return application_; }

bool XcbWindow::HasFocus() {
  if (!xcb_window_) return false;
  auto cookie = xcb_get_input_focus(application_->GetXcbConnection());
  auto focus = xcb_get_input_focus_reply(application_->GetXcbConnection(),
                                         cookie, nullptr);
  return focus->focus == *xcb_window_;
}

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

  xcb_window_ = window;

  std::vector<xcb_atom_t> wm_protocols{
      application_->GetXcbAtomWM_DELETE_WINDOW()};
  xcb_change_property(application_->GetXcbConnection(), XCB_PROP_MODE_REPLACE,
                      window, application_->GetXcbAtomWM_PROTOCOLS(),
                      XCB_ATOM_ATOM, 32, wm_protocols.size(),
                      wm_protocols.data());

  DoSetStyleFlags(window);
  DoSetParent(window);
  DoSetCursor(window, cursor_.get());

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
  resize_event_.Raise(current_size_);
  RequestRepaint();

  return window;
}

void XcbWindow::HandleEvent(xcb_generic_event_t *event) {
  switch (event->response_type & ~0x80) {
    case XCB_EXPOSE: {
      RequestRepaint();
      break;
    }
    case XCB_DESTROY_NOTIFY: {
      destroy_event_.Raise(nullptr);

      cairo_surface_destroy(cairo_surface_);
      cairo_surface_ = nullptr;
      xcb_window_ = std::nullopt;

      if (application_->IsQuitOnAllWindowClosed() &&
          std::ranges::none_of(
              application_->GetAllWindow(),
              [](INativeWindow *window) { return window->IsCreated(); })) {
        application_->RequestQuit(0);
      }

      break;
    }
    case XCB_CONFIGURE_NOTIFY: {
      xcb_configure_notify_event_t *configure =
          (xcb_configure_notify_event_t *)event;
      auto width = configure->width, height = configure->height;
      if (width != current_size_.width || height != current_size_.height) {
        CRU_LOG_TAG_DEBUG("{:#x} Size changed {} x {}.", *xcb_window_, width,
                          height);
        current_size_ = Size(width, height);
        assert(cairo_surface_);
        cairo_xcb_surface_set_size(cairo_surface_, width, height);
        resize_event_.Raise(current_size_);
      }
      break;
    }
    case XCB_MAP_NOTIFY: {
      visibility_change_event_.Raise(WindowVisibilityType::Show);
      mapped_ = true;
      break;
    }
    case XCB_UNMAP_NOTIFY: {
      visibility_change_event_.Raise(WindowVisibilityType::Hide);
      mapped_ = false;
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
                                       ConvertModifiersOfEvent(bp->state),
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
                                      ConvertModifiersOfEvent(bp->state));
      mouse_down_event_.Raise(std::move(args));
      break;
    }
    case XCB_BUTTON_RELEASE: {
      xcb_button_release_event_t *br = (xcb_button_release_event_t *)event;
      NativeMouseButtonEventArgs args(ConvertMouseButton(br->detail),
                                      Point(br->event_x, br->event_y),
                                      ConvertModifiersOfEvent(br->state));
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
                              ConvertModifiersOfEvent(kp->state));
      key_down_event_.Raise(std::move(args));
      break;
    }
    case XCB_KEY_RELEASE: {
      xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;
      NativeKeyEventArgs args(XorgKeycodeToCruKeyCode(application_, kr->detail),
                              ConvertModifiersOfEvent(kr->state));
      key_up_event_.Raise(std::move(args));
      break;
    }
    case XCB_CLIENT_MESSAGE: {
      xcb_client_message_event_t *cm = (xcb_client_message_event_t *)event;
      if (cm->data.data32[0] == application_->GetXcbAtomWM_DELETE_WINDOW() &&
          xcb_window_.has_value()) {
        xcb_destroy_window(application_->GetXcbConnection(),
                           xcb_window_.value());
        xcb_flush(application_->GetXcbConnection());
      }
      break;
    }
    default:
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
      return destroy->window;
    }
    case XCB_CONFIGURE_NOTIFY: {
      xcb_configure_notify_event_t *configure =
          (xcb_configure_notify_event_t *)event;
      return configure->window;
    }
    case XCB_MAP_NOTIFY: {
      xcb_map_notify_event_t *map = (xcb_map_notify_event_t *)event;
      return map->window;
    }
    case XCB_UNMAP_NOTIFY: {
      xcb_unmap_notify_event_t *unmap = (xcb_unmap_notify_event_t *)event;
      return unmap->window;
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
    case XCB_CLIENT_MESSAGE: {
      xcb_client_message_event_t *cm = (xcb_client_message_event_t *)event;
      return cm->window;
    }
    default:
      return std::nullopt;
  }
}

void XcbWindow::DoSetParent(xcb_window_t window) {
  auto real_parent =
      application_->GetFirstXcbScreen()->root;  // init to desktop
  if (parent_ && parent_->xcb_window_) {
    real_parent = *parent_->xcb_window_;
  }
  xcb_reparent_window(application_->GetXcbConnection(), window, real_parent, 0,
                      0);
  // TODO: Maybe restore position?
}

void XcbWindow::DoSetStyleFlags(xcb_window_t window) {
  std::vector<xcb_atom_t> atoms;
  if (style_.Has(WindowStyleFlags::NoCaptionAndBorder)) {
    atoms = {application_->GetXcbAtom_NET_WM_WINDOW_TYPE_UTILITY()};
  } else {
    atoms = {application_->GetXcbAtom_NET_WM_WINDOW_TYPE_NORMAL()};
  }

  xcb_change_property(application_->GetXcbConnection(), XCB_PROP_MODE_REPLACE,
                      window, application_->GetXcbAtom_NET_WM_WINDOW_TYPE(),
                      XCB_ATOM_ATOM, 32, atoms.size(), atoms.data());
}

void XcbWindow::DoSetTitle(xcb_window_t window) {
  for (auto atom : {static_cast<xcb_atom_t>(XCB_ATOM_WM_NAME),
                    application_->GetXcbAtom_NET_WM_NAME()}) {
    xcb_change_property(application_->GetXcbConnection(), XCB_PROP_MODE_REPLACE,
                        window, atom, XCB_ATOM_STRING, 8, title_.size(),
                        title_.data());
  }
}

void XcbWindow::DoSetClientRect(xcb_window_t window, const Rect &rect) {
  auto tree_cookie = xcb_query_tree(application_->GetXcbConnection(), window);
  auto tree_reply = FreeLater(xcb_query_tree_reply(
      application_->GetXcbConnection(), tree_cookie, nullptr));
  auto parent_position = GetXcbWindowPosition(tree_reply->parent);

  std::uint32_t values[4]{
      static_cast<std::uint32_t>(rect.left - parent_position.x),
      static_cast<std::uint32_t>(rect.top - parent_position.y),
      static_cast<std::uint32_t>(rect.width),
      static_cast<std::uint32_t>(rect.height)};
  xcb_configure_window(application_->GetXcbConnection(), window,
                       XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                           XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                       values);
}

void XcbWindow::DoSetCursor(xcb_window_t window, XcbCursor *cursor) {
  std::uint32_t values[]{cursor ? cursor->GetXcbCursor() : XCB_CURSOR_NONE};
  xcb_change_window_attributes(application_->GetXcbConnection(), window,
                               XCB_CW_CURSOR, values);
  application_->XcbFlush();
}

void *XcbWindow::XcbGetProperty(xcb_window_t window, xcb_atom_t property,
                                xcb_atom_t type, std::uint32_t offset,
                                std::uint32_t length,
                                std::uint32_t *out_length) {
  auto cookie = xcb_get_property(application_->GetXcbConnection(), false,
                                 window, property, type, offset, length);
  auto reply = FreeLater(
      xcb_get_property_reply(application_->GetXcbConnection(), cookie, NULL));
  if (reply->type == XCB_ATOM_NONE) {
    return nullptr;
  }
  if (out_length != nullptr) {
    *out_length = xcb_get_property_value_length(reply);
  }
  return xcb_get_property_value(reply);
}

std::optional<Thickness> XcbWindow::Get_NET_FRAME_EXTENTS(xcb_window_t window) {
  auto frame_properties = static_cast<std::uint32_t *>(
      XcbGetProperty(window, application_->GetXcbAtom_NET_FRAME_EXTENTS(),
                     XCB_ATOM_CARDINAL, 0, 4));

  if (frame_properties == nullptr) {
    return std::nullopt;
  }

  return Thickness(frame_properties[0], frame_properties[2],
                   frame_properties[1], frame_properties[3]);
}

Point XcbWindow::GetXcbWindowPosition(xcb_window_t window) {
  Point result;

  while (true) {
    auto cookie = xcb_get_geometry(application_->GetXcbConnection(), window);
    auto reply = FreeLater(xcb_get_geometry_reply(
        application_->GetXcbConnection(), cookie, nullptr));
    result.x += reply->x;
    result.y += reply->y;

    auto tree_cookie = xcb_query_tree(application_->GetXcbConnection(), window);
    auto tree_reply = FreeLater(xcb_query_tree_reply(
        application_->GetXcbConnection(), tree_cookie, nullptr));
    window = tree_reply->parent;
    // TODO: Multi-screen offset?
    if (tree_reply->root == window || window == XCB_WINDOW_NONE) {
      break;
    }
  }

  return result;
}

}  // namespace cru::platform::gui::xcb
