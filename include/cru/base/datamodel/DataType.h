#pragma once

#include "../Base.h"
#include "../StringUtil.h"
#include "../xml/XmlNode.h"

#include <optional>
#include <string>
#include <string_view>

namespace cru::datamodel {
template <typename T>
class DataConvertResult {
 public:
  DataConvertResult() = default;
  DataConvertResult(std::optional<T> value, std::vector<std::string> errors)
      : value_(std::move(value)), errors_(std::move(errors)) {}

  static DataConvertResult<T> Success(T value) {
    return DataConvertResult<T>(std::move(value), {});
  }

  static DataConvertResult<T> SuccessWithError(T value, std::string error) {
    return DataConvertResult<T>(std::move(value), {std::move(error)});
  }

  static DataConvertResult<T> SuccessWithErrors(
      T value, std::vector<std::string> errors) {
    return DataConvertResult<T>(std::move(value), std::move(errors));
  }

  static DataConvertResult<T> Failure(std::string error) {
    return DataConvertResult<T>(std::nullopt, {std::move(error)});
  }

  static DataConvertResult<T> Failure(std::vector<std::string> errors) {
    return DataConvertResult<T>(std::nullopt, std::move(errors));
  }

  bool IsSuccess() const { return value_.has_value(); }
  bool HasErrors() const { return !errors_.empty(); }
  T GetValue() const {
    if (!value_.has_value()) {
      throw Exception("DataConvertResult has no value.");
    }
    return *value_;
  }
  std::optional<T> GetOptionalValue() const { return value_; }
  const std::vector<std::string>& GetErrors() const { return errors_; }

  void SetValue(std::optional<T> value) { value_ = std::move(value); }
  void AddError(std::string error) { errors_.push_back(std::move(error)); }
  void RemoveError(std::string_view error) { std::erase(errors_, error); }

 private:
  std::optional<T> value_;
  std::vector<std::string> errors_;
};

struct IDataType : virtual Interface {
  virtual bool SupportConvertFromString() = 0;
  virtual bool SupportConvertToString() = 0;
  virtual bool SupportConvertFromXml() = 0;
  virtual bool SupportConvertToXml() = 0;
  virtual bool XmlIsOfThisType(cru::xml::XmlElementNode* node) = 0;
};

template <typename T>
class DataTypeBase : public Object, public IDataType {
 public:
  ~DataTypeBase() override = 0;

  std::string GetName() { return name_; }

  bool SupportConvertFromString() final { return DoSupportConvertFromString(); }
  bool SupportConvertToString() final { return DoSupportConvertToString(); }
  DataConvertResult<T> ConvertFromString(std::string_view value) {
    if (!SupportConvertFromString()) {
      throw Exception("Convert from string is not supported.");
    }
    return DoConvertFromString(value);
  }
  DataConvertResult<std::string> ConvertToString(T value) {
    if (!SupportConvertToString()) {
      throw Exception("Convert to string is not supported.");
    }
    return DoConvertToString(value);
  }

  bool SupportConvertFromXml() final {
    auto native_able = DoSupportConvertFromXml();
    if (!native_able && IsAutoConvertFromXmlByString() &&
        SupportConvertFromString()) {
      return true;
    }
    return native_able;
  }

  bool SupportConvertToXml() final { return DoSupportConvertToXml(); }

  bool XmlIsOfThisType(cru::xml::XmlElementNode* node) final {
    if (!SupportConvertFromXml()) {
      throw Exception("Convert from xml is not supported.");
    }
    auto native_able = DoSupportConvertFromXml();
    if (!native_able && IsAutoConvertFromXmlByString() &&
        SupportConvertFromString()) {
      return DoXmlIsOfThisTypeByName(node);
    }
    return DoXmlIsOfThisType(node);
  }

  DataConvertResult<T> ConvertFromXml(cru::xml::XmlElementNode* node) {
    if (!SupportConvertFromXml()) {
      throw Exception("Convert from xml is not supported.");
    }
    if (!XmlIsOfThisType(node)) {
      throw Exception("Xml node is not of this type.");
    }
    auto native_able = DoSupportConvertFromXml();
    if (!native_able && IsAutoConvertFromXmlByString() &&
        SupportConvertFromString()) {
      return DoConvertFromXmlByString(node);
    }
    return DoConvertFromXml(node);
  }

  DataConvertResult<cru::xml::XmlElementNode*> ConvertToXml(T value) {
    if (!SupportConvertToXml()) {
      throw Exception("Convert to xml is not supported.");
    }
    return DoConvertToXml(value);
  }

