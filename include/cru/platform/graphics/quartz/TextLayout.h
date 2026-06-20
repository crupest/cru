#pragma once
#include "Base.h"
#include "Font.h"

#include <cru/base/platform/osx/Base.h>
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

  CTFrameRef GetCTFrameRef() const { return ct_frame_.Get(); }

  CFWrapper<CTFrameRef> CreateFrameWithColor(const Color& color);

  Matrix GetTransform() { return transform_; }

  std::string GetDebugString() override;

 private:
  void DoSetText(std::string text);

  void ReleaseResource();
  void RecreateFrame();

  CGRect DoGetTextBounds(bool includingTrailingSpace = false);

  /**
   * @brief Get the rects of the text range. The rects are in the coordinate system of the text
   * layout, that is, left bottom is (0, 0).
   * @param text_range The text range to get the rects, indexed by code units.
   * @param includingTrailingSpace Whether to include the trailing space in the rects.
   * @return The rects of the text range.
   */
  std::vector<CGRect> DoTextRangeRect(const TextRange& text_range, bool includingTrailingSpace);

  /**
   * @brief Get the rect of a single point in the text layout. The rect is in the coordinate system
   * of the text layout, that is, left bottom is (0, 0).
   * @param position The position of the point, indexed by code units.
   * @param trailing Whether the point is at the trailing of the character.
   * @return The rect of the single point. Width is always 0.
   */
  CGRect DoTextSinglePoint(Index position, bool trailing);

  CGRect GetTextBoundsForEmptyString();

  double MeasureHeightOfOneLine();

 private:
  float max_width_;
  float max_height_;

  bool edit_mode_;

  std::shared_ptr<OsxCTFont> font_;

  std::string text_;

  CFMutableAttributedStringRef cf_attributed_text_;
  CFWrapper<CTFramesetterRef> ct_framesetter_;
  float suggest_height_;
  CFWrapper<CTFrameRef> ct_frame_;

  double height_of_one_line_;

  // Just for cache and debug.
  struct Line {
    CTLineRef ct_line;
    CFRange text_range;
    CGPoint origin;
    double ascent;
    double descent;
    double leading;
    double width;
    double trailing_whitespace_width;
  };
  std::vector<Line> lines_;
  CGRect text_bounds_without_trailing_space_;
  CGRect text_bounds_with_trailing_space_;

  Matrix transform_;
};
}  // namespace cru::platform::graphics::quartz
