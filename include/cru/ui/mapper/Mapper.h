#pragma once
#include "../Base.h"

#include <cru/base/Base.h>
#include <cru/base/ClonePtr.h>
#include <cru/base/xml/XmlNode.h>

#include <memory>
#include <optional>
#include <typeindex>

namespace cru::ui::mapper {
class CRU_UI_API MapException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_UI_API MapperBase : public Object {
 public:
  explicit MapperBase(std::type_index type_index);

 public:
  std::type_index GetTypeIndex() const { return type_index_; }

  virtual bool SupportMapFromString() { return false; }
  virtual bool SupportMapFromXml() { return false; }
  virtual bool XmlElementIsOfThisType(xml::XmlElementNode* node) {
    return false;
  }

 private:
  std::type_index type_index_;
};

template <typename T>
class CRU_UI_API BasicMapper : public MapperBase {
 public:
  BasicMapper() : MapperBase(typeid(T)) {}

  T MapFromString(std::string str) {
    if (!SupportMapFromString()) {
      throw Exception("This mapper does not support map from string.");
    }

    return DoMapFromString(str);
  }

  T MapFromXml(xml::XmlElementNode* node) {
    if (!SupportMapFromXml()) {
      throw Exception("This mapper does not support map from xml.");
    }

    if (!XmlElementIsOfThisType(node)) {
      throw Exception("This xml element is not of mapping type.");
    }

    return DoMapFromXml(node);
  }

 protected:
  virtual T DoMapFromString(std::string str) { NotImplemented(); }
  virtual T DoMapFromXml(xml::XmlElementNode* node) { NotImplemented(); }

  T MapFromXmlAsStringValue(xml::XmlElementNode* node,
                            std::optional<T> default_value = std::nullopt) {
    std::optional<T>& value = default_value;

    auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
    if (value_attr) {
      value = DoMapFromString(*value_attr);
    }

    for (auto child : node->GetChildren()) {
      if (child->IsElementNode()) {
        throw MapException("XML node can't contain element child.");
      } else if (auto c = child->AsText()) {
        auto text = c->GetText();
        if (!text.empty() && value.has_value()) {
          throw MapException("XML node has multiple values specified.");
        }
        value = DoMapFromString(text);
      }
    }

    if (!value.has_value()) {
      throw MapException("XML node has no value specified.");
    }

    return *value;
  }
};

template <typename T>
using BasicSharedPtrMapper = BasicMapper<std::shared_ptr<T>>;

template <typename T>
using BasicClonePtrMapper = BasicMapper<ClonePtr<T>>;
}  // namespace cru::ui::mapper

#define CRU_UI_DECLARE_CAN_MAP_FROM_STRING(type)        \
 public:                                                \
  bool SupportMapFromString() override { return true; } \
                                                        \
 protected:                                             \
  type DoMapFromString(std::string str) override;

#define CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT(type)              \
 public:                                                           \
  bool SupportMapFromXml() override { return true; }               \
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override; \
                                                                   \
 protected:                                                        \
  type DoMapFromXml(xml::XmlElementNode* node) override;

#define CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(xml_tag, type)  \
 public:                                                            \
  bool SupportMapFromXml() override { return true; }                \
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override { \
    return node->HasTag(#xml_tag);                                  \
  }                                                                 \
                                                                    \
 protected:                                                         \
  type DoMapFromXml(xml::XmlElementNode* node) override;
