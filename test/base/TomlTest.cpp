#include "cru/base/Toml.h"

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace cru::toml;

namespace {
template <typename Action>
void RequireTomlThrowsContaining(Action action, std::string_view text) {
  try {
    action();
    FAIL("Expected TOML operation to throw");
  } catch (const TomlParsingException& exception) {
    REQUIRE(std::string_view(exception.what()).find(text) !=
            std::string_view::npos);
  }
}

static_assert(std::same_as<TomlSection::value_type,
                           std::pair<std::string, cru::json::JsonValue>>);
static_assert(
    std::same_as<TomlSection::storage_type,
                 std::vector<std::pair<std::string, cru::json::JsonValue>>>);
static_assert(std::same_as<TomlSection::size_type,
                           typename TomlSection::storage_type::size_type>);
static_assert(
    std::same_as<TomlSection::difference_type,
                 typename TomlSection::storage_type::difference_type>);
static_assert(std::same_as<TomlSection::reference,
                           typename TomlSection::storage_type::reference>);
static_assert(
    std::same_as<TomlSection::const_reference,
                 typename TomlSection::storage_type::const_reference>);
static_assert(std::same_as<TomlSection::pointer,
                           typename TomlSection::storage_type::pointer>);
static_assert(std::same_as<TomlSection::const_pointer,
                           typename TomlSection::storage_type::const_pointer>);
static_assert(std::same_as<TomlSection::iterator,
                           typename TomlSection::storage_type::iterator>);
static_assert(std::same_as<TomlSection::const_iterator,
                           typename TomlSection::storage_type::const_iterator>);
static_assert(std::random_access_iterator<TomlSection::iterator>);
static_assert(std::random_access_iterator<TomlSection::const_iterator>);
static_assert(std::same_as<decltype(std::declval<TomlSection&>().begin()),
                           TomlSection::iterator>);
static_assert(std::same_as<decltype(std::declval<const TomlSection&>().begin()),
                           TomlSection::const_iterator>);
static_assert(std::same_as<decltype(std::declval<TomlSection&>().cbegin()),
                           TomlSection::const_iterator>);

static_assert(std::same_as<TomlDocument::value_type,
                           std::pair<std::string, TomlSection>>);
static_assert(std::same_as<TomlDocument::storage_type,
                           std::vector<std::pair<std::string, TomlSection>>>);
static_assert(std::same_as<TomlDocument::size_type,
                           typename TomlDocument::storage_type::size_type>);
static_assert(
    std::same_as<TomlDocument::difference_type,
                 typename TomlDocument::storage_type::difference_type>);
static_assert(std::same_as<TomlDocument::reference,
                           typename TomlDocument::storage_type::reference>);
static_assert(
    std::same_as<TomlDocument::const_reference,
                 typename TomlDocument::storage_type::const_reference>);
static_assert(std::same_as<TomlDocument::pointer,
                           typename TomlDocument::storage_type::pointer>);
static_assert(std::same_as<TomlDocument::const_pointer,
                           typename TomlDocument::storage_type::const_pointer>);
static_assert(std::same_as<TomlDocument::iterator,
                           typename TomlDocument::storage_type::iterator>);
static_assert(
    std::same_as<TomlDocument::const_iterator,
                 typename TomlDocument::storage_type::const_iterator>);
static_assert(std::random_access_iterator<TomlDocument::iterator>);
static_assert(std::random_access_iterator<TomlDocument::const_iterator>);
static_assert(std::same_as<decltype(std::declval<TomlDocument&>().begin()),
                           TomlDocument::iterator>);
static_assert(
    std::same_as<decltype(std::declval<const TomlDocument&>().begin()),
                 TomlDocument::const_iterator>);
static_assert(std::same_as<decltype(std::declval<TomlDocument&>().cbegin()),
                           TomlDocument::const_iterator>);
}  // namespace

