#include "cru/base/Toml.h"

#include "cru/base/Json.h"
#include "cru/base/StringUtil.h"

#include <algorithm>

namespace cru::toml {

namespace {
template <typename T>
auto FindIteratorByKey(T& values, std::string_view key) {
  return std::ranges::find_if(
      values, [key](const auto& pair) { return pair.first == key; });
}
}  // namespace

bool TomlSection::HasKey(std::string_view key) const {
  return FindIteratorByKey(values_, key) != values_.end();
}

json::JsonValue& TomlSection::GetValue(std::string_view key) {
  auto it = FindIteratorByKey(values_, key);
  if (it == values_.end()) {
    throw Exception("Key not found: " + std::string(key));
  }
  return it->second;
}

const json::JsonValue& TomlSection::GetValue(std::string_view key) const {
  auto it = FindIteratorByKey(values_, key);
  if (it == values_.end()) {
    throw Exception("Key not found: " + std::string(key));
  }
  return it->second;
}

void TomlSection::SetValue(std::string_view key, json::JsonValue value) {
  auto it = FindIteratorByKey(values_, key);
  if (it == values_.end()) {
    values_.emplace_back(key, std::move(value));
  } else {
    it->second = std::move(value);
  }
}

bool TomlSection::DeleteValue(std::string_view key) {
  auto it = FindIteratorByKey(values_, key);
  if (it != values_.end()) {
    values_.erase(it);
    return true;
  }
  return false;
}

bool TomlDocument::HasSection(std::string_view name) const {
  return FindIteratorByKey(sections_, name) != sections_.end();
}

TomlSection& TomlDocument::GetSection(std::string_view name) {
  auto it = FindIteratorByKey(sections_, name);
  if (it == sections_.end()) {
    throw Exception("Section not found: " + std::string(name));
  }
  return it->second;
}

const TomlSection& TomlDocument::GetSection(std::string_view name) const {
  auto it = FindIteratorByKey(sections_, name);
  if (it == sections_.end()) {
    throw Exception("Section not found: " + std::string(name));
  }
  return it->second;
}

TomlSection& TomlDocument::GetSectionOrCreate(std::string_view name) {
  auto it = FindIteratorByKey(sections_, name);
  if (it == sections_.end()) {
    sections_.emplace_back(name, TomlSection());
    return sections_.back().second;
  }
  return it->second;
}

void TomlDocument::SetSection(std::string_view name, TomlSection section) {
  auto it = FindIteratorByKey(sections_, name);
  if (it == sections_.end()) {
    sections_.emplace_back(name, std::move(section));
  } else {
    it->second = std::move(section);
  }
}

bool TomlDocument::DeleteSection(std::string_view name) {
  auto it = FindIteratorByKey(sections_, name);
  if (it != sections_.end()) {
    sections_.erase(it);
    return true;
  }
  return false;
}

TomlParser::TomlParser(std::string_view source)
    : source_(std::move(source)), position_(0), line_(1) {}

TomlDocument TomlParser::Parse() {
  position_ = 0;
  line_ = 1;

  TomlDocument document;

  TomlSection* current_section = &document.GetSectionOrCreate("");

  while (!IsEnd()) {
    ReadAllSpace();
    if (IsEnd()) {
      break;
    }

    if (PeekNext() == '[') {
      ReadNext();
      auto section_name_view = ReadUntil(']', true);
      auto current_line = line_;
      ReadAllSpace();
      if (!IsEnd() && line_ == current_line) {
        Error("Expected newline after section header.");
      }
      current_section = &document.GetSectionOrCreate(section_name_view);
    } else if (PeekNext() == '#') {
      // Skip comment
      ReadUntil('\n', true, true);
    } else {
      auto current_line = line_;
      auto key = cru::string::TrimEnd(ReadUntil('=', true));
      if (IsEnd() || line_ != current_line) {
        Error("Expected '=' after key at the same line.");
      }
      ReadAllSpace();
      json::JsonValue value;
      if (IsEnd() || line_ != current_line) {
        value = "";
      } else {
        try {
          Index trailing_junk_start = 0;
          value = json::ParseJsonAllowTrailingJunk(source_.substr(position_),
                                                   &trailing_junk_start);
          position_ += trailing_junk_start;
        } catch (const Exception& e) {
          value = cru::string::TrimEnd(ReadUntil('\n', false, true));
        }
        current_line = line_;
        ReadAllSpace();
        if (!IsEnd() && line_ == current_line) {
          Error("Expected newline after value.");
        }
      }

      current_section->SetValue(key, std::move(value));
    }
  }

  return document;
}

[[noreturn]] void TomlParser::Error(std::string_view message) const {
  throw TomlParsingException(
      std::format("TomlParser::Parse failed. {}", message));
}

bool TomlParser::IsEnd() const { return position_ >= source_.size(); }

bool TomlParser::IsJsonSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n';
}

void TomlParser::ReadAllSpace() {
  while (!IsEnd() && IsJsonSpace(source_[position_])) {
    ReadNext();
  }
}

char TomlParser::PeekNext() {
  if (IsEnd()) {
    Error("Unexpected end of string.");
  }
  return source_[position_];
}

char TomlParser::ReadNext() {
  if (IsEnd()) {
    Error("Unexpected end of string.");
  }
  if (source_[position_] == '\n') {
    line_++;
  }
  return source_[position_++];
}

void TomlParser::ReadExpected(std::string_view expected) {
  for (char c : expected) {
    if (IsEnd() || ReadNext() != c) {
      Error(std::format("Expected '{}'.", expected));
    }
  }
}

std::string_view TomlParser::ReadUntil(char delimiter, bool consume_delimiter,
                                       bool allow_eof) {
  auto old_position = position_;
  while (!IsEnd() && PeekNext() != delimiter) {
    ReadNext();
  }
  if (IsEnd() && !allow_eof) {
    Error(std::format("Unexpected end of string while reading until '{}'.",
                      delimiter));
  }
  std::string_view result =
      source_.substr(old_position, position_ - old_position);
  if (consume_delimiter && !IsEnd()) {
    ReadNext();
  }
  return result;
}

TomlDocument ParseToml(std::string_view source) {
  TomlParser parser(source);
  return parser.Parse();
}
}  // namespace cru::toml
