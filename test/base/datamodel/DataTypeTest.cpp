#include "cru/base/datamodel/DataType.h"
#include "cru/base/datamodel/DataTypeRegistry.h"

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <utility>
#include <vector>

using cru::Exception;
using cru::datamodel::DataConvertResult;
using cru::datamodel::DataTypeBase;
using cru::datamodel::DataTypeRegistry;
using cru::datamodel::KeyValueObject;
using cru::datamodel::NumberDataType;
using cru::datamodel::NumberListDataType;
using cru::datamodel::ObjectDataType;
using cru::datamodel::StringDataType;
using cru::string::ParseToNumberFlags;
using cru::xml::XmlElementNode;
using cru::xml::XmlTextNode;

TEST_CASE("DataConvertResult basic behaviors", "[datamodel]") {
  auto success = DataConvertResult<int>::Success(42);
  REQUIRE(success.IsSuccess());
  REQUIRE(!success.HasErrors());
  REQUIRE(success.GetValue() == 42);
  REQUIRE(success.GetOptionalValue() == std::optional<int>(42));

  success.AddError("recoverable warning");
  REQUIRE(success.HasErrors());
  REQUIRE(success.GetErrors() ==
          std::vector<std::string>{"recoverable warning"});

  success.RemoveError("recoverable warning");
  REQUIRE(!success.HasErrors());

  auto failed = DataConvertResult<int>::Failure("not a number");
  REQUIRE(!failed.IsSuccess());
  REQUIRE(failed.HasErrors());
  REQUIRE(failed.GetErrors() == std::vector<std::string>{"not a number"});
  REQUIRE_THROWS_AS(failed.GetValue(), Exception);

  auto success_with_errors =
      DataConvertResult<int>::SuccessWithErrors(7, {"w1", "w2"});
  REQUIRE(success_with_errors.IsSuccess());
  REQUIRE(success_with_errors.HasErrors());
  REQUIRE(success_with_errors.GetValue() == 7);
  REQUIRE(success_with_errors.GetErrors() ==
          std::vector<std::string>{"w1", "w2"});
}

TEST_CASE("NumberDataType string conversion", "[datamodel]") {
  NumberDataType<int> type;

  REQUIRE(type.GetName() == "Number");

  auto parsed = type.ConvertFromString(" 10 trailing");
  REQUIRE(parsed.IsSuccess());
  REQUIRE(parsed.GetValue() == 10);

  auto failed = type.ConvertFromString("abc");
  REQUIRE(!failed.IsSuccess());
  REQUIRE(failed.HasErrors());

  auto serialized = type.ConvertToString(-25);
  REQUIRE(serialized.IsSuccess());
  REQUIRE(serialized.GetValue() == "-25");

  REQUIRE(type.ConvertToXml(1).IsNotSupported());
}

TEST_CASE("NumberDataType strict trailing junk handling", "[datamodel]") {
  NumberDataType<int> strict_type(ParseToNumberFlags::AllowLeadingSpaces |
                                  ParseToNumberFlags::AllowTrailingSpaces);

  auto parsed = strict_type.ConvertFromString(" 55 ");
  REQUIRE(parsed.IsSuccess());
  REQUIRE(parsed.GetValue() == 55);

  auto failed = strict_type.ConvertFromString("55junk");
  REQUIRE(!failed.IsSuccess());
  REQUIRE(failed.HasErrors());
}

TEST_CASE("NumberListDataType string conversion", "[datamodel]") {
  NumberListDataType<int> type;

  REQUIRE(type.GetName() == "NumberList");

  auto parsed = type.ConvertFromString(
      " 1\t2\n\xC2\xA0"
      "3\xE3\x80\x80"
      "4 ");
  REQUIRE(parsed.IsSuccess());
  REQUIRE(!parsed.HasErrors());
  REQUIRE(parsed.GetValue() == std::vector<int>{1, 2, 3, 4});

  auto serialized = type.ConvertToString(std::vector<int>{-1, 0, 42});
  REQUIRE(serialized.IsSuccess());
  REQUIRE(serialized.GetValue() == "-1 0 42");

  REQUIRE(type.ConvertToXml(std::vector<int>{1}).IsNotSupported());
}

TEST_CASE("NumberListDataType reports invalid substrings", "[datamodel]") {
  NumberListDataType<int> type;

  auto failed = type.ConvertFromString("1 2x 3");
  REQUIRE(!failed.IsSuccess());
  REQUIRE(failed.HasErrors());
  REQUIRE(failed.GetErrors().size() == 1);
  REQUIRE(failed.GetErrors()[0].starts_with("In substring 2, "));
  REQUIRE_THROWS_AS(failed.GetValue(), Exception);
}

