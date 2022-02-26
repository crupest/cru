#include "cru/ui/mapper/Mapper.h"

#include <typeindex>

namespace cru::ui::mapper {
MapperBase::MapperBase(std::type_index type_index)
    : type_index_(std::move(type_index)) {}

bool MapperBase::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  for (const auto& tag : allowed_tags_) {
    if (node->GetTag().CaseInsensitiveEqual(tag)) {
      return true;
    }
  }
  return false;
}
}  // namespace cru::ui::mapper
