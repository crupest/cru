#include "cru/base/StringToNumberConverter.h"
#include "cru/base/Exception.h"

namespace cru {
bool StringToIntegerConverter::CheckParams() const {
  return base == 0 || base >= 2 & base <= 36;
}

static bool IsSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

StringToIntegerResult StringToIntegerConverter::Parse(
    const char* const str, const Index size,
    Index* processed_characters_count) const {
  if (str == nullptr) throw std::invalid_argument("Invalid str.");
  if (size < 0) throw std::invalid_argument("Invalid size.");
  if (!CheckParams()) throw std::invalid_argument("Invalid parsing flags.");

  const bool throw_on_error = flags.Has(StringToNumberFlags::kThrowOnError);

  auto const end = str + size;

  auto current = str;

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

  int real_base = base;

  if (real_base == 0) {
    if (*current == '0') {
      ++current;
      if (current == end) {
        if (processed_characters_count) {
          *processed_characters_count = current - str;
        }
        return {negate, 0};
      } else if (*current == 'x' || *current == 'X') {
        ++current;
        real_base = 16;
      } else if (*current == 'b' || *current == 'B') {
        ++current;
        real_base = 2;
      } else {
        real_base = 8;
      }
    } else {
      real_base = 10;
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
      flags.Has(StringToNumberFlags::kAllowLeadingZeroForInteger);

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
      flags.Has(StringToNumberFlags::kAllowTrailingJunk);
  const bool allow_trailing_spaces =
      flags.Has(StringToNumberFlags::kAllowTrailingSpaces);

  unsigned long long result = 0;

  while (current != end) {
    const char c = *current;
    if (c >= '0' && c <= (real_base > 10 ? '9' : real_base + '0' - 1)) {
      result = result * real_base + c - '0';
      current++;
    } else if (real_base > 10 && c >= 'a' && c <= (real_base + 'a' - 10 - 1)) {
      result = result * real_base + c - 'a' + 10;
      current++;
    } else if (real_base > 10 && c >= 'A' && c <= (real_base + 'A' - 10 - 1)) {
      result = result * real_base + c - 'A' + 10;
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

}  // namespace cru
