#include "TextPropertyEditor.h"

namespace cru::theme_builder::components::properties {
TextPropertyEditor::TextPropertyEditor() {
  editor_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text_view = editor_.GetTextView();
    std::string error_message;
    auto validation_result = Validate(text_view, &error_message);
    if (validation_result) {
      OnTextChanged(text_view);
    }
  });
}

TextPropertyEditor::~TextPropertyEditor() {}

bool TextPropertyEditor::Validate(std::string_view text,
                                  std::string* error_message) {
  return true;
}

void TextPropertyEditor::OnTextChanged(std::string_view text) {}
}  // namespace cru::theme_builder::components::properties
