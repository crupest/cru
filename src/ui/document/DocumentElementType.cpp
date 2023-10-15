#include "cru/ui/document/DocumentElementType.h"

#include <utility>

namespace cru::ui::document {
DocumentElementType::DocumentElementType(String name)
    : name_(std::move(name)) {}

DocumentElementType::~DocumentElementType() {}

DocumentElementType* const DocumentElementTypes::kRootElementType =
    new DocumentElementType(u"Root");
}  // namespace cru::ui::document
