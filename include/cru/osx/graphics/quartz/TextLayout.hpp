#pragma once
#include "Resource.hpp"

#include "Font.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/graphics/TextLayout.hpp"

#include <memory>

namespace cru::platform::graphics::osx::quartz {
class OsxCTTextLayout : public OsxQuartzResource, public virtual ITextLayout {
 public:
  OsxCTTextLayout(IGraphicsFactory* graphics_factory,
                  std::shared_ptr<OsxCTFont> font, const String& str);

  CRU_DELETE_COPY(OsxCTTextLayout)
  CRU_DELETE_MOVE(OsxCTTextLayout)

  ~OsxCTTextLayout() override;

 public:
  String GetText() override { return text_; }
  void SetText(String new_text) override;

  std::shared_ptr<IFont> GetFont() override { return font_; }
  void SetFont(std::shared_ptr<IFont> font) override;

  void SetMaxWidth(float max_width) override;
  void SetMaxHeight(float max_height) override;

  Rect GetTextBounds(bool includingTrailingSpace = false) override;
  std::vector<Rect> TextRangeRect(const TextRange& text_range) override;
  Point TextSinglePoint(Index position, bool trailing) override;
  TextHitTestResult HitTest(const Point& point) override;

  CTFrameRef GetCTFrameRef() const { return ct_frame_; }

  CTFrameRef CreateFrameWithColor(const Color& color);

 private:
  void ReleaseResource();
  void RecreateFrame();

 private:
  float max_width_;
  float max_height_;

  std::shared_ptr<OsxCTFont> font_;

  String text_;
  CFMutableAttributedStringRef cf_attributed_text_;

  CTFramesetterRef ct_framesetter_ = nullptr;
  CTFrameRef ct_frame_ = nullptr;
  int line_count_;
  std::vector<CGPoint> line_origins_;
  std::vector<CTLineRef> lines_;
};
}  // namespace cru::platform::graphics::osx::quartz
