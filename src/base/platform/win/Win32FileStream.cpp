#include "cru/base/platform/win/Win32FileStream.h"

#include "Win32FileStreamPrivate.h"
#include "cru/base/io/OpenFileFlag.h"
#include "cru/base/platform/win/Exception.h"

#include <Windows.h>
#include <coml2api.h>
#include <shlwapi.h>
#include <winnt.h>
#include <filesystem>

namespace cru::platform::win {
using namespace cru::io;

Win32FileStream::Win32FileStream(String path, OpenFileFlag flags)
    : path_(std::move(path)),
      flags_(flags),
      p_(new details::Win32FileStreamPrivate()) {
  DWORD grfMode = STGM_SHARE_DENY_NONE;
  if (flags & io::OpenFileFlags::Read) {
    if (flags & io::OpenFileFlags::Write) {
      grfMode |= STGM_READWRITE;
    } else {
      grfMode |= STGM_READ;
    }
  } else {
    if (flags & io::OpenFileFlags::Write) {
      grfMode |= STGM_WRITE;
    } else {
      throw Exception(u"Stream must be readable or writable.");
    }
  }

  if (flags & io::OpenFileFlags::Truncate) {
    grfMode |= STGM_CREATE;
  }

  IStream* stream;

  ThrowIfFailed(SHCreateStreamOnFileEx(
      path_.WinCStr(), grfMode, FILE_ATTRIBUTE_NORMAL,
      flags & io::OpenFileFlags::Create ? TRUE : FALSE, NULL, &stream));

  p_->stream_ = stream;
}

Win32FileStream::~Win32FileStream() {
  Close();
  delete p_;
}

bool Win32FileStream::CanSeek() { return true; }

Index Win32FileStream::Seek(Index offset, SeekOrigin origin) {
  CheckClosed();

  DWORD dwOrigin = 0;

  if (origin == SeekOrigin::Current) {
    dwOrigin = STREAM_SEEK_CUR;
  } else if (origin == SeekOrigin::End) {
    dwOrigin = STREAM_SEEK_END;
  } else {
    dwOrigin = STREAM_SEEK_SET;
  }

  LARGE_INTEGER n_offset;
  n_offset.QuadPart = offset;
  ULARGE_INTEGER n_new_offset;

  ThrowIfFailed(p_->stream_->Seek(n_offset, dwOrigin, &n_new_offset));

  return n_new_offset.QuadPart;
}

bool Win32FileStream::CanRead() { return true; }

Index Win32FileStream::Read(std::byte* buffer, Index offset, Index size) {
  if (size < 0) {
    throw Exception(u"Size must be greater than 0.");
  }

  CheckClosed();

  ULONG n_read;
  ThrowIfFailed(p_->stream_->Read(buffer + offset, size, &n_read));
  return n_read;
}

bool Win32FileStream::CanWrite() { return true; }

Index Win32FileStream::Write(const std::byte* buffer, Index offset,
                             Index size) {
  if (size < 0) {
    throw Exception(u"Size must be greater than 0.");
  }

  CheckClosed();

  ULONG n_written;
  ThrowIfFailed(p_->stream_->Write(buffer + offset, size, &n_written));

  return n_written;
}

void Win32FileStream::Close() {
  if (closed_) return;

  if (p_->stream_) {
    p_->stream_->Release();
    p_->stream_ = nullptr;
  }

  closed_ = true;
}

void Win32FileStream::CheckClosed() {
  if (closed_) throw Exception(u"Stream is closed.");
}

}  // namespace cru::platform::win
