#pragma once
#include "Component.h"
#include "cru/common/Base.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/Popup.h"
#include "cru/ui/controls/TextBlock.h"

#include <functional>
#include <vector>

namespace cru::ui::components {
class CRU_UI_API MenuItem : public Component {
 public:
  MenuItem();
  explicit MenuItem(String text);

  CRU_DELETE_COPY(MenuItem)
  CRU_DELETE_MOVE(MenuItem)

  ~MenuItem();

 public:
  controls::Control* GetRootControl() override { return &container_; }

  void SetText(String text);

  void SetOnClick(std::function<void()> on_click) {
    on_click_ = std::move(on_click);
  }

 private:
  controls::Button container_;
  controls::TextBlock text_;
  std::function<void()> on_click_;
};

class CRU_UI_API Menu : public Component {
 public:
  Menu();

  CRU_DELETE_COPY(Menu)
  CRU_DELETE_MOVE(Menu)

  ~Menu();

 public:
  controls::Control* GetRootControl() override { return &container_; }

  gsl::index GetItemCount() const {
    return static_cast<gsl::index>(items_.size());
  }

  void AddItem(Component* component) { AddItem(component, GetItemCount()); }
  void AddItem(Component* component, gsl::index index);
  Component* RemoveItem(gsl::index index);
  void ClearItems();

  void AddTextItem(String text, std::function<void()> on_click) {
    AddTextItem(std::move(text), GetItemCount(), std::move(on_click));
  }
  void AddTextItem(String text, Index index, std::function<void()> on_click);

  void SetOnItemClick(std::function<void(Index)> on_item_click) {
    on_item_click_ = std::move(on_item_click);
  }

 private:
  controls::FlexLayout container_;
  std::vector<Component*> items_;

  std::function<void(Index)> on_item_click_;
};

class CRU_UI_API PopupMenu : public Component {
 public:
  explicit PopupMenu(controls::Control* attached_control = nullptr);

  CRU_DELETE_COPY(PopupMenu)
  CRU_DELETE_MOVE(PopupMenu)

  ~PopupMenu();

 public:
  controls::Control* GetRootControl() override;

  controls::Popup* GetPopup() { return &popup_; }
  Menu* GetMenu() { return &menu_; }

  // position relative to screen left top.
  void SetPosition(const Point& position);
  void Show();
  // position relative to screen left top.
  void Show(const Point& position) {
    SetPosition(position);
    Show();
  }
  void Close();

 private:
  controls::Control* attached_control_;

  controls::Popup popup_;
  Menu menu_;
};
}  // namespace cru::ui::components
