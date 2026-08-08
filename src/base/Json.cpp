#include "cru/base/Json.h"

#include "cru/base/Base.h"

#include <algorithm>
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

namespace {
template <typename R>
auto FindObjectItem(R& range, std::string_view key) {
  return std::ranges::find_if(range,
                              [&key](auto& item) { return item.first == key; });
}
}  // namespace

JsonValue::JsonValue() : storage_(nullptr) {}

JsonValue::JsonValue(const char* value) { Set(value); }

JsonValue::JsonValue(JsonValue&& other) noexcept
    : storage_(std::move(other.storage_)) {
  other.storage_ = nullptr;
}

JsonValue& JsonValue::operator=(JsonValue&& other) noexcept {
  if (this != &other) {
    storage_ = std::move(other.storage_);
    other.storage_ = nullptr;
  }
  return *this;
}

JsonValue JsonValue::Array() {
  JsonValue value;
  value.SetArray();
  return value;
}

JsonValue JsonValue::Object() {
  JsonValue value;
  value.SetObject();
  return value;
}

JsonValueType JsonValue::GetType() const {
  switch (storage_.index()) {
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

std::nullptr_t& JsonValue::GetNull() {
  CheckType(JsonValueType::Null);
  return std::get<std::nullptr_t>(storage_);
}

const std::nullptr_t& JsonValue::GetNull() const {
  CheckType(JsonValueType::Null);
  return std::get<std::nullptr_t>(storage_);
}

bool& JsonValue::GetBoolean() {
  CheckType(JsonValueType::Boolean);
  return std::get<bool>(storage_);
}

const bool& JsonValue::GetBoolean() const {
  CheckType(JsonValueType::Boolean);
  return std::get<bool>(storage_);
}

double& JsonValue::GetNumber() {
  CheckType(JsonValueType::Number);
  return std::get<double>(storage_);
}

const double& JsonValue::GetNumber() const {
  CheckType(JsonValueType::Number);
  return std::get<double>(storage_);
}

std::string& JsonValue::GetString() {
  CheckType(JsonValueType::String);
  return std::get<std::string>(storage_);
}

const std::string& JsonValue::GetString() const {
  CheckType(JsonValueType::String);
  return std::get<std::string>(storage_);
}

void JsonValue::Set(const char* value) {
  CheckArgumentNonNull(value);
  Set(std::string(value));
}

void JsonValue::SetArray() { storage_ = ArrayStorage{}; }

void JsonValue::SetObject() { storage_ = ObjectStorage{}; }

Index JsonValue::GetSize() const {
  if (IsArray()) {
    return static_cast<Index>(std::get<ArrayStorage>(storage_).size());
  }
  if (IsObject()) {
    return static_cast<Index>(std::get<ObjectStorage>(storage_).size());
  }
  throw Exception(std::format("Json value is not an array or object, but {}.",
                              ToString(GetType())));
}

JsonValue& JsonValue::Get(Index index) { return operator[](index); }

const JsonValue& JsonValue::Get(Index index) const { return operator[](index); }

JsonValue& JsonValue::Get(std::string_view key) {
  auto& values = RequireObject();
  auto iter = FindObjectItem(values, key);
  if (iter == values.end()) {
    throw Exception(std::format("Object doesn't have key '{}'.", key));
  }
  return iter->second;
}

const JsonValue& JsonValue::Get(std::string_view key) const {
  auto& values = RequireObject();
  auto iter = FindObjectItem(values, key);
  if (iter == values.end()) {
    throw Exception(std::format("Object doesn't have key '{}'.", key));
  }
  return iter->second;
}

JsonValue& JsonValue::operator[](Index index) {
  auto& values = RequireArray();
  CheckArgumentRange(index, 0, static_cast<Index>(values.size()));
  return values[index];
}

const JsonValue& JsonValue::operator[](Index index) const {
  const auto& values = RequireArray();
  CheckArgumentRange(index, 0, static_cast<Index>(values.size()));
  return values[index];
}

JsonValue& JsonValue::operator[](std::string_view key) {
  auto& values = RequireObject();
  auto iter = FindObjectItem(values, key);
  if (iter == values.end()) {
    return values.emplace_back(key, nullptr).second;
  }
  return iter->second;
}

const JsonValue& JsonValue::operator[](std::string_view key) const {
  return Get(key);
}

bool JsonValue::Set(Index index, JsonValue value) {
  auto& values = RequireArray();
  CheckArgumentRange(index, 0, static_cast<Index>(values.size()));
  values[index] = std::move(value);
  return true;
}

bool JsonValue::Set(std::string_view key, JsonValue value) {
  auto& values = RequireObject();
  auto iter = FindObjectItem(values, key);
  if (iter == values.end()) {
    values.emplace_back(key, std::move(value));
    return false;
  }
  iter->second = std::move(value);
  return true;
}

bool JsonValue::Insert(Index index, JsonValue value) {
  auto& values = RequireArray();
  CheckArgumentRange(index, 0, static_cast<Index>(values.size()) + 1);
  values.insert(values.begin() + index, std::move(value));
  return true;
}

bool JsonValue::Insert(std::string key, JsonValue value) {
  auto& values = RequireObject();
  if (FindObjectItem(values, key) != values.end()) {
    return true;
  }
  values.emplace_back(std::move(key), std::move(value));
  return false;
}

std::optional<JsonValue> JsonValue::Remove(Index index) {
  auto& values = RequireArray();
  if (index < 0 || index >= static_cast<Index>(values.size())) {
    return std::nullopt;
  }
  std::optional<JsonValue> value = std::move(values[index]);
  values.erase(values.begin() + index);
  return value;
}

std::optional<JsonValue> JsonValue::Remove(std::string_view key) {
  auto& values = RequireObject();
  auto iter = FindObjectItem(values, key);
  if (iter == values.end()) {
    return std::nullopt;
  }
  std::optional<JsonValue> value = std::move(iter->second);
  values.erase(iter);
  return value;
}

bool JsonValue::Contains(std::string_view key) const {
  return TryGet(key) != nullptr;
}

JsonValue* JsonValue::TryGet(std::string_view key) {
  auto& values = RequireObject();
  auto iter = FindObjectItem(values, key);
  return iter == values.end() ? nullptr : &iter->second;
}

const JsonValue* JsonValue::TryGet(std::string_view key) const {
  const auto& values = RequireObject();
  auto iter = FindObjectItem(values, key);
  return iter == values.end() ? nullptr : &iter->second;
}

JsonValue::iterator JsonValue::begin() { return iterator(this, 0); }

JsonValue::iterator JsonValue::end() {
  return iterator(this, GetIterationSize());
}

JsonValue::const_iterator JsonValue::begin() const {
  return const_iterator(this, 0);
}

JsonValue::const_iterator JsonValue::end() const {
  return const_iterator(this, GetIterationSize());
}

JsonValue::const_iterator JsonValue::cbegin() const { return begin(); }

JsonValue::const_iterator JsonValue::cend() const { return end(); }

void JsonValue::CheckType(JsonValueType type) const {
  if (type != GetType()) {
    throw Exception(std::format("Json value is not of type {}, but {}.",
                                ToString(type), ToString(GetType())));
  }
}

JsonValue& JsonValue::GetIteratedValue(Index index) {
  if (IsArray()) {
    return std::get<ArrayStorage>(storage_)[index];
  }
  if (IsObject()) {
    return std::get<ObjectStorage>(storage_)[index].second;
  }
  return *this;
}

const JsonValue& JsonValue::GetIteratedValue(Index index) const {
  if (IsArray()) {
    return std::get<ArrayStorage>(storage_)[index];
  }
  if (IsObject()) {
    return std::get<ObjectStorage>(storage_)[index].second;
  }
  return *this;
}

Index JsonValue::GetIterationSize() const {
  if (IsArray() || IsObject()) {
    return GetSize();
  }
  return 1;
}

JsonValue::ArrayStorage& JsonValue::RequireArray() {
  CheckType(JsonValueType::Array);
  return std::get<ArrayStorage>(storage_);
}

const JsonValue::ArrayStorage& JsonValue::RequireArray() const {
  CheckType(JsonValueType::Array);
  return std::get<ArrayStorage>(storage_);
}

JsonValue::ObjectStorage& JsonValue::RequireObject() {
  CheckType(JsonValueType::Object);
  return std::get<ObjectStorage>(storage_);
}

const JsonValue::ObjectStorage& JsonValue::RequireObject() const {
  CheckType(JsonValueType::Object);
  return std::get<ObjectStorage>(storage_);
}

void JsonParser::Error(std::string_view message) const {
  throw Exception(std::format("JsonParser::Parse failed. {}", message));
}

bool JsonParser::IsEnd() const { return position_ >= source_.size(); }

bool JsonParser::IsJsonSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n';
}

void JsonParser::ReadAllSpace() {
  while (position_ < source_.size() && IsJsonSpace(source_[position_])) {
    position_++;
  }
}

char JsonParser::PeekNext() {
  if (IsEnd()) {
    Error("Unexpected end of string.");
  }
  return source_[position_];
}

char JsonParser::ReadNext() {
  if (IsEnd()) {
    Error("Unexpected end of string.");
  }
  return source_[position_++];
}

std::nullptr_t JsonParser::ParseNull() {
  if (IsEnd() || ReadNext() != 'n') {
    Error("Invalid null value.");
  }
  if (IsEnd() || ReadNext() != 'u') {
    Error("Invalid null value.");
  }
  if (IsEnd() || ReadNext() != 'l') {
    Error("Invalid null value.");
  }
  if (IsEnd() || ReadNext() != 'l') {
    Error("Invalid null value.");
  }
  return nullptr;
}

double JsonParser::ParseNumber() {
  auto result = cru::string::ParseToNumber<double>(
      std::string_view(source_).substr(position_),
      cru::string::ParseToNumberFlags::AllowTrailingJunk);
  if (!result.valid) {
    Error(std::format("Invalid number: {}", result.message));
  }
  position_ += result.processed_char_count;
  return result.value;
}
}  // namespace cru::json
