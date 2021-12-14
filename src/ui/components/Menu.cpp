#include "cru/ui/components/Menu.hpp"
#include <functional>
#include "cru/platform/gui/Window.hpp"
#include "cru/ui/UiManager.hpp"
#include "cru/ui/controls/Button.hpp"
#include "cru/ui/controls/Control.hpp"
#include "cru/ui/controls/FlexLayout.hpp"
#include "cru/ui/controls/TextBlock.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/style/StyleRuleSet.hpp"

namespace cru::ui::components {
MenuItem::MenuItem() {
  container_ = controls::Button::Create();
  text_ = controls::TextBlock::Create();
  container_->SetChild(text_);
  container_->GetStyleRuleSet()->SetParent(
      &UiManager::GetInstance()->GetThemeResources()->menu_item_style);
  container_->ClickEvent()->AddHandler([this](const helper::ClickEventArgs&) {
    if (this->on_click_) this->on_click_();
  });
}

MenuItem::MenuItem(String text) : MenuItem() { SetText(std::move(text)); }

MenuItem::~MenuItem() {
  if (!container_->GetWindowHost()) {
    delete container_;
    delete text_;
  }
}

void MenuItem::SetText(String text) { text_->SetText(std::move(text)); }

Menu::Menu() {
  container_ = controls::FlexLayout::Create();
  container_->SetFlexDirection(FlexDirection::Vertical);
}

Menu::~Menu() {
  if (!container_->GetWindowHost()) {
    delete container_;
  }

  for (auto item : items_) {
    delete item;
  }
}

void Menu::AddItem(Component* item, gsl::index index) {
  Expects(index >= 0 && index <= GetItemCount());

  items_.insert(items_.cbegin() + index, item);
  container_->AddChild(item->GetRootControl(), index);
}

Component* Menu::RemoveItem(gsl::index index) {
  Expects(index >= 0 && index < GetItemCount());

  Component* item = items_[index];

  items_.erase(items_.cbegin() + index);
  container_->RemoveChild(index);

  return item;
}

void Menu::ClearItems() {
  for (auto item : items_) {
    delete item;
  }

  items_.clear();

  container_->ClearChildren();
}

void Menu::AddTextItem(String text, gsl::index index,
                       std::function<void()> on_click) {
  MenuItem* item = new MenuItem(std::move(text));
  item->SetOnClick([this, index, on_click = std::move(on_click)] {
    on_click();
    if (on_item_click_) on_item_click_(index);
  });
  AddItem(item, index);
}

PopupMenu::PopupMenu(controls::Control* attached_control)
    : attached_control_(attached_control) {
  popup_ = controls::Popup::Create(attached_control);

  menu_ = new Menu();

  menu_->SetOnItemClick([this](Index _) { this->Close(); });

  popup_->AddChild(menu_->GetRootControl(), 0);
}

PopupMenu::~PopupMenu() {
  delete menu_;

  if (!popup_->GetWindowHost()) {
    delete popup_;
  }
}

controls::Control* PopupMenu::GetRootControl() { return popup_; }

void PopupMenu::SetPosition(const Point& position) {
  popup_->GetWindowHost()->GetNativeWindow()->SetClientRect(Rect{position, {}});
}

void PopupMenu::Show() {
  popup_->GetWindowHost()->RelayoutWithSize(Size::Infinate(), true);
  popup_->GetWindowHost()->GetNativeWindow()->SetVisibility(
      platform::gui::WindowVisibilityType::Show);
}

void PopupMenu::Close() { popup_->GetWindowHost()->GetNativeWindow()->Close(); }
}  // namespace cru::ui::components
