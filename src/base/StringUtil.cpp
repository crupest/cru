#include "cru/base/StringUtil.h"
#include "cru/base/Base.h"

#include <unicode/uchar.h>
#include <unicode/unistr.h>

#include <string_view>
#include <utility>

namespace cru::string {

bool CaseInsensitiveEqual(std::string_view left, std::string_view right) {
  auto icu_left = icu::UnicodeString::fromUTF8(left).foldCase();
  auto icu_right = icu::UnicodeString::fromUTF8(right).foldCase();
  return icu_left == icu_right;
}

static Index FindFirstNonSpace(std::string_view str) {
  Index pos = 0;
  while (pos < str.size()) {
    Index next = pos;
    auto c = Utf8NextCodePoint(str.data(), str.size(), pos, &next);
    if (!u_isspace(c)) return pos;
    pos = next;
  }
  return str.size();
}

static Index FindLastNonSpace(std::string_view str) {
  Index pos = str.size();
  while (pos > 0) {
    Index previous = pos;
    auto c = Utf8PreviousCodePoint(str.data(), str.size(), pos, &pos);
    if (!u_isspace(c)) return previous;
  }
  return 0;
}

std::string TrimBegin(std::string_view str) {
  auto first = FindFirstNonSpace(str);
  return std::string(str.data() + first, str.size() - first);
}

std::string TrimEnd(std::string_view str) {
  auto last = FindLastNonSpace(str);
  return std::string(str.data(), last);
}

std::string Trim(std::string_view str) {
  auto first = FindFirstNonSpace(str);
  auto last = FindLastNonSpace(str);
  if (first >= last) return {};
  return std::string(str.data() + first, last - first);
}

bool IsSpace(std::string_view str) {
  for (auto c : Utf8CodePointIterator(str.data(), str.size())) {
    if (!u_isspace(c)) return false;
  }
  return true;
}

std::vector<std::string> Split(std::string_view str, std::string_view sep,
                               SplitOption options) {
  using size_type = std::string_view::size_type;

  if (sep.empty()) throw std::invalid_argument("Sep can't be empty.");
  if (str.empty()) return {};

  size_type current_pos = 0;
  std::vector<std::string> result;

  while (current_pos != std::string_view::npos) {
    if (current_pos == str.size()) {
      if (!options.Has(SplitOptions::RemoveEmpty)) {
        result.push_back({});
      }
      break;
    }

    auto next_pos = str.find(sep, current_pos);
    auto sub = str.substr(current_pos, next_pos == std::string_view::npos
                                           ? std::string_view::npos
                                           : next_pos - current_pos);
    if (!(options.Has(SplitOptions::RemoveEmpty) && sub.empty() ||
          options.Has(SplitOptions::RemoveSpace) && IsSpace(sub))) {
      result.push_back(std::string(sub));
    }
    current_pos = next_pos == std::string_view::npos ? std::string_view::npos
                                                     : next_pos + sep.size();
  }

  return result;
}

using details::ExtractBits;

CodePoint Utf8NextCodePoint(const Utf8CodeUnit* ptr, Index size, Index current,
                            Index* next_position) {
  CodePoint result;

  if (current >= size) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = static_cast<Utf8CodeUnit>(ptr[current++]);

    auto read_next_following_code = [ptr, size, &current]() -> CodePoint {
      if (current == size)
        throw TextEncodeException(
            "Unexpected end when read continuing byte of multi-byte code "
            "point.");

      const auto u = static_cast<Utf8CodeUnit>(ptr[current]);
      if (!(u & (1u << 7)) || (u & (1u << 6))) {
        throw TextEncodeException(
            "Unexpected bad-format (not 0b10xxxxxx) continuing byte of "
            "multi-byte code point.");
      }

      return ExtractBits<Utf8CodeUnit, 6, CodePoint>(ptr[current++]);
    };

    if ((1u << 7) & cu0) {
      if ((1u << 6) & cu0) {      // 2~4-length code point
        if ((1u << 5) & cu0) {    // 3~4-length code point
          if ((1u << 4) & cu0) {  // 4-length code point
            if (cu0 & (1u << 3)) {
              throw TextEncodeException(
                  "Unexpected bad-format begin byte (not 0b11110xxx) of 4-byte"
                  "code point.");
            }

            const CodePoint s0 = ExtractBits<Utf8CodeUnit, 3, CodePoint>(cu0)
                                 << (6 * 3);
            const CodePoint s1 = read_next_following_code() << (6 * 2);
            const CodePoint s2 = read_next_following_code() << 6;
            const CodePoint s3 = read_next_following_code();
            result = s0 + s1 + s2 + s3;
          } else {  // 3-length code point
            const CodePoint s0 = ExtractBits<Utf8CodeUnit, 4, CodePoint>(cu0)
                                 << (6 * 2);
            const CodePoint s1 = read_next_following_code() << 6;
            const CodePoint s2 = read_next_following_code();
            result = s0 + s1 + s2;
          }
        } else {  // 2-length code point
          const CodePoint s0 = ExtractBits<Utf8CodeUnit, 5, CodePoint>(cu0)
                               << 6;
          const CodePoint s1 = read_next_following_code();
          result = s0 + s1;
        }
      } else {
        throw TextEncodeException(
            "Unexpected bad-format (0b10xxxxxx) begin byte of a code point.");
      }
    } else {
      result = static_cast<CodePoint>(cu0);
    }
  }

