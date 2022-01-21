#pragma once
#include "Mapper.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper {
class ColorMapper : public BasicMapper<Color> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ColorMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::unique_ptr<Color> DoMapFromString(String str) override;
  std::unique_ptr<Color> DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
