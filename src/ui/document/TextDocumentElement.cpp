#include "cru/ui/document/TextDocumentElement.h"
#include "cru/common/String.h"
#include "cru/ui/document/DocumentElement.h"
#include "cru/ui/document/DocumentElementType.h"

namespace cru::ui::document {
TextDocumentElement::TextDocumentElement(String text, TextStyle style,
                                         IDocumentLink* link)
    : DocumentElement(DocumentElementTypes::kTextElementType),
      text_(std::move(text)),
      style_(style),
      link_(link) {}

TextDocumentElement::~TextDocumentElement() {}

void TextDocumentElement::SetText(String text) { text_ = std::move(text); }
}  // namespace cru::ui::document
