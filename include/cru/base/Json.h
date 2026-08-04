#pragma once

#include "Base.h"

#include <cstddef>
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

  ~JsonValue() override;

  JsonValueType GetType() const { return type_; }

  bool IsNull() const { return type_ == JsonValueType::Null; }
  bool IsBoolean() const { return type_ == JsonValueType::Boolean; }
  bool IsNumber() const { return type_ == JsonValueType::Number; }
  bool IsString() const { return type_ == JsonValueType::String; }
  bool IsArray() const { return type_ == JsonValueType::Array; }
  bool IsObject() const { return type_ == JsonValueType::Object; }

  std::nullptr_t GetNull() const;
  bool GetBoolean() const;
  double GetNumber() const;
  const std::string& GetString() const;
  const ArrayStorage& GetArray() const;
  const ObjectStorage& GetObject() const;


  void SetNull();

 private:
  void CheckTypeForGet(JsonValueType type) const;
  void DestroyChildren();

 private:
  JsonValueType type_;
  std::variant<std::nullptr_t, bool, double, std::string, ArrayStorage,
               ObjectStorage>
      value_;
};
}  // namespace cru::json
