#include "cru/base/Json.h"

#include <format>
#include <ranges>
#include <utility>
#include "cru/base/Base.h"

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

JsonValue::JsonValue(JsonValueType type) : type_(type) {}

JsonValueType JsonValue::GetType() const { return type_; }

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

JsonArrayValue::JsonArrayValue() : JsonValue(JsonValueType::Array) {}

JsonArrayValue::~JsonArrayValue() {
  for (auto child : children_) {
    delete child;
  }
}

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

}  // namespace cru::json
