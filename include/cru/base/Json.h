#pragma once

#include "Base.h"

#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cru::json {

enum class JsonValueType { Null, Boolean, Number, String, Array, Object };

CRU_BASE_API std::string ToString(JsonValueType type);

template <JsonValueType type, typename T>
class JsonScalarValue;

using JsonNullValue = JsonScalarValue<JsonValueType::Null, std::nullptr_t>;
using JsonBooleanValue = JsonScalarValue<JsonValueType::Boolean, bool>;
using JsonNumberValue = JsonScalarValue<JsonValueType::Number, double>;
using JsonStringValue = JsonScalarValue<JsonValueType::String, std::string>;
class JsonArrayValue;
class JsonObjectValue;

class CRU_BASE_API JsonValue : public Object {
 public:
  virtual JsonValueType GetType() const = 0;

  bool IsNull() const { return GetType() == JsonValueType::Null; }
  bool IsBoolean() const { return GetType() == JsonValueType::Boolean; }
  bool IsNumber() const { return GetType() == JsonValueType::Number; }
  bool IsString() const { return GetType() == JsonValueType::String; }
  bool IsArray() const { return GetType() == JsonValueType::Array; }
  bool IsObject() const { return GetType() == JsonValueType::Object; }

  JsonNullValue* AsNull();
  const JsonNullValue* AsNull() const;
  JsonBooleanValue* AsBoolean();
  const JsonBooleanValue* AsBoolean() const;
  JsonNumberValue* AsNumber();
  const JsonNumberValue* AsNumber() const;
  JsonStringValue* AsString();
  const JsonStringValue* AsString() const;
  JsonArrayValue* AsArray();
  const JsonArrayValue* AsArray() const;
  JsonObjectValue* AsObject();
  const JsonObjectValue* AsObject() const;

  virtual JsonValue* Clone() const = 0;

  static JsonNullValue* CreateNull(std::nullptr_t value = nullptr);
  static JsonBooleanValue* CreateBoolean(bool value = false);
  static JsonNumberValue* CreateNumber(double value = 0.0);
  static JsonStringValue* CreateString(std::string value = "");
  static JsonArrayValue* CreateArray();
  static JsonObjectValue* CreateObject();

 private:
  void CheckType(JsonValueType type) const;
};

template <JsonValueType type, typename T>
class JsonScalarValue : public JsonValue {
  friend JsonValue;

 private:
  explicit JsonScalarValue(T value) : value_(std::move(value)) {}

 public:
  JsonValueType GetType() const override { return type; }

  T GetValue() const { return value_; }
  void SetValue(T value) { value_ = std::move(value); }

  JsonScalarValue* Clone() const override {
    return new JsonScalarValue(value_);
  }

 private:
  T value_;
};

extern CRU_BASE_API template class JsonScalarValue<JsonValueType::Null,
                                                   std::nullptr_t>;
extern CRU_BASE_API template class JsonScalarValue<JsonValueType::Boolean,
                                                   bool>;
extern CRU_BASE_API template class JsonScalarValue<JsonValueType::Number,
                                                   double>;
extern CRU_BASE_API template class JsonScalarValue<JsonValueType::String,
                                                   std::string>;

/**
 * @brief A JSON array value.
 *
 * Owns its elements and deletes them in its destructor.
 *
 * Use JsonArrayValue::Values() to iterate over the elements.
 *
 * @remark Be careful when using APIs that assign element pointers directly,
 * such as operator[] or a non-const Values() iterator. Save and delete, or
 * otherwise take ownership of, the old pointer before assigning a new one;
 * otherwise, the old value may leak. Do not assign nullptr. Prefer Add(),
 * AddAt(), SetAt(), and RemoveAt() when possible.
 */
class CRU_BASE_API JsonArrayValue : public JsonValue {
  friend JsonValue;

 private:
  JsonArrayValue() = default;

 public:
  ~JsonArrayValue();

  JsonValueType GetType() const override;

  Index GetSize() const;
  /**
   * @brief Gets the element at an index.
   * @param index The index of the element to get. Must be valid.
   * @return A reference to the stored element pointer.
   *
   * The non-const overload returns a writable reference, matching operator[].
   */
  JsonValue*& GetAt(Index index);
  const JsonValue* const& GetAt(Index index) const;
  JsonValue*& operator[](Index index);
  const JsonValue* const& operator[](Index index) const;
  auto Values() { return std::views::all(children_); }
  auto Values() const {
    return std::views::transform(
        children_, [](JsonValue* const& child) -> const JsonValue* const& {
          return child;
        });
  }

