#pragma once
#include "Mapper.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper {
class CRU_UI_API CursorMapper : public BasicRefMapper<platform::gui::ICursor> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(CursorMapper);

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::shared_ptr<platform::gui::ICursor> DoMapFromString(String str) override;
  std::shared_ptr<platform::gui::ICursor> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
