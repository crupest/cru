#include "cru/common/platform/unix/PosixSpawnSubProcess.h"
#include "cru/common/Exception.h"
#include "cru/common/Format.h"
#include "cru/common/Guard.h"
#include "cru/common/String.h"
#include "cru/common/SubProcess.h"

#include <spawn.h>
#include <unistd.h>
#include <memory>
#include <unordered_map>

namespace cru::platform::unix {
PosixSpawnSubProcess::PosixSpawnSubProcess(
    const PlatformSubProcessStartInfo& start_info)
    : PlatformSubProcessBase(start_info),
      pid_(0),
      exit_code_(0),
      stdin_pipe_(UnixPipe::Usage::Send),
      stdout_pipe_(UnixPipe::Usage::Receive),
      stderr_pipe_(UnixPipe::Usage::Receive) {
  stdin_stream_ = std::make_unique<UnixFileStream>(
      stdin_pipe_.GetSelfFileDescriptor(), false, false, true, false);
  stdout_stream_ = std::make_unique<UnixFileStream>(
      stdout_pipe_.GetSelfFileDescriptor(), false, true, false, false);
  stderr_stream_ = std::make_unique<UnixFileStream>(
      stderr_pipe_.GetSelfFileDescriptor(), false, true, false, false);

  stdout_buffer_stream_ =
      std::make_unique<io::AutoReadStream>(stdout_stream_.get(), false);
  stderr_buffer_stream_ =
      std::make_unique<io::AutoReadStream>(stdout_stream_.get(), false);
}

PosixSpawnSubProcess::~PosixSpawnSubProcess() {}

io::Stream* PosixSpawnSubProcess::GetStdinStream() {
  return stdin_stream_.get();
}

io::Stream* PosixSpawnSubProcess::GetStdoutStream() {
  return stdout_buffer_stream_.get();
}

io::Stream* PosixSpawnSubProcess::GetStderrStream() {
  return stderr_buffer_stream_.get();
}

namespace {
char** CreateCstrArray(const std::vector<String>& argv) {
  std::vector<Buffer> utf8_argv;
  for (const auto& arg : argv) {
    utf8_argv.push_back(arg.ToUtf8Buffer());
  }
  char** result = new char*[utf8_argv.size() + 1];
  for (int i = 0; i < utf8_argv.size(); i++) {
    result[i] = reinterpret_cast<char*>(utf8_argv[i].Detach());
  }
  result[utf8_argv.size()] = nullptr;
  return result;
}

char** CreateCstrArray(const std::unordered_map<String, String>& envp) {
  std::vector<String> str_array;
  for (auto& [key, value] : envp) {
    str_array.push_back(cru::Format(u"{}={}", key, value));
  }
  return CreateCstrArray(str_array);
}

void DestroyCstrArray(char** argv) {
  int index = 0;
  char* arg = argv[index];
  while (arg) {
    delete[] arg;
    index++;
    arg = argv[index];
  }
  delete[] argv;
}
}  // namespace

void PosixSpawnSubProcess::PlatformCreateProcess() {
  int error;
  auto check_error = [&error](String message) {
    if (error == 0) return;
    std::unique_ptr<ErrnoException> inner(new ErrnoException({}, error));
    throw SubProcessFailedToStartException(std::move(message),
                                           std::move(inner));
  };

  posix_spawn_file_actions_t file_actions;
  error = posix_spawn_file_actions_init(&file_actions);
  check_error(u"Failed to call posix_spawn_file_actions_init.");
  Guard file_actions_guard(
      [&file_actions] { posix_spawn_file_actions_destroy(&file_actions); });

  // dup2 stdin/stdout/stderr
  error = posix_spawn_file_actions_adddup2(
      &file_actions, stdin_pipe_.GetOtherFileDescriptor(), STDIN_FILENO);
  check_error(u"Failed to call posix_spawn_file_actions_adddup2 on stdin.");
  error = posix_spawn_file_actions_adddup2(
      &file_actions, stdout_pipe_.GetOtherFileDescriptor(), STDOUT_FILENO);
  check_error(u"Failed to call posix_spawn_file_actions_adddup2 on stdout.");
  error = posix_spawn_file_actions_adddup2(
      &file_actions, stderr_pipe_.GetOtherFileDescriptor(), STDERR_FILENO);
  check_error(u"Failed to call posix_spawn_file_actions_adddup2 on stderr.");

  posix_spawnattr_t attr;
  error = posix_spawnattr_init(&attr);
  check_error(u"Failed to call posix_spawnattr_init.");
  Guard attr_guard([&attr] { posix_spawnattr_destroy(&attr); });

#ifdef CRU_PLATFORM_OSX
  error = posix_spawnattr_setflags(&attr, POSIX_SPAWN_CLOEXEC_DEFAULT);
  check_error(u"Failed to set flag POSIX_SPAWN_CLOEXEC_DEFAULT (osx).");
#endif

  auto exe = start_info_.program.ToUtf8();

  auto argv = CreateCstrArray(start_info_.arguments);
  Guard argv_guard([argv] { DestroyCstrArray(argv); });

  auto envp = CreateCstrArray(start_info_.environments);
  Guard envp_guard([envp] { DestroyCstrArray(envp); });

  error = posix_spawnp(&pid_, exe.c_str(), &file_actions, &attr, argv, envp);
  check_error(u"Failed to call posix_spawnp.");
}

PlatformSubProcessExitResult PosixSpawnSubProcess::PlatformWaitForProcess() {}

void PosixSpawnSubProcess::PlatformKillProcess() {}
}  // namespace cru::platform::unix
