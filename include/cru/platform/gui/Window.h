#pragma once
#include "Base.h"

#include "Input.h"

#include <cru/base/Event.h>
#include <cru/platform/GraphicsBase.h>
#include <cru/platform/graphics/Painter.h>

namespace cru::platform::gui {
class ICursor;
class IInputMethodContext;

namespace details {
struct WindowStyleFlagTag;
}

using WindowStyleFlag = Bitmask<details::WindowStyleFlagTag>;

struct WindowStyleFlags {
  static constexpr WindowStyleFlag NoCaptionAndBorder =
      WindowStyleFlag::FromOffset(1);
};

enum class WindowVisibilityType { Show, Hide, Minimize };

struct NativePaintEventArgs {
  Rect repaint_area;
};

enum class FocusChangeType { Gain, Lose };

enum class MouseEnterLeaveType { Enter, Leave };

struct NativeMouseButtonEventArgs {
  MouseButton button;
  Point point;
  KeyModifier modifier;
};

struct NativeMouseWheelEventArgs {
  // Positive means down. Negative means up.
  float delta;
  Point point;
  KeyModifier modifier;
  bool horizontal;  // true if horizontal wheel.
};

struct NativeKeyEventArgs {
  KeyCode key;
  KeyModifier modifier;
};

/**
 * Window is created when it is about to show. So the lifetime of the window is
 * not the same as the INativeWindow instance. And INativeWindow may re-create
 * the window if old window is destroyed and re-show is requested.
 *
 * Many methods are only meaningful when window is created or visible.
 */
struct CRU_PLATFORM_GUI_API INativeWindow : virtual IPlatformResource {
  virtual bool IsCreated();
  virtual void Close() = 0;

  virtual INativeWindow* GetParent() = 0;
  virtual void SetParent(INativeWindow* parent) = 0;

  virtual WindowStyleFlag GetStyleFlag() = 0;
  virtual void SetStyleFlag(WindowStyleFlag flag) = 0;

  virtual std::string GetTitle() = 0;
  virtual void SetTitle(std::string title) = 0;

  virtual WindowVisibilityType GetVisibility() = 0;
  virtual void SetVisibility(WindowVisibilityType visibility) = 0;

  /**
   * Implementation may return empty if window is not created or not visible.
   */
  virtual Size GetClientSize() = 0;

  /**
   * Implementation may do nothing if window is not created or not visible. So
   * call this method after window is shown.
   */
  virtual void SetClientSize(const Size& size) = 0;

  /**
   * The lefttop point of the rect is relative to screen lefttop.
   *
   * Implementation may return empty if window is not created or not visible.
   */
  virtual Rect GetClientRect() = 0;

  /**
   * The lefttop point of the rect is relative to screen lefttop.
   *
   * Implementation may do nothing if window is not created or not visible. So
   * call this method after window is shown.
   */
  virtual void SetClientRect(const Rect& rect) = 0;

  /**
   * The lefttop point of the rect is relative to screen lefttop.
   *
   * Implementation may return empty if window is not created or not visible.
   */
  virtual Rect GetWindowRect() = 0;

  /**
   * The lefttop point of the rect is relative to screen lefttop.
   *
   * Implementation may do nothing if window is not created or not visible. So
   * call this method after window is shown.
   */
  virtual void SetWindowRect(const Rect& rect) = 0;

  // Return true if window gained the focus. But the return value should be
  // ignored, since it does not guarantee anything.
  virtual bool RequestFocus() = 0;

  // Relative to client lefttop.
  virtual Point GetMousePosition() = 0;

  virtual bool CaptureMouse() = 0;
  virtual bool ReleaseMouse() = 0;

  virtual void SetCursor(std::shared_ptr<ICursor> cursor) = 0;

  virtual void SetToForeground() = 0;

  virtual void RequestRepaint() = 0;

  // Remember to call EndDraw on return value and destroy it.
  virtual std::unique_ptr<graphics::IPainter> BeginPaint() = 0;

  virtual IEvent<std::nullptr_t>* CreateEvent() = 0;
  virtual IEvent<std::nullptr_t>* DestroyEvent() = 0;
  virtual IEvent<std::nullptr_t>* PaintEvent() = 0;
  virtual IEvent<const NativePaintEventArgs&>* Paint1Event();

  virtual IEvent<WindowVisibilityType>* VisibilityChangeEvent() = 0;
  virtual IEvent<const Size&>* ResizeEvent() = 0;
  virtual IEvent<FocusChangeType>* FocusEvent() = 0;

  virtual IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() = 0;
  virtual IEvent<const Point&>* MouseMoveEvent() = 0;
  virtual IEvent<const NativeMouseButtonEventArgs&>* MouseDownEvent() = 0;
  virtual IEvent<const NativeMouseButtonEventArgs&>* MouseUpEvent() = 0;
  virtual IEvent<const NativeMouseWheelEventArgs&>* MouseWheelEvent() = 0;
  virtual IEvent<const NativeKeyEventArgs&>* KeyDownEvent() = 0;
  virtual IEvent<const NativeKeyEventArgs&>* KeyUpEvent() = 0;

  virtual IInputMethodContext* GetInputMethodContext() = 0;
};
}  // namespace cru::platform::gui

#define CRU_DEFINE_CRU_PLATFORM_GUI_I_NATIVE_WINDOW_OVERRIDE_EVENTS()     \
  CRU_DEFINE_EVENT_OVERRIDE(Create, std::nullptr_t)                       \
  CRU_DEFINE_EVENT_OVERRIDE(Destroy, std::nullptr_t)                      \
  CRU_DEFINE_EVENT_OVERRIDE(Paint, std::nullptr_t)                        \
  CRU_DEFINE_EVENT_OVERRIDE(Paint1, const NativePaintEventArgs&)          \
  CRU_DEFINE_EVENT_OVERRIDE(VisibilityChange, WindowVisibilityType)       \
  CRU_DEFINE_EVENT_OVERRIDE(Resize, const Size&)                          \
  CRU_DEFINE_EVENT_OVERRIDE(Focus, FocusChangeType)                       \
  CRU_DEFINE_EVENT_OVERRIDE(MouseEnterLeave, MouseEnterLeaveType)         \
  CRU_DEFINE_EVENT_OVERRIDE(MouseMove, const Point&)                      \
  CRU_DEFINE_EVENT_OVERRIDE(MouseDown, const NativeMouseButtonEventArgs&) \
  CRU_DEFINE_EVENT_OVERRIDE(MouseUp, const NativeMouseButtonEventArgs&)   \
  CRU_DEFINE_EVENT_OVERRIDE(MouseWheel, const NativeMouseWheelEventArgs&) \
  CRU_DEFINE_EVENT_OVERRIDE(KeyDown, const NativeKeyEventArgs&)           \
  CRU_DEFINE_EVENT_OVERRIDE(KeyUp, const NativeKeyEventArgs&)
