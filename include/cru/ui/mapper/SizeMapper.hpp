#pragma once
#include "Mapper.hpp"

namespace cru::ui::mapper {
class CRU_UI_API SizeMapper : public BasicMapper<Size> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(SizeMapper)

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::unique_ptr<Size> DoMapFromString(String str) override;
  std::unique_ptr<Size> DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
