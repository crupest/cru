#pragma once
#include "../no_child_control.hpp"

#include <memory>

namespace cru::ui::render {
class TextRenderObject;
}

namespace cru::ui::controls {
class TextBlock : public NoChildControl {
 public:
  static constexpr auto control_type = L"TextBlock";

  static TextBlock* Create() { return new TextBlock(); }

 protected:
  TextBlock();

 public:
  TextBlock(const TextBlock& other) = delete;
  TextBlock(TextBlock&& other) = delete;
  TextBlock& operator=(const TextBlock& other) = delete;
  TextBlock& operator=(TextBlock&& other) = delete;
  ~TextBlock() override = default;

  std::wstring_view GetControlType() const override final {
    return control_type;
  }

  render::RenderObject* GetRenderObject() const override;

  std::wstring GetText() const;
  void SetText(std::wstring text);

 private:
  std::shared_ptr<render::TextRenderObject> render_object_;
};
}  // namespace cru::ui::controls
