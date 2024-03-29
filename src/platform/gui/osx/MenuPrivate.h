#pragma once
#include "cru/platform/gui/osx/Menu.h"

#import <AppKit/NSMenu.h>
#import <AppKit/NSMenuItem.h>

@interface CruOsxMenuItemClickHandler : NSObject
- init:(cru::platform::gui::osx::details::OsxMenuItemPrivate*)p;
- (void)handleClick;
@end

namespace cru::platform::gui::osx {
namespace details {

class OsxMenuItemPrivate {
  friend OsxMenuItem;

 public:
  explicit OsxMenuItemPrivate(OsxMenuItem* d);

  CRU_DELETE_COPY(OsxMenuItemPrivate)
  CRU_DELETE_MOVE(OsxMenuItemPrivate)

  ~OsxMenuItemPrivate();

 public:
  NSMenuItem* GetNative() { return menu_item_; }
  void SetParentMenu(OsxMenu* menu) { parent_menu_ = menu; }
  void AttachToNative(NSMenuItem* native_menu_item, bool check_submenu);

  const std::function<void()> GetOnClickHandler() const { return on_click_handler_; }

 private:
  OsxMenuItem* d_;
  OsxMenu* parent_menu_ = nullptr;
  NSMenuItem* menu_item_ = nullptr;
  OsxMenu* sub_menu_ = nullptr;
  std::function<void()> on_click_handler_;
  CruOsxMenuItemClickHandler* handler_;
};

class OsxMenuPrivate {
  friend OsxMenu;

 public:
  explicit OsxMenuPrivate(OsxMenu* d);

  CRU_DELETE_COPY(OsxMenuPrivate)
  CRU_DELETE_MOVE(OsxMenuPrivate)

  ~OsxMenuPrivate();

 public:
  void SetParentItem(OsxMenuItem* item) { parent_item_ = item; }
  void AttachToNative(NSMenu* native_menu);

 private:
  OsxMenu* d_;
  OsxMenuItem* parent_item_ = nullptr;
  NSMenu* menu_ = nullptr;
  std::vector<OsxMenuItem*> items_;
};
}  // namespace details

}  // namespace cru::platform::gui::osx
