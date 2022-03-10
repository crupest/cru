#include "cru/common/StringToNumberConverter.h"
#include "cru/common/Exception.h"

namespace cru {
bool StringToIntegerConverterImpl::CheckParams() const {
  return base == 0 || base >= 2 & base <= 36;
}

static bool IsSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

namespace {
template <typename T>
StringToIntegerConverterImplResult GenericParseInteger(
    const StringToIntegerConverterImpl* converter, const T* const str,
    const Index size, Index* processed_characters_count) {
  if (str == nullptr) throw std::invalid_argument("Invalid str.");
  if (size < 0) throw std::invalid_argument("Invalid size.");
  if (!converter->CheckParams())
    throw std::invalid_argument("Invalid parsing flags.");

  const bool throw_on_error =
      (converter->flags & StringToNumberFlags::kThrowOnError) != 0;

  auto const end = str + size;

  auto current = str;

  if (converter->flags & StringToNumberFlags::kAllowLeadingSpaces) {
    while (current != end && IsSpace(*current)) {
      current++;
    }
  }

  if (current == end) {
    if (processed_characters_count) {
      *processed_characters_count = 0;
    }
    if (throw_on_error) {
      throw Exception(u"Empty string (after reading leading spaces).");
    } else {
      return {false, 0};
    }
  }

  bool negate = false;

  if (*current == '-') {
    ++current;
    negate = true;
  } else if (*current == '+') {
    ++current;
  }

  if (current == end) {
    if (processed_characters_count) {
      *processed_characters_count = 0;
    }
    if (throw_on_error) {
      throw Exception(u"Empty string (after reading sign).");
    } else {
      return {false, 0};
    }
  }

  int base = converter->base;

  if (base == 0) {
    if (*current == '0') {
      ++current;
      if (current == end) {
        if (processed_characters_count) {
          *processed_characters_count = current - str;
        }
        return {negate, 0};
      } else if (*current == 'x' || *current == 'X') {
        ++current;
        base = 16;
      } else if (*current == 'b' || *current == 'B') {
        ++current;
        base = 2;
      } else {
        base = 8;
      }
    } else {
      base = 10;
    }
  }

  if (current == end) {
    if (processed_characters_count) {
      *processed_characters_count = 0;
    }
    if (throw_on_error) {
      throw Exception(u"Empty string (after reading head base indicator).");
    } else {
      return {false, 0};
    }
  }

  const bool allow_leading_zero =
      converter->flags & StringToNumberFlags::kAllowLeadingZeroForInteger;

  while (current != end && *current == '0') {
    current++;
  }

  if (current == end) {
    if (processed_characters_count) {
      *processed_characters_count = current - str;
    }
    return {negate, 0};
  }

  const bool allow_trailing_junk =
      converter->flags & StringToNumberFlags::kAllowTrailingJunk;
  const bool allow_trailing_spaces =
      converter->flags & StringToNumberFlags::kAllowTrailingSpaces;
  unsigned long long result = 0;

  while (current != end) {
    const char c = *current;
    if (c >= '0' && c <= (base > 10 ? '9' : base + '0' - 1)) {
      result = result * base + c - '0';
      current++;
    } else if (base > 10 && c >= 'a' && c <= (base + 'a' - 10 - 1)) {
      result = result * base + c - 'a' + 10;
      current++;
    } else if (base > 10 && c >= 'A' && c <= (base + 'A' - 10 - 1)) {
      result = result * base + c - 'A' + 10;
      current++;
    } else if (allow_trailing_junk) {
      break;
    } else if (allow_trailing_spaces && IsSpace(c)) {
      break;
    } else {
      if (processed_characters_count) {
        *processed_characters_count = 0;
      }
      if (throw_on_error) {
        throw Exception(String(u"Read invalid character '") + c + u"'.");
      } else {
        return {false, 0};
      }
    }
  }

  if (allow_trailing_spaces) {
    while (current != end && IsSpace(*current)) {
      current++;
    }

    if (current != end) {
      if (processed_characters_count) {
        *processed_characters_count = 0;
      }
      if (throw_on_error) {
        throw Exception(u"There is trailing junk.");
      } else {
        return {false, 0};
      }
    }
  }

  if (processed_characters_count) {
    *processed_characters_count = current - str;
  }

  return {negate, result};
}
}  // namespace

StringToIntegerConverterImplResult StringToIntegerConverterImpl::Parse(
    const char* const str, const Index size,
    Index* processed_characters_count) const {
  return GenericParseInteger(this, str, size, processed_characters_count);
}

StringToIntegerConverterImplResult StringToIntegerConverterImpl::Parse(
    const char16_t* const str, const Index size,
    Index* processed_characters_count) const {
  return GenericParseInteger(this, str, size, processed_characters_count);
}
}  // namespace cru
