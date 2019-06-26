#pragma once
#include "../graphic_base.hpp"
#include "../native_resource.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cru::platform::graph {
class Font;

class TextLayout : public NativeResource {
 protected:
  TextLayout() = default;

 public:
  TextLayout(const TextLayout& other) = delete;
  TextLayout& operator=(const TextLayout& other) = delete;

  TextLayout(TextLayout&& other) = delete;
  TextLayout& operator=(TextLayout&& other) = delete;

  ~TextLayout() override = default;

 public:
  virtual std::wstring GetText() = 0;
  virtual void SetText(std::wstring new_text) = 0;

  virtual std::shared_ptr<Font> GetFont() = 0;
  virtual void SetFont(std::shared_ptr<Font> font) = 0;

  virtual void SetMaxWidth(float max_width) = 0;
  virtual void SetMaxHeight(float max_height) = 0;

  virtual Rect GetTextBounds() = 0;
  virtual std::vector<Rect> TextRangeRect(const TextRange& text_range) = 0;
};
}  // namespace cru::platform::graph
