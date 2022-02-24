#pragma once
#include "../Base.h"
#include "ApplyBorderStyleInfo.h"
#include "cru/common/ClonablePtr.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/ui/render/MeasureRequirement.h"

#include <memory>
#include <vector>

namespace cru::ui::style {
class CRU_UI_API Styler : public Object {
 public:
  virtual void Apply(controls::Control* control) const = 0;

  virtual Styler* Clone() const = 0;
};

class CRU_UI_API CompoundStyler : public Styler {
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

  std::vector<ClonablePtr<Styler>> GetChildren() const { return stylers_; }

  virtual CompoundStyler* Clone() const override {
    return new CompoundStyler(stylers_);
  }

 private:
  std::vector<ClonablePtr<Styler>> stylers_;
};

class CRU_UI_API BorderStyler : public Styler {
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

  ApplyBorderStyleInfo GetBorderStyle() const { return style_; }

  BorderStyler* Clone() const override { return new BorderStyler(style_); }

 private:
  ApplyBorderStyleInfo style_;
};

class CRU_UI_API CursorStyler : public Styler {
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

  std::shared_ptr<platform::gui::ICursor> GetCursor() const { return cursor_; }

 private:
  std::shared_ptr<platform::gui::ICursor> cursor_;
};

class CRU_UI_API PreferredSizeStyler : public Styler {
 public:
  static ClonablePtr<PreferredSizeStyler> Create(render::MeasureSize size) {
    return ClonablePtr<PreferredSizeStyler>(new PreferredSizeStyler(size));
  }

  explicit PreferredSizeStyler(render::MeasureSize size) : size_(size) {}

  void Apply(controls::Control* control) const override;

  PreferredSizeStyler* Clone() const override {
    return new PreferredSizeStyler(size_);
  }

  render::MeasureSize GetPreferredSize() const { return size_; }

 private:
  render::MeasureSize size_;
};
}  // namespace cru::ui::style
