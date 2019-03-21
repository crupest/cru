#pragma once
#include "pre.hpp"

#include "ui/control.hpp"

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
  ~TextBlock() override;

  StringView GetControlType() const override final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  String GetText() const;
  void SetText(const String& text);

 private:
  render::TextRenderObject* render_object_;
};
}  // namespace cru::ui::controls
