#pragma once
#include "Base.h"
#include "Bitmask.h"

#include <cstddef>
#include <ostream>

namespace cru {
namespace details {
struct StringToNumberFlagTag {};
}  // namespace details

using StringToNumberFlag = Bitmask<details::StringToNumberFlagTag>;

struct StringToNumberFlags {
  constexpr static StringToNumberFlag kAllowLeadingSpaces =
      StringToNumberFlag::FromOffset(0);
  constexpr static StringToNumberFlag kAllowTrailingSpaces =
      StringToNumberFlag::FromOffset(1);
  constexpr static StringToNumberFlag kAllowTrailingJunk =
      StringToNumberFlag::FromOffset(2);
  constexpr static StringToNumberFlag kAllowLeadingZeroForInteger =
      StringToNumberFlag::FromOffset(3);
  constexpr static StringToNumberFlag kThrowOnError =
      StringToNumberFlag::FromOffset(4);
};

template <typename TResult>
struct IStringToNumberConverter : virtual Interface {
  virtual TResult Parse(const char* str, Index size,
                        Index* processed_characters_count) const = 0;

  template <std::size_t Size>
  TResult Parse(const char (&str)[Size],
                Index* processed_characters_count) const {
    return Parse(str, Size - 1, processed_characters_count);
  }
};

struct CRU_BASE_API StringToIntegerResult {
  StringToIntegerResult() = default;
  StringToIntegerResult(bool negate, unsigned long long value)
      : negate(negate), value(value) {}

  bool negate;
  unsigned long long value;
};

inline bool CRU_BASE_API operator==(const StringToIntegerResult& left,
                                    const StringToIntegerResult& right) {
  return left.negate == right.negate && left.value == right.value;
}

inline bool CRU_BASE_API operator!=(const StringToIntegerResult& left,
                                    const StringToIntegerResult& right) {
  return !(left == right);
}

inline std::ostream& CRU_BASE_API
operator<<(std::ostream& stream, const StringToIntegerResult& result) {
  return stream << "StringToIntegerConverterImplResult("
                << (result.negate ? "-" : "") << result.value << ")";
}

/**
 * \brief A converter that convert number into long long.
 */
struct CRU_BASE_API StringToIntegerConverter
    : IStringToNumberConverter<StringToIntegerResult> {
 public:
  explicit StringToIntegerConverter(StringToNumberFlag flags, int base = 0)
      : flags(flags), base(base) {}

  bool CheckParams() const;

  /**
   * \brief Convert string to long long.
   * \param str The string to convert.
   * \param size The size of the string.
   * \param processed_characters_count The number of characters that were
   * processed. Or nullptr to not retrieve.
   */
  StringToIntegerResult Parse(const char* str, Index size,
                              Index* processed_characters_count) const override;
  using IStringToNumberConverter<StringToIntegerResult>::Parse;

  StringToNumberFlag flags;
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

struct CRU_BASE_API StringToFloatConverter {
  StringToFloatConverter(StringToNumberFlag flags) : flags(flags) {}

  double Parse(const char* str, Index size,
               Index* processed_characters_count) const;

  StringToNumberFlag flags;
};
}  // namespace cru
