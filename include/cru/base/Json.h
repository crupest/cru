#pragma once

#include "Base.h"

#include <cstddef>
#include <format>
#include <ranges>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace cru::json {

enum class JsonValueType { Null, Boolean, Number, String, Array, Object };

CRU_BASE_API std::string ToString(JsonValueType type);

class CRU_BASE_API JsonValue : public Object {
 public:
  using ArrayStorage = std::vector<JsonValue*>;
  using ObjectStorage = std::vector<std::pair<std::string, JsonValue*>>;

  JsonValue();
  ~JsonValue() override;

  JsonValueType GetType() const;

  bool IsNull() const { return GetType() == JsonValueType::Null; }
  bool IsBoolean() const { return GetType() == JsonValueType::Boolean; }
  bool IsNumber() const { return GetType() == JsonValueType::Number; }
  bool IsString() const { return GetType() == JsonValueType::String; }
  bool IsArray() const { return GetType() == JsonValueType::Array; }
  bool IsObject() const { return GetType() == JsonValueType::Object; }

  std::nullptr_t GetNull() const;
  bool GetBoolean() const;
  double GetNumber() const;
  const std::string& GetString() const;
  const ArrayStorage& GetArray() const;
  const ObjectStorage& GetObject() const;

  void SetNull();
  void SetBoolean(bool value);
  void SetNumber(double value);
  void SetString(std::string value);

  template <typename... T>
    requires std::is_constructible_v<ArrayStorage, T...>
  void SetArray(T&&... args) {
    DestroyChildren();
    ArrayStorage value(std::forward<T>(args)...);
    for (auto [index, child] : std::views::enumerate(value)) {
      if (child->parent_) {
        throw Exception(
            std::format("Child at index {} already has a parent.", index));
      }
    }
    for (auto child : value) {
      child->parent_ = this;
    }
    value_ = std::move(value);
  }

  // TODO: need to handle duplicate key.
  template <typename... T>
    requires std::is_constructible_v<ObjectStorage, T...>
  void SetObject(T&&... args) {
    DestroyChildren();
    ObjectStorage value(std::forward<T>(args)...);
    for (const auto& [key, child] : value) {
      if (child->parent_) {
        throw Exception(
            std::format("Child of key {} already has a parent.", key));
      }
    }
    for (const auto& [key, child] : value) {
      child->parent_ = this;
    }
    value_ = std::move(value);
  }

 private:
  void CheckTypeForGet(JsonValueType type) const;
  void DestroyChildren();

 private:
  JsonValue* parent_;
  std::variant<std::nullptr_t, bool, double, std::string, ArrayStorage,
               ObjectStorage>
      value_;
};
}  // namespace cru::json
