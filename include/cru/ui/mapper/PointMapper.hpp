#pragma once
#include "Mapper.hpp"

namespace cru::ui::mapper {
class CRU_UI_API PointMapper : public BasicMapper<Point> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(PointMapper)
  CRU_DELETE_COPY(PointMapper)
  CRU_DELETE_MOVE(PointMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::unique_ptr<Point> DoMapFromString(String str) override;
  std::unique_ptr<Point> DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
