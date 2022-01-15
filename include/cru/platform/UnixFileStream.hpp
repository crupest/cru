#pragma once

#include "cru/common/String.hpp"
#include "cru/common/io/OpenFileFlag.hpp"
#include "cru/common/io/Stream.hpp"

namespace cru::platform {
class UnixFileStream : public io::Stream {
 public:
  UnixFileStream(String path, io::OpenFileFlag flags);
  UnixFileStream(int file_descriptor, bool auto_close);

  CRU_DELETE_COPY(UnixFileStream)
  CRU_DELETE_MOVE(UnixFileStream)

  ~UnixFileStream() override;

 private:
};
}  // namespace cru::platform