TEST_CASE("NumberDataType xml conversion through string fallback",
          "[datamodel]") {
  NumberDataType<int> type;

  XmlElementNode from_text("Number");
  from_text.AddChild(new XmlTextNode("77"));
  REQUIRE(type.XmlIsOfThisType(&from_text));

  auto parsed_from_text = type.ConvertFromXml(&from_text);
  REQUIRE(parsed_from_text.IsSuccess());
  REQUIRE(parsed_from_text.GetValue() == 77);

  XmlElementNode wrong_tag("String");
  wrong_tag.AddChild(new XmlTextNode("77"));
  REQUIRE(!type.XmlIsOfThisType(&wrong_tag));
  auto wrong_tag_result = type.ConvertFromXml(&wrong_tag);
  REQUIRE(!wrong_tag_result.IsSuccess());
  REQUIRE(wrong_tag_result.HasErrors());

  XmlElementNode nested_element("Number");
  nested_element.AddChild(new XmlElementNode("child"));

  auto nested_failed = type.ConvertFromXml(&nested_element);
  REQUIRE(!nested_failed.IsSuccess());
  REQUIRE(nested_failed.HasErrors());

  XmlElementNode attr_and_text("Number", {{"value", "33"}});
  attr_and_text.AddChild(new XmlTextNode("44"));

  auto attr_and_text_result = type.ConvertFromXml(&attr_and_text);
  REQUIRE(attr_and_text_result.IsSuccess());
  REQUIRE(attr_and_text_result.GetValue() == 33);
  REQUIRE(attr_and_text_result.HasErrors());

  XmlElementNode attr_only("Number", {{"value", "99"}});
  auto attr_only_result = type.ConvertFromXml(&attr_only);
  REQUIRE(attr_only_result.IsSuccess());
  REQUIRE(attr_only_result.GetValue() == 99);
  REQUIRE(!attr_only_result.HasErrors());
}

TEST_CASE("StringDataType conversion", "[datamodel]") {
  StringDataType type;

  REQUIRE(type.GetName() == "String");

  auto parsed = type.ConvertFromString("hello");
  REQUIRE(parsed.IsSuccess());
  REQUIRE(parsed.GetValue() == "hello");

  auto serialized = type.ConvertToString("world");
  REQUIRE(serialized.IsSuccess());
  REQUIRE(serialized.GetValue() == "world");

  XmlElementNode xml_value("String");
  xml_value.AddChild(new XmlTextNode("from-xml"));

  auto from_xml = type.ConvertFromXml(&xml_value);
  REQUIRE(from_xml.IsSuccess());
  REQUIRE(from_xml.GetValue() == "from-xml");

  REQUIRE(type.ConvertToXml(std::string("x")).IsNotSupported());
}

namespace {

// DataTypeRegistry takes ownership of every data type registered into it, so
// the registry must outlive the ObjectDataType that reads from it.
class ObjectDataTypeTestFixture {
 public:
  ObjectDataTypeTestFixture() : object_data_type_(&registry_) {
    registry_.RegisterDataType(new StringDataType());
    registry_.RegisterDataType(new NumberDataType<int>());
    registry_.RegisterDataType(new NumberDataType<double>());
  }

  DataConvertResult<std::string> ConvertToString(const KeyValueObject& value) {
    return object_data_type_.ConvertToString(value);
  }

 private:
  DataTypeRegistry registry_;
  ObjectDataType object_data_type_;
};

// A value type whose data type intentionally fails to convert to string.
struct UnconvertibleValue {};

class UnconvertibleDataType : public DataTypeBase<UnconvertibleValue> {
 public:
  UnconvertibleDataType() : DataTypeBase<UnconvertibleValue>("Unconvertible") {}

 protected:
  DataConvertResult<std::string> DoConvertToString(
      const UnconvertibleValue&) override {
    return DataConvertResult<std::string>::Failure("value cannot be converted");
  }
};

// A value type whose data type succeeds but attaches a warning.
struct WarnedValue {};

class WarningDataType : public DataTypeBase<WarnedValue> {
 public:
  WarningDataType() : DataTypeBase<WarnedValue>("Warning") {}

 protected:
  DataConvertResult<std::string> DoConvertToString(
      const WarnedValue&) override {
    return DataConvertResult<std::string>::SuccessWithError(
        "OK", "non-fatal warning");
  }
};

}  // namespace

