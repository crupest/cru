#pragma once
#include "../no_child_control.hpp"

#include <memory>

namespace cru::ui::render {
class TextRenderObject;
}

namespace cru::ui::controls {
class TextBlock : public NoChildControl {
 public:
  static constexpr std::string_view control_type = "TextBlock";

  static TextBlock* Create() { return new TextBlock(); }

 protected:
  TextBlock();

 public:
  TextBlock(const TextBlock& other) = delete;
  TextBlock(TextBlock&& other) = delete;
  TextBlock& operator=(const TextBlock& other) = delete;
  TextBlock& operator=(TextBlock&& other) = delete;
  ~TextBlock() override;

  std::string_view GetControlType() const final {
    return control_type;
  }

  render::RenderObject* GetRenderObject() const override;

  std::string GetText() const;
  void SetText(std::string text);

 private:
  std::unique_ptr<render::TextRenderObject> render_object_;
};
}  // namespace cru::ui::controls
