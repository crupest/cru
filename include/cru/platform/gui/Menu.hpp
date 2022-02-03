#pragma once
#include "Base.hpp"

#include <functional>

namespace cru::platform::gui {
struct IMenu;

struct CRU_PLATFORM_GUI_API IMenuItem : virtual IPlatformResource {
  virtual String GetTitle() = 0;
  virtual void SetTitle(String title) = 0;
  virtual bool IsEnabled() = 0;
  virtual void SetEnabled(bool enabled) = 0;
  virtual IMenu* GetParentMenu() = 0;
  virtual IMenu* GetSubmenu() = 0;
  virtual void SetOnClickHandler(std::function<void()> handler) = 0;
};

struct CRU_PLATFORM_GUI_API IMenu : virtual IPlatformResource {
  virtual IMenuItem* GetItemAt(int index) = 0;
  virtual int GetItemCount() = 0;
  virtual IMenuItem* CreateItemAt(int index) = 0;
  virtual void RemoveItemAt(int index) = 0;

  virtual std::vector<IMenuItem*> GetItems();
  bool Empty() { return GetItemCount() == 0; }
};
}  // namespace cru::platform::gui
