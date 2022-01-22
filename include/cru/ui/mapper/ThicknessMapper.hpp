#pragma once
#include "Mapper.hpp"

#include "../Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper {
class CRU_UI_API ThicknessMapper : public BasicMapper<Thickness> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ThicknessMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  Thickness DoMapFromString(String str) override;
  Thickness DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
