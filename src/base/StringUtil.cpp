#include "cru/base/StringUtil.h"
#include "cru/base/Base.h"

#include <algorithm>
#include <cctype>
#include <compare>
#include <string_view>

namespace cru::string {

std::weak_ordering CaseInsensitiveCompare(std::string_view left,
                                          std::string_view right) {
  return std::lexicographical_compare_three_way(
      left.cbegin(), left.cend(), right.cbegin(), right.cend(),
      [](char left, char right) {
        auto l = tolower(left), r = tolower(right);
        return l < r ? std::weak_ordering::less
                     : (l == r ? std::weak_ordering::equivalent
                               : std::weak_ordering::greater);
      });
}

std::string TrimBegin(std::string_view str) {
  auto iter = std::find_if(str.cbegin(), str.cend(),
                           [](char c) { return !std::isspace(c); });
  return std::string(iter, str.cend());
}

std::string TrimEnd(std::string_view str) {
  auto iter = std::find_if(str.crbegin(), str.crend(),
                           [](char c) { return !std::isspace(c); });
  return std::string(str.cbegin(), str.cend() - (iter - str.crbegin()));
}

std::string Trim(std::string_view str) {
  auto iter1 = std::find_if(str.cbegin(), str.cend(),
                            [](char c) { return !std::isspace(c); });
  auto iter2 = std::find_if(str.crbegin(), str.crend(),
                            [](char c) { return !std::isspace(c); });
  return std::string(iter1, str.cend() - (iter2 - str.crbegin()));
}

bool IsSpace(std::string_view str) {
  return std::ranges::all_of(str, [](char c) { return std::isspace(c); });
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

namespace {

template <typename CharType,
          NextCodePointFunctionType<CharType> NextCodePointFunction>
Index Until(const CharType* ptr, Index size, Index position,
            const std::function<bool(CodePoint)>& predicate) {
  if (position <= 0) return position;
  while (true) {
    Index p = position;
    auto c = NextCodePointFunction(ptr, size, p, &position);
    if (predicate(c)) return p;
    if (c == k_invalid_code_point) return p;
  }
  UnreachableCode();
}

static bool IsSpace(CodePoint c) { return c == 0x20 || c == 0xA; }

template <typename CharType>
using UntilFunctionType = Index (*)(const CharType*, Index, Index,
                                    const std::function<bool(CodePoint)>&);

template <typename CharType,
          NextCodePointFunctionType<CharType> NextCodePointFunction,
          UntilFunctionType<CharType> UntilFunction>
Index Word(const CharType* ptr, Index size, Index position, bool* is_space) {
  if (position <= 0) return position;
  auto c = NextCodePointFunction(ptr, size, position, nullptr);
  if (IsSpace(c)) {  // TODO: Currently only test against 0x20(space).
    if (is_space) *is_space = true;
    return UntilFunction(ptr, size, position,
                         [](CodePoint c) { return !IsSpace(c); });
  } else {
    if (is_space) *is_space = false;
    return UntilFunction(ptr, size, position,
                         [](CodePoint c) { return IsSpace(c); });
  }
}

}  // namespace

using details::ExtractBits;

CodePoint Utf8NextCodePoint(const Utf8CodeUnit* ptr, Index size, Index current,
                            Index* next_position) {
  CodePoint result;

  if (current >= size) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = static_cast<Utf8CodeUnit>(ptr[current++]);

    auto read_next_folowing_code = [ptr, size, &current]() -> CodePoint {
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
            const CodePoint s1 = read_next_folowing_code() << (6 * 2);
            const CodePoint s2 = read_next_folowing_code() << 6;
            const CodePoint s3 = read_next_folowing_code();
            result = s0 + s1 + s2 + s3;
          } else {  // 3-length code point
            const CodePoint s0 = ExtractBits<Utf8CodeUnit, 4, CodePoint>(cu0)
                                 << (6 * 2);
            const CodePoint s1 = read_next_folowing_code() << 6;
            const CodePoint s2 = read_next_folowing_code();
            result = s0 + s1 + s2;
          }
        } else {  // 2-length code point
          const CodePoint s0 = ExtractBits<Utf8CodeUnit, 5, CodePoint>(cu0)
                               << 6;
          const CodePoint s1 = read_next_folowing_code();
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

Index Utf8BackwardUntil(const Utf8CodeUnit* ptr, Index size, Index position,
                        const std::function<bool(CodePoint)>& predicate) {
  return Until<Utf8CodeUnit, Utf8PreviousCodePoint>(ptr, size, position,
                                                    predicate);
}

Index Utf8ForwardUntil(const Utf8CodeUnit* ptr, Index size, Index position,
                       const std::function<bool(CodePoint)>& predicate) {
  return Until<Utf8CodeUnit, Utf8NextCodePoint>(ptr, size, position, predicate);
}

static bool IsSpace(CodePoint c) { return c == 0x20 || c == 0xA; }

Index Utf8PreviousWord(const Utf8CodeUnit* ptr, Index size, Index position,
                       bool* is_space) {
  return Word<Utf8CodeUnit, Utf8PreviousCodePoint, Utf8BackwardUntil>(
      ptr, size, position, is_space);
}

Index Utf8NextWord(const Utf8CodeUnit* ptr, Index size, Index position,
                   bool* is_space) {
  return Word<Utf8CodeUnit, Utf8NextCodePoint, Utf8ForwardUntil>(
      ptr, size, position, is_space);
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

Index Utf16BackwardUntil(const Utf16CodeUnit* ptr, Index size, Index position,
                         const std::function<bool(CodePoint)>& predicate) {
  return Until<Utf16CodeUnit, Utf16PreviousCodePoint>(ptr, size, position,
                                                      predicate);
}

Index Utf16ForwardUntil(const Utf16CodeUnit* ptr, Index size, Index position,
                        const std::function<bool(CodePoint)>& predicate) {
  return Until<Utf16CodeUnit, Utf16NextCodePoint>(ptr, size, position,
                                                  predicate);
}

Index Utf16PreviousWord(const Utf16CodeUnit* ptr, Index size, Index position,
                        bool* is_space) {
  return Word<Utf16CodeUnit, Utf16PreviousCodePoint, Utf16BackwardUntil>(
      ptr, size, position, is_space);
}

Index Utf16NextWord(const Utf16CodeUnit* ptr, Index size, Index position,
                    bool* is_space) {
  return Word<Utf16CodeUnit, Utf16NextCodePoint, Utf16ForwardUntil>(
      ptr, size, position, is_space);
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
  Utf8CodePointIterator iter(str.data(),str.size());
  std::wstring result;
  for (auto c : iter) {
    Utf16EncodeCodePointAppend(c, [&result](char16_t c) { result += c; });
  }
  return result;
}

std::string ToUtf8String(std::wstring_view str) {
  Utf16CodePointIterator iter(reinterpret_cast<const char16_t*>( str.data()),str.size());
  std::string result;
  for (auto c : iter) {
    Utf8EncodeCodePointAppend(c, [&result](char c) { result += c; });
  }
  return result;
}

#endif

}  // namespace cru::string
