#include "cru/ui/ThemeResourceDictionary.h"
#include "cru/base/StringUtil.h"
#include "cru/base/Xml.h"
#include "cru/base/io/CFileStream.h"
#include "cru/base/log/Logger.h"

#include <cassert>

namespace cru::ui {

std::unique_ptr<ThemeResourceDictionary> ThemeResourceDictionary::FromFile(
    std::filesystem::path file_path) {
  io::CFileStream stream(file_path.generic_string().c_str(), "r");
  auto xml_string = stream.ReadToEndAsUtf8String();
  auto parser = xml::XmlParser(xml_string);
  return std::make_unique<ThemeResourceDictionary>(parser.Parse(), false);
}

ThemeResourceDictionary::ThemeResourceDictionary(xml::XmlElementNode* xml_root,
                                                 bool clone) {
  assert(xml_root);
  xml_root_.reset(clone ? xml_root->Clone()->AsElement() : xml_root);
  UpdateResourceMap(xml_root_.get());
}

void ThemeResourceDictionary::UpdateResourceMap(xml::XmlElementNode* xml_root) {
  if (!cru::string::CaseInsensitiveEqual(xml_root->GetTag(), "Theme")) {
    throw Exception("Root tag of theme must be 'Theme'.");
  }

  for (auto resource_node : xml_root->GetChildren()) {
    if (resource_node->IsElementNode()) {
      auto resource_element = resource_node->AsElement();
      if (cru::string::CaseInsensitiveEqual(resource_element->GetTag(),
                                            "Resource")) {
        ResourceEntry entry;

        auto key_attr = resource_element->GetOptionalAttributeValue("key");
        if (!key_attr) {
          throw Exception("'key' attribute is required for resource.");
        }
        entry.name = *key_attr;

        xml::XmlElementNode* resource_child_element = nullptr;
        std::string resource_text;

        const auto error_message =
            "Resource must have only one child element or text/comment "
            "only nodes.";

        for (const auto& resource_child : resource_element->GetChildren()) {
          if (resource_child_element && !resource_child->IsCommentNode()) {
            throw Exception(error_message);
          }

          if (resource_child->IsElementNode()) {
            if (!resource_text.empty()) {
              throw Exception(error_message);
            }
            resource_child_element = resource_child->AsElement();
          } else if (resource_child->IsTextNode()) {
            resource_text += resource_child->AsText()->GetText();
          }
        }

        if (resource_child_element) {
          entry.value = resource_child_element;
        } else {
          entry.value = resource_text;
        }

        resource_map_[entry.name] = std::move(entry);
      } else {
        CruLogDebug(kLogTag, "Ignore unknown element {} of theme.",
                    resource_element->GetTag());
      }
    } else {
      CruLogDebug(kLogTag, "Ignore text or comment node of theme.");
    }
  }
}
}  // namespace cru::ui
