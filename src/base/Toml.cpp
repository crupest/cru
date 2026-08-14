#include "cru/base/Toml.h"

#include "cru/base/Json.h"
#include "cru/base/StringUtil.h"

namespace cru::toml {
bool TomlSection::HasKey(std::string_view key) const {
  return values_.contains(key);
}

json::JsonValue& TomlSection::GetValue(std::string_view key) {
  return values_.at(key);
}

const json::JsonValue& TomlSection::GetValue(std::string_view key) const {
  return values_.at(key);
}

void TomlSection::SetValue(std::string_view key, json::JsonValue value) {
  values_.insert_or_assign(key, std::move(value));
}

bool TomlSection::DeleteValue(std::string_view key) {
  return values_.erase(key) != 0;
}

bool TomlDocument::HasSection(std::string_view name) const {
  return sections_.contains(name);
}

TomlSection& TomlDocument::GetSection(std::string_view name) {
  return sections_.at(name);
}

const TomlSection& TomlDocument::GetSection(std::string_view name) const {
  return sections_.at(name);
}

TomlSection& TomlDocument::GetSectionOrCreate(std::string_view name) {
  return sections_[name];
}

void TomlDocument::SetSection(std::string_view name, TomlSection section) {
  sections_.insert_or_assign(name, section);
}

bool TomlDocument::DeleteSection(std::string_view name) {
  return sections_.erase(name) > 0;
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
