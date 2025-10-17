#include "cru/ui/mapper/Mapper.h"
#include "cru/base/StringUtil.h"

#include <typeindex>

namespace cru::ui::mapper {
MapperBase::MapperBase(std::type_index type_index)
    : type_index_(std::move(type_index)) {}

bool MapperBase::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  for (const auto& tag : allowed_tags_) {
    if (cru::string::CaseInsensitiveCompare(node->GetTag(), tag.ToUtf8()) ==
        0) {
      return true;
    }
  }
  return false;
}
}  // namespace cru::ui::mapper
