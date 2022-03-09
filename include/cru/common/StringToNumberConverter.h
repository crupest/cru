#pragma once
#include "Base.h"

namespace cru {
struct StringToNumberFlags {
  constexpr static unsigned kNoFlags = 0;
  constexpr static unsigned kAllowLeadingSpaces = 1 << 0;
  constexpr static unsigned kAllowTrailingSpaces = 1 << 1;
  constexpr static unsigned kAllowTrailingJunk = 1 << 2;
  constexpr static unsigned kAllowLeadingZeroForInteger = 1 << 2;
  constexpr static unsigned kThrowOnError = 1 << 3;
};

struct StringToIntegerConverterImplResult {
  bool negate;
  unsigned long long value;
};

/**
 * \brief A converter that convert number into long long.
 */
struct StringToIntegerConverterImpl {
 public:
  explicit StringToIntegerConverterImpl(unsigned flags, int base = 0)
      : flags(flags), base(base) {}

  bool CheckParams() const;

  /**
   * \brief Convert string to long long.
   * \param str The string to convert.
   * \param size The size of the string.
   * \param processed_characters_count The number of characters that were
   * processed. Or nullptr to not retrieve.
   */
  StringToIntegerConverterImplResult Parse(
      const char* str, Index size, Index* processed_characters_count) const;

  unsigned flags;
  /**
   * \brief The base of the number used for parse or 0 for auto detect.
   * \remarks Base can only be of range [2, 36] or 0. If base is 0, decimal is
   * assumed by default ,or if str is started with "0x" or "0X" hexadecimal is
   * assumed, or if str is started with a single "0" octoral is assumed, or if
   * str is started with "0b" or "0B" binary is assumed. Otherwise it is an
   * error.
   */
  int base;
};
}  // namespace cru
