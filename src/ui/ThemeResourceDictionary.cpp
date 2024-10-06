#include "cru/ui/ThemeResourceDictionary.h"
#include "cru/base/io/CFileStream.h"
#include "cru/base/log/Logger.h"
#include "cru/xml/XmlNode.h"
#include "cru/xml/XmlParser.h"

namespace cru::ui {

std::unique_ptr<ThemeResourceDictionary> ThemeResourceDictionary::FromFile(
    const String& file_path) {
  io::CFileStream stream(file_path.ToUtf8().c_str(), "r");
  auto xml_string = stream.ReadToEndAsUtf8String();
  auto parser = xml::XmlParser(xml_string);
  return std::make_unique<ThemeResourceDictionary>(parser.Parse(), false);
}

ThemeResourceDictionary::ThemeResourceDictionary(xml::XmlElementNode* xml_root,
                                                 bool clone) {
  Expects(xml_root);
  xml_root_.reset(clone ? xml_root->Clone()->AsElement() : xml_root);
  UpdateResourceMap(xml_root_.get());
}

ThemeResourceDictionary::~ThemeResourceDictionary() = default;

void ThemeResourceDictionary::UpdateResourceMap(xml::XmlElementNode* xml_root) {
  if (!xml_root->GetTag().CaseInsensitiveEqual(u"Theme")) {
    throw Exception(u"Root tag of theme must be 'Theme'.");
  }

  for (auto child : xml_root->GetChildren()) {
    if (child->IsElementNode()) {
      auto c = child->AsElement();
      if (c->GetTag().CaseInsensitiveEqual(u"Resource")) {
        auto key_attr = c->GetOptionalAttributeValueCaseInsensitive(u"key");
        if (!key_attr) {
          throw Exception(u"'key' attribute is required for resource.");
        }
        if (c->GetChildElementCount() != 1) {
          throw Exception(u"Resource must have only one child element.");
        }

        ResourceEntry entry;

        entry.name = *key_attr;
        entry.xml_node = c->GetFirstChildElement();

        resource_map_[entry.name] = std::move(entry);
      } else {
        CRU_LOG_DEBUG(u"Ignore unknown element {} of theme.", c->GetTag());
      }
    } else {
      CRU_LOG_DEBUG(u"Ignore text or comment node of theme.");
    }
  }
}
}  // namespace cru::ui
