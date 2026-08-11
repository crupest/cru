#include "cru/base/Json.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using cru::Exception;
using cru::Index;
using namespace cru::json;

namespace {
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
  JsonValue null_value;
  REQUIRE(null_value.GetType() == JsonValueType::Null);
  REQUIRE(null_value.IsNull());
  REQUIRE(null_value.GetNull() == nullptr);

  JsonValue boolean_value(true);
  REQUIRE(boolean_value.GetType() == JsonValueType::Boolean);
  REQUIRE(boolean_value.IsBoolean());
  REQUIRE(boolean_value.GetBoolean());
  boolean_value.Set(false);
  REQUIRE_FALSE(boolean_value.GetBoolean());

  JsonValue number_value(12.5);
  REQUIRE(number_value.GetType() == JsonValueType::Number);
  REQUIRE(number_value.IsNumber());
  REQUIRE(number_value.GetNumber() == Catch::Approx(12.5));
  number_value.Set(-4.25);
  REQUIRE(number_value.GetNumber() == Catch::Approx(-4.25));

  JsonValue integer_value(12);
  REQUIRE(integer_value.IsNumber());
  REQUIRE(integer_value.GetNumber() == Catch::Approx(12.0));

  JsonValue string_value("hello");
  REQUIRE(string_value.GetType() == JsonValueType::String);
  REQUIRE(string_value.IsString());
  REQUIRE(string_value.GetString() == "hello");
  string_value.Set(std::string("world"));
  REQUIRE(string_value.GetString() == "world");
}

TEST_CASE("Json scalar Set and assignment change types", "[json]") {
  JsonValue value = JsonValue::Object();
  REQUIRE(value.IsObject());

  value.Set(true);
  REQUIRE(value.IsBoolean());
  REQUIRE(value.GetBoolean());

  value.Set(3.5);
  REQUIRE(value.IsNumber());
  REQUIRE(value.GetNumber() == Catch::Approx(3.5));

  value = "hello";
  REQUIRE(value.IsString());
  REQUIRE(value.GetString() == "hello");

  value = 7;
  REQUIRE(value.IsNumber());
  REQUIRE(value.GetNumber() == Catch::Approx(7.0));

  value = nullptr;
  REQUIRE(value.IsNull());
}

TEST_CASE("JsonValue equality only compares scalar values", "[json]") {
  REQUIRE(JsonValue() == JsonValue(nullptr));
  REQUIRE(JsonValue(true) == JsonValue(true));
  REQUIRE_FALSE(JsonValue(true) == JsonValue(false));
  REQUIRE(JsonValue(12.5) == JsonValue(12.5));
  REQUIRE_FALSE(JsonValue(12.5) == JsonValue(12.0));
  REQUIRE(JsonValue("hello") == JsonValue(std::string("hello")));
  REQUIRE_FALSE(JsonValue("hello") == JsonValue("world"));
  REQUIRE_FALSE(JsonValue(true) == JsonValue(1.0));
  REQUIRE_FALSE(JsonValue("1") == JsonValue(1.0));

  JsonValue array = JsonValue::Array();
  REQUIRE(array.Insert(0, JsonValue(1.0)));
  JsonValue same_array = JsonValue::Array();
  REQUIRE(same_array.Insert(0, JsonValue(1.0)));
  REQUIRE_FALSE(array == same_array);
  REQUIRE_FALSE(array == array);

  JsonValue object = JsonValue::Object();
  REQUIRE_FALSE(object.Insert("value", JsonValue(1.0)));
  JsonValue same_object = JsonValue::Object();
  REQUIRE_FALSE(same_object.Insert("value", JsonValue(1.0)));
  REQUIRE_FALSE(object == same_object);
  REQUIRE_FALSE(object == object);
}

TEST_CASE("Json scalar Get validates types", "[json]") {
  JsonValue value(true);

  REQUIRE(value.GetBoolean());
  RequireThrowsContaining([&] { value.GetNumber(); }, "Number");
  RequireThrowsContaining([&] { value.GetString(); }, "String");
  RequireThrowsContaining([&] { JsonValue(nullptr).GetBoolean(); }, "Boolean");
}

