#pragma once
#include "win_pre_config.hpp"

#include "../text_layout.hpp"
#include "win_font.hpp"

#include <memory>

namespace cru::platform::win {
class GraphManager;

class WinTextLayout : public Object, public virtual TextLayout {
 public:
  explicit WinTextLayout(GraphManager* graph_manager,
                         std::shared_ptr<WinFontDescriptor> font, std::wstring text);
  WinTextLayout(const WinTextLayout& other) = delete;
  WinTextLayout(WinTextLayout&& other) = delete;
  WinTextLayout& operator=(const WinTextLayout& other) = delete;
  WinTextLayout& operator=(WinTextLayout&& other) = delete;
  ~WinTextLayout() override;

  std::wstring GetText() override;
  void SetText(std::wstring new_text) override;
  std::shared_ptr<FontDescriptor> GetFont();
  void SetFont(std::shared_ptr<FontDescriptor> font);
  void SetMaxWidth(float max_width) override;
  void SetMaxHeight(float max_height) override;
  ui::Rect GetTextBounds() override;
  std::vector<ui::Rect> TextRangeRect(
      const ui::TextRange& text_range) override;

  IDWriteTextLayout* GetDWriteTextLayout() const { return text_layout_.Get(); }

 private:
  GraphManager* graph_manager_;
  std::wstring text_;
  std::shared_ptr<WinFontDescriptor> font_descriptor_;
  float max_width_ = 0.0f;
  float max_height_ = 0.0f;
  Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;
};
}  // namespace cru::platform::win