TEST_CASE("TomlSection supports value lookup mutation deletion and iteration",
          "[toml]") {
  TomlSection section;
  REQUIRE(section.begin() == section.end());
  REQUIRE_FALSE(section.HasKey("title"));

  section.SetValue("title", cru::json::JsonValue("demo"));
  section.SetValue("enabled", cru::json::JsonValue(true));
  REQUIRE(section.HasKey("title"));
  REQUIRE(section.GetValue("title") == "demo");
  REQUIRE(section.GetValue("enabled") == true);

  section.SetValue("title", cru::json::JsonValue("changed"));
  REQUIRE(section.GetValue("title") == "changed");

  TomlSection::iterator title = section.begin();
  REQUIRE(title->first == "title");
  REQUIRE(title->second == "changed");
  title->second = false;
  REQUIRE(section.GetValue("title") == false);

  const TomlSection& const_section = section;
  TomlSection::const_iterator enabled = const_section.begin() + 1;
  REQUIRE(enabled->first == "enabled");
  REQUIRE(enabled->second == true);
  REQUIRE(const_section.cbegin()->first == "title");
  REQUIRE(const_section.cend() - const_section.cbegin() == 2);

  std::vector<std::string> value_names;
  for (TomlSection::value_type& value : section) {
    value_names.push_back(value.first);
    if (value.first == "enabled") {
      value.second = false;
    }
  }
  REQUIRE(value_names == std::vector<std::string>{"title", "enabled"});
  REQUIRE(section.GetValue("enabled") == false);

  std::vector<std::string> const_value_names;
  for (const TomlSection::value_type& value : const_section) {
    const_value_names.push_back(value.first);
  }
  REQUIRE(const_value_names == value_names);

  REQUIRE_FALSE(section.DeleteValue("missing"));
  REQUIRE(section.DeleteValue("title"));
  REQUIRE_FALSE(section.HasKey("title"));
  REQUIRE(section.begin()->first == "enabled");
}

TEST_CASE(
    "TomlDocument supports section lookup mutation deletion and iteration",
    "[toml]") {
  TomlDocument document;
  REQUIRE(document.begin() == document.end());
  REQUIRE_FALSE(document.HasSection("root"));
  REQUIRE_THROWS_AS(document.GetSection("root"), cru::Exception);

  TomlSection root;
  root.SetValue("title", cru::json::JsonValue("demo"));
  document.SetSection("root", std::move(root));

  TomlSection& root_section = document.GetSection("root");
  REQUIRE(root_section.GetValue("title") == "demo");

  TomlSection& owner = document.GetSectionOrCreate("owner");
  owner.SetValue("name", cru::json::JsonValue("Ada"));
  REQUIRE(&document.GetSectionOrCreate("owner") == &owner);

  std::vector<std::string> section_names;
  for (TomlDocument::value_type& section : document) {
    section_names.push_back(section.first);
    if (section.first == "owner") {
      section.second.SetValue("active", cru::json::JsonValue(true));
    }
  }
  REQUIRE(section_names == std::vector<std::string>{"root", "owner"});
  REQUIRE(document.GetSection("owner").GetValue("active") == true);

  const TomlDocument& const_document = document;
  std::vector<std::string> const_section_names;
  for (const TomlDocument::value_type& section : const_document) {
    const_section_names.push_back(section.first);
  }
  REQUIRE(const_section_names == section_names);

  TomlDocument::const_iterator first_section = const_document.begin();
  REQUIRE(first_section->first == "root");
  REQUIRE(const_document.GetSection("owner").GetValue("name") == "Ada");
  REQUIRE(const_document.cend() - const_document.cbegin() == 2);
  REQUIRE(const_document.HasSection("owner"));
  REQUIRE_THROWS_AS(const_document.GetSection("missing"), cru::Exception);

  TomlSection replacement;
  replacement.SetValue("name", cru::json::JsonValue("Grace"));
  document.SetSection("owner", std::move(replacement));
  REQUIRE(document.GetSection("owner").GetValue("name") == "Grace");

  REQUIRE_FALSE(document.DeleteSection("missing"));
  REQUIRE(document.DeleteSection("root"));
  REQUIRE_FALSE(document.HasSection("root"));
  REQUIRE_THROWS_AS(document.GetSection("root"), cru::Exception);
  REQUIRE(document.begin()->first == "owner");
}