TEST_CASE("Json arrays support Get operator Set Insert and Remove", "[json]") {
  JsonValue array = JsonValue::Array();
  REQUIRE(array.GetType() == JsonValueType::Array);
  REQUIRE(array.IsArray());
  REQUIRE(array.GetSize() == 0);

  REQUIRE(array.Insert(0, JsonValue(1.0)));
  REQUIRE(array.Insert(1, JsonValue("third")));
  REQUIRE(array.Insert(1, JsonValue(true)));

  REQUIRE(array.GetSize() == 3);
  REQUIRE(array.Get(0).GetNumber() == Catch::Approx(1.0));
  REQUIRE(array[1].GetBoolean());
  REQUIRE(array[2].GetString() == "third");

  REQUIRE(array.Set(1, JsonValue(false)));
  REQUIRE_FALSE(array.Get(1).GetBoolean());

  std::vector<JsonValueType> mutable_types;
  for (JsonValue& value : array) {
    mutable_types.push_back(value.GetType());
  }
  REQUIRE(mutable_types == std::vector<JsonValueType>{JsonValueType::Number,
                                                      JsonValueType::Boolean,
                                                      JsonValueType::String});

  const JsonValue& const_array = array;
  std::vector<JsonValueType> const_types;
  for (const JsonValue& value : const_array) {
    const_types.push_back(value.GetType());
  }
  REQUIRE(const_types == mutable_types);
  REQUIRE(const_array.Get(2).GetString() == "third");

  std::optional<JsonValue> removed = array.Remove(1);
  REQUIRE(removed.has_value());
  REQUIRE(removed->GetBoolean() == false);
  REQUIRE(array.GetSize() == 2);
  REQUIRE(array.Get(1).GetString() == "third");

  REQUIRE_FALSE(array.Remove(array.GetSize()).has_value());
  RequireThrowsContaining([&] { array.Get(array.GetSize()); }, "out of range");
  RequireThrowsContaining([&] { array.Insert(array.GetSize() + 1, nullptr); },
                          "out of range");
  RequireThrowsContaining([&] { JsonValue().Get(0); }, "Array");
}

TEST_CASE("JsonValue direct iteration can mutate scalar and child values",
          "[json]") {
  static_assert(std::random_access_iterator<JsonValue::iterator>);
  static_assert(std::random_access_iterator<JsonValue::const_iterator>);

  JsonValue scalar("before");
  auto scalar_value = scalar.begin();
  REQUIRE(scalar_value != scalar.end());
  REQUIRE(scalar_value == scalar.begin());
  REQUIRE(scalar.end() - scalar.begin() == 1);
  REQUIRE(scalar.begin()[0].GetString() == "before");
  REQUIRE(&*(scalar.begin() + 0) == &scalar);
  REQUIRE(0 + scalar.begin() == scalar.begin());
  REQUIRE(scalar.end() - 1 == scalar.begin());

  *scalar_value = "after";
  REQUIRE(scalar.GetString() == "after");
  ++scalar_value;
  REQUIRE(scalar_value == scalar.end());

  std::vector<JsonValueType> scalar_types;
  for (JsonValue& value : scalar) {
    scalar_types.push_back(value.GetType());
    value = "range-for";
  }
  REQUIRE(scalar_types == std::vector<JsonValueType>{JsonValueType::String});
  REQUIRE(scalar.GetString() == "range-for");

  const JsonValue& const_scalar = scalar;
  auto const_scalar_value = const_scalar.begin();
  JsonValue::const_iterator converted_scalar_value = scalar.begin();
  REQUIRE(converted_scalar_value == const_scalar_value);
  REQUIRE(const_scalar_value != const_scalar.end());
  REQUIRE(&*const_scalar_value == &const_scalar);
  ++const_scalar_value;
  REQUIRE(const_scalar_value == const_scalar.end());

  std::vector<JsonValueType> const_scalar_types;
  for (const JsonValue& value : const_scalar) {
    const_scalar_types.push_back(value.GetType());
  }
  REQUIRE(const_scalar_types == scalar_types);

  JsonValue array = JsonValue::Array();
  REQUIRE(array.Insert(0, JsonValue(1.0)));
  REQUIRE(array.Insert(1, JsonValue("before")));

  auto array_value = array.begin();
  REQUIRE(array.end() - array.begin() == 2);
  REQUIRE(array_value[0].GetNumber() == Catch::Approx(1.0));
  REQUIRE(array_value[1].GetString() == "before");
  REQUIRE((array.begin() + 1)->GetString() == "before");
  REQUIRE(array.begin() < array.end());
  REQUIRE(array.end() > array.begin());
  REQUIRE(array.begin() <= array.begin());
  REQUIRE(array.end() >= array.begin());

  *array_value = true;
  REQUIRE(array.Get(0).GetBoolean());

  ++array_value;
  REQUIRE(array_value == array.end() - 1);
  auto last_array_value = array.end();
  --last_array_value;
  REQUIRE(last_array_value == array_value);
  *array_value = "after";
  REQUIRE(array.Get(1).GetString() == "after");

  JsonValue object = JsonValue::Object();
  REQUIRE_FALSE(object.Insert("enabled", JsonValue(true)));
  REQUIRE_FALSE(object.Insert("count", JsonValue(3.0)));
  REQUIRE(object.end() - object.begin() == 2);
  REQUIRE(object.begin()[0].GetBoolean());
  REQUIRE(object.begin()[1].GetNumber() == Catch::Approx(3.0));

  auto object_value = object.begin();
  object_value += 1;
  REQUIRE(object_value->GetNumber() == Catch::Approx(3.0));
  object_value -= 1;
  REQUIRE(object_value->GetBoolean());

  for (JsonValue& value : object) {
    if (value.IsBoolean()) {
      value = false;
    } else if (value.IsNumber()) {
      value = "changed";
    }
  }

  REQUIRE_FALSE(object.Get("enabled").GetBoolean());
  REQUIRE(object.Get("count").GetString() == "changed");
}

