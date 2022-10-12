#include "cru/common/platform/web/WebFileStream.h"
#include "cru/common/io/OpenFileFlag.h"
#include "cru/common/platform/web/WebException.h"

#include <cstdio>
#include <stdexcept>

namespace cru::platform::web {
WebFileStream::WebFileStream(String path, io::OpenFileFlag flags)
    : path_(std::move(path)), flags_(flags) {
  // 1. Check all invalid flag combinations.

  using io::OpenFileFlags;

  if (!(flags & OpenFileFlags::Read) && !(flags & OpenFileFlags::Write)) {
    throw std::invalid_argument(
        "At least one of Read and Write flag must be specified when opening a "
        "file.");
  }

  if (!(flags & OpenFileFlags::Write) &&
      (flags & OpenFileFlags::Truncate || flags & OpenFileFlags::Append)) {
    throw std::invalid_argument(
        "Truncate and Append flag can only be used when opening a file with "
        "Write flag.");
  }

  if (flags & OpenFileFlags::Truncate && flags & OpenFileFlags::Append) {
    throw std::invalid_argument(
        "Truncate and Append flag can not be used together.");
  }

  if (!(flags & OpenFileFlags::Create) && flags & OpenFileFlags::Exclusive) {
    // Although linux doc says that this is an undefined behavior, but let's
    // make it an error so you won't get some unexpected flags without noticing.
    throw std::invalid_argument("Exclusive flag must be set with Create flag.");
  }

  // 2. Handle with OpenFileFlags::Create and OpenFileFlags::Exclusive.
  const auto utf8_path = path.ToUtf8();

  if (!(flags & OpenFileFlags::Create)) {
    auto file = std::fopen(utf8_path.c_str(), "r");
    if (file == NULL) {
      // Note: Is there any other possible reason why fopen fails?
      throw WebException(u"File does not exist.");
    } else {
      if (!(flags & OpenFileFlags::Write)) {
        // We are not going to write the file, so we can just use this handle.
        file_ = file;
        return;
      } else {
        // We are going to write the file, so we have to close this handle and
        // create a new one.
        std::fclose(file);
      }
    }
  } else {
    if (flags & OpenFileFlags::Exclusive) {
      auto file = std::fopen(utf8_path.c_str(), "r");
      if (file != NULL) {
        std::fclose(file);
        throw WebException(u"File already exists.");
      }
    }
  }

  // 3. Now everything is ok, we can open the file with correct flags.
  // There is only one edge case for C fopen: if you open a file read-only and
  // specify Create flag, fopen won't create the file for you.

  if (flags & OpenFileFlags::Write) {
    if (flags & OpenFileFlags::Read) {
      if (flags & OpenFileFlags::Append) {
        file_ = std::fopen(utf8_path.c_str(), "a+");
      } else {
        file_ = std::fopen(utf8_path.c_str(), "w+");
      }
    } else {
      if (flags & OpenFileFlags::Append) {
        file_ = std::fopen(utf8_path.c_str(), "a");
      } else {
        file_ = std::fopen(utf8_path.c_str(), "w");
      }
    }
  } else {
    // Open file read-only.
    auto file = std::fopen(utf8_path.c_str(), "r");
    if (file == NULL) {
      file = std::fopen(utf8_path.c_str(), "w");
      if (file == NULL) {
        throw WebException(u"Failed to create file.");
      }
      std::fclose(file);
      // Open it again.
      file = std::fopen(utf8_path.c_str(), "r");
    }
    file_ = file;
  }

  if (file_ == NULL) {
    throw WebException(u"Failed to open file.");
  }
}

WebFileStream::~WebFileStream() {
  if (file_ != NULL) {
    std::fclose(file_);
  }
}
}  // namespace cru::platform::web
