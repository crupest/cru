#pragma once

#ifndef CRU_PLATFORM_UNIX
#error "This file can only be included on unix."
#endif

#include "../../Base.h"
#include "../../Bitmask.h"

namespace cru::platform::unix {
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

  CRU_DELETE_COPY(UnixPipe)
  CRU_DELETE_MOVE(UnixPipe)

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
}  // namespace cru::platform::unix