TEST_CASE("Json objects support Get operator Set Insert and Remove", "[json]") {
  JsonValue object = JsonValue::Object();
  REQUIRE(object.GetType() == JsonValueType::Object);
  REQUIRE(object.IsObject());
  REQUIRE(object.GetSize() == 0);
  REQUIRE_FALSE(object.Contains("enabled"));
  REQUIRE(object.TryGet("enabled") == nullptr);

  REQUIRE_FALSE(object.Insert("enabled", JsonValue(true)));
  REQUIRE_FALSE(object.Insert("count", JsonValue(3.0)));
  REQUIRE(object.GetSize() == 2);
  REQUIRE(object.Contains("enabled"));
  REQUIRE(object.Get("enabled").GetBoolean());
  REQUIRE(object.TryGet("count")->GetNumber() == Catch::Approx(3.0));

  REQUIRE(object.Insert("enabled", JsonValue(nullptr)));
  REQUIRE(object.Get("enabled").GetBoolean());
  REQUIRE(object.GetSize() == 2);

  object["created"].GetNull();
  REQUIRE(object.Contains("created"));
  REQUIRE(object.GetSize() == 3);
  object["created"] = "now";
  REQUIRE(object.Get("created").GetString() == "now");

  const JsonValue& const_object = object;
  REQUIRE(const_object["created"].GetString() == "now");
  REQUIRE(const_object.TryGet("missing") == nullptr);

  REQUIRE(object.Set("enabled", JsonValue(false)));
  REQUIRE_FALSE(object.Get("enabled").GetBoolean());
  REQUIRE_FALSE(object.Set("name", JsonValue("created")));
  REQUIRE(object.Get("name").GetString() == "created");

  std::optional<JsonValue> removed = object.Remove("count");
  REQUIRE(removed.has_value());
  REQUIRE(removed->GetNumber() == Catch::Approx(3.0));
  REQUIRE_FALSE(object.Contains("count"));
  REQUIRE_FALSE(object.Remove("count").has_value());

  RequireThrowsContaining([&] { object.Get("missing"); }, "missing");
  RequireThrowsContaining([&] { const_object.Get("missing"); }, "missing");
  RequireThrowsContaining([&] { JsonValue().Get("missing"); }, "Object");
}

