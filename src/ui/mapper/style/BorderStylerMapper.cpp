#include "cru/ui/mapper/style/BorderStylerMapper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"
#include "cru/ui/style/Styler.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
using cru::ui::style::ApplyBorderStyleInfo;
using cru::ui::style::BorderStyler;

bool BorderStylerMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"BorderStyler");
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
