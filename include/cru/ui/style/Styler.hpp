#pragma once
#include "../Base.hpp"
#include "cru/common/Base.hpp"

#include <optional>

namespace cru::ui::style {
class Styler : public Object {
 public:
  virtual void Apply(controls::Control* control) const;
};

}  // namespace cru::ui::style
