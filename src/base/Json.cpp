#include "cru/base/Json.h"

#include <concepts>
#include <format>
#include <ranges>
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

std::nullptr_t& JsonValue::AsNull() {
  CheckTypeForGet(JsonValueType::Null);
  return std::get<std::nullptr_t>(value_);
}

const std::nullptr_t& JsonValue::AsNull() const {
  CheckTypeForGet(JsonValueType::Null);
  return std::get<std::nullptr_t>(value_);
}

bool& JsonValue::AsBoolean() {
  CheckTypeForGet(JsonValueType::Boolean);
  return std::get<bool>(value_);
}

const bool& JsonValue::AsBoolean() const {
  CheckTypeForGet(JsonValueType::Boolean);
  return std::get<bool>(value_);
}

double& JsonValue::AsNumber() {
  CheckTypeForGet(JsonValueType::Number);
  return std::get<double>(value_);
}

const double& JsonValue::AsNumber() const {
  CheckTypeForGet(JsonValueType::Number);
  return std::get<double>(value_);
}

std::string& JsonValue::AsString() {
  CheckTypeForGet(JsonValueType::String);
  return std::get<std::string>(value_);
}

const std::string& JsonValue::AsString() const {
  CheckTypeForGet(JsonValueType::String);
  return std::get<std::string>(value_);
}

auto JsonValue::AsArray() {
  CheckTypeForGet(JsonValueType::Array);
  auto result = std::views::all(std::get<ArrayStorage>(value_));
  static_assert(
      std::same_as<std::ranges::range_value_t<decltype(result)>, JsonValue*>);
  return result;
}

auto JsonValue::AsArray() const {
  CheckTypeForGet(JsonValueType::Array);
  auto result = std::get<ArrayStorage>(value_) |
                std::views::transform([](JsonValue* child) {
                  return static_cast<const JsonValue*>(child);
                });
  static_assert(std::same_as<std::ranges::range_value_t<decltype(result)>,
                             const JsonValue*>);
  return result;
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
