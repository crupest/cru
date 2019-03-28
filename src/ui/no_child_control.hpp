#pragma once
#include "pre.hpp"

#include "control.hpp"

namespace cru::ui {
class NoChildControl : public Control {
 private:
  static const std::vector<Control*> empty_control_vector;

 protected:
  NoChildControl() = default;

 public:
  NoChildControl(const NoChildControl& other) = delete;
  NoChildControl(NoChildControl&& other) = delete;
  NoChildControl& operator=(const NoChildControl& other) = delete;
  NoChildControl& operator=(NoChildControl&& other) = delete;
  ~NoChildControl() override = default;

 protected:
  const std::vector<Control*>& GetChildren() const override final {
    return empty_control_vector;
  }
};
}  // namespace cru::ui
