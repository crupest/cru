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
 * To iterate over the elements, use JsonArrayValue::Values().
 */
class CRU_BASE_API JsonArrayValue : public JsonValue {
  friend JsonValue;

 private:
  JsonArrayValue() = default;

 public:
  ~JsonArrayValue();

  JsonValueType GetType() const override;

  Index GetSize() const;
  JsonValue* GetValueAt(Index index);
  const JsonValue* GetValueAt(Index index) const;
  JsonValue*& operator[](Index index);
  const JsonValue* const& operator[](Index index) const;
  auto Values() { return std::views::all(children_); }
  auto Values() const {
    return std::views::transform(children_, [](JsonValue* child) {
      return static_cast<const JsonValue*>(child);
    });
  }

  void AddValue(JsonValue* value);
  void AddValueAt(Index index, JsonValue* value);

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

  bool TryAdd(std::string key, JsonValue* value);
  JsonValue* TryRemove(std::string_view key);

  JsonObjectValue* Clone() const override;

 private:
  std::vector<std::pair<std::string, JsonValue*>> children_;
};

// TODO: Unit tests.

}  // namespace cru::json
