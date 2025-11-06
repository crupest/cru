#pragma once
#include "Base.h"

#include <cru/platform/graphics/TextLayout.h>

#include <limits>
#include <memory>

namespace cru::platform::graphics::direct2d {
class DWriteFont;

class CRU_WIN_GRAPHICS_DIRECT_API DWriteTextLayout
    : public DirectGraphicsResource,
      public virtual ITextLayout,
      public virtual IComResource<IDWriteTextLayout> {
 public:
  DWriteTextLayout(DirectGraphicsFactory* factory, std::shared_ptr<IFont> font,
                   std::string text);

  ~DWriteTextLayout() override;

 public:
  IDWriteTextLayout* GetComInterface() const override {
    return text_layout_.Get();
  }

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
  std::string text_;
  std::wstring utf16_text_;
  std::shared_ptr<DWriteFont> font_;
  float max_width_ = std::numeric_limits<float>::max();
  float max_height_ = std::numeric_limits<float>::max();
  Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;
};
}  // namespace cru::platform::graphics::direct2d
