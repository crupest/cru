#pragma once
#include "com_resource.hpp"
#include "direct_factory.hpp"
#include "platform_id.hpp"

#include "cru/platform/graph/text_layout.hpp"

#include "font.hpp"

#include <memory>

namespace cru::platform::graph::win::direct {
class DWriteTextLayout : public TextLayout,
                         public IComResource<IDWriteTextLayout> {
 public:
  explicit DWriteTextLayout(IDirectFactory* factory, std::shared_ptr<Font> font,
                            std::wstring text);

  DWriteTextLayout(const DWriteTextLayout& other) = delete;
  DWriteTextLayout& operator=(const DWriteTextLayout& other) = delete;

  DWriteTextLayout(DWriteTextLayout&& other) = delete;
  DWriteTextLayout& operator=(DWriteTextLayout&& other) = delete;

  ~DWriteTextLayout() override = default;

  CRU_PLATFORMID_IMPLEMENT_DIRECT

 public:
  IDWriteTextLayout* GetComInterface() const override {
    return text_layout_.Get();
  }

 public:
  std::wstring GetText() override;
  void SetText(std::wstring new_text) override;

  std::shared_ptr<Font> GetFont();
  void SetFont(std::shared_ptr<Font> font);

  void SetMaxWidth(float max_width) override;
  void SetMaxHeight(float max_height) override;

  Rect GetTextBounds() override;
  std::vector<Rect> TextRangeRect(const TextRange& text_range) override;

 private:
  IDirectFactory* factory_;
  std::wstring text_;
  std::shared_ptr<DWriteFont> font_;
  float max_width_ = 0.0f;
  float max_height_ = 0.0f;
  Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;
};
}  // namespace cru::platform::graph::win::direct
