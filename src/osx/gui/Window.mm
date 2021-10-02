#include "cru/osx/gui/Window.hpp"

#include "cru/osx/graphics/quartz/Painter.hpp"
#include "cru/osx/gui/UiApplication.hpp"

#include <AppKit/NSGraphicsContext.h>
#include <AppKit/NSWindow.h>
#include <memory>

namespace cru::platform::gui::osx {
namespace details {
class OsxWindowPrivate {
  friend OsxWindow;

 public:
  explicit OsxWindowPrivate(OsxWindow* osx_window) : osx_window_(osx_window) {}

  CRU_DELETE_COPY(OsxWindowPrivate)
  CRU_DELETE_MOVE(OsxWindowPrivate)

  ~OsxWindowPrivate() = default;

 private:
  OsxWindow* osx_window_;

  INativeWindow* parent_;

  bool frame_;
  Rect content_rect_;

  NSWindow* window_;
};
}

namespace {
inline NSWindowStyleMask CalcWindowStyleMask(bool frame) {
  return frame ? NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                     NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
               : NSWindowStyleMaskBorderless;
}

}

OsxWindow::OsxWindow(OsxUiApplication* ui_application, INativeWindow* parent, bool frame)
    : OsxGuiResource(ui_application), p_(new details::OsxWindowPrivate(this)) {
  p_->parent_ = parent;

  p_->frame_ = frame;
  p_->content_rect_ = {100, 100, 400, 200};
}

OsxWindow::~OsxWindow() {
  if (p_->window_) {
    [p_->window_ close];
  }
}

void OsxWindow::Close() {
  if (p_->window_) {
    [p_->window_ close];
  }
}

INativeWindow* OsxWindow::GetParent() { return p_->parent_; }

bool OsxWindow::IsVisible() {
  if (!p_->window_) return false;
  return [p_->window_ isVisible];
}

void OsxWindow::SetVisible(bool is_visible) {
  if (p_->window_) {
    if (is_visible) {
      [p_->window_ orderFront:p_->window_];
    } else {
      [p_->window_ orderOut:p_->window_];
    }
  } else {
    if (is_visible) {
      CreateWindow();
    }
  }
}

Size OsxWindow::GetClientSize() { return p_->content_rect_.GetSize(); }

void OsxWindow::SetClientSize(const Size& size) {
  if (p_->window_) {
    [p_->window_ setContentSize:NSSize{size.width, size.height}];
  } else {
    p_->content_rect_.SetSize(size);
  }
}

Rect OsxWindow::GetWindowRect() {
  if (p_->window_) {
    auto r = [p_->window_ frame];
    return Rect(r.origin.x, r.origin.y, r.size.width, r.size.height);
  } else {
    NSRect rr{p_->content_rect_.left, p_->content_rect_.top, p_->content_rect_.width,
              p_->content_rect_.height};
    auto r = [NSWindow frameRectForContentRect:rr styleMask:CalcWindowStyleMask(p_->frame_)];
    return Rect(r.origin.x, r.origin.y, r.size.width, r.size.height);
  }
}

void OsxWindow::SetWindowRect(const Rect& rect) {
  auto rr = NSRect{rect.left, rect.top, rect.width, rect.height};
  if (p_->window_) {
    [p_->window_ setFrame:rr display:false];
  } else {
    auto r = [NSWindow contentRectForFrameRect:rr styleMask:CalcWindowStyleMask(p_->frame_)];
    p_->content_rect_ = Rect(r.origin.x, r.origin.y, r.size.width, r.size.height);
  }
}

std::unique_ptr<graphics::IPainter> OsxWindow::BeginPaint() {
  NSGraphicsContext* ns_graphics_context =
      [NSGraphicsContext graphicsContextWithWindow:p_->window_];

  CGContextRef cg_context = [ns_graphics_context CGContext];

  return std::make_unique<cru::platform::graphics::osx::quartz::QuartzCGContextPainter>(
      GetUiApplication()->GetGraphicsFactory(), cg_context, true, GetClientSize());
}

void OsxWindow::CreateWindow() {
  NSRect content_rect{p_->content_rect_.left, p_->content_rect_.top, p_->content_rect_.width,
                      p_->content_rect_.height};

  NSWindowStyleMask style_mask = CalcWindowStyleMask(p_->frame_);

  p_->window_ = [[NSWindow alloc] initWithContentRect:content_rect
                                            styleMask:style_mask
                                              backing:NSBackingStoreBuffered
                                                defer:false];
}
}
