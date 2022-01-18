#pragma once
#include "Mapper.hpp"

#include "../Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper {
class ThicknessMapper : public BasicMapper<Thickness> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ThicknessMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::unique_ptr<Thickness> DoMapFromString(String str) override;
  std::unique_ptr<Thickness> DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