  /**
   * @brief Appends an element to the array.
   * @param value The element to append. Ownership is transferred to the array.
   */
  void Add(JsonValue* value);

  /**
   * @brief Inserts an element at an index.
   * @param index The index where the element should be inserted. It may be
   * equal to GetSize() to append.
   * @param value The element to insert. Ownership is transferred to the array.
   */
  void AddAt(Index index, JsonValue* value);

  /**
   * @brief Replaces the element at an index.
   * @param index The index of the element to replace. Must be valid.
   * @param value The new element. Ownership is transferred to the array.
   *
   * The previous element at the index is deleted before the new element is
   * stored.
   */
  void SetAt(Index index, JsonValue* value);

  /**
   * @brief Removes an element from the array and releases its ownership.
   * @param index The index of the element to remove. Must be a valid index.
   * @return The removed element.
   *
   * After removal, ownership of the element is transferred to the caller.
   */
  JsonValue* RemoveAt(Index index);

  JsonArrayValue* Clone() const override;

 private:
  std::vector<JsonValue*> children_;
};

/**
 * @brief A JSON object value.
 *
 * Owns its child values and deletes them in its destructor.
 *
 * Items are stored in insertion order.
 *
 * Use JsonObjectValue::Keys(), JsonObjectValue::Values(), and
 * JsonObjectValue::Items() to iterate over keys, values, and key-value pairs.
 *
 * @remark Be careful when using APIs that assign child-value pointers directly,
 * such as operator[] or a non-const Values() or Items() iterator. Save and
 * delete, or otherwise take ownership of, the old pointer before assigning a
 * new one; otherwise, the old value may leak. Do not assign nullptr. Prefer
 * TryAdd(), Set(), and TryRemove() when possible.
 */
class CRU_BASE_API JsonObjectValue : public JsonValue {
  friend JsonValue;

 private:
  JsonObjectValue() = default;

 public:
  ~JsonObjectValue();

  JsonValueType GetType() const override;

  Index GetSize() const;
  JsonValue*& GetValue(std::string_view key);
  const JsonValue* const& GetValue(std::string_view key) const;
  JsonValue* GetOptionalValue(std::string_view key);
  const JsonValue* GetOptionalValue(std::string_view key) const;
  bool ContainsKey(std::string_view key) const;
  JsonValue*& operator[](std::string_view key);

  auto Keys() const {
    return std::views::transform(
        children_,
        [](const std::pair<std::string, JsonValue*>& item)
            -> const std::string& { return item.first; });
  }

  auto Values() {
    return std::views::transform(
        children_, [](std::pair<std::string, JsonValue*>& item) -> JsonValue*& {
          return item.second;
        });
  }

  auto Values() const {
    return std::views::transform(
        children_,
        [](const std::pair<std::string, JsonValue*>& item)
            -> const JsonValue* const& { return item.second; });
  }

  auto Items() {
    return std::views::transform(
        children_,
        [](std::pair<std::string, JsonValue*>& item)
            -> std::pair<const std::string&, JsonValue*&> { return item; });
  }

  auto Items() const {
    return std::views::transform(
        children_,
        [](const std::pair<std::string, JsonValue*>& item)
            -> std::pair<const std::string&, const JsonValue* const&> {
          return item;
        });
  }

  /**
   * @brief Inserts a value for a new key.
   * @param key The key to insert.
   * @param value The value to insert. Ownership is transferred only when the
   * insertion succeeds.
   * @return true if the value is inserted; false if the key already exists.
   *
   * If the key already exists, the value is not inserted, the existing value is
   * not replaced, and the object is left unchanged.
   */
  bool TryAdd(std::string key, JsonValue* value);

  /**
   * @brief Removes the value for a key and releases ownership of it.
   * @param key The key of the value to remove. The key may be absent.
   * @return The removed value, or nullptr if the key does not exist.
   *
   * When a value is removed, ownership is transferred to the caller.
   */
  JsonValue* TryRemove(std::string_view key);

  /**
   * @brief Sets the value for a key.
   * @param key The key whose value should be set. The key may be absent.
   * @param value The new value. Ownership is transferred to the object.
   *
   * If the key already exists, the old value is deleted and replaced. If the
   * key is absent, a new item is appended in insertion order.
   */
  void Set(std::string_view key, JsonValue* value);

  JsonObjectValue* Clone() const override;

 private:
  std::vector<std::pair<std::string, JsonValue*>> children_;
};
}  // namespace cru::json
