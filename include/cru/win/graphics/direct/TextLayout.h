#pragma once
#include "ComResource.h"
#include "Resource.h"

#include "cru/platform/graphics/TextLayout.h"

#include <limits>
#include <memory>

namespace cru::platform::graphics::win::direct {
class DWriteFont;

class CRU_WIN_GRAPHICS_DIRECT_API DWriteTextLayout
    : public DirectGraphicsResource,
      public virtual ITextLayout,
      public virtual IComResource<IDWriteTextLayout> {
 public:
  DWriteTextLayout(DirectGraphicsFactory* factory, std::shared_ptr<IFont> font,
                   String text);

  CRU_DELETE_COPY(DWriteTextLayout)
  CRU_DELETE_MOVE(DWriteTextLayout)

  ~DWriteTextLayout() override;

 public:
  IDWriteTextLayout* GetComInterface() const override {
    return text_layout_.Get();
  }

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
  float GetLineHeight(Index line_index) override;
  Index GetLineCount() override;

  Rect GetTextBounds(bool includingTrailingSpace = false) override;
  // Return empty vector if text_range.count is 0. Text range could be in
  // reverse direction, it should be normalized first in implementation.
  std::vector<Rect> TextRangeRect(const TextRange& text_range) override;
  Rect TextSinglePoint(Index position, bool trailing) override;
  TextHitTestResult HitTest(const Point& point) override;

 private:
  bool edit_mode_ = false;
  String text_;
  std::shared_ptr<DWriteFont> font_;
  float max_width_ = std::numeric_limits<float>::max();
  float max_height_ = std::numeric_limits<float>::max();
  Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;
};
}  // namespace cru::platform::graphics::win::direct
