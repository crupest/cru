#pragma once
#include "../Base.hpp"
#include "ApplyBorderStyleInfo.hpp"
#include "cru/common/Base.hpp"

#include <memory>

namespace cru::ui::style {
class Styler : public Object {
 public:
  virtual void Apply(controls::Control* control) const;

  virtual std::unique_ptr<Styler> Clone() const = 0;
};

class BorderStyler : public Styler {
 public:
  explicit BorderStyler(ApplyBorderStyleInfo style);

  void Apply(controls::Control* control) const override;

  std::unique_ptr<Styler> Clone() const override {
    return std::make_unique<BorderStyler>(style_);
  }

 private:
  ApplyBorderStyleInfo style_;
};
}  // namespace cru::ui::style
