
#pragma once

#if !defined(__unix__) && !defined(__APPLE__)
#error "This file can only be included on unix."
#endif

#include "AutoReadStream.h"
#include "Base.h"
#include "SubProcess.h"

#include <spawn.h>

namespace cru {
class UnixFileStream : public Stream {
 private:
  static constexpr auto kLogTag = u"cru::platform::unix::UnixFileStream";

 public:
  UnixFileStream(const char* path, int oflag, mode_t mode = 0660);
  UnixFileStream(int fd, bool can_seek, bool can_read, bool can_write,
                 bool auto_close);
  ~UnixFileStream() override;

 public:
  CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE

  int GetFileDescriptor() const { return file_descriptor_; }

 protected:
  Index DoSeek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;

 private:
  void DoClose();

 private:
  int file_descriptor_;  // -1 for no file descriptor
  bool auto_close_;
};

namespace details {
struct UnixPipeFlagTag;
}

using UnixPipeFlag = Bitmask<details::UnixPipeFlagTag>;
struct UnixPipeFlags {
  constexpr static auto NonBlock = UnixPipeFlag::FromOffset(1);
};

/**
 * @brief an unix pipe, commonly for communication of parent process and child
 * process.
 *
 * There are two types of pipes sorted by its usage. For stdin, parent process
 * SEND data to child process. For stdout and stderr, parent process RECEIVE
 * data from child process. Each pipe has two ends, one for read and the other
 * for write. But for send and receive, they are reversed. It is a little
 * confused to consider which end should be used by parent and which end should
 * be used by child. So this class help you make it clear. You specify SEND or
 * RECEIVE, and this class give you a parent used end and a child used end.
 *
 * This class will only close the end used by parent when it is destructed. It
 * is the user's duty to close the one used by child.
 */
class UnixPipe : public Object {
 private:
  constexpr static auto kLogTag = u"cru::platform::unix::UnixPipe";

 public:
  enum class Usage {
    Send,
    Receive,
  };

  explicit UnixPipe(Usage usage, bool auto_close, UnixPipeFlag flags = {});


  ~UnixPipe();

  /**
   * @brief aka, the one used by parent process.
   */
  int GetSelfFileDescriptor();

  /**
   * @brief aka, the one used by child process.
   */
  int GetOtherFileDescriptor();

 private:
  Usage usage_;
  bool auto_close_;
  UnixPipeFlag flags_;
  int read_fd_;
  int write_fd_;
};

class PosixSpawnSubProcessImpl : public Object,
                                 public virtual IPlatformSubProcessImpl {
  CRU_DEFINE_CLASS_LOG_TAG(u"PosixSpawnSubProcess")

 public:
  explicit PosixSpawnSubProcessImpl();
  ~PosixSpawnSubProcessImpl();

  void PlatformCreateProcess(const SubProcessStartInfo& start_info) override;
  SubProcessExitResult PlatformWaitForProcess() override;
  void PlatformKillProcess() override;

  Stream* GetStdinStream() override;
  Stream* GetStdoutStream() override;
  Stream* GetStderrStream() override;

 private:
  pid_t pid_;
  int exit_code_;

  UnixPipe stdin_pipe_;
  UnixPipe stdout_pipe_;
  UnixPipe stderr_pipe_;

  std::unique_ptr<UnixFileStream> stdin_stream_;
  std::unique_ptr<UnixFileStream> stdout_stream_;
  std::unique_ptr<UnixFileStream> stderr_stream_;

  std::unique_ptr<AutoReadStream> stdout_buffer_stream_;
  std::unique_ptr<AutoReadStream> stderr_buffer_stream_;
};
}  // namespace cru
