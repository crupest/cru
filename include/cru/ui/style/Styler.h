#pragma once
#include "../Base.h"
#include "ApplyBorderStyleInfo.h"
#include "cru/common/Base.h"
#include "cru/common/ClonablePtr.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/ui/controls/Control.h"

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

  static ClonablePtr<CompoundStyler> Create(
      std::vector<ClonablePtr<Styler>> stylers) {
    return ClonablePtr<CompoundStyler>(new CompoundStyler(std::move(stylers)));
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
  static ClonablePtr<BorderStyler> Create() {
    return ClonablePtr<BorderStyler>(new BorderStyler());
  }

  static ClonablePtr<BorderStyler> Create(ApplyBorderStyleInfo style) {
    return ClonablePtr<BorderStyler>(new BorderStyler(std::move(style)));
  }

  BorderStyler() = default;
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
