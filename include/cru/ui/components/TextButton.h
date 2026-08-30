#pragma once
#include "../Base.h"
#include "../controls/Button.h"
#include "../controls/TextBlock.h"
#include "Component.h"

namespace cru::ui::components {
class CRU_UI_API TextButton : public Component {
 public:
  TextButton() : TextButton("") {}
  explicit TextButton(std::string text);

  controls::Control* GetRootControl() override { return &button_; }

  std::string GetText() { return text_.GetText(); }
  void SetText(std::string text) { text_.SetText(std::move(text)); }

  controls::Button* GetButton() { return &button_; }
  controls::TextBlock* GetTextBlock() { return &text_; }

  IEvent<const helper::ClickEventArgs&>* ClickEvent() {
    return button_.ClickEvent();
  }

 private:
  controls::Button button_;
  controls::TextBlock text_;
};
}  // namespace cru::ui::components
