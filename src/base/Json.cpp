#include "cru/base/Json.h"
#include "cru/base/Base.h"

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

JsonNullValue* JsonValue::AsNull() {
  CheckType(JsonValueType::Null);
  return static_cast<JsonNullValue*>(this);
}

const JsonNullValue* JsonValue::AsNull() const {
  CheckType(JsonValueType::Null);
  return static_cast<const JsonNullValue*>(this);
}

JsonBooleanValue* JsonValue::AsBoolean() {
  CheckType(JsonValueType::Boolean);
  return static_cast<JsonBooleanValue*>(this);
}

const JsonBooleanValue* JsonValue::AsBoolean() const {
  CheckType(JsonValueType::Boolean);
  return static_cast<const JsonBooleanValue*>(this);
}

JsonNumberValue* JsonValue::AsNumber() {
  CheckType(JsonValueType::Number);
  return static_cast<JsonNumberValue*>(this);
}

const JsonNumberValue* JsonValue::AsNumber() const {
  CheckType(JsonValueType::Number);
  return static_cast<const JsonNumberValue*>(this);
}

JsonStringValue* JsonValue::AsString() {
  CheckType(JsonValueType::String);
  return static_cast<JsonStringValue*>(this);
}

const JsonStringValue* JsonValue::AsString() const {
  CheckType(JsonValueType::String);
  return static_cast<const JsonStringValue*>(this);
}

JsonArrayValue* JsonValue::AsArray() {
  CheckType(JsonValueType::Array);
  return static_cast<JsonArrayValue*>(this);
}

const JsonArrayValue* JsonValue::AsArray() const {
  CheckType(JsonValueType::Array);
  return static_cast<const JsonArrayValue*>(this);
}

JsonObjectValue* JsonValue::AsObject() {
  CheckType(JsonValueType::Object);
  return static_cast<JsonObjectValue*>(this);
}

const JsonObjectValue* JsonValue::AsObject() const {
  CheckType(JsonValueType::Object);
  return static_cast<const JsonObjectValue*>(this);
}

void JsonValue::CheckType(JsonValueType type) const {
  if (type != GetType()) {
    throw Exception(std::format("Json value is not of type {}, but {}.",
                                ToString(type), ToString(GetType())));
  }
}

template class JsonScalarValue<JsonValueType::Null, std::nullptr_t>;
template class JsonScalarValue<JsonValueType::Boolean, bool>;
template class JsonScalarValue<JsonValueType::Number, double>;
template class JsonScalarValue<JsonValueType::String, std::string>;

JsonArrayValue::~JsonArrayValue() {
  for (auto child : children_) {
    delete child;
  }
}

JsonValueType JsonArrayValue::GetType() const { return JsonValueType::Array; }

Index JsonArrayValue::GetSize() const {
  return static_cast<Index>(children_.size());
}

JsonValue* JsonArrayValue::GetValueAt(Index index) { return operator[](index); }

const JsonValue* JsonArrayValue::GetValueAt(Index index) const {
  return operator[](index);
}

JsonValue*& JsonArrayValue::operator[](Index index) {
  CheckArgumentRange(index, 0, GetSize());
  return children_[index];
}

const JsonValue* const& JsonArrayValue::operator[](Index index) const {
  CheckArgumentRange(index, 0, GetSize());
  return static_cast<const JsonValue* const&>(children_[index]);
}

auto JsonArrayValue::Values() { return std::views::all(children_); }

auto JsonArrayValue::Values() const {
  return std::views::transform(children_, [](JsonValue* child) {
    return static_cast<const JsonValue*>(child);
  });
}

void JsonArrayValue::AddValue(JsonValue* value) { children_.push_back(value); }

void JsonArrayValue::AddValueAt(Index index, JsonValue* value) {
  CheckArgumentRange(index, 0, GetSize() + 1);
  children_.insert(children_.begin() + index, value);
}

JsonValue* JsonArrayValue::RemoveAt(Index index) {
  CheckArgumentRange(index, 0, GetSize());
  auto value = children_[index];
  children_.erase(children_.begin() + index);
  return value;
}

JsonArrayValue* JsonArrayValue::Clone() const {
  auto new_value = new JsonArrayValue();
  for (auto child : children_) {
    new_value->AddValue(child->Clone());
  }
  return new_value;
}

JsonObjectValue::~JsonObjectValue() {
  for (const auto& [k, v] : children_) {
    delete v;
  }
}

JsonValueType JsonObjectValue::GetType() const { return JsonValueType::Object; }

Index JsonObjectValue::GetSize() const {
  return static_cast<Index>(children_.size());
}

JsonValue*& JsonObjectValue::GetValue(std::string_view key) {
  for (auto& [k, v] : children_) {
    if (k == key) {
      return v;
    }
  }
  throw Exception(std::format("Object doesn't have key '{}'.", key));
}

const JsonValue* const& JsonObjectValue::GetValue(std::string_view key) const {
  for (const auto& [k, v] : children_) {
    if (k == key) {
      return static_cast<const JsonValue* const&>(v);
    }
  }
  throw Exception(std::format("Object doesn't have key '{}'.", key));
}

JsonValue* JsonObjectValue::GetOptionalValue(std::string_view key) {
  for (const auto& [k, v] : children_) {
    if (k == key) {
      return v;
    }
  }
  return nullptr;
}

const JsonValue* JsonObjectValue::GetOptionalValue(std::string_view key) const {
  for (const auto& [k, v] : children_) {
    if (k == key) {
      return static_cast<const JsonValue*>(v);
    }
  }
  return nullptr;
}

bool JsonObjectValue::ContainsKey(std::string_view key) const {
  return GetOptionalValue(key) != nullptr;
}

JsonValue*& JsonObjectValue::operator[](std::string_view key) {
  for (auto& [k, v] : children_) {
    if (k == key) {
      return v;
    }
  }
  return children_.emplace_back(key, new JsonNullValue()).second;
}

bool JsonObjectValue::TryAdd(std::string key, JsonValue* value) {
  if (ContainsKey(key)) return false;

  children_.emplace_back(std::move(key), value);
  return true;
}

JsonValue* JsonObjectValue::TryRemove(std::string_view key) {
  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    if (iter->first == key) {
      auto value = iter->second;
      children_.erase(iter);
      return value;
    }
  }
  return nullptr;
}

JsonObjectValue* JsonObjectValue::Clone() const {
  auto new_value = new JsonObjectValue();
  for (const auto& [k, v] : children_) {
    new_value->TryAdd(k, v->Clone());
  }
  return new_value;
}
}  // namespace cru::json
