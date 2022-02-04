#pragma once
#include "Resource.hpp"

#include "cru/platform/gui/Menu.hpp"

namespace cru::platform::gui::osx {
namespace details {
struct OsxMenuItemPrivate;
struct OsxMenuPrivate;
}  // namespace details

class OsxMenu;

class OsxMenuItem : public OsxGuiResource, public virtual IMenuItem {
  friend OsxMenu;
  friend details::OsxMenuPrivate;

 private:
  explicit OsxMenuItem(IUiApplication* ui_application);

 public:
  CRU_DELETE_COPY(OsxMenuItem)
  CRU_DELETE_MOVE(OsxMenuItem)

  ~OsxMenuItem() override;

 public:
  String GetTitle() override;
  void SetTitle(String title) override;
  bool IsEnabled() override;
  void SetEnabled(bool enabled) override;
  IMenu* GetParentMenu() override;
  IMenu* GetSubmenu() override;
  void SetKeyboardShortcut(KeyCode key, KeyModifier modifiers) override;
  void DeleteKeyboardShortcut() override;
  void SetOnClickHandler(std::function<void()> handler) override;

 private:
  details::OsxMenuItemPrivate* p_;
};

class OsxMenu : public OsxGuiResource, public virtual IMenu {
  friend OsxMenuItem;
  friend details::OsxMenuPrivate;
  friend details::OsxMenuItemPrivate;

 private:
  explicit OsxMenu(IUiApplication* ui_application);

 public:
  static OsxMenu* CreateOrGetApplicationMenu(IUiApplication* ui_application);

  CRU_DELETE_COPY(OsxMenu)
  CRU_DELETE_MOVE(OsxMenu)

  ~OsxMenu() override;

 public:
  IMenuItem* GetItemAt(int index) override;
  int GetItemCount() override;
  IMenuItem* CreateItemAt(int index) override;
  void RemoveItemAt(int index) override;

 private:
  details::OsxMenuPrivate* p_;
};
}  // namespace cru::platform::gui::osx
