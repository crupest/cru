#pragma once

#ifndef _WIN32
#error "This file can only be used on Windows."
#endif

#include "WinPreConfig.h"

#include "../../Exception.h"

#include <stdexcept>
#include <string_view>

namespace cru::platform::win {
class CRU_BASE_API HResultError : public Exception {
 public:
  explicit HResultError(HRESULT h_result);
  HResultError(HRESULT h_result, std::string_view message);

  HRESULT GetHResult() const { return h_result_; }

 private:
  HRESULT h_result_;
};

inline void ThrowIfFailed(const HRESULT h_result) {
  if (FAILED(h_result)) throw HResultError(h_result);
}

inline void ThrowIfFailed(const HRESULT h_result, std::string_view message) {
  if (FAILED(h_result)) throw HResultError(h_result, message);
}

class CRU_BASE_API Win32Error : public Exception {
 public:
  // ::GetLastError is automatically called to get the error code.
  // The same as Win32Error(::GetLastError(), message)
  explicit Win32Error(String message);
  Win32Error(DWORD error_code, String message);

  DWORD GetErrorCode() const { return error_code_; }

 private:
  DWORD error_code_;
};
}  // namespace cru::platform::win
