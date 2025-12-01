#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "../../Base.h"
#include "../../Bitmask.h"
#include "../../Guard.h"

#include <string_view>

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
  explicit HResultError(HRESULT h_result, std::string_view message = "");

  HRESULT GetHResult() const { return h_result_; }

 private:
  HRESULT h_result_;
};

inline void CheckHResult(const HRESULT h_result,
                         std::string_view message = "") {
  if (FAILED(h_result)) throw HResultError(h_result, message);
}

class CRU_BASE_API Win32Error : public Exception {
 public:
  Win32Error();
  // ::GetLastError is automatically called to get the error code.
  // The same as Win32Error(::GetLastError(), message)
  explicit Win32Error(std::string_view message);
  Win32Error(DWORD error_code, std::string_view message);

  DWORD GetErrorCode() const { return error_code_; }

 private:
  DWORD error_code_;
};

inline void CheckWinReturn(BOOL r, std::string_view message = "") {
  if (r == FALSE) {
    throw Win32Error(message);
  }
}

namespace details {
struct HandleCloser {
  void operator()(HANDLE handle) noexcept { ::CloseHandle(handle); }
};
}  // namespace details

using Win32Handle = AutoDestruct<HANDLE, details::HandleCloser>;

struct UniDirectionalWin32PipeResult {
  Win32Handle read;
  Win32Handle write;
};

CRU_DEFINE_BITMASK(Win32PipeFlag) {
  constexpr static auto ReadInheritable = Win32PipeFlag::FromOffset(1);
  constexpr static auto WriteInheritable = Win32PipeFlag::FromOffset(2);
};

CRU_BASE_API UniDirectionalWin32PipeResult
OpenUniDirectionalPipe(Win32PipeFlag flag = {});
}  // namespace cru::platform::win
