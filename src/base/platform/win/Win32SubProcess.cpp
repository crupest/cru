#include "cru/base/platform/win/Win32SubProcess.h"
#include "cru/base/StringUtil.h"
#include "cru/base/SubProcess.h"
#include "cru/base/io/AutoReadStream.h"

#include <processthreadsapi.h>
#include <synchapi.h>

#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace cru::platform::win {
using cru::string::ToUtf16WString;

Win32SubProcessImpl::Win32SubProcessImpl() : exit_code_(0) {}

Win32SubProcessImpl::~Win32SubProcessImpl() {}

std::map<std::string, std::string> GetEnvironmentVariables() {
  std::map<std::string, std::string> envs;

  auto env_block = GetEnvironmentStringsW();
  if (env_block == nullptr) return envs;

  auto p = env_block;
  while (*p) {
    std::wstring_view env(p);
    auto pos = env.find(L'=');
    if (pos != std::wstring_view::npos) {
      auto key = cru::string::ToUtf8String(env.substr(0, pos));
      auto value = cru::string::ToUtf8String(env.substr(pos + 1));
      envs.emplace(std::move(key), std::move(value));
    }
    p += env.size() + 1;
  }

  FreeEnvironmentStringsW(env_block);
  return envs;
}

void Win32SubProcessImpl::PlatformCreateProcess(
    const SubProcessStartInfo& start_info) {
  auto check_error = [](int error, std::string message) {
    if (error == 0) return;
    std::unique_ptr<ErrnoException> inner(new ErrnoException(error));
    throw SubProcessFailedToStartException(std::move(message),
                                           std::move(inner));
  };

  auto app = ToUtf16WString(start_info.program);
  // TODO: Space and quoting problem.
  auto command_line =
      app + L" " + ToUtf16WString(cru::string::Join(" ", start_info.arguments));

  auto environments = start_info.environments;

  if (start_info.inherit_parent_environments) {
    auto parent_envs = GetEnvironmentVariables();
    for (const auto& [key, value] : parent_envs) {
      if (environments.find(key) == environments.end()) {
        environments.emplace(key, value);
      }
    }
  }

  std::wstring env_block;
  for (const auto& [key, value] : environments) {
    env_block += ToUtf16WString(key) + L"=" + ToUtf16WString(value);
    // Don't use env_block += L"\0" here, because L"\0" will be treated as L"".
    env_block.push_back(L'\0');
  }
  // CreateProcessW expects a double-NUL-terminated environment block.
  env_block.push_back(L'\0');

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
                                  static_cast<LPVOID>(env_block.data()), nullptr,
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
