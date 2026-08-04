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

JsonValue::~JsonValue() { DestroyChildren(); }

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
  type_ = JsonValueType::Null;
  value_ = nullptr;
}

void JsonValue::CheckTypeForGet(JsonValueType type) const {
  if (type != type_) {
    throw Exception(std::format("Json value is not of type {}, but {}.",
                                ToString(type), ToString(type_)));
  }
}

void JsonValue::DestroyChildren() {
  if (type_ == JsonValueType::Array) {
    for (const auto& value : std::get<ArrayStorage>(value_)) {
      delete value;
    }
  } else if (type_ == JsonValueType::Object) {
    for (const auto& kv : std::get<ObjectStorage>(value_)) {
      delete kv.second;
    }
  }
  type_ = JsonValueType::Null;
  value_ = nullptr;
}

}  // namespace cru::json
