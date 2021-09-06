#pragma once
#include <memory>
#include "Resource.hpp"

#include "Font.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/graphics/TextLayout.hpp"

namespace cru::platform::graphics::osx::quartz {
class OsxCTTextLayout : public OsxQuartzResource, public virtual ITextLayout {
 public:
  OsxCTTextLayout(IGraphFactory* graphics_factory,
                  std::shared_ptr<OsxCTFont> font, const String& str);

  CRU_DELETE_COPY(OsxCTTextLayout)
  CRU_DELETE_MOVE(OsxCTTextLayout)

  ~OsxCTTextLayout() override;

 public:
  std::u16string GetText() override;
  std::u16string_view GetTextView() override;
  void SetText(std::u16string new_text) override;

  std::shared_ptr<IFont> GetFont() override;
  void SetFont(std::shared_ptr<IFont> font) override;

  void SetMaxWidth(float max_width) override;
  void SetMaxHeight(float max_height) override;

  Rect GetTextBounds(bool includingTrailingSpace = false) override;
  std::vector<Rect> TextRangeRect(const TextRange& text_range) override;
  Point TextSinglePoint(Index position, bool trailing) override;
  TextHitTestResult HitTest(const Point& point) override;

 private:
  std::shared_ptr<OsxCTFont> font_;

  CTFramesetterRef ct_framesetter_;
  CTFrameRef ct_frame_;
};
}  // namespace cru::platform::graphics::osx::quartz
