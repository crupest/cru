#pragma once
#include "Control.hpp"

namespace cru::ui::controls {
class ContentControl : public Control {
 protected:
  ContentControl() = default;

 public:
  ContentControl(const ContentControl& other) = delete;
  ContentControl(ContentControl&& other) = delete;
  ContentControl& operator=(const ContentControl& other) = delete;
  ContentControl& operator=(ContentControl&& other) = delete;
  ~ContentControl() override = default;

  Control* GetChild() const;
  void SetChild(Control* child);

 protected:
  virtual void OnChildChanged(Control* old_child, Control* new_child);

 private:
  using Control::AddChild;
  using Control::RemoveChild;
};
}  // namespace cru::ui::controls
