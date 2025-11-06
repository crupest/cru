#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "../../Base.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef CreateWindow
#undef DrawText
#undef CreateFont
#undef CreateEvent

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
  explicit Win32Error(std::string_view message);
  Win32Error(DWORD error_code, std::string_view message);

  DWORD GetErrorCode() const { return error_code_; }

 private:
  DWORD error_code_;
};
}  // namespace cru::platform::win
