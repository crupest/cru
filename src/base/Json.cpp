#include "cru/base/Json.h"

#include <format>
#include <utility>

namespace cru::json {
std::string ToString(JsonValueType type) {
  switch (type) {
    case JsonValueType::Null:
      return "Null";
    case JsonValueType::Boolean:
      return "Boolean";
    case JsonValueType::Number:
      return "Number";
    case JsonValueType::String:
      return "String";
    case JsonValueType::Array:
      return "Array";
    case JsonValueType::Object:
      return "Object";
    default:
      std::unreachable();
  }
}

JsonValue::JsonValue() : parent_(nullptr) {}

JsonValue::~JsonValue() { DestroyChildren(); }

JsonValueType JsonValue::GetType() const {
  switch (value_.index()) {
    case 0:
      return JsonValueType::Null;
    case 1:
      return JsonValueType::Boolean;
    case 2:
      return JsonValueType::Number;
    case 3:
      return JsonValueType::String;
    case 4:
      return JsonValueType::Array;
    case 5:
      return JsonValueType::Object;
    default:
      std::unreachable();
  }
}

std::nullptr_t JsonValue::GetNull() const {
  CheckTypeForGet(JsonValueType::Null);
  return nullptr;
}

bool JsonValue::GetBoolean() const {
  CheckTypeForGet(JsonValueType::Boolean);
  return std::get<bool>(value_);
}

double JsonValue::GetNumber() const {
  CheckTypeForGet(JsonValueType::Number);
  return std::get<double>(value_);
}

const std::string& JsonValue::GetString() const {
  CheckTypeForGet(JsonValueType::String);
  return std::get<std::string>(value_);
}

const JsonValue::ArrayStorage& JsonValue::GetArray() const {
  CheckTypeForGet(JsonValueType::Array);
  return std::get<ArrayStorage>(value_);
}

const JsonValue::ObjectStorage& JsonValue::GetObject() const {
  CheckTypeForGet(JsonValueType::Object);
  return std::get<ObjectStorage>(value_);
}

void JsonValue::SetNull() {
  DestroyChildren();
  value_ = nullptr;
}

void JsonValue::SetBoolean(bool value) {
  DestroyChildren();
  value_ = value;
}

void JsonValue::SetNumber(double value) {
  DestroyChildren();
  value_ = value;
}

void JsonValue::SetString(std::string value) {
  DestroyChildren();
  value_ = std::move(value);
}

void JsonValue::CheckTypeForGet(JsonValueType type) const {
  if (type != GetType()) {
    throw Exception(std::format("Json value is not of type {}, but {}.",
                                ToString(type), ToString(GetType())));
  }
}

void JsonValue::DestroyChildren() {
  std::visit(
      [](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, ArrayStorage>) {
          for (const auto& value : arg) {
            delete value;
          }
        } else if constexpr (std::is_same_v<T, ObjectStorage>) {
          for (const auto& kv : arg) {
            delete kv.second;
          }
        }
      },
      value_);
  value_ = nullptr;
}

}  // namespace cru::json
