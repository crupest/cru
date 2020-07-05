#pragma once
#include "WinPreConfig.hpp"

#include "cru/platform/Exception.hpp"

#include <stdexcept>
#include <string_view>

namespace cru::platform::win {
class HResultError : public platform::PlatformException {
 public:
  explicit HResultError(HRESULT h_result);
  explicit HResultError(HRESULT h_result, std::string_view message);

  CRU_DEFAULT_COPY(HResultError)
  CRU_DEFAULT_MOVE(HResultError)

  ~HResultError() override = default;

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

class Win32Error : public platform::PlatformException {
 public:
  // ::GetLastError is automatically called to get the error code.
  // The same as Win32Error(::GetLastError(), message)
  explicit Win32Error(std::string_view message);
  Win32Error(DWORD error_code, std::string_view message);

  CRU_DEFAULT_COPY(Win32Error)
  CRU_DEFAULT_MOVE(Win32Error)

  ~Win32Error() override = default;

  DWORD GetErrorCode() const { return error_code_; }

 private:
  DWORD error_code_;
};
}  // namespace cru::platform::win
