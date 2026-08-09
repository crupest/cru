#pragma once

#include "Base.h"

#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace cru::json {

/**
 * @brief The concrete kind stored by a JsonValue.
 */
enum class JsonValueType { Null, Boolean, Number, String, Array, Object };

/**
 * @brief Converts a JSON value type to a stable diagnostic string.
 * @param type The JSON value type.
 * @return One of "Null", "Boolean", "Number", "String", "Array", or
 * "Object".
 */
CRU_BASE_API std::string ToString(JsonValueType type);

template <typename T>
concept JsonScalarValueType =
    std::same_as<T, std::nullptr_t> || std::same_as<T, bool> ||
    std::same_as<T, double> || std::same_as<T, std::string>;

template <typename T>
concept JsonNumberValueCompatibleType =
    std::is_arithmetic_v<T> && !std::same_as<T, double> &&
    !std::same_as<T, bool>;

/**
 * @brief Value-semantic JSON value.
 *
 * Holds null, boolean, number, string, array, or object. Copies are deep;
 * moves reset the source to null. Objects preserve insertion order.
 *
 * Direct iteration is random-access: scalars yield themselves once, arrays
 * yield elements, and objects yield property values. Use Keys() or Items() for
 * object names.
 *
 * References, pointers, iterators, and ranges refer to internal storage and may
 * be invalidated by assignment, move, SetArray(), SetObject(), Insert(), or
 * Remove().
 */
class CRU_BASE_API JsonValue {
 private:
  using ArrayStorage = std::vector<JsonValue>;
  using ObjectItem = std::pair<std::string, JsonValue>;
  using ObjectStorage = std::vector<ObjectItem>;

  using Storage = std::variant<std::nullptr_t, bool, double, std::string,
                               ArrayStorage, ObjectStorage>;

 public:
  /**
   * @brief Random-access iterator over exposed values.
   */
  template <bool IsConst>
  class Iterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;
    using value_type = JsonValue;
    using difference_type = std::ptrdiff_t;
    using reference = std::conditional_t<IsConst, const JsonValue&, JsonValue&>;
    using pointer = std::conditional_t<IsConst, const JsonValue*, JsonValue*>;

    Iterator() = default;

    template <bool OtherIsConst>
      requires(IsConst && !OtherIsConst)
    Iterator(const Iterator<OtherIsConst>& other)
        : owner_(other.owner_), index_(other.index_) {}

    reference operator*() const { return owner_->GetIteratedValue(index_); }

    pointer operator->() const { return &**this; }

    reference operator[](difference_type offset) const {
      return *(*this + offset);
    }

    Iterator& operator++() {
      ++index_;
      return *this;
    }

    Iterator operator++(int) {
      Iterator copy(*this);
      ++*this;
      return copy;
    }

    Iterator& operator--() {
      --index_;
      return *this;
    }

    Iterator operator--(int) {
      Iterator copy(*this);
      --*this;
      return copy;
    }

    Iterator& operator+=(difference_type offset) {
      index_ += offset;
      return *this;
    }

    Iterator& operator-=(difference_type offset) {
      index_ -= offset;
      return *this;
    }

    friend Iterator operator+(Iterator iter, difference_type offset) {
      iter += offset;
      return iter;
    }

    friend Iterator operator+(difference_type offset, Iterator iter) {
      iter += offset;
      return iter;
    }

    friend Iterator operator-(Iterator iter, difference_type offset) {
      iter -= offset;
      return iter;
    }

    friend difference_type operator-(const Iterator& lhs, const Iterator& rhs) {
      return lhs.index_ - rhs.index_;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
      return lhs.owner_ == rhs.owner_ && lhs.index_ == rhs.index_;
    }

    friend auto operator<=>(const Iterator& lhs, const Iterator& rhs) {
      return lhs.index_ <=> rhs.index_;
    }

   private:
    using Owner = std::conditional_t<IsConst, const JsonValue, JsonValue>;

    friend class JsonValue;
    template <bool>
    friend class Iterator;

