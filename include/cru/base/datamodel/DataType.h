#pragma once

#include "../Base.h"
#include "../Dictionary.h"
#include "../StringUtil.h"
#include "../Xml.h"

#include <algorithm>
#include <any>
#include <concepts>
#include <format>
#include <initializer_list>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>

namespace cru::datamodel {
class CRU_BASE_API KeyValueObject {
 private:
  using Storage = Dictionary<std::string, std::any>;

 public:
  using key_type = Storage::key_type;
  using mapped_type = Storage::mapped_type;
  using value_type = Storage::value_type;
  using size_type = Storage::size_type;
  using difference_type = Storage::difference_type;
  using reference = Storage::reference;
  using const_reference = Storage::const_reference;
  using iterator = Storage::iterator;
  using const_iterator = Storage::const_iterator;
  using reverse_iterator = Storage::reverse_iterator;
  using const_reverse_iterator = Storage::const_reverse_iterator;

  KeyValueObject() = default;
  KeyValueObject(std::initializer_list<value_type> init) : dictionary_(init) {}
  CRU_DEFAULT_COPY(KeyValueObject)
  CRU_DEFAULT_MOVE(KeyValueObject)
  ~KeyValueObject() = default;

  auto begin() { return dictionary_.begin(); }
  auto end() { return dictionary_.end(); }
  auto begin() const { return dictionary_.begin(); }
  auto end() const { return dictionary_.end(); }
  auto cbegin() const { return dictionary_.cbegin(); }
  auto cend() const { return dictionary_.cend(); }
  auto rbegin() { return dictionary_.rbegin(); }
  auto rend() { return dictionary_.rend(); }
  auto rbegin() const { return dictionary_.rbegin(); }
  auto rend() const { return dictionary_.rend(); }
  auto crbegin() const { return dictionary_.crbegin(); }
  auto crend() const { return dictionary_.crend(); }

  auto empty() const noexcept { return dictionary_.empty(); }
  auto size() const noexcept { return dictionary_.size(); }
  Index ssize() const noexcept { return dictionary_.ssize(); }
  auto max_size() const noexcept { return dictionary_.max_size(); }

  auto clear() noexcept { return dictionary_.clear(); }

  template <typename P>
  std::pair<iterator, bool> insert(P&& value) {
    return dictionary_.insert(std::forward<P>(value));
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return dictionary_.emplace(std::forward<Args>(args)...);
  }

  template <typename K, typename M>
  std::pair<iterator, bool> insert_or_assign(K&& key, M&& obj) {
    return dictionary_.insert_or_assign(std::forward<K>(key),
                                        std::forward<M>(obj));
  }

  auto erase(const_iterator pos) { return dictionary_.erase(pos); }
  auto erase(iterator pos) { return dictionary_.erase(pos); }
  auto erase(const_iterator first, const_iterator last) {
    return dictionary_.erase(first, last);
  }

  template <typename K>
  size_type erase(K&& key) {
    return dictionary_.erase(std::forward<K>(key));
  }

  void swap(KeyValueObject& other) { dictionary_.swap(other.dictionary_); }

  friend void swap(KeyValueObject& lhs, KeyValueObject& rhs) { lhs.swap(rhs); }

  template <typename K>
  mapped_type& at(const K& key) {
    return dictionary_.at(key);
  }

  template <typename K>
  const mapped_type& at(const K& key) const {
    return dictionary_.at(key);
  }

  template <typename K>
  mapped_type& operator[](K&& key) {
    return dictionary_[std::forward<K>(key)];
  }

  template <typename K>
  iterator find(const K& key) {
    return dictionary_.find(key);
  }

  template <typename K>
  const_iterator find(const K& key) const {
    return dictionary_.find(key);
  }

  template <typename K>
  size_type count(const K& key) const {
    return dictionary_.count(key);
  }

  template <typename K>
  bool contains(const K& key) const {
    return dictionary_.contains(key);
  }

  auto Keys() const { return dictionary_.Keys(); }
  auto Values() { return dictionary_.Values(); }
  auto Values() const { return dictionary_.Values(); }

