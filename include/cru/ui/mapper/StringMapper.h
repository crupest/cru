#pragma once
#include "Mapper.h"

#include <string>

namespace cru::ui::mapper {
class CRU_UI_API StringMapper : public BasicMapper<std::string> {
 public:
  StringMapper();
  ~StringMapper();

 public:
  bool SupportMapFromString() override { return true; }
  bool SupportMapFromXml() override { return true; }

 protected:
  std::string DoMapFromString(std::string str) override;
  std::string DoMapFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
