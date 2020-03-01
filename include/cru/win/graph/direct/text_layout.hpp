#pragma once
#include "com_resource.hpp"
#include "resource.hpp"

#include "cru/platform/graph/text_layout.hpp"

#include <memory>

namespace cru::platform::graph::win::direct {
class DWriteFont;

class DWriteTextLayout : public DirectGraphResource,
                         public virtual ITextLayout,
                         public virtual IComResource<IDWriteTextLayout> {
 public:
  DWriteTextLayout(DirectGraphFactory* factory, std::shared_ptr<IFont> font,
                   std::string text);

  CRU_DELETE_COPY(DWriteTextLayout)
  CRU_DELETE_MOVE(DWriteTextLayout)

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

  Rect GetTextBounds() override;
  std::vector<Rect> TextRangeRect(const TextRange& text_range) override;
  Point TextSingleRect(int position, bool trailing) override;
  TextHitTestResult HitTest(const Point& point) override;

 private:
  std::string text_;
  std::wstring w_text_;
  std::shared_ptr<DWriteFont> font_;
  float max_width_ = 0.0f;
  float max_height_ = 0.0f;
  Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;
};
}  // namespace cru::platform::graph::win::direct