TEST_CASE("ObjectDataType metadata", "[datamodel][ObjectDataType]") {
  DataTypeRegistry registry;
  ObjectDataType type(&registry);

  REQUIRE(type.GetName() == "Object");
  REQUIRE(type.GetTypeInfo() == typeid(KeyValueObject));
}

TEST_CASE("ObjectDataType converts an empty object",
          "[datamodel][ObjectDataType]") {
  ObjectDataTypeTestFixture fixture;

  auto result = fixture.ConvertToString(KeyValueObject{});

  REQUIRE(result.IsSuccess());
  REQUIRE(!result.HasErrors());
  REQUIRE(result.GetValue() == "{  }");
}

TEST_CASE("ObjectDataType converts flat properties in insertion order",
          "[datamodel][ObjectDataType]") {
  ObjectDataTypeTestFixture fixture;

  KeyValueObject object;
  object.SetProperty("name", std::string("Alice"));
  object.SetProperty("age", 42);
  object.SetProperty("ratio", 0.5);

  auto result = fixture.ConvertToString(object);

  REQUIRE(result.IsSuccess());
  REQUIRE(!result.HasErrors());
  REQUIRE(result.GetValue() == "{ name : Alice, age : 42, ratio : 0.500000 }");
}

TEST_CASE("ObjectDataType converts nested objects as inline objects",
          "[datamodel][ObjectDataType]") {
  ObjectDataTypeTestFixture fixture;

  SECTION("single nesting level") {
    KeyValueObject position;
    position.SetProperty("x", 1);
    position.SetProperty("y", 2);

    KeyValueObject outer;
    outer.SetProperty("position", std::move(position));

    auto result = fixture.ConvertToString(outer);

    REQUIRE(result.IsSuccess());
    REQUIRE(!result.HasErrors());
    REQUIRE(result.GetValue() == "{ position : { x : 1, y : 2 } }");
  }

  SECTION("multiple nesting levels") {
    KeyValueObject inner;
    inner.SetProperty("value", 3);

    KeyValueObject middle;
    middle.SetProperty("inner", std::move(inner));

    KeyValueObject outer;
    outer.SetProperty("middle", std::move(middle));

    auto result = fixture.ConvertToString(outer);

    REQUIRE(result.IsSuccess());
    REQUIRE(!result.HasErrors());
    REQUIRE(result.GetValue() == "{ middle : { inner : { value : 3 } } }");
  }

  SECTION("empty nested object") {
    KeyValueObject object;
    object.SetProperty("child", KeyValueObject{});

    auto result = fixture.ConvertToString(object);

    REQUIRE(result.IsSuccess());
    REQUIRE(!result.HasErrors());
    REQUIRE(result.GetValue() == "{ child : {  } }");
  }
}

TEST_CASE("ObjectDataType marks values it cannot convert",
          "[datamodel][ObjectDataType]") {
  ObjectDataTypeTestFixture fixture;

  SECTION("unknown value type") {
    KeyValueObject object;
    // std::vector<int> has no registered data type in this fixture.
    object.SetProperty("unknown", std::vector<int>{1, 2, 3});

    auto result = fixture.ConvertToString(object);

    REQUIRE(result.IsSuccess());
    REQUIRE(!result.HasErrors());
    REQUIRE(result.GetValue() == "{ unknown : UNKNOWN-TYPE }");
  }

  SECTION("valueless std::any") {
    KeyValueObject object;
    object["empty"];  // Default-constructs a valueless std::any.

    auto result = fixture.ConvertToString(object);

    REQUIRE(result.IsSuccess());
    REQUIRE(!result.HasErrors());
    REQUIRE(result.GetValue() == "{ empty : EMPTY-ANY }");
  }
}

TEST_CASE("ObjectDataType reports failing property conversions",
          "[datamodel][ObjectDataType]") {
  // The registry owns the data type instance.
  DataTypeRegistry registry;
  registry.RegisterDataType(new UnconvertibleDataType());
  ObjectDataType type(&registry);

  SECTION("top level property") {
    KeyValueObject object;
    object.SetProperty("bad", UnconvertibleValue{});

    auto result = type.ConvertToString(object);

    // A placeholder value is still produced; the failure is reported
    // through the error list.
    REQUIRE(result.IsSuccess());
    REQUIRE(result.HasErrors());
    REQUIRE(result.GetValue() == "{ bad : CONVERSION-ERROR }");
    REQUIRE(result.GetErrors() ==
            std::vector<std::string>{"In property bad, value cannot be "
                                     "converted"});
  }

  SECTION("nested property reports dotted path") {
    KeyValueObject inner;
    inner.SetProperty("bad", UnconvertibleValue{});

    KeyValueObject outer;
    outer.SetProperty("child", std::move(inner));

    auto result = type.ConvertToString(outer);

    REQUIRE(result.IsSuccess());
    REQUIRE(result.HasErrors());
    REQUIRE(result.GetValue() == "{ child : { bad : CONVERSION-ERROR } }");
    REQUIRE(result.GetErrors() ==
            std::vector<std::string>{"In property child.bad, value cannot be "
                                     "converted"});
  }

  SECTION("sibling properties do not leak path segments") {
    KeyValueObject object;
    object.SetProperty("first", UnconvertibleValue{});
    object.SetProperty("second", UnconvertibleValue{});

    auto result = type.ConvertToString(object);

    REQUIRE(result.IsSuccess());
    REQUIRE(result.GetErrors() ==
            std::vector<std::string>{
                "In property first, value cannot be converted",
                "In property second, value cannot be converted"});
  }
}