TEST_CASE("Json object iteration views expose keys values and items",
          "[json]") {
  JsonValue object = JsonValue::Object();
  REQUIRE_FALSE(object.Insert("enabled", JsonValue(true)));
  REQUIRE_FALSE(object.Insert("count", JsonValue(3.0)));

  std::vector<std::string> keys;
  for (const std::string& key : object.Keys()) {
    keys.push_back(key);
  }
  REQUIRE(keys == std::vector<std::string>{"enabled", "count"});

  std::vector<JsonValueType> item_value_types;
  std::vector<std::string> item_keys;
  for (auto [key, value] : object.Items()) {
    item_keys.push_back(key);
    item_value_types.push_back(value.GetType());
  }
  REQUIRE(item_keys == keys);
  REQUIRE(item_value_types ==
          std::vector<JsonValueType>{JsonValueType::Boolean,
                                     JsonValueType::Number});

  const JsonValue& const_object = object;
  std::vector<std::string> const_keys;
  for (const std::string& key : const_object.Keys()) {
    const_keys.push_back(key);
  }
  REQUIRE(const_keys == keys);

  std::vector<std::string> const_item_keys;
  std::vector<JsonValueType> const_item_value_types;
  for (auto [key, value] : const_object.Items()) {
    const_item_keys.push_back(key);
    const_item_value_types.push_back(value.GetType());
  }
  REQUIRE(const_item_keys == keys);
  REQUIRE(const_item_value_types == item_value_types);
}

TEST_CASE("Json object mutable Items view can replace children", "[json]") {
  JsonValue object = JsonValue::Object();
  REQUIRE_FALSE(object.Insert("enabled", JsonValue(true)));
  REQUIRE_FALSE(object.Insert("count", JsonValue(3.0)));

  for (auto [key, value] : object.Items()) {
    if (key == "enabled") {
      value = false;
    } else if (key == "count") {
      value = "changed";
    }
  }

  REQUIRE_FALSE(object.Get("enabled").GetBoolean());
  REQUIRE(object.Get("count").GetString() == "changed");
}

TEST_CASE("JsonValue copy deep copies arrays and objects", "[json]") {
  JsonValue original = JsonValue::Object();
  REQUIRE_FALSE(original.Insert("name", JsonValue("alpha")));
  JsonValue nested = JsonValue::Array();
  REQUIRE(nested.Insert(0, JsonValue(true)));
  REQUIRE_FALSE(original.Insert("items", std::move(nested)));

  JsonValue clone = original;
  REQUIRE(clone.GetSize() == original.GetSize());
  REQUIRE(clone.Get("name").GetString() == "alpha");
  REQUIRE(clone.Get("items").Get(0).GetBoolean());

  original.Get("name") = "changed";
  original.Get("items").Set(0, JsonValue(false));

  REQUIRE(original.Get("name").GetString() == "changed");
  REQUIRE_FALSE(original.Get("items").Get(0).GetBoolean());
  REQUIRE(clone.Get("name").GetString() == "alpha");
  REQUIRE(clone.Get("items").Get(0).GetBoolean());
}

TEST_CASE("JsonValue move resets source to null", "[json]") {
  JsonValue source = JsonValue::Array();
  REQUIRE(source.Insert(0, JsonValue("moved")));

  JsonValue moved(std::move(source));
  REQUIRE(moved.IsArray());
  REQUIRE(moved.Get(0).GetString() == "moved");
  REQUIRE(source.IsNull());

  JsonValue assigned(true);
  assigned = std::move(moved);
  REQUIRE(assigned.IsArray());
  REQUIRE(assigned.Get(0).GetString() == "moved");
  REQUIRE(moved.IsNull());
}

TEST_CASE("JsonParser parses scalar values", "[json]") {
  REQUIRE(ParseJson("null").IsNull());

  JsonValue true_value = ParseJson("true");
  REQUIRE(true_value.IsBoolean());
  REQUIRE(true_value.GetBoolean());

  JsonValue false_value = ParseJson("  \n\tfalse\t\n  ");
  REQUIRE(false_value.IsBoolean());
  REQUIRE_FALSE(false_value.GetBoolean());

  JsonValue integer_value = ParseJson("42");
  REQUIRE(integer_value.IsNumber());
  REQUIRE(integer_value.GetNumber() == Catch::Approx(42.0));

  JsonValue decimal_value = ParseJson("-12.5");
  REQUIRE(decimal_value.IsNumber());
  REQUIRE(decimal_value.GetNumber() == Catch::Approx(-12.5));

  JsonValue exponent_value = ParseJson("6.25e+2");
  REQUIRE(exponent_value.IsNumber());
  REQUIRE(exponent_value.GetNumber() == Catch::Approx(625.0));
}