  template <typename K>
  bool HasProperty(const K& key) const {
    return dictionary_.contains(key);
  }

  template <typename K>
  bool PropertyIsTrue(const K& key) const {
    auto it = dictionary_.find(key);
    if (it == dictionary_.end()) {
      return false;
    }
    try {
      return std::any_cast<bool>(it->second);
    } catch (const std::bad_any_cast&) {
      return false;
    }
  }

  template <typename T, typename K>
  T& GetProperty(const K& key) {
    auto& value = dictionary_.at(key);
    if (!value.has_value()) {
      throw Exception(std::format("Property {} has no value.", key));
    }
    return std::any_cast<T&>(value);
  }

  template <typename T, typename K>
  const T& GetProperty(const K& key) const {
    const auto& value = dictionary_.at(key);
    if (!value.has_value()) {
      throw Exception(std::format("Property {} has no value.", key));
    }
    return std::any_cast<const T&>(value);
  }

  template <typename T, typename K>
  bool AddProperty(K&& key, T&& value) {
    return dictionary_
        .emplace(std::forward<K>(key), std::make_any<T>(std::forward<T>(value)))
        .second;
  }

  template <typename T, typename K>
  bool SetProperty(K&& key, T&& value) {
    return dictionary_
        .insert_or_assign(std::forward<K>(key),
                          std::make_any<T>(std::forward<T>(value)))
        .second;
  }

  template <typename T, typename K, typename... Args>
  bool EmplaceProperty(K&& key, Args&&... args) {
    return dictionary_
        .emplace(std::forward<K>(key),
                 std::make_any<T>(std::forward<Args>(args)...))
        .second;
  }

  template <typename K>
  bool RemoveProperty(const K& key) {
    static_assert(
        !(std::same_as<K, iterator> || std::same_as<K, const_iterator>),
        "RemoveProperty does not accept iterator.");
    return dictionary_.erase(key);
  }

 private:
  Storage dictionary_;
};

template <class Pred>
typename KeyValueObject::size_type erase_if(KeyValueObject& object, Pred pred) {
  auto old_size = object.size();
  for (auto iter = object.begin(); iter != object.end();) {
    if (pred(*iter))
      iter = object.erase(iter);
    else
      ++iter;
  }
  return old_size - object.size();
}

template <typename T>
class DataConvertResult {
 public:
  static constexpr auto NotSupportedKey = "NotSupported";

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

  static DataConvertResult<T> NotSupported(std::string error) {
    DataConvertResult<T> result(std::nullopt, {std::move(error)});
    result.GetMetadata().SetProperty(NotSupportedKey, true);
    return result;
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

  KeyValueObject& GetMetadata() { return metadata_; }
  const KeyValueObject& GetMetadata() const { return metadata_; }

  bool IsNotSupported() const {
    return GetMetadata().PropertyIsTrue(NotSupportedKey);
  }

 private:
  std::optional<T> value_;
  std::vector<std::string> errors_;
  KeyValueObject metadata_;
};

struct IDataTypeBase : virtual Interface {
  virtual std::string GetName() = 0;
  virtual const std::type_info& GetTypeInfo() = 0;
  std::type_index GetTypeIndex() { return std::type_index(GetTypeInfo()); }
  virtual DataConvertResult<std::string> ConvertToString(
      const std::any& value) = 0;
  virtual DataConvertResult<cru::xml::XmlElementNode*> ConvertToXml(
      const std::any& value) = 0;
  virtual bool XmlIsOfThisType(cru::xml::XmlElementNode* node) = 0;
};

template <typename T>
struct IDataType : virtual IDataTypeBase {
 private:
  static const T& GetValueFromAny(const std::any& value) {
    if (!value.has_value()) {
      throw Exception("std::any value is empty.");
    }
    try {
      return std::any_cast<const T&>(value);
    } catch (const std::bad_any_cast&) {
      throw Exception("std::any value is not of the expected type.");
    }
  }

 public:
  const std::type_info& GetTypeInfo() override { return typeid(T); }

