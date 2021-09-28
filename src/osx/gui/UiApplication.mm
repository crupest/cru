#include "cru/osx/gui/UiApplication.hpp"

#include <AppKit/NSApplication.h>
#include <algorithm>

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
};

void OsxUiApplicationPrivate::CallQuitHandlers() {
  for (const auto& handler : quit_handlers_) {
    handler();
  }
}
}

OsxUiApplication::OsxUiApplication() : OsxGuiResource(this) {}

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

}

@implementation AppDelegate
cru::platform::gui::osx::details::OsxUiApplicationPrivate* _p;

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
