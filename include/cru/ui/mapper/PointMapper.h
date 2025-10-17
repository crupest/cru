#pragma once
#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API PointMapper : public BasicMapper<Point> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(PointMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  Point DoMapFromString(std::string str) override;
  Point DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
