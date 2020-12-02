#pragma once
#include "../Base.hpp"
#include "ApplyBorderStyleInfo.hpp"
#include "cru/common/Base.hpp"

namespace cru::ui::style {
class Styler : public Object {
 public:
  virtual void Apply(controls::Control* control) const;
};

class BorderStyler : public Styler {
 public:
  explicit BorderStyler(ApplyBorderStyleInfo style);

  void Apply(controls::Control* control) const override;

 private:
  ApplyBorderStyleInfo style_;
};
}  // namespace cru::ui::style