 private:
  bool DoXmlIsOfThisTypeByName(cru::xml::XmlElementNode* node) {
    return node->HasTag(GetName());
  }

 protected:
  DataConvertResult<T> DoConvertFromXmlByString(
      cru::xml::XmlElementNode* node) {
    auto value_attr = node->GetOptionalAttributeValue("value");
    std::string content;
    for (auto child : node->GetChildren()) {
      if (child->GetType() == cru::xml::XmlNode::Type::Element) {
        return DataConvertResult<T>::Failure(
            "Element node has child element nodes.");
      } else if (child->GetType() == cru::xml::XmlNode::Type::Text) {
        content += child->AsText()->GetText();
      }
    }
    if (value_attr) {
      auto result = DoConvertFromString(*value_attr);
      if (!content.empty()) {
        result.AddError(
            "Element node has both value attribute and text content. Value "
            "attribute will be used.");
      }
      return result;
    }

    if (!content.empty()) {
      return DoConvertFromString(content);
    }

    return DoConvertFromString(content);
  }

 protected:
  struct Supports {
    bool from_string;
    bool to_string;
    bool from_xml;
    bool to_xml;
  };

  DataTypeBase(std::string name,
               const Supports& supports = {true, true, false, false})
      : supports_(supports),
        name_(std::move(name)),
        auto_convert_from_xml_by_string_(true) {}

  virtual bool DoSupportConvertFromString() { return supports_.from_string; }
  virtual bool DoSupportConvertToString() { return supports_.to_string; }
  virtual DataConvertResult<T> DoConvertFromString(std::string_view value) {
    CRU_UNUSED(value)
    NotImplemented();
  }
  virtual DataConvertResult<std::string> DoConvertToString(const T& value) {
    CRU_UNUSED(value)
    NotImplemented();
  }

  virtual bool DoSupportConvertFromXml() { return supports_.from_xml; }
  virtual bool DoSupportConvertToXml() { return supports_.to_xml; }
  virtual bool DoXmlIsOfThisType(cru::xml::XmlElementNode* node) {
    CRU_UNUSED(node)
    NotImplemented();
  }
  virtual DataConvertResult<T> DoConvertFromXml(
      cru::xml::XmlElementNode* node) {
    CRU_UNUSED(node)
    NotImplemented();
  }
  virtual DataConvertResult<cru::xml::XmlElementNode*> DoConvertToXml(
      const T& value) {
    CRU_UNUSED(value)
    NotImplemented();
  }

  bool IsAutoConvertFromXmlByString() const {
    return auto_convert_from_xml_by_string_;
  }
  void SetAutoConvertFromXmlByString(bool enable) {
    auto_convert_from_xml_by_string_ = enable;
  }

  Supports supports_;

 private:
  std::string name_;
  bool auto_convert_from_xml_by_string_;
};

template <typename T>
DataTypeBase<T>::~DataTypeBase() = default;

template <typename T>
  requires(std::is_arithmetic_v<T>)
class NumberDataType : public DataTypeBase<T> {
 public:
  explicit NumberDataType(
      cru::string::ParseToNumberFlag parse_flags =
          cru::string::ParseToNumberFlags::AllowLeadingSpaces |
          cru::string::ParseToNumberFlags::AllowTrailingSpaces |
          cru::string::ParseToNumberFlags::AllowTrailingJunk)
      : DataTypeBase<T>("Number", {true, true, false, false}),
        parse_flag_(parse_flags) {}

 protected:
  DataConvertResult<T> DoConvertFromString(std::string_view value) override {
    auto result = cru::string::ParseToNumber<T>(value, parse_flag_);
    if (result.valid) {
      return DataConvertResult<T>::Success(result.value);
    } else {
      return DataConvertResult<T>::Failure(result.message);
    }
  }

  DataConvertResult<std::string> DoConvertToString(const T& value) override {
    return DataConvertResult<std::string>::Success(std::to_string(value));
  }

 private:
  cru::string::ParseToNumberFlag parse_flag_;
};

class CRU_BASE_API StringDataType : public DataTypeBase<std::string> {
 public:
  StringDataType();

 protected:
  DataConvertResult<std::string> DoConvertFromString(
      std::string_view value) override;

  DataConvertResult<std::string> DoConvertToString(
      const std::string& value) override;
};
}  // namespace cru::datamodel
