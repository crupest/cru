#include "cru/base/Json.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

using cru::Exception;
using cru::Index;
using namespace cru::json;

namespace {
using JsonValuePtr = std::unique_ptr<JsonValue>;
using JsonArrayPtr = std::unique_ptr<JsonArrayValue>;
using JsonObjectPtr = std::unique_ptr<JsonObjectValue>;

template <typename Action>
void RequireThrowsContaining(Action action, std::string_view text) {
  try {
    action();
    FAIL("Expected JSON operation to throw");
  } catch (const Exception& exception) {
    REQUIRE(std::string_view(exception.what()).find(text) !=
            std::string_view::npos);
  }
}
}  // namespace

TEST_CASE("JsonValueType converts to string", "[json]") {
  REQUIRE(ToString(JsonValueType::Null) == "Null");
  REQUIRE(ToString(JsonValueType::Boolean) == "Boolean");
  REQUIRE(ToString(JsonValueType::Number) == "Number");
  REQUIRE(ToString(JsonValueType::String) == "String");
  REQUIRE(ToString(JsonValueType::Array) == "Array");
  REQUIRE(ToString(JsonValueType::Object) == "Object");
}

TEST_CASE("Json scalar values expose type and value", "[json]") {
  JsonValuePtr null_value(JsonValue::CreateNull());
  REQUIRE(null_value->GetType() == JsonValueType::Null);
  REQUIRE(null_value->IsNull());
  REQUIRE(null_value->AsNull()->GetValue() == nullptr);

  JsonValuePtr boolean_value(JsonValue::CreateBoolean(true));
  REQUIRE(boolean_value->GetType() == JsonValueType::Boolean);
  REQUIRE(boolean_value->IsBoolean());
  REQUIRE(boolean_value->AsBoolean()->GetValue());
  boolean_value->AsBoolean()->SetValue(false);
  REQUIRE_FALSE(boolean_value->AsBoolean()->GetValue());

  JsonValuePtr number_value(JsonValue::CreateNumber(12.5));
  REQUIRE(number_value->GetType() == JsonValueType::Number);
  REQUIRE(number_value->IsNumber());
  REQUIRE(number_value->AsNumber()->GetValue() == Catch::Approx(12.5));
  number_value->AsNumber()->SetValue(-4.25);
  REQUIRE(number_value->AsNumber()->GetValue() == Catch::Approx(-4.25));

  JsonValuePtr string_value(JsonValue::CreateString("hello"));
  REQUIRE(string_value->GetType() == JsonValueType::String);
  REQUIRE(string_value->IsString());
  REQUIRE(string_value->AsString()->GetValue() == "hello");
  string_value->AsString()->SetValue("world");
  REQUIRE(string_value->AsString()->GetValue() == "world");
}

TEST_CASE("Json scalar values clone independently", "[json]") {
  JsonValuePtr original(JsonValue::CreateString("original"));
  JsonValuePtr clone(original->Clone());

  REQUIRE(clone.get() != original.get());
  REQUIRE(clone->IsString());
  REQUIRE(clone->AsString()->GetValue() == "original");

  original->AsString()->SetValue("changed");
  REQUIRE(original->AsString()->GetValue() == "changed");
  REQUIRE(clone->AsString()->GetValue() == "original");
}

TEST_CASE("JsonValue typed accessors validate types", "[json]") {
  JsonValuePtr value(JsonValue::CreateBoolean(true));

  REQUIRE(value->AsBoolean()->GetValue());
  RequireThrowsContaining([&] { value->AsNumber(); }, "Number");
  RequireThrowsContaining([&] { value->AsString(); }, "String");

  const JsonValue& const_value = *value;
  REQUIRE(const_value.AsBoolean()->GetValue());
  RequireThrowsContaining([&] { const_value.AsArray(); }, "Array");
}

