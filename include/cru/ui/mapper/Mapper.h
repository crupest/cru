#pragma once
#include "../Base.h"

#include "cru/common/ClonablePtr.h"
#include "cru/common/Exception.h"
#include "cru/xml/XmlNode.h"

#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>

namespace cru::ui::mapper {
template <typename T>
class BasicMapper;

class CRU_UI_API MapperBase : public Object {
 public:
  explicit MapperBase(std::type_index type_index);

  CRU_DELETE_COPY(MapperBase)
  CRU_DELETE_MOVE(MapperBase)

  ~MapperBase() override = default;

 public:
  std::type_index GetTypeIndex() const { return type_index_; }

  template <typename T>
  BasicMapper<T>* StaticCast() {
    return static_cast<BasicMapper<T>*>(this);
  }

  template <typename T>
  BasicMapper<T>* DynamicCast() {
    return dynamic_cast<BasicMapper<T>*>(this);
  }

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
  static_assert(std::is_default_constructible_v<T>,
                "T must be default constructible.");

  BasicMapper() : MapperBase(typeid(T)) {}

  CRU_DELETE_COPY(BasicMapper)
  CRU_DELETE_MOVE(BasicMapper)

  ~BasicMapper() override = default;

  virtual T MapFromString(String str) {
    if (!SupportMapFromString()) {
      throw Exception(u"This mapper does not support map from string.");
    }

    return DoMapFromString(str);
  }

  T MapFromXml(xml::XmlElementNode* node) {
    if (!SupportMapFromXml()) {
      throw Exception(u"This mapper does not support map from xml.");
    }

    if (!XmlElementIsOfThisType(node)) {
      throw Exception(u"This xml element is not of mapping type.");
    }

    return DoMapFromXml(node);
  }

 protected:
  virtual T DoMapFromString(String str) { return {}; }
  virtual T DoMapFromXml(xml::XmlElementNode* node) { return {}; }
};

template <typename T>
using BasicRefMapper = BasicMapper<std::shared_ptr<T>>;

template <typename T>
using BasicPtrMapper = BasicMapper<ClonablePtr<T>>;
}  // namespace cru::ui::mapper