    Iterator(Owner* owner, Index index) : owner_(owner), index_(index) {}

    Owner* owner_ = nullptr;
    Index index_ = 0;
  };

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;

  JsonValue();

  template <JsonScalarValueType T>
  JsonValue(T value) : storage_(std::move(value)) {}

  template <JsonNumberValueCompatibleType T>
  JsonValue(T value) : storage_(static_cast<double>(value)) {}

  /**
   * @brief Creates a JSON string value from a null-terminated string.
   * @throws cru::Exception if value is nullptr. Use JsonValue(nullptr) for JSON
   * null.
   */
  JsonValue(const char* value);

  JsonValue(const JsonValue& other) = default;

  JsonValue& operator=(const JsonValue& other) = default;

  /**
   * @brief Moves and resets other to null.
   */
  JsonValue(JsonValue&& other) noexcept;

  /**
   * @brief Moves and resets other to null.
   */
  JsonValue& operator=(JsonValue&& other) noexcept;

  ~JsonValue() = default;

  static JsonValue Array();

  static JsonValue Object();

  JsonValueType GetType() const;

  bool IsNull() const { return GetType() == JsonValueType::Null; }
  bool IsBoolean() const { return GetType() == JsonValueType::Boolean; }
  bool IsNumber() const { return GetType() == JsonValueType::Number; }
  bool IsString() const { return GetType() == JsonValueType::String; }
  bool IsArray() const { return GetType() == JsonValueType::Array; }
  bool IsObject() const { return GetType() == JsonValueType::Object; }

  std::nullptr_t& GetNull();

  const std::nullptr_t& GetNull() const;

  bool& GetBoolean();

  const bool& GetBoolean() const;

  double& GetNumber();

  const double& GetNumber() const;

  std::string& GetString();

  const std::string& GetString() const;

  template <JsonScalarValueType T>
  T& Get() {
    if constexpr (std::same_as<T, std::nullptr_t>) {
      return GetNull();
    } else if constexpr (std::same_as<T, bool>) {
      return GetBoolean();
    } else if constexpr (std::same_as<T, double>) {
      return GetNumber();
    } else if constexpr (std::same_as<T, std::string>) {
      return GetString();
    }
    std::unreachable();
  }

  template <JsonScalarValueType T>
  T& Get() const {
    if constexpr (std::same_as<T, std::nullptr_t>) {
      return GetNull();
    } else if constexpr (std::same_as<T, bool>) {
      return GetBoolean();
    } else if constexpr (std::same_as<T, double>) {
      return GetNumber();
    } else if constexpr (std::same_as<T, std::string>) {
      return GetString();
    }
    std::unreachable();
  }

  template <JsonScalarValueType T>
  void Set(T value) {
    storage_ = std::move(value);
  }

  template <JsonNumberValueCompatibleType T>
  void Set(T value) {
    Set(static_cast<double>(value));
  }

  /**
   * @brief Replaces this value with a JSON string copied from value.
   * @throws cru::Exception if value is nullptr. Use Set(nullptr) for JSON null.
   */
  void Set(const char* value);

  void SetArray();

  void SetObject();

  template <typename T>
    requires(JsonScalarValueType<T> || JsonNumberValueCompatibleType<T> ||
             std::same_as<T, const char*>)
  JsonValue& operator=(T value) {
    Set(std::move(value));
    return *this;
  }

  /**
   * @brief Gets the array/object size.
   * @throws cru::Exception if this value is not an array or object.
   */
  Index GetSize() const;

  /**
   * @brief Gets an array element.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  JsonValue& Get(Index index);

  const JsonValue& Get(Index index) const;

  /**
   * @brief Gets an object property.
   * @throws cru::Exception if this value is not an object or key is absent.
   */
  JsonValue& Get(std::string_view key);

  const JsonValue& Get(std::string_view key) const;

  /**
   * @brief Gets an array element.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  JsonValue& operator[](Index index);

  const JsonValue& operator[](Index index) const;

  /**
   * @brief Gets or appends a null object property.
   * @throws cru::Exception if this value is not an object.
   */
  JsonValue& operator[](std::string_view key);

  /**
   * @brief Gets an object property.
   * @throws cru::Exception if this value is not an object or key is absent.
   */
  const JsonValue& operator[](std::string_view key) const;

  /**
   * @brief Replaces an array element.
   * @return Always true when the operation succeeds.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  bool Set(Index index, JsonValue value);

  /**
   * @brief Sets an object property.
   * @return true if replaced; false if appended.
   * @throws cru::Exception if this value is not an object.
   */
  bool Set(std::string_view key, JsonValue value);

  /**
   * @brief Inserts an array element.
   *
   * Inserts before index; index may equal GetSize() to append.
   * @return Always true when the operation succeeds.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  bool Insert(Index index, JsonValue value);

  /**
   * @brief Inserts an object property if key is absent.
   * @return true if key already existed; false if inserted.
   * @throws cru::Exception if this value is not an object.
   */
  bool Insert(std::string key, JsonValue value);

  /**
   * @brief Removes an array element or object property.
   * @return The removed value, or std::nullopt if index/key is absent.
   * @throws cru::Exception if this value is not the matching container type.
   */
  std::optional<JsonValue> Remove(Index index);

  std::optional<JsonValue> Remove(std::string_view key);

  void PushBack(JsonValue value);

  /**
   * @brief Checks whether an object contains key.
   * @throws cru::Exception if this value is not an object.
   */
  bool Contains(std::string_view key) const;

  JsonValue* TryGet(std::string_view key);

  const JsonValue* TryGet(std::string_view key) const;

  iterator begin();

  iterator end();

  const_iterator begin() const;

  const_iterator end() const;

  const_iterator cbegin() const;

  const_iterator cend() const;

  /**
   * @brief Iterates over object property names.
   * @throws cru::Exception if this value is not an object.
   */
  auto Keys() const {
    return std::views::transform(
        RequireObject(), [](const ObjectItem& item) -> const std::string& {
          return item.first;
        });
  }

  /**
   * @brief Iterates over object properties.
   * @throws cru::Exception if this value is not an object.
   */
  auto Items() {
    return std::views::transform(
        RequireObject(),
        [](ObjectItem& item) -> std::pair<const std::string&, JsonValue&> {
          return {item.first, item.second};
        });
  }

  /**
   * @brief Iterates over object properties.
   * @throws cru::Exception if this value is not an object.
   */
  auto Items() const {
    return std::views::transform(
        RequireObject(),
        [](const ObjectItem& item)
            -> std::pair<const std::string&, const JsonValue&> {
          return {item.first, item.second};
        });
  }

 private:
  void CheckType(JsonValueType type) const;
  JsonValue& GetIteratedValue(Index index);
  const JsonValue& GetIteratedValue(Index index) const;
  Index GetIterationSize() const;
  ArrayStorage& RequireArray();
  const ArrayStorage& RequireArray() const;
  ObjectStorage& RequireObject();
  const ObjectStorage& RequireObject() const;

  Storage storage_ = nullptr;
};

class CRU_BASE_API JsonParser {
 public:
  explicit JsonParser(std::string source);

  JsonValue Parse();

 private:
  [[noreturn]] void Error(std::string_view message) const;

  bool IsEnd() const;
  static bool IsJsonSpace(char c);
  void ReadAllSpace();
  char PeekNext();
  char ReadNext();
  void ReadExpected(std::string_view expected);

  std::nullptr_t ParseNull();
  bool ParseBoolean();
  double ParseNumber();
  std::string ParseString();
  JsonValue ParseArray();
  JsonValue ParseObject();

  /**
   * @brief Parses a JSON value.
   * @return The parsed value.
   * @throws cru::Exception if the JSON is invalid.
   *
   * Caller should call ReadAllSpace() before calling this function.
   */
  JsonValue ParseValue();

 private:
  std::string source_;

  /**
   * @brief Always points to the next character to be parsed.
   */
  Index position_;
};

CRU_BASE_API JsonValue ParseJson(std::string source);
}  // namespace cru::json