  virtual DataConvertResult<T> ConvertFromString(std::string_view value) = 0;
  virtual DataConvertResult<std::string> ConvertToString(T value) = 0;
  DataConvertResult<std::string> ConvertToString(const std::any& value) final {
    return ConvertToString(GetValueFromAny(value));
  }
  virtual DataConvertResult<T> ConvertFromXml(
      cru::xml::XmlElementNode* node) = 0;
  virtual DataConvertResult<cru::xml::XmlElementNode*> ConvertToXml(
      T value) = 0;
  DataConvertResult<cru::xml::XmlElementNode*> ConvertToXml(
      const std::any& value) final {
    return ConvertToXml(GetValueFromAny(value));
  }
};

template <typename T>
class DataTypeBase : public Object, public virtual IDataType<T> {
 public:
  std::string GetName() override { return name_; }

  DataConvertResult<T> ConvertFromString(std::string_view value) final {
    return DoConvertFromString(value);
  }
  DataConvertResult<std::string> ConvertToString(T value) final {
    return DoConvertToString(value);
  }

  bool XmlIsOfThisType(cru::xml::XmlElementNode* node) final {
    return DoXmlIsOfThisType(node);
  }

  DataConvertResult<T> ConvertFromXml(cru::xml::XmlElementNode* node) final {
    if (!XmlIsOfThisType(node)) {
      return DataConvertResult<T>::Failure("Xml node is not of this type.");
    }
    auto result = DoConvertFromXml(node);
    if (result.IsNotSupported() && IsAutoConvertFromXmlByString()) {
      return DoConvertFromXmlByString(node);
    }
    return result;
  }

  DataConvertResult<cru::xml::XmlElementNode*> ConvertToXml(T value) final {
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
  DataTypeBase(std::string name)
      : name_(std::move(name)), auto_convert_from_xml_by_string_(true) {}

  virtual DataConvertResult<T> DoConvertFromString(std::string_view value) {
    CRU_UNUSED(value)
    return DataConvertResult<T>::NotSupported(
        "Convert from string is not supported.");
  }

  virtual DataConvertResult<std::string> DoConvertToString(const T& value) {
    CRU_UNUSED(value)
    return DataConvertResult<std::string>::NotSupported(
        "Convert to string is not supported.");
  }

  virtual bool DoXmlIsOfThisType(cru::xml::XmlElementNode* node) {
    return node->HasTag(GetName());
  }

  virtual DataConvertResult<T> DoConvertFromXml(
      cru::xml::XmlElementNode* node) {
    return DataConvertResult<T>::NotSupported(
        "Convert from xml is not supported.");
  }

  virtual DataConvertResult<cru::xml::XmlElementNode*> DoConvertToXml(
      const T& value) {
    CRU_UNUSED(value)
    return DataConvertResult<cru::xml::XmlElementNode*>::NotSupported(
        "Convert to xml is not supported.");
  }

  bool IsAutoConvertFromXmlByString() const {
    return auto_convert_from_xml_by_string_;
  }
  void SetAutoConvertFromXmlByString(bool enable) {
    auto_convert_from_xml_by_string_ = enable;
  }

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
      : DataTypeBase<T>("Number"), parse_flag_(parse_flags) {}

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
      : DataTypeBase<std::vector<T>>("NumberList"),
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

class DataTypeRegistry;

class CRU_BASE_API ObjectDataType : public DataTypeBase<KeyValueObject> {
 public:
  explicit ObjectDataType(DataTypeRegistry* registry);

 protected:
  DataConvertResult<std::string> DoConvertToString(
      const KeyValueObject& value) override {
    DataConvertResult<std::string> result;
    std::string result_value;
    std::string path;
    ConvertToStringImpl(value, result, result_value, path);
    result.SetValue(std::move(result_value));
    return result;
  }

  void ConvertToStringImpl(const KeyValueObject& value,
                           DataConvertResult<std::string>& result,
                           std::string& result_value, std::string& path);

 private:
  DataTypeRegistry* registry_;
};

}  // namespace cru::datamodel