  if (next_position != nullptr) *next_position = current;
  return result;
}

CodePoint Utf8PreviousCodePoint(const Utf8CodeUnit* ptr, Index size,
                                Index current, Index* previous_position) {
  CRU_UNUSED(size)

  CodePoint result;
  if (current <= 0) {
    result = k_invalid_code_point;
  } else {
    current--;
    int i;
    for (i = 0; i < 4; i++) {
      if (IsUtf8LeadingByte(ptr[current])) {
        break;
      }
      current--;
    }
    if (i == 4) {
      throw TextEncodeException(
          "Failed to find UTF-8 leading byte in 4 previous bytes.");
    }

    result = Utf8NextCodePoint(ptr, size, current, nullptr);
  }

  if (previous_position != nullptr) *previous_position = current;
  return result;
}

bool Utf8IsValidInsertPosition(const Utf8CodeUnit* ptr, Index size,
                               Index position) {
  return position == 0 || position == size ||
         (position > 0 && position < size && IsUtf8LeadingByte(ptr[position]));
}

CodePoint Utf16NextCodePoint(const Utf16CodeUnit* ptr, Index size,
                             Index current, Index* next_position) {
  CodePoint result;

  if (current >= size) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = ptr[current++];

    if (!IsUtf16SurrogatePairCodeUnit(cu0)) {  // 1-length code point
      result = static_cast<CodePoint>(cu0);
    } else if (IsUtf16SurrogatePairLeading(cu0)) {  // 2-length code point
      if (current >= size) {
        throw TextEncodeException(
            "Unexpected end when reading second code unit of surrogate pair.");
      }
      const auto cu1 = ptr[current++];

      if (!IsUtf16SurrogatePairTrailing(cu1)) {
        throw TextEncodeException(
            "Unexpected bad-range second code unit of surrogate pair.");
      }

      const auto s0 = ExtractBits<Utf16CodeUnit, 10, CodePoint>(cu0) << 10;
      const auto s1 = ExtractBits<Utf16CodeUnit, 10, CodePoint>(cu1);

      result = s0 + s1 + 0x10000;

    } else {
      throw TextEncodeException(
          "Unexpected bad-range first code unit of surrogate pair.");
    }
  }

  if (next_position != nullptr) *next_position = current;
  return result;
}

CodePoint Utf16PreviousCodePoint(const Utf16CodeUnit* ptr, Index size,
                                 Index current, Index* previous_position) {
  CRU_UNUSED(size)

  CodePoint result;
  if (current <= 0) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = ptr[--current];

    if (!IsUtf16SurrogatePairCodeUnit(cu0)) {  // 1-length code point
      result = static_cast<CodePoint>(cu0);
    } else if (IsUtf16SurrogatePairTrailing(cu0)) {  // 2-length code point
      if (current <= 0) {
        throw TextEncodeException(
            "Unexpected end when reading first code unit of surrogate pair.");
      }
      const auto cu1 = ptr[--current];

      if (!IsUtf16SurrogatePairLeading(cu1)) {
        throw TextEncodeException(
            "Unexpected bad-range first code unit of surrogate pair.");
      }

      const auto s0 = ExtractBits<Utf16CodeUnit, 10, CodePoint>(cu1) << 10;
      const auto s1 = ExtractBits<Utf16CodeUnit, 10, CodePoint>(cu0);

      result = s0 + s1 + 0x10000;

    } else {
      throw TextEncodeException(
          "Unexpected bad-range second code unit of surrogate pair.");
    }
  }

  if (previous_position != nullptr) *previous_position = current;
  return result;
}

bool Utf16IsValidInsertPosition(const Utf16CodeUnit* ptr, Index size,
                                Index position) {
  if (position < 0) return false;
  if (position > size) return false;
  if (position == 0) return true;
  if (position == size) return true;
  return !IsUtf16SurrogatePairTrailing(ptr[position]);
}

template <typename CharType,
          NextCodePointFunctionType<CharType> NextCodePointFunction>
static Index IndexCodeUnitToCodePoint(const CharType* ptr, Index size,
                                      Index position) {
  CodePointIterator<CharType, NextCodePointFunction> iter(ptr, size);
  Index result = 0;
  while (!iter.IsPastEnd() && iter.GetPosition() < position) {
    ++iter;
    ++result;
  }
  return result;
}

template <typename CharType,
          NextCodePointFunctionType<CharType> NextCodePointFunction>
static Index IndexCodePointToCodeUnit(const CharType* ptr, Index size,
                                      Index position) {
  CodePointIterator<CharType, NextCodePointFunction> iter(ptr, size);
  for (Index i = 0; i < position; i++) {
    ++iter;
  }
  return iter.GetPosition();
}

