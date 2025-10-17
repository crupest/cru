#include "cru/ui/document/TextDocumentElement.h"
#include "cru/ui/document/DocumentElement.h"
#include "cru/ui/document/DocumentElementType.h"

namespace cru::ui::document {
TextDocumentElement::TextDocumentElement(std::string text, TextStyle style,
                                         IDocumentLink* link)
    : DocumentElement(DocumentElementTypes::kTextElementType),
      text_(std::move(text)),
      style_(style),
      link_(link) {}

TextDocumentElement::~TextDocumentElement() {}

void TextDocumentElement::SetText(std::string text) { text_ = std::move(text); }
}  // namespace cru::ui::document
