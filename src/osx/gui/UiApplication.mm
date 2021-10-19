#include "cru/osx/gui/UiApplication.hpp"

#include "cru/common/Logger.hpp"
#include "cru/osx/graphics/quartz/Factory.hpp"
#include "cru/osx/gui/Cursor.hpp"
#include "cru/osx/gui/Window.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/platform/gui/Window.hpp"

#include <AppKit/NSApplication.h>
#include <Foundation/NSRunLoop.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <vector>

@interface AppDelegate : NSObject <NSApplicationDelegate>
- (id)init:(cru::platform::gui::osx::details::OsxUiApplicationPrivate*)p;
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender;
- (void)applicationWillTerminate:(NSNotification*)notification;
@end

namespace cru::platform::gui::osx {

namespace details {
class OsxUiApplicationPrivate {
  friend OsxUiApplication;

 public:
  explicit OsxUiApplicationPrivate(OsxUiApplication* osx_ui_application)
      : osx_ui_application_(osx_ui_application) {
    app_delegate_ = [[AppDelegate alloc] init:this];
  }

  CRU_DELETE_COPY(OsxUiApplicationPrivate)
  CRU_DELETE_MOVE(OsxUiApplicationPrivate)

  ~OsxUiApplicationPrivate() = default;

  void CallQuitHandlers();

 private:
  OsxUiApplication* osx_ui_application_;
  AppDelegate* app_delegate_;
  std::vector<std::function<void()>> quit_handlers_;

  long long current_timer_id_ = 1;
  std::unordered_map<long long, std::function<void()>> next_tick_;
  std::unordered_map<long long, NSTimer*> timers_;

  std::vector<OsxWindow*> windows_;

  std::unique_ptr<OsxCursorManager> cursor_manager_;

  std::unique_ptr<platform::graphics::osx::quartz::QuartzGraphicsFactory> quartz_graphics_factory_;
};

void OsxUiApplicationPrivate::CallQuitHandlers() {
  for (const auto& handler : quit_handlers_) {
    handler();
  }
}
}

OsxUiApplication::OsxUiApplication()
    : OsxGuiResource(this), p_(new details::OsxUiApplicationPrivate(this)) {
  // Add stdio logger.
  log::Logger::GetInstance()->AddSource(std::make_unique<log::StdioLogSource>());

  [NSApp setDelegate:p_->app_delegate_];
  p_->quartz_graphics_factory_ = std::make_unique<graphics::osx::quartz::QuartzGraphicsFactory>();
  p_->cursor_manager_ = std::make_unique<OsxCursorManager>(this);
}

OsxUiApplication::~OsxUiApplication() {}

int OsxUiApplication::Run() {
  [NSApp run];
  return 0;
}

void OsxUiApplication::RequestQuit(int quit_code) {
  [NSApp terminate:[NSNumber numberWithInteger:quit_code]];
}

void OsxUiApplication::AddOnQuitHandler(std::function<void()> handler) {
  p_->quit_handlers_.push_back(std::move(handler));
}

long long OsxUiApplication::SetImmediate(std::function<void()> action) {
  const long long id = p_->current_timer_id_++;
  p_->next_tick_.emplace(id, std::move(action));

  [[NSRunLoop mainRunLoop] performBlock:^{
    const auto i = p_->next_tick_.find(id);
    if (i != p_->next_tick_.cend()) {
      i->second();
    }
    p_->next_tick_.erase(i);
  }];

  return id;
}

long long OsxUiApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                       std::function<void()> action) {
  long long id = p_->current_timer_id_++;
  p_->timers_.emplace(id, [NSTimer scheduledTimerWithTimeInterval:milliseconds.count() / 1000.0
                                                          repeats:false
                                                            block:^(NSTimer* timer) {
                                                              action();
                                                              p_->timers_.erase(id);
                                                            }]);

  return id;
}

long long OsxUiApplication::SetInterval(std::chrono::milliseconds milliseconds,
                                        std::function<void()> action) {
  long long id = p_->current_timer_id_++;
  p_->timers_.emplace(id, [NSTimer scheduledTimerWithTimeInterval:milliseconds.count() / 1000.0
                                                          repeats:true
                                                            block:^(NSTimer* timer) {
                                                              action();
                                                            }]);

  return id;
}

void OsxUiApplication::CancelTimer(long long id) {
  p_->next_tick_.erase(id);
  auto i = p_->timers_.find(id);
  if (i != p_->timers_.cend()) {
    [i->second invalidate];
    p_->timers_.erase(i);
  }
}

std::vector<INativeWindow*> OsxUiApplication::GetAllWindow() {
  std::vector<INativeWindow*> result;
  std::transform(p_->windows_.cbegin(), p_->windows_.cend(), std::back_inserter(result),
                 [](OsxWindow* w) { return static_cast<INativeWindow*>(w); });
  return result;
}

INativeWindow* OsxUiApplication::CreateWindow(INativeWindow* parent, CreateWindowFlag flags) {
  auto window = new OsxWindow(this, parent, !(flags & CreateWindowFlags::NoCaptionAndBorder));
  p_->windows_.push_back(window);
  return window;
}

ICursorManager* OsxUiApplication::GetCursorManager() { return p_->cursor_manager_.get(); }

graphics::IGraphicsFactory* OsxUiApplication::GetGraphicsFactory() {
  return p_->quartz_graphics_factory_.get();
}

void OsxUiApplication::UnregisterWindow(OsxWindow* window) {
  p_->windows_.erase(
      std::remove(p_->windows_.begin(), p_->windows_.end(), static_cast<INativeWindow*>(window)),
      p_->windows_.cend());
}
}

@implementation AppDelegate {
  cru::platform::gui::osx::details::OsxUiApplicationPrivate* _p;
}

- (id)init:(cru::platform::gui::osx::details::OsxUiApplicationPrivate*)p {
  _p = p;
  return self;
}
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender {
  return NSApplicationTerminateReply::NSTerminateNow;
}
- (void)applicationWillTerminate:(NSNotification*)notification {
  _p->CallQuitHandlers();
}
@end
