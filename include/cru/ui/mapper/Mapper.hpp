#pragma once

#include "../Base.hpp"

#include "cru/common/Exception.hpp"
#include "cru/xml/XmlNode.hpp"

#include <typeindex>
#include <typeinfo>

namespace cru::ui::mapper {
template <typename T>
class Mapper;

class CRU_UI_API MapperBase : public Object {
 public:
  explicit MapperBase(std::type_index type_index);

  CRU_DELETE_COPY(MapperBase)
  CRU_DELETE_MOVE(MapperBase)

  ~MapperBase() override = default;

 public:
  std::type_index GetTypeIndex() const { return type_index_; }

  template <typename T>
  Mapper<T>* StaticCast() {
    return static_cast<Mapper<T>*>(this);
  }

  template <typename T>
  Mapper<T>* DynamicCast() {
    return dynamic_cast<Mapper<T>*>(this);
  }

 private:
  std::type_index type_index_;
};

template <typename T>
class CRU_UI_API BasicMapper : public MapperBase {
 public:
  BasicMapper() : MapperBase(typeid(T)) {}

  CRU_DELETE_COPY(BasicMapper)
  CRU_DELETE_MOVE(BasicMapper)

  ~BasicMapper() override;

  virtual bool SupportMapFromString() { return false; }
  virtual std::unique_ptr<T> MapFromString(String str) {
    if (!SupportMapFromString()) {
      throw Exception(u"This mapper does not support map from string.");
    }

    return DoMapFromString(str);
  }

  virtual bool SupportMapFromXml() { return false; }
  virtual bool XmlElementIsOfThisType(xml::XmlElementNode* node) {
    return false;
  }
  std::unique_ptr<T> MapFromXml(xml::XmlElementNode* node) {
    if (!SupportMapFromXml()) {
      throw new Exception(u"This mapper does not support map from xml.");
    }

    if (!XmlElementIsOfThisType(node)) {
      throw new Exception(u"This xml element is not of mapping type.");
    }

    return DoMapFromXml(node);
  }

 protected:
  virtual std::unique_ptr<T> DoMapFromString(String str) { return nullptr; }
  virtual std::unique_ptr<T> DoMapFromXml(xml::XmlElementNode* node) {
    return nullptr;
  }
};
}  // namespace cru::ui::mapper
