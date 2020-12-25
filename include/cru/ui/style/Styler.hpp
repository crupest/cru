#pragma once
#include "../Base.hpp"
#include "ApplyBorderStyleInfo.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/ui/controls/Control.hpp"

#include <memory>
#include <vector>

namespace cru::ui::style {
class Styler : public Object {
 public:
  virtual void Apply(controls::Control* control) const = 0;

  virtual Styler* Clone() const = 0;
};

class CompoundStyler : public Styler {
 public:
  template <typename... S>
  static ClonablePtr<CompoundStyler> Create(ClonablePtr<S>... s) {
    return ClonablePtr<CompoundStyler>(
        new CompoundStyler(std::vector<ClonablePtr<Styler>>{std::move(s)...}));
  }

  explicit CompoundStyler(std::vector<ClonablePtr<Styler>> stylers)
      : stylers_(std::move(stylers)) {}

  void Apply(controls::Control* control) const override {
    for (const auto& styler : stylers_) {
      styler->Apply(control);
    }
  }

  virtual CompoundStyler* Clone() const override {
    return new CompoundStyler(stylers_);
  }

 private:
  std::vector<ClonablePtr<Styler>> stylers_;
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

class CursorStyler : public Styler {
 public:
  static ClonablePtr<CursorStyler> Create(
      std::shared_ptr<platform::gui::ICursor> cursor) {
    return ClonablePtr<CursorStyler>(new CursorStyler(std::move(cursor)));
  }

  static ClonablePtr<CursorStyler> Create(platform::gui::SystemCursorType type);

  explicit CursorStyler(std::shared_ptr<platform::gui::ICursor> cursor)
      : cursor_(std::move(cursor)) {}

  void Apply(controls::Control* control) const override;

  CursorStyler* Clone() const override { return new CursorStyler(cursor_); }

 private:
  std::shared_ptr<platform::gui::ICursor> cursor_;
};
}  // namespace cru::ui::style
