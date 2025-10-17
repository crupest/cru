#include "cru/ui/mapper/style/BorderStylerMapper.h"
#include "cru/base/ClonablePtr.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/style/ApplyBorderStyleInfo.h"
#include "cru/ui/style/Styler.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
using cru::ui::style::ApplyBorderStyleInfo;
using cru::ui::style::BorderStyler;

bool BorderStylerMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "BorderStyler") == 0;
}

ClonablePtr<BorderStyler> BorderStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto border_style_mapper =
      MapperRegistry::GetInstance()->GetMapper<ApplyBorderStyleInfo>();

  ApplyBorderStyleInfo border_style;

  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlElementNode::Type::Element) {
      auto child_element = child->AsElement();
      if (border_style_mapper->XmlElementIsOfThisType(child_element)) {
        border_style = border_style_mapper->MapFromXml(child_element);
      }
    }
  }

  return BorderStyler::Create(std::move(border_style));
}
}  // namespace cru::ui::mapper::style
