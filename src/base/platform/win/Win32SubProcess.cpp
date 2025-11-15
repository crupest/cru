#include "cru/base/platform/win/Win32SubProcess.h"
#include <processthreadsapi.h>
#include <synchapi.h>
#include "cru/base/StringUtil.h"
#include "cru/base/SubProcess.h"

#include <memory>
#include <string_view>

namespace cru::platform::win {
using cru::string::ToUtf16;

Win32SubProcessImpl::Win32SubProcessImpl() : exit_code_(0) {}

Win32SubProcessImpl::~Win32SubProcessImpl() {}

void Win32SubProcessImpl::PlatformCreateProcess(
    const SubProcessStartInfo& start_info) {
  auto check_error = [](int error, std::string message) {
    if (error == 0) return;
    std::unique_ptr<ErrnoException> inner(new ErrnoException(error));
    throw SubProcessFailedToStartException(std::move(message),
                                           std::move(inner));
  };

  auto app = ToUtf16(start_info.program);
  // TODO: Space and quoting problem.
  auto command_line =
      app + L" " + ToUtf16(cru::string::Join(" ", start_info.arguments));

  std::wstring env_str;
  for (const auto& [key, value] : start_info.environments) {
    env_str += ToUtf16(key) + L"=" + ToUtf16(value) + L"\0";
  }
  env_str += L"\0";

  STARTUPINFO startup_info;
  ZeroMemory(&startup_info, sizeof(startup_info));
  startup_info.cb = sizeof(startup_info);

  auto my_stdin = OpenUniDirectionalPipe(Win32PipeFlags::ReadInheritable);
  auto my_stdout = OpenUniDirectionalPipe(Win32PipeFlags::WriteInheritable);
  auto my_stderr = OpenUniDirectionalPipe(Win32PipeFlags::WriteInheritable);
  startup_info.dwFlags = STARTF_USESTDHANDLES;
  startup_info.hStdInput = my_stdin.read.Get();
  startup_info.hStdOutput = my_stdout.write.Get();
  startup_info.hStdError = my_stderr.write.Get();

  CheckWinReturn(::CreateProcessW(app.c_str(), command_line.data(), nullptr,
                                  nullptr, TRUE, CREATE_UNICODE_ENVIRONMENT,
                                  static_cast<LPVOID>(env_str.data()), nullptr,
                                  &startup_info, &process_));

  stdin_stream_ = std::make_unique<Win32HandleStream>(std::move(my_stdin.write),
                                                      false, false, true);
  stdout_stream_ = std::make_unique<Win32HandleStream>(
      std::move(my_stdout.read), false, true, false);
  stderr_stream_ = std::make_unique<Win32HandleStream>(
      std::move(my_stderr.read), false, true, false);

  stdout_buffer_stream_ =
      std::make_unique<io::AutoReadStream>(stdout_stream_.get(), true, false);
  stderr_buffer_stream_ =
      std::make_unique<io::AutoReadStream>(stderr_stream_.get(), true, false);
}

SubProcessExitResult Win32SubProcessImpl::PlatformWaitForProcess() {
  ::WaitForSingleObject(process_.hProcess, INFINITE);

  DWORD exit_code;
  CheckWinReturn(::GetExitCodeProcess(process_.hProcess, &exit_code));

  return SubProcessExitResult::Normal(exit_code);
}

void Win32SubProcessImpl::PlatformKillProcess() {
  CheckWinReturn(TerminateProcess(process_.hProcess, -1));
}

io::Stream* Win32SubProcessImpl::GetStdinStream() {
  return stdin_stream_.get();
}

io::Stream* Win32SubProcessImpl::GetStdoutStream() {
  return stdout_buffer_stream_.get();
}

io::Stream* Win32SubProcessImpl::GetStderrStream() {
  return stderr_buffer_stream_.get();
}
}  // namespace cru::platform::win
