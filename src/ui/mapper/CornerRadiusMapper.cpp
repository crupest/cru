#include "cru/ui/mapper/CornerRadiusMapper.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/mapper/PointMapper.h"

namespace cru::ui::mapper {
bool CornerRadiusMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveCompare(u"CornerRadius") == 0;
}

CornerRadius CornerRadiusMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto point_mapper = MapperRegistry::GetInstance()->GetMapper<Point>();
  CornerRadius result;

  auto all = node->GetOptionalAttributeValueCaseInsensitive(u"all");
  if (all) {
    result.SetAll(point_mapper->MapFromString(*all));
  }

  auto lefttop = node->GetOptionalAttributeValueCaseInsensitive(u"lefttop");
  if (lefttop) {
    result.left_top = point_mapper->MapFromString(*lefttop);
  }

  auto righttop = node->GetOptionalAttributeValueCaseInsensitive(u"righttop");
  if (righttop) {
    result.right_top = point_mapper->MapFromString(*righttop);
  }

  auto rightbottom = node->GetOptionalAttributeValueCaseInsensitive(u"rightbottom");
  if (rightbottom) {
    result.right_bottom = point_mapper->MapFromString(*rightbottom);
  }

  auto leftbottom = node->GetOptionalAttributeValueCaseInsensitive(u"leftbottom");
  if (leftbottom) {
    result.left_bottom = point_mapper->MapFromString(*leftbottom);
  }

  return result;
}
}  // namespace cru::ui::mapper