TEST_CASE("JsonParser parses string escapes", "[json]") {
  JsonValue value =
      ParseJson(R"("quote\" slash\\ newline\n return\r tab\t back\b")");

  REQUIRE(value.IsString());
  REQUIRE(value.GetString() ==
          std::string("quote\" slash\\ newline\n return\r tab\t back\b"));
}

TEST_CASE("JsonParser parses arrays and nested objects", "[json]") {
  JsonValue numeric_array = ParseJson("[1,2]");
  REQUIRE(numeric_array.IsArray());
  REQUIRE(numeric_array.GetSize() == 2);
  REQUIRE(numeric_array.Get(0).GetNumber() == Catch::Approx(1.0));
  REQUIRE(numeric_array.Get(1).GetNumber() == Catch::Approx(2.0));

  JsonValue value = ParseJson(
      R"({
        "name": "widget",
        "enabled": true,
        "count": 3,
        "items": [null, false, {"nested": "value"}]
      })");

  REQUIRE(value.IsObject());
  REQUIRE(value.GetSize() == 4);
  REQUIRE(value.Get("name").GetString() == "widget");
  REQUIRE(value.Get("enabled").GetBoolean());
  REQUIRE(value.Get("count").GetNumber() == Catch::Approx(3.0));

  const JsonValue& items = value.Get("items");
  REQUIRE(items.IsArray());
  REQUIRE(items.GetSize() == 3);
  REQUIRE(items.Get(0).IsNull());
  REQUIRE_FALSE(items.Get(1).GetBoolean());
  REQUIRE(items.Get(2).Get("nested").GetString() == "value");
}

TEST_CASE("JsonParser can parse the same source repeatedly", "[json]") {
  JsonParser parser(R"({"value": 7})");

  JsonValue first = parser.Parse();
  JsonValue second = parser.Parse();

  REQUIRE(first.Get("value").GetNumber() == Catch::Approx(7.0));
  REQUIRE(second.Get("value").GetNumber() == Catch::Approx(7.0));
}

TEST_CASE("JsonParser allows trailing junk and reports its start", "[json]") {
  std::string object_source = R"(  {"items":[1]}tail)";
  JsonParser parser(object_source);
  Index trailing_junk_start = 0;

  JsonValue object = parser.ParseAllowTrailingJunk(&trailing_junk_start);

  REQUIRE(object.Get("items").Get(0).GetNumber() == Catch::Approx(1.0));
  REQUIRE(trailing_junk_start == object_source.find("tail"));

  std::string spaced_source = "false \n";
  trailing_junk_start = spaced_source.size();
  JsonValue boolean =
      ParseJsonAllowTrailingJunk(spaced_source, &trailing_junk_start);

  REQUIRE_FALSE(boolean.GetBoolean());
  REQUIRE(trailing_junk_start == 5);
  REQUIRE(spaced_source[trailing_junk_start] == ' ');

  std::string complete_source = "null";
  trailing_junk_start = 99;
  REQUIRE(ParseJsonAllowTrailingJunk(complete_source, &trailing_junk_start)
              .IsNull());
  REQUIRE(trailing_junk_start == complete_source.size());

  REQUIRE(ParseJsonAllowTrailingJunk("12 junk").GetNumber() ==
          Catch::Approx(12.0));
}

TEST_CASE("JsonParser rejects invalid JSON", "[json]") {
  RequireThrowsContaining([] { ParseJson(""); }, "Unexpected end");
  RequireThrowsContaining([] { ParseJson("true false"); },
                          "Unexpected characters");
  RequireThrowsContaining([] { ParseJson("[1 2]"); }, "Expected ','");
  RequireThrowsContaining([] { ParseJson(R"({"a" 1})"); }, "Expected ':'");
  RequireThrowsContaining([] { ParseJson(R"("bad\q")"); },
                          "Invalid escape sequence");
  RequireThrowsContaining([] { ParseJson("[1,]"); }, "Invalid JSON value");
}

TEST_CASE("JsonParser allowing trailing junk still rejects invalid prefix",
          "[json]") {
  RequireThrowsContaining([] { ParseJsonAllowTrailingJunk(""); },
                          "Unexpected end");
  RequireThrowsContaining([] { ParseJsonAllowTrailingJunk("[1 2] ignored"); },
                          "Expected ','");
  RequireThrowsContaining(
      [] { ParseJsonAllowTrailingJunk(R"({"value": "bad\q"} ignored)"); },
      "Invalid escape sequence");
}
