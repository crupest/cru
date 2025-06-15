#include <cru/Exception.h>
#include <cru/Format.h>
#include <cru/Unix.h>
#include <cru/Stream.h>
#include <cru/Logger.h>
#include <cru/Guard.h>

#include <fcntl.h>
#include <signal.h>
#include <spawn.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <memory>

namespace cru {

namespace {
bool OflagCanSeek([[maybe_unused]] int oflag) {
  // Treat every file seekable.
  return true;
}

bool OflagCanRead(int oflag) {
  // There is a problem: O_RDONLY is 0. So we must test it specially.
  // If it is not write-only. Then it can read.
  return !(oflag & O_WRONLY);
}

bool OflagCanWrite(int oflag) { return oflag & O_WRONLY || oflag & O_RDWR; }

int MapSeekOrigin(Stream::SeekOrigin origin) {
  switch (origin) {
    case Stream::SeekOrigin::Begin:
      return SEEK_SET;
    case Stream::SeekOrigin::Current:
      return SEEK_CUR;
    case Stream::SeekOrigin::End:
      return SEEK_END;
    default:
      throw Exception(u"Invalid seek origin.");
  }
}
}  // namespace

UnixFileStream::UnixFileStream(const char* path, int oflag, mode_t mode) {
  file_descriptor_ = ::open(path, oflag, mode);
  if (file_descriptor_ == -1) {
    throw ErrnoException(
        Format(u"Failed to open file {} with oflag {}, mode {}.",
               String::FromUtf8(path), oflag, mode));
  }

  SetSupportedOperations(
      {OflagCanSeek(oflag), OflagCanRead(oflag), OflagCanWrite(oflag)});

  auto_close_ = true;
}

UnixFileStream::UnixFileStream(int fd, bool can_seek, bool can_read,
                               bool can_write, bool auto_close)
    : Stream(can_seek, can_read, can_write) {
  file_descriptor_ = fd;
  auto_close_ = auto_close;
}

UnixFileStream::~UnixFileStream() { DoClose(); }

Index UnixFileStream::DoSeek(Index offset, SeekOrigin origin) {
  off_t result = ::lseek(file_descriptor_, offset, MapSeekOrigin(origin));
  if (result == -1) {
    throw ErrnoException(u"Failed to seek file.");
  }
  return result;
}

Index UnixFileStream::DoRead(std::byte* buffer, Index offset, Index size) {
  auto result = ::read(file_descriptor_, buffer + offset, size);
  if (result == -1) {
    throw ErrnoException(u"Failed to read file.");
  }
  return result;
}

Index UnixFileStream::DoWrite(const std::byte* buffer, Index offset,
                              Index size) {
  auto result = ::write(file_descriptor_, buffer + offset, size);
  if (result == -1) {
    throw ErrnoException(u"Failed to write file.");
  }
  return result;
}

void UnixFileStream::DoClose() {
  CRU_STREAM_BEGIN_CLOSE
  if (auto_close_ && ::close(file_descriptor_) == -1) {
    throw ErrnoException(u"Failed to close file.");
  }
  file_descriptor_ = -1;
}

UnixPipe::UnixPipe(Usage usage, bool auto_close, UnixPipeFlag flags)
    : usage_(usage), auto_close_(auto_close), flags_(flags) {
  int fds[2];
  if (pipe(fds) != 0) {
    throw ErrnoException(u"Failed to create unix pipe.");
  }

  if (flags & UnixPipeFlags::NonBlock) {
    fcntl(fds[0], F_SETFL, O_NONBLOCK);
    fcntl(fds[1], F_SETFL, O_NONBLOCK);
  }

  read_fd_ = fds[0];
  write_fd_ = fds[1];
}

int UnixPipe::GetSelfFileDescriptor() {
  if (usage_ == Usage::Send) {
    return write_fd_;
  } else {
    return read_fd_;
  }
}

int UnixPipe::GetOtherFileDescriptor() {
  if (usage_ == Usage::Send) {
    return read_fd_;
  } else {
    return write_fd_;
  }
}

UnixPipe::~UnixPipe() {
  if (auto_close_) {
    auto self_fd = GetSelfFileDescriptor();
    if (::close(self_fd) != 0) {
      CRU_LOG_ERROR(u"Failed to close unix pipe file descriptor {}, errno {}.",
                    self_fd, errno);
    }
  }
}

PosixSpawnSubProcessImpl::PosixSpawnSubProcessImpl()
    : pid_(0),
      exit_code_(0),
      stdin_pipe_(UnixPipe::Usage::Send, false),
      stdout_pipe_(UnixPipe::Usage::Receive, false),
      stderr_pipe_(UnixPipe::Usage::Receive, false) {
  stdin_stream_ = std::make_unique<UnixFileStream>(
      stdin_pipe_.GetSelfFileDescriptor(), false, false, true, true);
  stdout_stream_ = std::make_unique<UnixFileStream>(
      stdout_pipe_.GetSelfFileDescriptor(), false, true, false, true);
  stderr_stream_ = std::make_unique<UnixFileStream>(
      stderr_pipe_.GetSelfFileDescriptor(), false, true, false, true);

  stdout_buffer_stream_ =
      std::make_unique<AutoReadStream>(stdout_stream_.get(), true, false);
  stderr_buffer_stream_ =
      std::make_unique<AutoReadStream>(stderr_stream_.get(), true, false);
}

PosixSpawnSubProcessImpl::~PosixSpawnSubProcessImpl() {}

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

void PosixSpawnSubProcessImpl::PlatformCreateProcess(
    const SubProcessStartInfo& start_info) {
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

  error = posix_spawn_file_actions_addclose(
      &file_actions, stdin_pipe_.GetOtherFileDescriptor());
  check_error(
      u"Failed to call posix_spawn_file_actions_addclose on self fd of stdin.");
  error = posix_spawn_file_actions_addclose(
      &file_actions, stdout_pipe_.GetOtherFileDescriptor());
  check_error(
      u"Failed to call posix_spawn_file_actions_addclose on self fd stdout.");
  error = posix_spawn_file_actions_addclose(
      &file_actions, stderr_pipe_.GetOtherFileDescriptor());
  check_error(
      u"Failed to call posix_spawn_file_actions_addclose on self fd stderr.");

  error = posix_spawn_file_actions_addclose(
      &file_actions, stdin_pipe_.GetSelfFileDescriptor());
  check_error(
      u"Failed to call posix_spawn_file_actions_addclose on parent fd of "
      u"stdin.");
  error = posix_spawn_file_actions_addclose(
      &file_actions, stdout_pipe_.GetSelfFileDescriptor());
  check_error(
      u"Failed to call posix_spawn_file_actions_addclose on parent fd of "
      u"stdout.");
  error = posix_spawn_file_actions_addclose(
      &file_actions, stderr_pipe_.GetSelfFileDescriptor());
  check_error(
      u"Failed to call posix_spawn_file_actions_addclose on parent fd of "
      u"stderr.");

  posix_spawnattr_t attr;
  error = posix_spawnattr_init(&attr);
  check_error(u"Failed to call posix_spawnattr_init.");
  Guard attr_guard([&attr] { posix_spawnattr_destroy(&attr); });

#ifdef CRU_PLATFORM_OSX
  error = posix_spawnattr_setflags(&attr, POSIX_SPAWN_CLOEXEC_DEFAULT);
  check_error(u"Failed to set flag POSIX_SPAWN_CLOEXEC_DEFAULT (osx).");
#endif

  auto exe = start_info.program.ToUtf8();
  std::vector<String> arguments{start_info.program};
  arguments.insert(arguments.cend(), start_info.arguments.cbegin(),
                   start_info.arguments.cend());

  auto argv = CreateCstrArray(arguments);
  Guard argv_guard([argv] { DestroyCstrArray(argv); });

  auto envp = CreateCstrArray(start_info.environments);
  Guard envp_guard([envp] { DestroyCstrArray(envp); });

  error = posix_spawnp(&pid_, exe.c_str(), &file_actions, &attr, argv, envp);
  check_error(u"Failed to call posix_spawnp.");

  error = ::close(stdin_pipe_.GetOtherFileDescriptor());
  check_error(u"Failed to close child stdin.");
  error = ::close(stdout_pipe_.GetOtherFileDescriptor());
  check_error(u"Failed to close child stdout.");
  error = ::close(stderr_pipe_.GetOtherFileDescriptor());
  check_error(u"Failed to close child stderr.");
}

SubProcessExitResult PosixSpawnSubProcessImpl::PlatformWaitForProcess() {
  int wstatus;

  while (waitpid(pid_, &wstatus, 0) == -1) {
    if (errno == EINTR) {
      CRU_LOG_INFO(u"Waitpid is interrupted by a signal. Call it again.");
      continue;
    }

    std::unique_ptr<ErrnoException> inner(new ErrnoException({}, errno));

    throw SubProcessInternalException(
        u"Failed to call waitpid on a subprocess.", std::move(inner));
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
    std::unique_ptr<ErrnoException> inner(new ErrnoException({}, errno));
    throw SubProcessInternalException(u"Failed to call kill on a subprocess.",
                                      std::move(inner));
  }
}

Stream* PosixSpawnSubProcessImpl::GetStdinStream() {
  return stdin_stream_.get();
}

Stream* PosixSpawnSubProcessImpl::GetStdoutStream() {
  return stdout_buffer_stream_.get();
}

Stream* PosixSpawnSubProcessImpl::GetStderrStream() {
  return stderr_buffer_stream_.get();
}
}  // namespace cru::platform::unix
