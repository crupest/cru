#pragma once
#include "../TextLayout.h"
#include "CairoResource.h"

namespace cru::platform::graphics::cairo {
class PangoTextLayout : public CairoResource, public virtual ITextLayout {
 public:
  explicit PangoTextLayout(CairoGraphicsFactory* factory);

  ~PangoTextLayout() override;

 public:
  String GetText() override;
  void SetText(String new_text) override;

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

 private:
  String text_;
};
}  // namespace cru::platform::graphics::cairo