TEST_CASE("CruTomlParserTest Simple", "[toml]") {
  TomlParser parser(
      R"(
a1 = v1
a2 = v2
# comment

[s1]
# comment
a3 = v3
a4 = v4

[s2]
a5 = v5
a6 = v6
  )");
  auto document = parser.Parse();
  REQUIRE(document.GetSection("").GetValue("a1") == "v1");
  REQUIRE(document.GetSection("").GetValue("a2") == "v2");
  REQUIRE(document.GetSection("s1").GetValue("a3") == "v3");
  REQUIRE(document.GetSection("s1").GetValue("a4") == "v4");
  REQUIRE(document.GetSection("s2").GetValue("a5") == "v5");
  REQUIRE(document.GetSection("s2").GetValue("a6") == "v6");
}

TEST_CASE("TomlParser parses empty and whitespace-only sources", "[toml]") {
  TomlDocument empty = ParseToml("");
  REQUIRE(empty.HasSection(""));
  REQUIRE(empty.GetSection("").begin() == empty.GetSection("").end());

  TomlDocument whitespace = ParseToml(" \t\n\n\t ");
  REQUIRE(whitespace.HasSection(""));
  REQUIRE(whitespace.GetSection("").begin() == whitespace.GetSection("").end());
}

TEST_CASE("TomlParser parses JSON values empty values and fallback strings",
          "[toml]") {
  TomlDocument document = ParseToml(
      "empty=\n"
      "space_empty =   \n"
      "null_value = null\n"
      "enabled = true\n"
      "count = 12.5\n"
      "quoted = \"hello\"\n"
      "array = [1, false, \"three\"]\n"
      "object = {\"name\": \"widget\", \"enabled\": false}\n"
      "fallback = bare value  \n"
      "eof_fallback = bare eof  ");

  const TomlSection& root = document.GetSection("");
  REQUIRE(root.GetValue("empty") == "");
  REQUIRE(root.GetValue("space_empty") == "");
  REQUIRE(root.GetValue("null_value").IsNull());
  REQUIRE(root.GetValue("enabled") == true);
  REQUIRE(root.GetValue("count") == 12.5);
  REQUIRE(root.GetValue("quoted") == "hello");
  REQUIRE(root.GetValue("fallback") == "bare value");
  REQUIRE(root.GetValue("eof_fallback") == "bare eof");

  const cru::json::JsonValue& array = root.GetValue("array");
  REQUIRE(array.IsArray());
  REQUIRE(array.GetSize() == 3);
  REQUIRE(array.Get(0) == 1.0);
  REQUIRE(array.Get(1) == false);
  REQUIRE(array.Get(2) == "three");

  const cru::json::JsonValue& object = root.GetValue("object");
  REQUIRE(object.IsObject());
  REQUIRE(object.Get("name") == "widget");
  REQUIRE(object.Get("enabled") == false);
}

TEST_CASE("TomlParser parses JSON values spanning multiple lines", "[toml]") {
  TomlDocument document = ParseToml(
      R"(config = {
  "name": "widget",
  "items": [
    1,
    {
      "active": true
    }
  ]
}
after = done
)");

  const TomlSection& root = document.GetSection("");
  const cru::json::JsonValue& config = root.GetValue("config");
  REQUIRE(config.IsObject());
  REQUIRE(config.Get("name") == "widget");

  const cru::json::JsonValue& items = config.Get("items");
  REQUIRE(items.IsArray());
  REQUIRE(items.GetSize() == 2);
  REQUIRE(items.Get(0) == 1.0);
  REQUIRE(items.Get(1).Get("active") == true);
  REQUIRE(root.GetValue("after") == "done");
}

