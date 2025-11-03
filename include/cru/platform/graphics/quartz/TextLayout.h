#pragma once
#include "Base.h"
#include "Font.h"

#include <cru/platform/graphics/TextLayout.h>

#include <memory>

namespace cru::platform::graphics::quartz {
class OsxCTTextLayout : public OsxQuartzResource, public virtual ITextLayout {
 public:
  OsxCTTextLayout(IGraphicsFactory* graphics_factory, std::shared_ptr<OsxCTFont> font,
                  const std::string& str);
  ~OsxCTTextLayout() override;

 public:
  std::string GetText() override { return text_; }
  void SetText(std::string new_text) override;

  std::shared_ptr<IFont> GetFont() override { return font_; }
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
  Rect TextSinglePoint(Index position, bool trailing) override;
  TextHitTestResult HitTest(const Point& point) override;

  CTFrameRef GetCTFrameRef() const { return ct_frame_; }

  CTFrameRef CreateFrameWithColor(const Color& color);

  Matrix GetTransform() { return transform_; }

  std::string GetDebugString() override;

 private:
  void DoSetText(std::string text);

  void ReleaseResource();
  void RecreateFrame();

  CGRect DoGetTextBounds(bool includingTrailingSpace = false);
  CGRect DoGetTextBoundsIncludingEmptyLines(bool includingTrailingSpace = false);
  std::vector<CGRect> DoTextRangeRect(const TextRange& text_range);
  CGRect DoTextSinglePoint(Index position, bool trailing);

 private:
  float max_width_;
  float max_height_;

  bool edit_mode_;

  std::shared_ptr<OsxCTFont> font_;

  std::string text_;
  std::string actual_text_;
  CFMutableAttributedStringRef cf_attributed_text_;

  CTFramesetterRef ct_framesetter_ = nullptr;
  float suggest_height_;
  CTFrameRef ct_frame_ = nullptr;
  int line_count_;
  std::vector<CGPoint> line_origins_;
  std::vector<CTLineRef> lines_;
  std::vector<float> line_ascents_;
  std::vector<float> line_descents_;
  std::vector<float> line_heights_;
  // The empty line count in the front of the lines.
  int head_empty_line_count_;
  // The trailing empty line count in the back of the lines.
  int tail_empty_line_count_;

  // Just for cache.
  CGRect text_bounds_without_trailing_space_;
  CGRect text_bounds_with_trailing_space_;

  Matrix transform_;
};
}  // namespace cru::platform::graphics::quartz
