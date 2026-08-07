#pragma once

#include "Base.h"

#include <cstddef>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace cru::json {

enum class JsonValueType { Null, Boolean, Number, String, Array, Object };

CRU_BASE_API std::string ToString(JsonValueType type);

/**
 * @brief A JSON value with value semantics.
 *
 * JsonValue stores null, boolean, number, string, array, and object values in a
 * private variant. Arrays and objects are internal storage details; use the
 * public Get, Set, Insert, Remove, operator[], and iterator APIs to access
 * them.
 *
 * Copying a JsonValue deep-copies array and object contents. Moving a JsonValue
 * transfers the value and resets the moved-from value to JSON null.
 */
class CRU_BASE_API JsonValue {
 private:
  using ArrayStorage = std::vector<JsonValue>;
  using ObjectItem = std::pair<std::string, JsonValue>;
  using ObjectStorage = std::vector<ObjectItem>;

  using Storage = std::variant<std::nullptr_t, bool, double, std::string,
                               ArrayStorage, ObjectStorage>;

 public:
  JsonValue();
  JsonValue(std::nullptr_t value);
  JsonValue(bool value);
  JsonValue(double value);
  JsonValue(std::string value);
  JsonValue(const char* value);

  template <typename T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>> &&
             !std::is_same_v<std::remove_cvref_t<T>, bool>)
  JsonValue(T value) : JsonValue(static_cast<double>(value)) {}

  JsonValue(const JsonValue& other) = default;
  JsonValue& operator=(const JsonValue& other) = default;
  JsonValue(JsonValue&& other) noexcept;
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

  /**
   * @brief Gets nullptr if value is null.
   * @throws cru::Exception if the value is not null.
   */
  std::nullptr_t& GetNull();

  /**
   * @brief Gets nullptr if value is null.
   * @throws cru::Exception if the value is not null.
   */
  const std::nullptr_t& GetNull() const;

  /**
   * @brief Gets bool if value is boolean type.
   * @throws cru::Exception if the value is not boolean type.
   */
  bool& GetBoolean();

  /**
   * @brief Gets bool if value is boolean type.
   * @throws cru::Exception if the value is not boolean type.
   */
  const bool& GetBoolean() const;

  /**
   * @brief Gets double if value is number type.
   * @throws cru::Exception if the value is not number type.
   */
  double& GetNumber();

  /**
   * @brief Gets double if value is number type.
   * @throws cru::Exception if the value is not number type.
   */
  const double& GetNumber() const;

  /**
   * @brief Gets string if value is string type.
   * @throws cru::Exception if the value is not string type.
   */
  std::string& GetString();

  /**
   * @brief Gets string if value is string type.
   * @throws cru::Exception if the value is not string type.
   */
  const std::string& GetString() const;

  /**
   * @brief Sets this value to null.
   *
   * Scalar Set overloads replace the whole current value and may change the
   * JSON type. They do not throw only because the previous type was different.
   */
  void Set(std::nullptr_t value);

  /**
   * @brief Sets this value to a boolean, replacing the previous value.
   */
  void Set(bool value);

  /**
   * @brief Sets this value to a number, replacing the previous value.
   */
  void Set(double value);

  template <typename T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>> &&
             !std::is_same_v<std::remove_cvref_t<T>, bool>)
  void Set(T value) {
    Set(static_cast<double>(value));
  }

  /**
   * @brief Sets this value to a string, replacing the previous value.
   */
  void Set(std::string value);

  /**
   * @brief Sets this value to a string, replacing the previous value.
   * @throws cru::Exception if value is nullptr. Use Set(nullptr) for JSON null.
   */
  void Set(const char* value);

  /**
   * @brief Sets this value to an empty array, replacing the previous value.
   */
  void SetArray();

  /**
   * @brief Sets this value to an empty object, replacing the previous value.
   */
  void SetObject();

  /**
   * @brief Scalar assignment equivalent to Set(nullptr).
   */
  JsonValue& operator=(std::nullptr_t value);

  /**
   * @brief Scalar assignment equivalent to Set(bool).
   */
  JsonValue& operator=(bool value);

  /**
   * @brief Scalar assignment equivalent to Set(double).
   */
  JsonValue& operator=(double value);

  template <typename T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>> &&
             !std::is_same_v<std::remove_cvref_t<T>, bool>)
  JsonValue& operator=(T value) {
    Set(value);
    return *this;
  }

  /**
   * @brief Scalar assignment equivalent to Set(std::string).
   */
  JsonValue& operator=(std::string value);

  /**
   * @brief Scalar assignment equivalent to Set(const char*).
   */
  JsonValue& operator=(const char* value);

  /**
   * @brief Gets the size of an array or object.
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
   * @brief Gets an array element, equivalent to Get(index).
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  JsonValue& operator[](Index index);
  const JsonValue& operator[](Index index) const;

  /**
   * @brief Gets or creates an object property.
   *
   * The non-const overload requires object type. If key exists, it returns the
   * existing value. If key is absent, it appends a null value for key and
   * returns it.
   *
   * The const overload is equivalent to Get(key), and throws when key is
   * absent.
   *
   * @throws cru::Exception if this value is not an object.
   */
  JsonValue& operator[](std::string_view key);
  const JsonValue& operator[](std::string_view key) const;

  /**
   * @brief Replaces an existing array element.
   * @return Always true when the operation succeeds.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  bool Set(Index index, JsonValue value);

  /**
   * @brief Sets an object property.
   *
   * If key exists, the old value is replaced. If key is absent, a new property
   * is appended in insertion order.
   *
   * @return true if an old key existed and was replaced; false if a new key was
   * added.
   * @throws cru::Exception if this value is not an object.
   */
  bool Set(std::string_view key, JsonValue value);

  /**
   * @brief Inserts an array element before index.
   *
   * index may be equal to GetSize() to append.
   *
   * @return Always true when the operation succeeds.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  bool Insert(Index index, JsonValue value);

  /**
   * @brief Inserts an object property if key is absent.
   *
   * If key exists, the object is left unchanged.
   *
   * @return true if an old key already existed; false if a new key was
   * inserted.
   * @throws cru::Exception if this value is not an object.
   */
  bool Insert(std::string key, JsonValue value);

  /**
   * @brief Removes an array element.
   *
   * If index is valid, the removed value is moved into the returned optional.
   * If index is invalid, no value is removed and std::nullopt is returned.
   *
   * @throws cru::Exception if this value is not an array.
   */
  std::optional<JsonValue> Remove(Index index);

  /**
   * @brief Removes an object property.
   *
   * If key exists, the removed value is moved into the returned optional. If
   * key is absent, no value is removed and std::nullopt is returned.
   *
   * @throws cru::Exception if this value is not an object.
   */
  std::optional<JsonValue> Remove(std::string_view key);

  /**
   * @brief Returns whether an object contains key.
   * @throws cru::Exception if this value is not an object.
   */
  bool Contains(std::string_view key) const;

  /**
   * @brief Gets an object property pointer, or nullptr when key is absent.
   * @throws cru::Exception if this value is not an object.
   */
  JsonValue* TryGet(std::string_view key);
  const JsonValue* TryGet(std::string_view key) const;

  /**
   * @brief Iterates over array values.
   * @throws cru::Exception if this value is not an array.
   */
  auto Values() { return std::views::all(RequireArray()); }
  auto Values() const { return std::views::all(RequireArray()); }

  /**
   * @brief Iterates over object keys.
   * @throws cru::Exception if this value is not an object.
   */
  auto Keys() const {
    return std::views::transform(
        RequireObject(), [](const ObjectItem& item) -> const std::string& {
          return item.first;
        });
  }

  /**
   * @brief Iterates over object key/value pairs.
   * @throws cru::Exception if this value is not an object.
   */
  auto Items() {
    return std::views::transform(
        RequireObject(),
        [](ObjectItem& item) -> std::pair<const std::string&, JsonValue&> {
          return {item.first, item.second};
        });
  }

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
  ArrayStorage& RequireArray();
  const ArrayStorage& RequireArray() const;
  ObjectStorage& RequireObject();
  const ObjectStorage& RequireObject() const;

  Storage storage_ = nullptr;
};
}  // namespace cru::json