TEST_CASE("TomlParser parses a larger mixed document", "[toml]") {
  TomlDocument document = ParseToml(
      R"(
# root settings
title = "Cru UI"
enabled = true
version = 3
tags = ["ui", "native", "cpp"]
limits = {"width": 1280, "height": 720}
empty =
asset_path = C:\cru\themes

[window]
width = 1024
height = 768
position = {"x": 10, "y": 20}
modes = [
  "windowed",
  "fullscreen"
]

[theme]
name = dark
palette = {
  "background": "#101010",
  "accent": "#0070C0",
  "high_contrast": true
}

[metadata]
owner = tooling
note = eof fallback value)");

  const TomlSection& root = document.GetSection("");
  REQUIRE(root.GetValue("title") == "Cru UI");
  REQUIRE(root.GetValue("enabled") == true);
  REQUIRE(root.GetValue("version") == 3.0);
  REQUIRE(root.GetValue("empty") == "");
  REQUIRE(root.GetValue("asset_path") == R"(C:\cru\themes)");

  const cru::json::JsonValue& tags = root.GetValue("tags");
  REQUIRE(tags.IsArray());
  REQUIRE(tags.GetSize() == 3);
  REQUIRE(tags.Get(0) == "ui");
  REQUIRE(tags.Get(1) == "native");
  REQUIRE(tags.Get(2) == "cpp");

  const cru::json::JsonValue& limits = root.GetValue("limits");
  REQUIRE(limits.IsObject());
  REQUIRE(limits.Get("width") == 1280.0);
  REQUIRE(limits.Get("height") == 720.0);

  const TomlSection& window = document.GetSection("window");
  REQUIRE(window.GetValue("width") == 1024.0);
  REQUIRE(window.GetValue("height") == 768.0);
  REQUIRE(window.GetValue("position").Get("x") == 10.0);
  REQUIRE(window.GetValue("position").Get("y") == 20.0);
  REQUIRE(window.GetValue("modes").Get(0) == "windowed");
  REQUIRE(window.GetValue("modes").Get(1) == "fullscreen");

  const TomlSection& theme = document.GetSection("theme");
  REQUIRE(theme.GetValue("name") == "dark");
  REQUIRE(theme.GetValue("palette").Get("background") == "#101010");
  REQUIRE(theme.GetValue("palette").Get("accent") == "#0070C0");
  REQUIRE(theme.GetValue("palette").Get("high_contrast") == true);

  const TomlSection& metadata = document.GetSection("metadata");
  REQUIRE(metadata.GetValue("owner") == "tooling");
  REQUIRE(metadata.GetValue("note") == "eof fallback value");
}

TEST_CASE("TomlParser handles comments sections and repeated parses",
          "[toml]") {
  TomlParser parser(
      "# root comment\n"
      "root = 1\n"
      "\n"
      "[owner]\n"
      "# owner comment\n"
      "name = \"Ada\"\n"
      "\n"
      "[empty]");

  TomlDocument first = parser.Parse();
  TomlDocument second = parser.Parse();

  REQUIRE(first.GetSection("").GetValue("root") == 1.0);
  REQUIRE(first.GetSection("owner").GetValue("name") == "Ada");
  REQUIRE(first.HasSection("empty"));
  REQUIRE(first.GetSection("empty").begin() == first.GetSection("empty").end());

  REQUIRE(second.GetSection("").GetValue("root") == 1.0);
  REQUIRE(second.GetSection("owner").GetValue("name") == "Ada");
  REQUIRE(second.HasSection("empty"));
}

TEST_CASE("TomlParser rejects malformed section headers", "[toml]") {
  RequireTomlThrowsContaining([] { ParseToml("[section] key = 1\n"); },
                              "Expected newline after section header");
  RequireTomlThrowsContaining(
      [] { ParseToml("[section\nkey = 1\n"); },
      "Unexpected end of string while reading until ']'");
}

TEST_CASE("TomlParser rejects malformed key value lines", "[toml]") {
  RequireTomlThrowsContaining([] { ParseToml("key\nnext = 1\n"); },
                              "Expected '=' after key at the same line");
  RequireTomlThrowsContaining(
      [] { ParseToml("key"); },
      "Unexpected end of string while reading until '='");
  RequireTomlThrowsContaining([] { ParseToml("value = 1 extra\n"); },
                              "Expected newline after value");
}

TEST_CASE("TomlParser accepts EOF-terminated comments and fallback strings",
          "[toml]") {
  TomlDocument comment = ParseToml("# comment without newline");
  REQUIRE(comment.HasSection(""));
  REQUIRE(comment.GetSection("").begin() == comment.GetSection("").end());

  TomlDocument fallback = ParseToml("value = bare");
  REQUIRE(fallback.GetSection("").GetValue("value") == "bare");
}
