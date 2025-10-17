#pragma once
#include "../TextLayout.h"
#include "CairoResource.h"
#include "PangoFont.h"

#include <pango/pango.h>

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API PangoTextLayout
    : public CairoResource,
      public virtual ITextLayout {
 public:
  PangoTextLayout(CairoGraphicsFactory* factory, std::shared_ptr<IFont> font);

  ~PangoTextLayout() override;

 public:
  std::string GetText() override;
  void SetText(std::string new_text) override;

  std::shared_ptr<IFont> GetFont() override;
  void SetFont(std::shared_ptr<IFont> font) override;

  void SetMaxWidth(float max_width) override;
  void SetMaxHeight(float max_height) override;

  bool IsEditMode() override;
  void SetEditMode(bool enable) override;

  Index GetLineIndexFromCharIndex(Index char_index) override;
  Index GetLineCount() override;
  float GetLineHeight(Index line_index) override;

  Rect GetTextBounds(bool includingTrailingSpace = false) override;
  std::vector<Rect> TextRangeRect(const TextRange& text_range) override;
  // Width is always 0, height is line height.
  Rect TextSinglePoint(Index position, bool trailing) override;
  TextHitTestResult HitTest(const Point& point) override;

  PangoLayout* GetPangoLayout() { return pango_layout_; }

 private:
  std::string text_;

  bool edit_mode_ = false;

  std::shared_ptr<PangoFont> font_;

  PangoLayout* pango_layout_;
};
}  // namespace cru::platform::graphics::cairo
