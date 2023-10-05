#include "cru/common/io/FileNotExistException.h"

#include "cru/common/Exception.h"
#include "cru/common/Format.h"

#include <utility>

namespace cru::io {
  FileNotExistException::FileNotExistException(String path): Exception(), path_(std::move(path)) {
    SetMessage(Format(u"File {} does not exist.", path_));
  }
}
