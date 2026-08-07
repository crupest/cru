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

/**
 * @brief A value-semantic JSON value.
 *
 * JsonValue can hold any JSON kind: null, boolean, number, string, array, or
 * object. It does not use inheritance or owning child pointers. Copying a
 * JsonValue deep-copies nested arrays and objects; moving a JsonValue transfers
 * the stored value and resets the moved-from value to JSON null.
 *
 * Arrays and objects are intentionally exposed only through JsonValue's public
 * API. Arrays are ordered sequences addressed by zero-based Index values.
 * Objects are insertion-ordered key/value collections addressed by property
 * name.
 *
 * Scalar APIs use named getters, such as GetBoolean() and GetString(), and
 * scalar Set overloads or assignment operators replace the entire current value
 * even when the previous type differs. Container APIs, such as Get(Index),
 * Set(key, value), Insert(...), Remove(...), Values(), Keys(), and Items(),
 * require the value to already be the matching container type and throw
 * cru::Exception on type mismatch.
 *
 * References, pointers, iterators, and ranges returned by this class refer to
 * internal storage. Treat them as invalid after assigning or moving the owning
 * JsonValue, calling SetArray() or SetObject(), or structurally modifying the
 * same array/object with Insert() or Remove().
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
   * @brief Creates a JSON null value.
   */
  JsonValue();

  /**
   * @brief Creates a JSON null value.
   */
  JsonValue(std::nullptr_t value);

  /**
   * @brief Creates a JSON boolean value.
   */
  JsonValue(bool value);

  /**
   * @brief Creates a JSON number value.
   */
  JsonValue(double value);

  /**
   * @brief Creates a JSON string value by moving value.
   */
  JsonValue(std::string value);

  /**
   * @brief Creates a JSON string value from a null-terminated string.
   * @throws cru::Exception if value is nullptr. Use JsonValue(nullptr) for JSON
   * null.
   */
  JsonValue(const char* value);

  /**
   * @brief Creates a JSON number value from any non-bool arithmetic value.
   */
  template <typename T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>> &&
             !std::is_same_v<std::remove_cvref_t<T>, bool>)
  JsonValue(T value) : JsonValue(static_cast<double>(value)) {}

  /**
   * @brief Deep-copies the JSON value.
   */
  JsonValue(const JsonValue& other) = default;

  /**
   * @brief Deep-copies the JSON value.
   */
  JsonValue& operator=(const JsonValue& other) = default;

  /**
   * @brief Moves the JSON value and resets other to null.
   */
  JsonValue(JsonValue&& other) noexcept;

  /**
   * @brief Moves the JSON value and resets other to null.
   */
  JsonValue& operator=(JsonValue&& other) noexcept;

  ~JsonValue() = default;

  /**
   * @brief Creates an empty JSON array value.
   */
  static JsonValue Array();

  /**
   * @brief Creates an empty JSON object value.
   */
  static JsonValue Object();

  /**
   * @brief Gets the currently stored JSON type.
   */
  JsonValueType GetType() const;

  bool IsNull() const { return GetType() == JsonValueType::Null; }
  bool IsBoolean() const { return GetType() == JsonValueType::Boolean; }
  bool IsNumber() const { return GetType() == JsonValueType::Number; }
  bool IsString() const { return GetType() == JsonValueType::String; }
  bool IsArray() const { return GetType() == JsonValueType::Array; }
  bool IsObject() const { return GetType() == JsonValueType::Object; }

  /**
   * @brief Gets the stored null value.
   * @return A reference to the stored nullptr token.
   * @throws cru::Exception if this value is not null.
   */
  std::nullptr_t& GetNull();

  /**
   * @brief Gets the stored null value.
   * @return A reference to the stored nullptr token.
   * @throws cru::Exception if this value is not null.
   */
  const std::nullptr_t& GetNull() const;

  /**
   * @brief Gets the stored boolean value.
   * @return A mutable reference to the stored bool.
   * @throws cru::Exception if this value is not a boolean.
   */
  bool& GetBoolean();

  /**
   * @brief Gets the stored boolean value.
   * @return A const reference to the stored bool.
   * @throws cru::Exception if this value is not a boolean.
   */
  const bool& GetBoolean() const;

  /**
   * @brief Gets the stored number value.
   * @return A mutable reference to the stored double.
   * @throws cru::Exception if this value is not a number.
   */
  double& GetNumber();

  /**
   * @brief Gets the stored number value.
   * @return A const reference to the stored double.
   * @throws cru::Exception if this value is not a number.
   */
  const double& GetNumber() const;

  /**
   * @brief Gets the stored string value.
   * @return A mutable reference to the stored string.
   * @throws cru::Exception if this value is not a string.
   */
  std::string& GetString();

  /**
   * @brief Gets the stored string value.
   * @return A const reference to the stored string.
   * @throws cru::Exception if this value is not a string.
   */
  const std::string& GetString() const;

  /**
   * @brief Replaces this value with JSON null.
   *
   * Scalar Set overloads replace the whole current value and may change the JSON
   * type. They do not throw merely because the previous type was different.
   */
  void Set(std::nullptr_t value);

  /**
   * @brief Replaces this value with a JSON boolean.
   */
  void Set(bool value);

  /**
   * @brief Replaces this value with a JSON number.
   */
  void Set(double value);

  /**
   * @brief Replaces this value with a JSON number converted from value.
   */
  template <typename T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>> &&
             !std::is_same_v<std::remove_cvref_t<T>, bool>)
  void Set(T value) {
    Set(static_cast<double>(value));
  }

  /**
   * @brief Replaces this value with a JSON string by moving value.
   */
  void Set(std::string value);

  /**
   * @brief Replaces this value with a JSON string copied from value.
   * @throws cru::Exception if value is nullptr. Use Set(nullptr) for JSON null.
   */
  void Set(const char* value);

  /**
   * @brief Replaces this value with an empty JSON array.
   */
  void SetArray();

  /**
   * @brief Replaces this value with an empty JSON object.
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

  /**
   * @brief Scalar assignment equivalent to Set(value) for non-bool arithmetic
   * values.
   */
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
   * @brief Gets the number of elements in an array or properties in an object.
   * @return The array element count or object property count.
   * @throws cru::Exception if this value is not an array or object.
   */
  Index GetSize() const;

  /**
   * @brief Gets an array element.
   * @param index The zero-based element index.
   * @return A mutable reference to the element.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  JsonValue& Get(Index index);

  /**
   * @brief Gets an array element.
   * @param index The zero-based element index.
   * @return A const reference to the element.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  const JsonValue& Get(Index index) const;

  /**
   * @brief Gets an object property.
   * @param key The property name to find.
   * @return A mutable reference to the property value.
   * @throws cru::Exception if this value is not an object or key is absent.
   */
  JsonValue& Get(std::string_view key);

  /**
   * @brief Gets an object property.
   * @param key The property name to find.
   * @return A const reference to the property value.
   * @throws cru::Exception if this value is not an object or key is absent.
   */
  const JsonValue& Get(std::string_view key) const;

  /**
   * @brief Gets an array element, equivalent to Get(index).
   * @param index The zero-based element index.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  JsonValue& operator[](Index index);

  /**
   * @brief Gets an array element, equivalent to Get(index).
   * @param index The zero-based element index.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  const JsonValue& operator[](Index index) const;

  /**
   * @brief Gets or creates an object property.
   *
   * The non-const overload requires object type. If key exists, it returns the
   * existing value. If key is absent, it appends a new property whose value is
   * JSON null and returns that value.
   *
   * @param key The property name to access.
   * @return A mutable reference to the property value.
   * @throws cru::Exception if this value is not an object.
   */
  JsonValue& operator[](std::string_view key);

  /**
   * @brief Gets an object property, equivalent to Get(key).
   * @param key The property name to find.
   * @return A const reference to the property value.
   * @throws cru::Exception if this value is not an object or key is absent.
   */
  const JsonValue& operator[](std::string_view key) const;

  /**
   * @brief Replaces an existing array element.
   * @param index The zero-based element index. It must already exist.
   * @param value The replacement value.
   * @return Always true when the operation succeeds.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  bool Set(Index index, JsonValue value);

  /**
   * @brief Sets an object property, replacing or appending as needed.
   *
   * If key exists, the old value is replaced. If key is absent, a new property
   * is appended in insertion order.
   *
   * @param key The property name to set.
   * @param value The value to store.
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
   * @param index The insertion index in the range [0, GetSize()].
   * @param value The value to insert.
   * @return Always true when the operation succeeds.
   * @throws cru::Exception if this value is not an array or index is invalid.
   */
  bool Insert(Index index, JsonValue value);

  /**
   * @brief Inserts an object property only if key is absent.
   *
   * If key exists, the object is left unchanged.
   *
   * @param key The property name to insert.
   * @param value The value to insert when key is absent.
   * @return true if an old key already existed and nothing was inserted; false
   * if a new key was inserted.
   * @throws cru::Exception if this value is not an object.
   */
  bool Insert(std::string key, JsonValue value);

  /**
   * @brief Removes an array element.
   *
   * If index is valid, the removed value is moved into the returned optional. If
   * index is invalid, no value is removed and std::nullopt is returned.
   *
   * @param index The zero-based element index to remove.
   * @return The removed value, or std::nullopt if index is invalid.
   * @throws cru::Exception if this value is not an array.
   */
  std::optional<JsonValue> Remove(Index index);

  /**
   * @brief Removes an object property.
   *
   * If key exists, the removed value is moved into the returned optional. If key
   * is absent, no value is removed and std::nullopt is returned.
   *
   * @param key The property name to remove.
   * @return The removed value, or std::nullopt if key is absent.
   * @throws cru::Exception if this value is not an object.
   */
  std::optional<JsonValue> Remove(std::string_view key);

  /**
   * @brief Returns whether an object contains key.
   * @param key The property name to find.
   * @return true if key exists; false otherwise.
   * @throws cru::Exception if this value is not an object.
   */
  bool Contains(std::string_view key) const;

  /**
   * @brief Tries to get an object property.
   * @param key The property name to find.
   * @return A mutable pointer to the property value, or nullptr if key is
   * absent.
   * @throws cru::Exception if this value is not an object.
   */
  JsonValue* TryGet(std::string_view key);

  /**
   * @brief Tries to get an object property.
   * @param key The property name to find.
   * @return A const pointer to the property value, or nullptr if key is absent.
   * @throws cru::Exception if this value is not an object.
   */
  const JsonValue* TryGet(std::string_view key) const;

  /**
   * @brief Iterates over array values.
   * @return A range of mutable JsonValue references.
   * @throws cru::Exception if this value is not an array.
   */
  auto Values() { return std::views::all(RequireArray()); }

  /**
   * @brief Iterates over array values.
   * @return A range of const JsonValue references.
   * @throws cru::Exception if this value is not an array.
   */
  auto Values() const { return std::views::all(RequireArray()); }

  /**
   * @brief Iterates over object property names in insertion order.
   * @return A range of const string references.
   * @throws cru::Exception if this value is not an object.
   */
  auto Keys() const {
    return std::views::transform(
        RequireObject(), [](const ObjectItem& item) -> const std::string& {
          return item.first;
        });
  }

  /**
   * @brief Iterates over object properties in insertion order.
   * @return A range whose elements are pairs of const key reference and mutable
   * value reference.
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
   * @brief Iterates over object properties in insertion order.
   * @return A range whose elements are pairs of const key reference and const
   * value reference.
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
  ArrayStorage& RequireArray();
  const ArrayStorage& RequireArray() const;
  ObjectStorage& RequireObject();
  const ObjectStorage& RequireObject() const;

  Storage storage_ = nullptr;
};
}  // namespace cru::json
