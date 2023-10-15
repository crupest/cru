#include "cru/ui/document/DocumentElement.h"
#include "cru/ui/document/DocumentElementType.h"

namespace cru::ui::document {
DocumentElement::DocumentElement(DocumentElementType* type) : type_(type) {}

DocumentElement::~DocumentElement() {}
}  // namespace cru::ui::document
