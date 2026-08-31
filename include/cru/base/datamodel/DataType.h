#pragma once

#include "../Base.h"
#include "../StringUtil.h"
#include "../Xml.h"

#include <format>
#include <optional>
#include <ranges>
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

struct IDataTypeBase : virtual Interface {
  virtual bool SupportConvertFromString() = 0;
  virtual bool SupportConvertToString() = 0;
  virtual bool SupportConvertFromXml() = 0;
  virtual bool SupportConvertToXml() = 0;
  virtual bool XmlIsOfThisType(cru::xml::XmlElementNode* node) = 0;
};

template <typename T>
struct IDataType : virtual IDataTypeBase {
  virtual DataConvertResult<T> ConvertFromString(std::string_view value) = 0;
  virtual DataConvertResult<std::string> ConvertToString(T value) = 0;
  virtual DataConvertResult<T> ConvertFromXml(
      cru::xml::XmlElementNode* node) = 0;
  virtual DataConvertResult<cru::xml::XmlElementNode*> ConvertToXml(
      T value) = 0;
};

template <typename T>
class DataTypeBase : public Object, public virtual IDataType<T> {
 public:
  std::string GetName() { return name_; }

  bool SupportConvertFromString() final { return DoSupportConvertFromString(); }
  bool SupportConvertToString() final { return DoSupportConvertToString(); }
  DataConvertResult<T> ConvertFromString(std::string_view value) final {
    if (!SupportConvertFromString()) {
      throw Exception("Convert from string is not supported.");
    }
    return DoConvertFromString(value);
  }
  DataConvertResult<std::string> ConvertToString(T value) final {
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
    if (!DoSupportConvertFromXml() && IsAutoConvertFromXmlByString() &&
        SupportConvertFromString()) {
      return node->HasTag(GetName());
    }
    return DoXmlIsOfThisType(node);
  }

  DataConvertResult<T> ConvertFromXml(cru::xml::XmlElementNode* node) final {
    if (!SupportConvertFromXml()) {
      throw Exception("Convert from xml is not supported.");
    }
    if (!XmlIsOfThisType(node)) {
      throw Exception("Xml node is not of this type.");
    }
    if (!DoSupportConvertFromXml() && IsAutoConvertFromXmlByString() &&
        SupportConvertFromString()) {
      return DoConvertFromXmlByString(node);
    }
    return DoConvertFromXml(node);
  }

  DataConvertResult<cru::xml::XmlElementNode*> ConvertToXml(T value) final {
    if (!SupportConvertToXml()) {
      throw Exception("Convert to xml is not supported.");
    }
    return DoConvertToXml(value);
  }

 protected:
  DataConvertResult<T> DoConvertFromXmlByString(
      cru::xml::XmlElementNode* node) {
    std::string content;
    for (auto child : node->GetChildren()) {
      if (child->IsElementNode()) {
        return DataConvertResult<T>::Failure(
            "Element node has child element nodes.");
      } else if (child->IsTextNode()) {
        content += child->AsText()->GetText();
      }
    }

    auto value_attr = node->GetOptionalAttributeValue("value");
    if (value_attr) {
      auto result = DoConvertFromString(*value_attr);
      if (!content.empty()) {
        result.AddError(
            "Element node has both value attribute and text content. Value "
            "attribute will be used.");
      }
      return result;
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
      return DataConvertResult<T>::Failure(std::string(result.message));
    }
  }

  DataConvertResult<std::string> DoConvertToString(const T& value) override {
    return DataConvertResult<std::string>::Success(std::to_string(value));
  }

 private:
  cru::string::ParseToNumberFlag parse_flag_;
};

template <typename T>
  requires(std::is_arithmetic_v<T>)
class NumberListDataType : public DataTypeBase<std::vector<T>> {
 public:
  explicit NumberListDataType()
      : DataTypeBase<std::vector<T>>("NumberList", {true, true, false, false}),
        number_data_type_(
            cru::string::ParseToNumberFlags::AllowLeadingSpaces |
            cru::string::ParseToNumberFlags::AllowTrailingSpaces) {}

 protected:
  DataConvertResult<std::vector<T>> DoConvertFromString(
      std::string_view value) override {
    auto strs = cru::string::SplitBySpace(value);
    bool success = true;
    std::vector<T> numbers;
    std::vector<std::string> errors;
    for (int i = 0; i < strs.size(); i++) {
      auto convert_result = number_data_type_.ConvertFromString(strs[i]);
      if (!convert_result.IsSuccess()) {
        success = false;
      } else {
        numbers.push_back(convert_result.GetValue());
      }
      for (const auto& error : convert_result.GetErrors()) {
        errors.push_back(std::format("In substring {}, {}", i + 1, error));
      }
    }
    std::optional<std::vector<T>> result_value;
    if (success) {
      result_value = std::move(numbers);
    }
    return DataConvertResult<std::vector<T>>(std::move(result_value),
                                             std::move(errors));
  }

  DataConvertResult<std::string> DoConvertToString(
      const std::vector<T>& value) override {
    return DataConvertResult<std::string>::Success(
        cru::string::Join(" ", value | std::views::transform([](T number) {
                                 return std::to_string(number);
                               })));
  }

 private:
  NumberDataType<T> number_data_type_;
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
