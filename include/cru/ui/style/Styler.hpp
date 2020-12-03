#pragma once
#include "../Base.hpp"
#include "ApplyBorderStyleInfo.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/ClonablePtr.hpp"

#include <memory>

namespace cru::ui::style {
class Styler : public Object {
 public:
  virtual void Apply(controls::Control* control) const = 0;

  virtual Styler* Clone() const = 0;
};

class BorderStyler : public Styler {
 public:
  static ClonablePtr<BorderStyler> Create(ApplyBorderStyleInfo style) {
    return ClonablePtr<BorderStyler>(new BorderStyler(std::move(style)));
  }

  explicit BorderStyler(ApplyBorderStyleInfo style);

  void Apply(controls::Control* control) const override;

  BorderStyler* Clone() const override { return new BorderStyler(style_); }

 private:
  ApplyBorderStyleInfo style_;
};
}  // namespace cru::ui::style
