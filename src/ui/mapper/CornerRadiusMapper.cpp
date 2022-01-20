#include "cru/ui/mapper/CornerRadiusMapper.hpp"
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/mapper/PointMapper.hpp"

namespace cru::ui::mapper {
bool CornerRadiusMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  if (node->GetTag() == u"CornerRadius") {
    return true;
  }
  return false;
}

std::unique_ptr<CornerRadius> CornerRadiusMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto point_mapper = MapperRegistry::GetInstance()->GetMapper<Point>();

  auto result = std::make_unique<CornerRadius>();

  auto all = node->GetOptionalAttribute(u"all");
  if (all) {
    result->SetAll(*point_mapper->MapFromString(*all));
  }

  auto lefttop = node->GetOptionalAttribute(u"lefttop");
  if (lefttop) {
    result->left_top = *point_mapper->MapFromString(*lefttop);
  }

  auto righttop = node->GetOptionalAttribute(u"righttop");
  if (righttop) {
    result->right_top = *point_mapper->MapFromString(*righttop);
  }

  auto rightbottom = node->GetOptionalAttribute(u"rightbottom");
  if (rightbottom) {
    result->right_bottom = *point_mapper->MapFromString(*rightbottom);
  }

  auto leftbottom = node->GetOptionalAttribute(u"leftbottom");
  if (leftbottom) {
    result->left_bottom = *point_mapper->MapFromString(*leftbottom);
  }

  return result;
}
}  // namespace cru::ui::mapper
