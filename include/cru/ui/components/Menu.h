#pragma once
#include "../controls/Button.h"
#include "../controls/Control.h"
#include "../controls/FlexLayout.h"
#include "../controls/TextBlock.h"
#include "../controls/Window.h"
#include "Component.h"

#include <functional>
#include <vector>

namespace cru::ui::components {
class CRU_UI_API MenuItem : public Component {
 public:
  MenuItem();
  explicit MenuItem(std::string text);

 public:
  controls::Control* GetRootControl() override { return &container_; }

  void SetText(std::string text);

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
  ~Menu();

 public:
  controls::Control* GetRootControl() override { return &container_; }

  Index GetItemCount() { return static_cast<Index>(items_.size()); }

  void AddItem(Component* component) { AddItemAt(component, GetItemCount()); }
  void AddItemAt(Component* component, Index index);
  Component* RemoveItemAt(Index index);
  void ClearItems();

  void AddTextItem(std::string text, std::function<void()> on_click) {
    AddTextItemAt(std::move(text), GetItemCount(), std::move(on_click));
  }
  void AddTextItemAt(std::string text, Index index,
                     std::function<void()> on_click);

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
  ~PopupMenu();

 public:
  controls::Control* GetRootControl() override;

  controls::Window* GetPopup() { return popup_.get(); }
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

  std::unique_ptr<controls::Window> popup_;
  Menu menu_;
};
}  // namespace cru::ui::components
