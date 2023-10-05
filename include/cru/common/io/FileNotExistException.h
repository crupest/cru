#pragma once

#include "../Base.h"
#include "../Exception.h"

namespace cru::io {
  class CRU_BASE_API FileNotExistException : public Exception {
    public:
      FileNotExistException(String path);

      CRU_DEFAULT_COPY(FileNotExistException)
      CRU_DEFAULT_MOVE(FileNotExistException)

      ~FileNotExistException() override = default;

    private:
      String path_;
  };
}
