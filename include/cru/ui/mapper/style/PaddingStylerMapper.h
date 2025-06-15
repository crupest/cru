#pragma once
#include "../Mapper.h"
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
class CRU_UI_API PaddingStylerMapper
    : public BasicCopyPtrMapper<ui::style::PaddingStyler>,
      public virtual IStylerMapper {
 public:
  PaddingStylerMapper();
  ~PaddingStylerMapper();

 public:
  bool SupportMapFromXml() override { return true; }

  CopyPtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  CopyPtr<ui::style::PaddingStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
