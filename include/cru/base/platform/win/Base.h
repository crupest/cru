#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "../../Base.h"
#include "../../Bitmask.h"

#include <optional>
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

template <typename H, void (*CloseFunc)(H handle) noexcept>
class CRU_BASE_API TWin32Handle {
 public:
  TWin32Handle() : handle_(std::nullopt), auto_close_(false) {}

  TWin32Handle(H handle, bool auto_close)
      : handle_(handle), auto_close_(auto_close) {}

  CRU_DELETE_COPY(TWin32Handle)

  TWin32Handle(TWin32Handle&& other) noexcept
      : handle_(other.handle_), auto_close_(other.auto_close_) {
    other.handle_ = std::nullopt;
    other.auto_close_ = false;
  }

  TWin32Handle& operator=(TWin32Handle&& other) noexcept {
    if (this != &other) {
      DoClose();
      handle_ = other.handle_;
      auto_close_ = other.auto_close_;
      other.handle_ = std::nullopt;
      other.auto_close_ = false;
    }
    return *this;
  }

  ~TWin32Handle() { DoClose(); }

 public:
  bool IsValid() const { return handle_.has_value(); }

  void CheckValid(
      std::optional<std::string_view> additional_message = std::nullopt) const {
    if (!IsValid()) {
      std::string message("The win32 handle is invalid.");
      if (additional_message) {
        message += " ";
        message += *additional_message;
      }
      throw Exception(std::move(message));
    }
  }

  H Get() const {
    CheckValid();
    return *handle_;
  }

  H Release() {
    CheckValid();
    auto handle = *handle_;
    handle_ = std::nullopt;
    auto_close_ = false;
    return handle;
  }

 private:
  void DoClose() {
    if (auto_close_ && handle_) {
      CloseFunc(*handle_);
    }
  }

 private:
  std::optional<H> handle_;
  bool auto_close_;
};

namespace details {
inline void MyCloseHandle(HANDLE handle) noexcept { ::CloseHandle(handle); }
}  // namespace details

using Win32Handle = TWin32Handle<HANDLE, details::MyCloseHandle>;

struct UniDirectionalWin32PipeResult {
  Win32Handle read;
  Win32Handle write;
};

namespace details {
struct Win32PipeFlagTag;
}
using Win32PipeFlag = Bitmask<details::Win32PipeFlagTag>;
struct Win32PipeFlags {
  constexpr static auto ReadInheritable = Win32PipeFlag::FromOffset(1);
  constexpr static auto WriteInheritable = Win32PipeFlag::FromOffset(2);
};

CRU_BASE_API UniDirectionalWin32PipeResult
OpenUniDirectionalPipe(Win32PipeFlag flag = {});
}  // namespace cru::platform::win