TEST_CASE("ObjectDataType keeps non-fatal conversion warnings",
          "[datamodel][ObjectDataType]") {
  DataTypeRegistry registry;
  registry.RegisterDataType(new WarningDataType());
  ObjectDataType type(&registry);

  KeyValueObject object;
  object.SetProperty("warned", WarnedValue{});

  auto result = type.ConvertToString(object);

  REQUIRE(result.IsSuccess());
  REQUIRE(result.HasErrors());
  REQUIRE(result.GetValue() == "{ warned : OK }");
  REQUIRE(result.GetErrors() ==
          std::vector<std::string>{"In property warned, non-fatal warning"});
}

TEST_CASE("ObjectDataType reports nested object paths in error info",
          "[datamodel][ObjectDataType]") {
  DataTypeRegistry registry;
  registry.RegisterDataType(new StringDataType());
  registry.RegisterDataType(new UnconvertibleDataType());
  registry.RegisterDataType(new WarningDataType());
  ObjectDataType type(&registry);

  SECTION("deeply nested property") {
    KeyValueObject leaf;
    leaf.SetProperty("bad", UnconvertibleValue{});

    KeyValueObject middle;
    middle.SetProperty("b", std::move(leaf));

    KeyValueObject outer;
    outer.SetProperty("a", std::move(middle));

    auto result = type.ConvertToString(outer);

    REQUIRE(result.IsSuccess());
    REQUIRE(result.GetValue() == "{ a : { b : { bad : CONVERSION-ERROR } } }");
    REQUIRE(result.GetErrors() ==
            std::vector<std::string>{
                "In property a.b.bad, value cannot be converted"});
  }

  SECTION("sibling nested objects keep separate paths") {
    KeyValueObject first;
    first.SetProperty("bad", UnconvertibleValue{});

    KeyValueObject second;
    second.SetProperty("bad", UnconvertibleValue{});

    KeyValueObject outer;
    outer.SetProperty("first", std::move(first));
    outer.SetProperty("second", std::move(second));

    auto result = type.ConvertToString(outer);

    REQUIRE(result.IsSuccess());
    REQUIRE(result.GetErrors() ==
            std::vector<std::string>{
                "In property first.bad, value cannot be converted",
                "In property second.bad, value cannot be converted"});
  }

  SECTION("multiple errors in one nested object") {
    KeyValueObject child;
    child.SetProperty("name", std::string("child"));
    child.SetProperty("bad1", UnconvertibleValue{});
    child.SetProperty("bad2", UnconvertibleValue{});

    KeyValueObject outer;
    outer.SetProperty("child", std::move(child));
    outer.SetProperty("bad3", UnconvertibleValue{});

    auto result = type.ConvertToString(outer);

    REQUIRE(result.IsSuccess());
    REQUIRE(result.GetValue() ==
            "{ child : { name : child, bad1 : CONVERSION-ERROR, bad2 : "
            "CONVERSION-ERROR }, bad3 : CONVERSION-ERROR }");
    REQUIRE(result.GetErrors() ==
            std::vector<std::string>{
                "In property child.bad1, value cannot be converted",
                "In property child.bad2, value cannot be converted",
                "In property bad3, value cannot be converted"});
  }

  SECTION("warning inside nested object reports nested path") {
    KeyValueObject inner;
    inner.SetProperty("warned", WarnedValue{});

    KeyValueObject outer;
    outer.SetProperty("inner", std::move(inner));

    auto result = type.ConvertToString(outer);

    REQUIRE(result.IsSuccess());
    REQUIRE(result.GetValue() == "{ inner : { warned : OK } }");
    REQUIRE(result.GetErrors() ==
            std::vector<std::string>{
                "In property inner.warned, non-fatal warning"});
  }
}
