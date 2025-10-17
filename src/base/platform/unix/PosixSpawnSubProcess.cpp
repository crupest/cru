#include "cru/base/platform/unix/PosixSpawnSubProcess.h"
#include "cru/base/Exception.h"
#include "cru/base/Guard.h"
#include "cru/base/SubProcess.h"
#include "cru/base/log/Logger.h"

#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <format>
#include <memory>
#include <string_view>
#include <unordered_map>

namespace cru::platform::unix {
PosixSpawnSubProcessImpl::PosixSpawnSubProcessImpl() : pid_(0), exit_code_(0) {}

PosixSpawnSubProcessImpl::~PosixSpawnSubProcessImpl() {}

namespace {
char** CreateCstrArray(const std::vector<std::string>& argv) {
  auto argv_len = argv.size();
  char** result = new char*[argv_len + 1];
  for (int i = 0; i < argv.size(); i++) {
    auto len = argv[i].size();
    char* str = new char[len + 1];
    result[i] = str;
    std::memcpy(str, argv[i].data(), len);
    str[len] = 0;
  }
  result[argv_len] = nullptr;
  return result;
}

char** CreateCstrArray(
    const std::unordered_map<std::string, std::string>& envp) {
  std::vector<std::string> str_array;
  for (auto& [key, value] : envp) {
    str_array.push_back(key + "=" + value);
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

void PosixSpawnSubProcessImpl::PlatformCreateProcess(
    const SubProcessStartInfo& start_info) {
  auto check_error = [](int error, std::string message) {
    if (error == 0) return;
    std::unique_ptr<ErrnoException> inner(new ErrnoException(error));
    throw SubProcessFailedToStartException(std::move(message),
                                           std::move(inner));
  };

  posix_spawn_file_actions_t file_actions;
  check_error(posix_spawn_file_actions_init(&file_actions),
              "Failed to call posix_spawn_file_actions_init.");
  Guard file_actions_guard(
      [&file_actions] { posix_spawn_file_actions_destroy(&file_actions); });

  auto add_dup2 = [&check_error, &file_actions](int from, int to,
                                                std::string_view name) {
    check_error(
        posix_spawn_file_actions_adddup2(&file_actions, from, to),
        std::format("Failed to call posix_spawn_file_actions_adddup2 on {}.",
                    name));
  };

  auto add_close = [&check_error, &file_actions](
                       UniDirectionalUnixPipeResult& pipe,
                       std::string_view name) {
    check_error(
        posix_spawn_file_actions_addclose(&file_actions, pipe.read),
        std::format("Failed to call posix_spawn_file_actions_addclose on "
                    "read end of pipe {}.",
                    name));
    check_error(
        posix_spawn_file_actions_addclose(&file_actions, pipe.write),
        std::format("Failed to call posix_spawn_file_actions_addclose on "
                    "write end of pipe {}.",
                    name));
  };

  auto my_stdin = OpenUniDirectionalPipe();
  auto my_stdout = OpenUniDirectionalPipe();
  auto my_stderr = OpenUniDirectionalPipe();

  add_dup2(my_stdin.read, STDIN_FILENO, "stdin");
  add_dup2(my_stdout.write, STDOUT_FILENO, "stdout");
  add_dup2(my_stderr.write, STDERR_FILENO, "stderr");
  add_close(my_stdin, "stdin");
  add_close(my_stdout, "stdout");
  add_close(my_stderr, "stderr");

  posix_spawnattr_t attr;
  check_error(posix_spawnattr_init(&attr),
              "Failed to call posix_spawnattr_init.");
  Guard attr_guard([&attr] { posix_spawnattr_destroy(&attr); });

#ifdef __APPLE__
  check_error(posix_spawnattr_setflags(&attr, POSIX_SPAWN_CLOEXEC_DEFAULT),
              "Failed to set flag POSIX_SPAWN_CLOEXEC_DEFAULT (osx).");
#endif

  auto exe = start_info.program;
  std::vector<std::string> arguments{start_info.program};
  arguments.insert(arguments.cend(), start_info.arguments.cbegin(),
                   start_info.arguments.cend());

  auto argv = CreateCstrArray(arguments);
  Guard argv_guard([argv] { DestroyCstrArray(argv); });

  auto envp = CreateCstrArray(start_info.environments);
  Guard envp_guard([envp] { DestroyCstrArray(envp); });

  check_error(
      posix_spawnp(&pid_, exe.c_str(), &file_actions, &attr, argv, envp),
      "Failed to call posix_spawnp.");

  stdin_stream_ = std::make_unique<UnixFileStream>(std::move(my_stdin.write),
                                                   false, false, true);
  stdout_stream_ = std::make_unique<UnixFileStream>(std::move(my_stdout.read),
                                                    false, true, false);
  stderr_stream_ = std::make_unique<UnixFileStream>(std::move(my_stderr.read),
                                                    false, true, false);

  stdout_buffer_stream_ =
      std::make_unique<io::AutoReadStream>(stdout_stream_.get(), true, false);
  stderr_buffer_stream_ =
      std::make_unique<io::AutoReadStream>(stderr_stream_.get(), true, false);
}

SubProcessExitResult PosixSpawnSubProcessImpl::PlatformWaitForProcess() {
  int wstatus;

  while (waitpid(pid_, &wstatus, 0) == -1) {
    if (errno == EINTR) {
      CRU_LOG_TAG_INFO("Waitpid is interrupted by a signal. Call it again.");
      continue;
    }

    std::unique_ptr<ErrnoException> inner(new ErrnoException(errno));

    throw SubProcessInternalException("Failed to call waitpid on a subprocess.",
                                      std::move(inner));
  }

  if (WIFEXITED(wstatus)) {
    return SubProcessExitResult::Normal(WEXITSTATUS(wstatus));
  } else if (WIFEXITED(wstatus)) {
    return SubProcessExitResult::Signal(WTERMSIG(wstatus), WCOREDUMP(wstatus));
  } else {
    return SubProcessExitResult::Unknown();
  }
}

void PosixSpawnSubProcessImpl::PlatformKillProcess() {
  int error = kill(pid_, SIGKILL);
  if (error != 0) {
    std::unique_ptr<ErrnoException> inner(new ErrnoException(errno));
    throw SubProcessInternalException("Failed to call kill on a subprocess.",
                                      std::move(inner));
  }
}

io::Stream* PosixSpawnSubProcessImpl::GetStdinStream() {
  return stdin_stream_.get();
}

io::Stream* PosixSpawnSubProcessImpl::GetStdoutStream() {
  return stdout_buffer_stream_.get();
}

io::Stream* PosixSpawnSubProcessImpl::GetStderrStream() {
  return stderr_buffer_stream_.get();
}
}  // namespace cru::platform::unix
