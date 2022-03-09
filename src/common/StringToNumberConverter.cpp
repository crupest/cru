#include "cru/common/StringToNumberConverter.h"
#include "cru/common/Exception.h"

namespace cru {
bool StringToIntegerConverterImpl::CheckParams() const {
  if (base == 0) {
    if (flags & StringToNumberFlags::kAllowLeadingZeroForInteger) {
      return false;
    } else {
      return true;
    }
  }

  return base >= 2 & base <= 36;
}

static bool IsSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

StringToIntegerConverterImplResult StringToIntegerConverterImpl::Parse(
    const char* const str, const Index size,
    Index* processed_characters_count) const {
  if (str == nullptr) throw std::invalid_argument("Invalid str.");
  if (size < 0) throw std::invalid_argument("Invalid size.");
  if (!CheckParams()) throw std::invalid_argument("Invalid parsing flags.");

  const bool throw_on_error = (flags & StringToNumberFlags::kThrowOnError) != 0;

  const char* const end = str + size;

  const char* current = str;

  if (flags & StringToNumberFlags::kAllowLeadingSpaces) {
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

  int base = this->base;

  if (base == 0) {
    if (*current == '0') {
      ++current;
      if (current == end) {
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
      flags & StringToNumberFlags::kAllowLeadingZeroForInteger;

  while (current != end && *current == '0') {
    current++;
  }

  if (current == end) {
    return {negate, 0};
  }

  const bool allow_trailing_junk =
      flags & StringToNumberFlags::kAllowTrailingJunk;
  const bool allow_trailing_spaces =
      flags & StringToNumberFlags::kAllowTrailingSpaces;
  unsigned long long result = 0;

  while (current != end) {
    const char c = *current;
    if (c >= '0' && c <= (base > 10 ? '9' : base + '0' - 1)) {
      result = result * base + c - '0';
    } else if (base > 10 && c >= 'a' && c <= 'z') {
      result = result * base + c - 'a' + 10;
    } else if (base > 10 && c >= 'A' && c <= 'Z') {
      result = result * base + c - 'A' + 10;
    } else if (allow_trailing_junk) {
      break;
    } else if (allow_trailing_spaces && IsSpace(c)) {
      break;
    } else {
      if (processed_characters_count) {
        *processed_characters_count = 0;
      }
      if (throw_on_error) {
        throw Exception(u"Read invalid character.");
      } else {
        return {false, 0};
      }
    }
  }

  if (allow_trailing_spaces) {
    while (current != end && IsSpace(*current)) {
      current++;
    }
  }

  if (current != end) {
    throw Exception(u"There is trailing junk.");
  }

  return {negate, result};
}
}  // namespace cru