Index Utf8IndexCodeUnitToCodePoint(const Utf8CodeUnit* ptr, Index size,
                                   Index position) {
  return IndexCodeUnitToCodePoint<Utf8CodeUnit, Utf8NextCodePoint>(ptr, size,
                                                                   position);
}

Index Utf8IndexCodePointToCodeUnit(const Utf8CodeUnit* ptr, Index size,
                                   Index position) {
  return IndexCodePointToCodeUnit<Utf8CodeUnit, Utf8NextCodePoint>(ptr, size,
                                                                   position);
}

Index Utf16IndexCodeUnitToCodePoint(const Utf16CodeUnit* ptr, Index size,
                                    Index position) {
  return IndexCodeUnitToCodePoint<Utf16CodeUnit, Utf16NextCodePoint>(ptr, size,
                                                                     position);
}
Index Utf16IndexCodePointToCodeUnit(const Utf16CodeUnit* ptr, Index size,
                                    Index position) {
  return IndexCodePointToCodeUnit<Utf16CodeUnit, Utf16NextCodePoint>(ptr, size,
                                                                     position);
}

#ifdef _WIN32
std::wstring ToUtf16WString(std::string_view str) {
  Utf8CodePointIterator iter(str.data(), str.size());
  std::wstring result;
  for (auto c : iter) {
    Utf16EncodeCodePointAppend(c, [&result](char16_t c) { result += c; });
  }
  return result;
}

std::string ToUtf8String(std::wstring_view str) {
  Utf16CodePointIterator iter(reinterpret_cast<const char16_t*>(str.data()),
                              str.size());
  std::string result;
  for (auto c : iter) {
    Utf8EncodeCodePointAppend(c, [&result](char c) { result += c; });
  }
  return result;
}

#endif

StringBreakIterator::StringBreakIterator(std::string str)
    : utf8_position_(0), utf16_position_(0) {
  UErrorCode error_code = U_ZERO_ERROR;
  character_break_iterator_.reset(icu::BreakIterator::createCharacterInstance(
      icu::Locale::getDefault(), error_code));
  if (U_FAILURE(error_code)) {
    throw Exception("Failed to create character break iterator.");
  }

  word_break_iterator_.reset(icu::BreakIterator::createWordInstance(
      icu::Locale::getDefault(), error_code));
  if (U_FAILURE(error_code)) {
    throw Exception("Failed to create word break iterator.");
  }

  line_break_iterator_.reset(icu::BreakIterator::createLineInstance(
      icu::Locale::getDefault(), error_code));
  if (U_FAILURE(error_code)) {
    throw Exception("Failed to create line break iterator.");
  }

  SetText(std::move(str));
}

void StringBreakIterator::SetText(std::string str) {
  str_ = std::move(str);
  icu_str_ = icu::UnicodeString::fromUTF8(str_);
  utf8_position_ = 0;
  utf16_position_ = 0;
  character_break_iterator_->setText(icu_str_);
  word_break_iterator_->setText(icu_str_);
  line_break_iterator_->setText(icu_str_);
}

void StringBreakIterator::SetCurrentPosition(Index position) {
  CheckArgumentRange(position, 0, str_.size(), "position", true);
  utf8_position_ = position;
  utf16_position_ = Utf16IndexCodePointToCodeUnit(
      icu_str_.getBuffer(), icu_str_.length(),
      Utf8IndexCodeUnitToCodePoint(str_, position));
}

Index StringBreakIterator::NextChar() {
  if (utf8_position_ >= str_.size()) {
    return utf8_position_;
  }
  SetCurrentPositionByUtf16(
      character_break_iterator_->following(utf16_position_));
  return utf8_position_;
}

Index StringBreakIterator::PreviousChar() {
  if (utf8_position_ <= 0) {
    return utf8_position_;
  }
  SetCurrentPositionByUtf16(
      character_break_iterator_->preceding(utf16_position_));
  return utf8_position_;
}

Index StringBreakIterator::NextWord() {
  if (utf8_position_ >= str_.size()) {
    return utf8_position_;
  }
  SetCurrentPositionByUtf16(word_break_iterator_->following(utf16_position_));
  return utf8_position_;
}

Index StringBreakIterator::PreviousWord() {
  if (utf8_position_ <= 0) {
    return utf8_position_;
  }
  SetCurrentPositionByUtf16(word_break_iterator_->preceding(utf16_position_));
  return utf8_position_;
}

Index StringBreakIterator::NextLine() {
  if (utf8_position_ >= str_.size()) {
    return utf8_position_;
  }
  SetCurrentPositionByUtf16(line_break_iterator_->following(utf16_position_));
  return utf8_position_;
}

Index StringBreakIterator::PreviousLine() {
  if (utf8_position_ <= 0) {
    return utf8_position_;
  }
  SetCurrentPositionByUtf16(line_break_iterator_->preceding(utf16_position_));
  return utf8_position_;
}

void StringBreakIterator::SetCurrentPositionByUtf16(Index position) {
  utf8_position_ = Utf8IndexCodePointToCodeUnit(
      str_, Utf16IndexCodeUnitToCodePoint(icu_str_.getBuffer(),
                                          icu_str_.length(), position));
  utf16_position_ = position;
}

}  // namespace cru::string
