#pragma once
#include "ComResource.hpp"
#include "Resource.hpp"

#include "cru/platform/graphics/TextLayout.hpp"

#include <limits>
#include <memory>

namespace cru::platform::graphics::win::direct {
class DWriteFont;

class DWriteTextLayout : public DirectGraphResource,
                         public virtual ITextLayout,
                         public virtual IComResource<IDWriteTextLayout> {
 public:
  DWriteTextLayout(DirectGraphFactory* factory, std::shared_ptr<IFont> font,
                   std::u16string text);

  CRU_DELETE_COPY(DWriteTextLayout)
  CRU_DELETE_MOVE(DWriteTextLayout)

  ~DWriteTextLayout() override;

 public:
  IDWriteTextLayout* GetComInterface() const override {
    return text_layout_.Get();
  }

 public:
  std::u16string GetText() override;
  std::u16string_view GetTextView() override;
  void SetText(std::u16string new_text) override;

  std::shared_ptr<IFont> GetFont() override;
  void SetFont(std::shared_ptr<IFont> font) override;

  void SetMaxWidth(float max_width) override;
  void SetMaxHeight(float max_height) override;

  Rect GetTextBounds(bool includingTrailingSpace = false) override;
  // Return empty vector if text_range.count is 0. Text range could be in
  // reverse direction, it should be normalized first in implementation.
  std::vector<Rect> TextRangeRect(const TextRange& text_range) override;
  Point TextSinglePoint(Index position, bool trailing) override;
  TextHitTestResult HitTest(const Point& point) override;

 private:
  std::u16string text_;
  std::shared_ptr<DWriteFont> font_;
  float max_width_ = std::numeric_limits<float>::max();
  float max_height_ = std::numeric_limits<float>::max();
  Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;
};
}  // namespace cru::platform::graphics::win::direct
