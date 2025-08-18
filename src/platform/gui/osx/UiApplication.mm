#include "cru/platform/gui/osx/UiApplication.h"

#include "ClipboardPrivate.h"
#include "cru/base/Osx.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/quartz/Factory.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/platform/gui/osx/Clipboard.h"
#include "cru/platform/gui/osx/Cursor.h"
#include "cru/platform/gui/osx/Menu.h"
#include "cru/platform/gui/osx/Window.h"

#include <AppKit/NSApplication.h>
#include <Foundation/NSRunLoop.h>
#include <UniformTypeIdentifiers/UTType.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <vector>

@interface CruAppDelegate : NSObject <NSApplicationDelegate>
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
    app_delegate_ = [[CruAppDelegate alloc] init:this];
  }

  CRU_DELETE_COPY(OsxUiApplicationPrivate)
  CRU_DELETE_MOVE(OsxUiApplicationPrivate)

  ~OsxUiApplicationPrivate() = default;

  void CallQuitHandlers();

 private:
  OsxUiApplication* osx_ui_application_;
  CruAppDelegate* app_delegate_;
  std::vector<std::function<void()>> quit_handlers_;
  bool quit_on_all_window_closed_ = true;

  long long current_timer_id_ = 1;
  std::unordered_map<long long, std::function<void()>> next_tick_;
  std::unordered_map<long long, NSTimer*> timers_;

  std::vector<OsxWindow*> windows_;

  std::unique_ptr<OsxCursorManager> cursor_manager_;

  std::unique_ptr<OsxClipboard> clipboard_;

  std::unique_ptr<platform::graphics::quartz::QuartzGraphicsFactory> quartz_graphics_factory_;
};

void OsxUiApplicationPrivate::CallQuitHandlers() {
  for (const auto& handler : quit_handlers_) {
    handler();
  }
}
}  // namespace details

OsxUiApplication::OsxUiApplication()
    : OsxGuiResource(this), p_(new details::OsxUiApplicationPrivate(this)) {
  [NSApplication sharedApplication];

  [NSApp setDelegate:p_->app_delegate_];
  p_->quartz_graphics_factory_ = std::make_unique<graphics::quartz::QuartzGraphicsFactory>();
  p_->cursor_manager_ = std::make_unique<OsxCursorManager>(this);
  p_->clipboard_ = std::make_unique<OsxClipboard>(
      this, std::make_unique<details::OsxClipboardPrivate>([NSPasteboard generalPasteboard]));
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

bool OsxUiApplication::IsQuitOnAllWindowClosed() { return p_->quit_on_all_window_closed_; }

void OsxUiApplication::SetQuitOnAllWindowClosed(bool quit_on_all_window_closed) {
  p_->quit_on_all_window_closed_ = quit_on_all_window_closed;
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

INativeWindow* OsxUiApplication::CreateWindow() {
  auto window = new OsxWindow(this);
  p_->windows_.push_back(window);
  return window;
}

ICursorManager* OsxUiApplication::GetCursorManager() { return p_->cursor_manager_.get(); }

IClipboard* OsxUiApplication::GetClipboard() { return p_->clipboard_.get(); }

IMenu* OsxUiApplication::GetApplicationMenu() { return OsxMenu::CreateOrGetApplicationMenu(this); }

graphics::IGraphicsFactory* OsxUiApplication::GetGraphicsFactory() {
  return p_->quartz_graphics_factory_.get();
}

std::optional<String> OsxUiApplication::ShowSaveDialog(SaveDialogOptions options) {
  NSSavePanel* panel = [NSSavePanel savePanel];
  [panel setTitle:(NSString*)ToCFString(options.title).ref];
  [panel setPrompt:(NSString*)ToCFString(options.prompt).ref];
  [panel setMessage:(NSString*)ToCFString(options.message).ref];

  NSMutableArray* allowed_content_types = [NSMutableArray array];

  for (const auto& file_type : options.allowed_file_types) {
    [allowed_content_types
        addObject:[UTType typeWithFilenameExtension:(NSString*)ToCFString(file_type).ref]];
  }

  [panel setAllowedContentTypes:allowed_content_types];
  [panel setAllowsOtherFileTypes:options.allow_all_file_types];

  auto model_result = [panel runModal];
  if (model_result == NSModalResponseOK) {
    return FromCFStringRef((CFStringRef)[[panel URL] path]);
  } else {
    return std::nullopt;
  }
}

std::optional<std::vector<String>> OsxUiApplication::ShowOpenDialog(OpenDialogOptions options) {
  NSOpenPanel* panel = [NSOpenPanel openPanel];
  [panel setTitle:(NSString*)ToCFString(options.title).ref];
  [panel setPrompt:(NSString*)ToCFString(options.prompt).ref];
  [panel setMessage:(NSString*)ToCFString(options.message).ref];

  NSMutableArray* allowed_content_types = [NSMutableArray array];

  for (const auto& file_type : options.allowed_file_types) {
    [allowed_content_types
        addObject:[UTType typeWithFilenameExtension:(NSString*)ToCFString(file_type).ref]];
  }

  [panel setAllowedContentTypes:allowed_content_types];
  [panel setAllowsOtherFileTypes:options.allow_all_file_types];

  [panel setCanChooseFiles:options.can_choose_files];
  [panel setCanChooseDirectories:options.can_choose_directories];
  [panel setAllowsMultipleSelection:options.allow_mulitple_selection];

  auto model_result = [panel runModal];
  if (model_result == NSModalResponseOK) {
    std::vector<String> result;
    for (NSURL* url in [panel URLs]) {
      result.push_back(FromCFStringRef((CFStringRef)[url path]));
    }
    return result;
  } else {
    return std::nullopt;
  }
}

void OsxUiApplication::UnregisterWindow(OsxWindow* window) {
  p_->windows_.erase(
      std::remove(p_->windows_.begin(), p_->windows_.end(), static_cast<INativeWindow*>(window)),
      p_->windows_.cend());
}
}  // namespace cru::platform::gui::osx

@implementation CruAppDelegate {
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
