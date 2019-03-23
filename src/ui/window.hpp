#pragma once
#include "pre.hpp"

#include <Windows.h>
#include <map>
#include <memory>

#include "content_control.hpp"
#include "events/ui_event.hpp"
#include "events/window_event.hpp"
#include "window_class.hpp"

namespace cru::graph {
class WindowRenderTarget;
}

namespace cru::ui::render {
class WindowRenderObject;
}

namespace cru::ui {
class WindowManager : public Object {
 public:
  static WindowManager* GetInstance();

 private:
  WindowManager();

 public:
  WindowManager(const WindowManager& other) = delete;
  WindowManager(WindowManager&& other) = delete;
  WindowManager& operator=(const WindowManager& other) = delete;
  WindowManager& operator=(WindowManager&& other) = delete;
  ~WindowManager() override = default;

  // Get the general window class for creating ordinary window.
  WindowClass* GetGeneralWindowClass() const {
    return general_window_class_.get();
  }

  // Register a window newly created.
  // This function adds the hwnd to hwnd-window map.
  // It should be called immediately after a window was created.
  void RegisterWindow(HWND hwnd, Window* window);

  // Unregister a window that is going to be destroyed.
  // This function removes the hwnd from the hwnd-window map.
  // It should be called immediately before a window is going to be destroyed,
  void UnregisterWindow(HWND hwnd);

  // Return a pointer to the Window object related to the HWND or nullptr if the
  // hwnd is not in the map.
  Window* FromHandle(HWND hwnd);

  std::vector<Window*> GetAllWindows() const;

 private:
  std::unique_ptr<WindowClass> general_window_class_;
  std::map<HWND, Window*> window_map_;
};

class Window final : public ContentControl {
  friend class WindowManager;

 public:
  static constexpr auto control_type = L"Window";

 public:
  static Window* CreateOverlapped();
  static Window* CreatePopup(Window* parent, bool caption = false);

 private:
  struct tag_overlapped_constructor {};
  struct tag_popup_constructor {};

  explicit Window(tag_overlapped_constructor);
  Window(tag_popup_constructor, Window* parent, bool caption);

  void BeforeCreateHwnd();
  void AfterCreateHwnd(WindowManager* window_manager);

 public:
  Window(const Window& other) = delete;
  Window(Window&& other) = delete;
  Window& operator=(const Window& other) = delete;
  Window& operator=(Window&& other) = delete;
  ~Window() override;

 public:
  StringView GetControlType() const override final;

  render::RenderObject* GetRenderObject() const override;

  void SetDeleteThisOnDestroy(bool value);

  //*************** region: handle ***************

  // Get the handle of the window. Return null if window is invalid.
  HWND GetWindowHandle() const { return hwnd_; }

  // Return if the window is still valid, that is, hasn't been closed or
  // destroyed.
  bool IsWindowValid() const { return hwnd_ != nullptr; }

  //*************** region: window operations ***************

  Window* GetParentWindow() const { return parent_window_; }

  // Close and destroy the window if the window is valid.
  void Close();

  // Send a repaint message to the window's message queue which may make the
  // window repaint.
  void InvalidateDraw();

  // Show the window.
  void Show();

  // Hide thw window.
  void Hide();

  // Get the client size.
  Size GetClientSize();

  // Set the client size and repaint.
  void SetClientSize(const Size& size);

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  Rect GetWindowRect();

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  void SetWindowRect(const Rect& rect);

  // Set the lefttop of the window relative to screen.
  void SetWindowPosition(const Point& position);

  Point PointToScreen(const Point& point);

  Point PointFromScreen(const Point& point);

  // Handle the raw window message.
  // Return true if the message is handled and get the result through "result"
  // argument. Return false if the message is not handled.
  bool HandleWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param,
                           LRESULT& result);

  //*************** region: mouse ***************

  Point GetMousePosition();

  Control* GetMouseHoverControl() const { return mouse_hover_control_; }

  //*************** region: focus ***************

  // Request focus for specified control.
  bool RequestFocusFor(Control* control);

  // Get the control that has focus.
  Control* GetFocusControl();

  //*************** region: mouse capture ***************

  Control* CaptureMouseFor(Control* control);
  Control* ReleaseCurrentMouseCapture();

 public:
  //*************** region: events ***************
  Event<events::UiEventArgs> activated_event;
  Event<events::UiEventArgs> deactivated_event;

  Event<events::WindowNativeMessageEventArgs> native_message_event;

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  Control* HitTest(const Point& point);

  //*************** region: native operations ***************

  // Get the client rect in pixel.
  RECT GetClientRectPixel();

  bool IsMessageInQueue(UINT message);

  void SetCursorInternal(HCURSOR cursor);

  //*************** region: native messages ***************

  void OnDestroyInternal();
  void OnPaintInternal();
  void OnResizeInternal(int new_width, int new_height);

  void OnSetFocusInternal();
  void OnKillFocusInternal();

  void OnMouseMoveInternal(POINT point);
  void OnMouseLeaveInternal();
  void OnMouseDownInternal(MouseButton button, POINT point);
  void OnMouseUpInternal(MouseButton button, POINT point);

  void OnMouseWheelInternal(short delta, POINT point);
  void OnKeyDownInternal(int virtual_code);
  void OnKeyUpInternal(int virtual_code);
  void OnCharInternal(wchar_t c);

  void OnActivatedInternal();
  void OnDeactivatedInternal();

  //*************** region: event dispatcher helper ***************

  void DispatchMouseHoverControlChangeEvent(Control* old_control,
                                            Control* new_control,
                                            const Point& point);

 private:
  bool delete_this_on_destroy_ = true;

  HWND hwnd_ = nullptr;
  Window* parent_window_ = nullptr;
  std::shared_ptr<graph::WindowRenderTarget> render_target_{};

  render::WindowRenderObject* render_object_;

  Control* mouse_hover_control_ = nullptr;

  bool window_focus_ = false;
  Control* focus_control_ = this;  // "focus_control_" can't be nullptr
  Control* mouse_capture_control_ = nullptr;
};
}  // namespace cru::ui
