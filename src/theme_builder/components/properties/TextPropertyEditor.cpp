#include "TextPropertyEditor.h"

namespace cru::theme_builder::components::properties {
TextPropertyEditor::TextPropertyEditor() {
  editor_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text_view = editor_.GetTextView();
    String error_message;
    auto validation_result = Validate(text_view, &error_message);
    if (validation_result) {
      OnTextChanged(text_view);
    }
  });
}

TextPropertyEditor::~TextPropertyEditor() { container_.RemoveFromParent(); }

bool TextPropertyEditor::Validate(StringView text, String* error_message) {
  return true;
}

void TextPropertyEditor::OnTextChanged(StringView text) {}
}  // namespace cru::theme_builder::components::properties