TEST_CASE("JsonArrayValue stores, inserts, iterates, and removes children",
          "[json]") {
  JsonArrayPtr array(JsonValue::CreateArray());
  REQUIRE(array->GetType() == JsonValueType::Array);
  REQUIRE(array->IsArray());
  REQUIRE(array->GetSize() == 0);

  array->AddValue(JsonValue::CreateNumber(1.0));
  array->AddValue(JsonValue::CreateString("third"));
  array->AddValueAt(1, JsonValue::CreateBoolean(true));

  REQUIRE(array->GetSize() == 3);
  REQUIRE(array->GetValueAt(0)->AsNumber()->GetValue() == Catch::Approx(1.0));
  REQUIRE((*array)[1]->AsBoolean()->GetValue());
  REQUIRE((*array)[2]->AsString()->GetValue() == "third");

  delete (*array)[1];
  (*array)[1] = JsonValue::CreateBoolean(false);
  REQUIRE_FALSE(array->GetValueAt(1)->AsBoolean()->GetValue());

  std::vector<JsonValueType> mutable_types;
  for (JsonValue* value : array->Values()) {
    mutable_types.push_back(value->GetType());
  }
  REQUIRE(mutable_types == std::vector<JsonValueType>{JsonValueType::Number,
                                                      JsonValueType::Boolean,
                                                      JsonValueType::String});

  const JsonArrayValue& const_array = *array;
  std::vector<JsonValueType> const_types;
  for (const JsonValue* value : const_array.Values()) {
    const_types.push_back(value->GetType());
  }
  REQUIRE(const_types == mutable_types);
  REQUIRE(const_array.GetValueAt(2)->AsString()->GetValue() == "third");

  JsonValuePtr removed(array->RemoveAt(1));
  REQUIRE(removed->AsBoolean()->GetValue() == false);
  REQUIRE(array->GetSize() == 2);
  REQUIRE(array->GetValueAt(1)->AsString()->GetValue() == "third");

  RequireThrowsContaining([&] { array->GetValueAt(array->GetSize()); },
                          "out of range");
  RequireThrowsContaining(
      [&] { array->AddValueAt(array->GetSize() + 1, nullptr); },
      "out of range");
}

TEST_CASE("JsonArrayValue mutable Values view can replace children", "[json]") {
  JsonArrayPtr array(JsonValue::CreateArray());
  array->AddValue(JsonValue::CreateNumber(1.0));
  array->AddValue(JsonValue::CreateString("before"));

  auto values = array->Values();
  auto value = values.begin();

  delete *value;
  *value = JsonValue::CreateBoolean(true);
  REQUIRE(array->GetValueAt(0)->AsBoolean()->GetValue());

  ++value;
  delete *value;
  *value = JsonValue::CreateString("after");
  REQUIRE(array->GetValueAt(1)->AsString()->GetValue() == "after");
}

TEST_CASE("JsonObjectValue iteration views expose keys values and items",
          "[json]") {
  JsonObjectPtr object(JsonValue::CreateObject());
  REQUIRE(object->TryAdd("enabled", JsonValue::CreateBoolean(true)));
  REQUIRE(object->TryAdd("count", JsonValue::CreateNumber(3.0)));

  std::vector<std::string> keys;
  for (const std::string& key : object->Keys()) {
    keys.push_back(key);
  }
  REQUIRE(keys == std::vector<std::string>{"enabled", "count"});

  std::vector<JsonValueType> value_types;
  for (JsonValue* value : object->Values()) {
    value_types.push_back(value->GetType());
  }
  REQUIRE(value_types == std::vector<JsonValueType>{JsonValueType::Boolean,
                                                    JsonValueType::Number});

  std::vector<std::string> item_keys;
  std::vector<JsonValueType> item_value_types;
  for (auto [key, value] : object->Items()) {
    item_keys.push_back(key);
    item_value_types.push_back(value->GetType());
  }
  REQUIRE(item_keys == keys);
  REQUIRE(item_value_types == value_types);

  const JsonObjectValue& const_object = *object;
  std::vector<std::string> const_keys;
  for (const std::string& key : const_object.Keys()) {
    const_keys.push_back(key);
  }
  REQUIRE(const_keys == keys);

  std::vector<JsonValueType> const_value_types;
  for (const JsonValue* value : const_object.Values()) {
    const_value_types.push_back(value->GetType());
  }
  REQUIRE(const_value_types == value_types);

  std::vector<std::string> const_item_keys;
  std::vector<JsonValueType> const_item_value_types;
  for (auto [key, value] : const_object.Items()) {
    const_item_keys.push_back(key);
    const_item_value_types.push_back(value->GetType());
  }
  REQUIRE(const_item_keys == keys);
  REQUIRE(const_item_value_types == value_types);
}

TEST_CASE("JsonObjectValue mutable iteration views can replace children",
          "[json]") {
  JsonObjectPtr object(JsonValue::CreateObject());
  REQUIRE(object->TryAdd("enabled", JsonValue::CreateBoolean(true)));
  REQUIRE(object->TryAdd("count", JsonValue::CreateNumber(3.0)));

  auto values = object->Values();
  auto value = values.begin();
  delete *value;
  *value = JsonValue::CreateBoolean(false);
  REQUIRE_FALSE(object->GetValue("enabled")->AsBoolean()->GetValue());

  for (auto [key, item_value] : object->Items()) {
    if (key == "count") {
      delete item_value;
      item_value = JsonValue::CreateString("changed");
    }
  }
  REQUIRE(object->GetValue("count")->AsString()->GetValue() == "changed");
}

