#include "cru/ui/document/DocumentElementType.h"

#include <utility>
#include <vector>

namespace cru::ui::document {
DocumentElementType::DocumentElementType(
    String name, std::vector<DocumentElementType*> parents)
    : name_(std::move(name)), parents_(std::move(parents)) {}

DocumentElementType::~DocumentElementType() {}

DocumentElementType* const DocumentElementTypes::kBaseElementType =
    new DocumentElementType(u"Base", {});

DocumentElementType* const DocumentElementTypes::kRootElementType =
    new DocumentElementType(u"Root", {kBaseElementType});

DocumentElementType* const DocumentElementTypes::kTextElementType =
    new DocumentElementType(u"Text", {kBaseElementType});
}  // namespace cru::ui::document
