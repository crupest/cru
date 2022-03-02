#pragma once
#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API StringMapper : public BasicMapper<String> {
 public:
  StringMapper();
  ~StringMapper();

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }

 protected:
  String DoMapFromString(String str) override;
  String DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
