#pragma once
#include "../Base.h"
#include "ApplyBorderStyleInfo.h"
#include <cru/CopyPtr.h>
#include "cru/graphics/Brush.h"
#include "cru/gui/Cursor.h"
#include "cru/ui/render/MeasureRequirement.h"

#include <memory>
#include <vector>

namespace cru::ui::style {
/**
 * \brief The base class for all styler implementations.
 * \remarks Styler should be immutable. And we use cru::CopyPtr to wrap it
 * in order to get both polymorphic and value semantics.
 */
class CRU_UI_API Styler : public Object {
 public:
  virtual void Apply(controls::Control* control) const = 0;

  virtual Styler* Clone() const = 0;
};

class CRU_UI_API CompoundStyler : public Styler {
 public:
  template <typename... S>
  static CopyPtr<CompoundStyler> Create(CopyPtr<S>... s) {
    return CopyPtr<CompoundStyler>(
        new CompoundStyler(std::vector<CopyPtr<Styler>>{std::move(s)...}));
  }

  static CopyPtr<CompoundStyler> Create(
      std::vector<CopyPtr<Styler>> stylers) {
    return CopyPtr<CompoundStyler>(new CompoundStyler(std::move(stylers)));
  }

  explicit CompoundStyler(std::vector<CopyPtr<Styler>> stylers)
      : stylers_(std::move(stylers)) {}

  void Apply(controls::Control* control) const override {
    for (const auto& styler : stylers_) {
      styler->Apply(control);
    }
  }

  std::vector<CopyPtr<Styler>> GetChildren() const { return stylers_; }

  virtual CompoundStyler* Clone() const override {
    return new CompoundStyler(stylers_);
  }

 private:
  std::vector<CopyPtr<Styler>> stylers_;
};

class CRU_UI_API BorderStyler : public Styler {
 public:
  static CopyPtr<BorderStyler> Create() {
    return CopyPtr<BorderStyler>(new BorderStyler());
  }

  static CopyPtr<BorderStyler> Create(ApplyBorderStyleInfo style) {
    return CopyPtr<BorderStyler>(new BorderStyler(std::move(style)));
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
  static CopyPtr<CursorStyler> Create(
      std::shared_ptr<platform::gui::ICursor> cursor) {
    return CopyPtr<CursorStyler>(new CursorStyler(std::move(cursor)));
  }

  static CopyPtr<CursorStyler> Create(platform::gui::SystemCursorType type);

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
  static CopyPtr<PreferredSizeStyler> Create(render::MeasureSize size) {
    return CopyPtr<PreferredSizeStyler>(new PreferredSizeStyler(size));
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

class CRU_UI_API MarginStyler : public Styler {
 public:
  static CopyPtr<MarginStyler> Create(const Thickness& margin) {
    return CopyPtr<MarginStyler>(new MarginStyler(margin));
  }

  explicit MarginStyler(const Thickness& margin) : margin_(margin) {}

  void Apply(controls::Control* control) const override;

  MarginStyler* Clone() const override { return new MarginStyler(margin_); }

  Thickness GetMargin() const { return margin_; }

 private:
  Thickness margin_;
};

class CRU_UI_API PaddingStyler : public Styler {
 public:
  static CopyPtr<PaddingStyler> Create(const Thickness& padding) {
    return CopyPtr<PaddingStyler>(new PaddingStyler(padding));
  }

  explicit PaddingStyler(const Thickness& padding) : padding_(padding) {}

  void Apply(controls::Control* control) const override;

  PaddingStyler* Clone() const override { return new PaddingStyler(padding_); }

  Thickness GetPadding() const { return padding_; }

 private:
  Thickness padding_;
};

class CRU_UI_API ContentBrushStyler : public Styler {
 public:
  static CopyPtr<ContentBrushStyler> Create(
      std::shared_ptr<graphics::IBrush> brush) {
    return CopyPtr<ContentBrushStyler>(
        new ContentBrushStyler(std::move(brush)));
  }

  explicit ContentBrushStyler(std::shared_ptr<graphics::IBrush> brush)
      : brush_(std::move(brush)) {}

  void Apply(controls::Control* control) const override;

  ContentBrushStyler* Clone() const override {
    return new ContentBrushStyler(brush_);
  }

  std::shared_ptr<graphics::IBrush> GetBrush() const {
    return brush_;
  }

 private:
  std::shared_ptr<graphics::IBrush> brush_;
};

class CRU_UI_API FontStyler : public Styler {
 public:
  static CopyPtr<FontStyler> Create(
      std::shared_ptr<graphics::IFont> font) {
    return CopyPtr<FontStyler>(new FontStyler(std::move(font)));
  }

  explicit FontStyler(std::shared_ptr<graphics::IFont> font)
      : font_(std::move(font)) {}

  void Apply(controls::Control* control) const override;

  FontStyler* Clone() const override { return new FontStyler(font_); }

  std::shared_ptr<graphics::IFont> GetFont() const { return font_; }

 private:
  std::shared_ptr<graphics::IFont> font_;
};
}  // namespace cru::ui::style
