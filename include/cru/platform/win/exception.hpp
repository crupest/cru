#pragma once
#include "win_pre_config.hpp"

#include <stdexcept>
#include <string_view>

namespace cru::platform::win {
class HResultError : public std::runtime_error {
 public:
  explicit HResultError(HRESULT h_result);
  explicit HResultError(HRESULT h_result,
                        const std::string_view& additional_message);
  HResultError(const HResultError& other) = default;
  HResultError(HResultError&& other) = default;
  HResultError& operator=(const HResultError& other) = default;
  HResultError& operator=(HResultError&& other) = default;
  ~HResultError() override = default;

  HRESULT GetHResult() const { return h_result_; }

 private:
  HRESULT h_result_;
};

inline void ThrowIfFailed(const HRESULT h_result) {
  if (FAILED(h_result)) throw HResultError(h_result);
}

inline void ThrowIfFailed(const HRESULT h_result,
                          const std::string_view& message) {
  if (FAILED(h_result)) throw HResultError(h_result, message);
}

class Win32Error : public std::runtime_error {
 public:
  explicit Win32Error(DWORD error_code);
  Win32Error(DWORD error_code, const std::string_view& additional_message);
  Win32Error(const Win32Error& other) = default;
  Win32Error(Win32Error&& other) = default;
  Win32Error& operator=(const Win32Error& other) = default;
  Win32Error& operator=(Win32Error&& other) = default;
  ~Win32Error() override = default;

  HRESULT GetErrorCode() const { return error_code_; }

 private:
  DWORD error_code_;
};
}  // namespace cru::platform::win