TEST_CASE("JsonArrayValue clone deep copies children", "[json]") {
  JsonArrayPtr original(JsonValue::CreateArray());
  original->AddValue(JsonValue::CreateString("alpha"));
  original->AddValue(JsonValue::CreateNumber(2.0));

  JsonArrayPtr clone(original->Clone());
  REQUIRE(clone.get() != original.get());
  REQUIRE(clone->GetSize() == original->GetSize());
  REQUIRE(clone->GetValueAt(0) != original->GetValueAt(0));
  REQUIRE(clone->GetValueAt(0)->AsString()->GetValue() == "alpha");
  REQUIRE(clone->GetValueAt(1)->AsNumber()->GetValue() == Catch::Approx(2.0));

  original->GetValueAt(0)->AsString()->SetValue("changed");
  REQUIRE(original->GetValueAt(0)->AsString()->GetValue() == "changed");
  REQUIRE(clone->GetValueAt(0)->AsString()->GetValue() == "alpha");
}

TEST_CASE("JsonObjectValue manages keyed children", "[json]") {
  JsonObjectPtr object(JsonValue::CreateObject());
  REQUIRE(object->GetType() == JsonValueType::Object);
  REQUIRE(object->IsObject());
  REQUIRE(object->GetSize() == 0);
  REQUIRE_FALSE(object->ContainsKey("enabled"));
  REQUIRE(object->GetOptionalValue("enabled") == nullptr);

  REQUIRE(object->TryAdd("enabled", JsonValue::CreateBoolean(true)));
  REQUIRE(object->TryAdd("count", JsonValue::CreateNumber(3.0)));
  REQUIRE(object->GetSize() == 2);
  REQUIRE(object->ContainsKey("enabled"));
  REQUIRE(object->GetValue("enabled")->AsBoolean()->GetValue());
  REQUIRE(object->GetOptionalValue("count")->AsNumber()->GetValue() ==
          Catch::Approx(3.0));

  JsonValuePtr duplicate(JsonValue::CreateNull());
  REQUIRE_FALSE(object->TryAdd("enabled", duplicate.get()));
  duplicate.reset();
  REQUIRE(object->GetSize() == 2);

  (*object)["created"]->AsNull();
  REQUIRE(object->ContainsKey("created"));
  REQUIRE(object->GetSize() == 3);
  delete (*object)["created"];
  (*object)["created"] = JsonValue::CreateString("now");
  REQUIRE(object->GetValue("created")->AsString()->GetValue() == "now");

  const JsonObjectValue& const_object = *object;
  REQUIRE(const_object.GetValue("created")->AsString()->GetValue() == "now");
  REQUIRE(const_object.GetOptionalValue("missing") == nullptr);

  JsonValuePtr removed(object->TryRemove("count"));
  REQUIRE(removed->AsNumber()->GetValue() == Catch::Approx(3.0));
  REQUIRE_FALSE(object->ContainsKey("count"));
  REQUIRE(object->GetSize() == 2);
  REQUIRE(object->TryRemove("count") == nullptr);

  RequireThrowsContaining([&] { object->GetValue("missing"); }, "missing");
  RequireThrowsContaining([&] { const_object.GetValue("missing"); }, "missing");
}

TEST_CASE("JsonObjectValue clone deep copies children", "[json]") {
  JsonObjectPtr original(JsonValue::CreateObject());
  original->TryAdd("name", JsonValue::CreateString("alpha"));
  auto nested = JsonValue::CreateArray();
  nested->AddValue(JsonValue::CreateBoolean(true));
  original->TryAdd("items", nested);

  JsonObjectPtr clone(original->Clone());
  REQUIRE(clone.get() != original.get());
  REQUIRE(clone->GetSize() == original->GetSize());
  REQUIRE(clone->GetValue("name") != original->GetValue("name"));
  REQUIRE(clone->GetValue("name")->AsString()->GetValue() == "alpha");
  REQUIRE(clone->GetValue("items") != original->GetValue("items"));
  REQUIRE(clone->GetValue("items")->AsArray()->GetValueAt(0) !=
          original->GetValue("items")->AsArray()->GetValueAt(0));
  REQUIRE(clone->GetValue("items")
              ->AsArray()
              ->GetValueAt(0)
              ->AsBoolean()
              ->GetValue());

  original->GetValue("name")->AsString()->SetValue("changed");
  original->GetValue("items")->AsArray()->GetValueAt(0)->AsBoolean()->SetValue(
      false);

  REQUIRE(clone->GetValue("name")->AsString()->GetValue() == "alpha");
  REQUIRE(clone->GetValue("items")
              ->AsArray()
              ->GetValueAt(0)
              ->AsBoolean()
              ->GetValue());
}
