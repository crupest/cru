#pragma once
#include "../UiApplication.h"
#include "Base.h"

#include <cru/base/platform/unix/EventLoop.h>
#include <cru/platform/graphics/cairo/CairoGraphicsFactory.h>

#include <xcb/xcb.h>
#include <functional>
#include <string>
#include <unordered_map>

namespace cru::platform::gui::xcb {
class XcbWindow;
class XcbCursorManager;

class XcbUiApplication : public XcbResource, public virtual IUiApplication {
  friend XcbWindow;

 public:
  explicit XcbUiApplication(
      graphics::cairo::CairoGraphicsFactory* cairo_factory = nullptr);
  ~XcbUiApplication();

 public:
  graphics::cairo::CairoGraphicsFactory* GetCairoFactory();

  void CheckXcbConnectionError();
  xcb_connection_t* GetXcbConnection();

  // This API is weird, but before we have correct screen API, we still use it.
  xcb_screen_t* GetFirstXcbScreen();

  xcb_atom_t GetOrCreateXcbAtom(std::string name);

#define CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(name) \
  xcb_atom_t GetXcbAtom##name() { return GetOrCreateXcbAtom(#name); }

  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(WM_NAME)
  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(WM_STATE)
  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(_NET_WM_NAME)
  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(_NET_WM_WINDOW_TYPE)
  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(_NET_WM_WINDOW_TYPE_NORMAL)
  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(_NET_WM_WINDOW_TYPE_UTILITY)
  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(_NET_FRAME_EXTENTS)
  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(WM_PROTOCOLS)
  CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM(WM_DELETE_WINDOW)

#undef CRU_XCB_UI_APPLICATION_DEFINE_XCB_ATOM

 public:
  int Run() override;

  void RequestQuit(int quit_code) override;

  void AddOnQuitHandler(std::function<void()> handler) override;

  bool IsQuitOnAllWindowClosed() override;
  void SetQuitOnAllWindowClosed(bool quit_on_all_window_closed) override;

  long long SetImmediate(std::function<void()> action) override;
  long long SetTimeout(std::chrono::milliseconds milliseconds,
                       std::function<void()> action) override;
  long long SetInterval(std::chrono::milliseconds milliseconds,
                        std::function<void()> action) override;
  void CancelTimer(long long id) override;

  std::vector<INativeWindow*> GetAllWindow() override;

  INativeWindow* CreateWindow() override;

  cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() override;

  ICursorManager* GetCursorManager() override;

  IClipboard* GetClipboard() override;

  // If return nullptr, it means the menu is not supported.
  IMenu* GetApplicationMenu() override;

 private:
  void HandleXEvents();

  void RegisterWindow(XcbWindow* window);
  void UnregisterWindow(XcbWindow* window);

 private:
  graphics::cairo::CairoGraphicsFactory* cairo_factory_;
  bool release_cairo_factory_;

  xcb_connection_t* xcb_connection_;
  xcb_screen_t* screen_;
  std::unordered_map<std::string, xcb_atom_t> xcb_atom_;

  cru::platform::unix::UnixEventLoop event_loop_;
  std::vector<std::function<void()>> quit_handlers_;

  bool is_quit_on_all_window_closed_;
  std::vector<XcbWindow*> windows_;

  XcbCursorManager* cursor_manager_;
};
}  // namespace cru::platform::gui::xcb
