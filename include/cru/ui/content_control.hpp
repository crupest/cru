#pragma once
#include "control.hpp"

namespace cru::ui {
class ContentControl : public Control {
 protected:
  ContentControl();

 public:
  ContentControl(const ContentControl& other) = delete;
  ContentControl(ContentControl&& other) = delete;
  ContentControl& operator=(const ContentControl& other) = delete;
  ContentControl& operator=(ContentControl&& other) = delete;
  ~ContentControl() override;

  const std::vector<Control*>& GetChildren() const override final {
    return child_vector_;
  }
  Control* GetChild() const { return child_; }
  void SetChild(Control* child);

 protected:
  virtual void OnChildChanged(Control* old_child, Control* new_child);

 private:
  std::vector<Control*> child_vector_;
  Control*& child_;
};
}  // namespace cru::ui
