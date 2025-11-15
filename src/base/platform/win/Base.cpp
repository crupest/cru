#include "cru/base/platform/win/Base.h"

#include <format>

namespace cru::platform::win {

HResultError::HResultError(HRESULT h_result,
                           std::string_view additional_message)
    : Exception(std::format("HRESULT is {}.", h_result)), h_result_(h_result) {
  AppendMessage(additional_message);
}

Win32Error::Win32Error(std::string_view message)
    : Win32Error(::GetLastError(), message) {}

Win32Error::Win32Error(DWORD error_code, std::string_view message)
    : Exception(std::format("Last Windows error code is {}.", error_code)),
      error_code_(error_code) {
  AppendMessage(message);
}

UniDirectionalWin32PipeResult OpenUniDirectionalPipe(Win32PipeFlag flag) {
  HANDLE read, write;
  CheckWinReturn(::CreatePipe(&read, &write, nullptr, 0));

  if (flag.Has(Win32PipeFlags::ReadInheritable)) {
    CheckWinReturn(
        ::SetHandleInformation(read, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT));
  }

  if (flag.Has(Win32PipeFlags::WriteInheritable)) {
    CheckWinReturn(::SetHandleInformation(write, HANDLE_FLAG_INHERIT,
                                          HANDLE_FLAG_INHERIT));
  }

  return {Win32Handle(read, true), Win32Handle(write, true)};
}
}  // namespace cru::platform::win
