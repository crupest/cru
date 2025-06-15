#pragma once
#include "../Mapper.h"
#include "IStylerMapper.h"

namespace cru::ui::mapper::style {
class ContentBrushStylerMapper
    : public BasicCopyPtrMapper<ui::style::ContentBrushStyler>,
      public virtual IStylerMapper {
 public:
  ContentBrushStylerMapper();
  ~ContentBrushStylerMapper() override;

 public:
  bool SupportMapFromXml() override { return true; }

  CopyPtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  CopyPtr<ui::style::ContentBrushStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
