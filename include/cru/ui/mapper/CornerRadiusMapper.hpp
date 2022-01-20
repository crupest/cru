#pragma once

#include "Mapper.hpp"

namespace cru::ui::mapper {
class CRU_UI_API CornerRadiusMapper : public BasicMapper<CornerRadius> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(CornerRadiusMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::unique_ptr<CornerRadius> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
