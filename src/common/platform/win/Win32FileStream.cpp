#include "cru/common/platform/win/Win32FileStream.hpp"

#include "cru/common/io/OpenFileFlag.hpp"
#include "cru/common/platform/win/Exception.hpp"

#include <Windows.h>

namespace cru::platform::win {
using namespace cru::io;

namespace details {
struct Win32FileStreamPrivate {
  HANDLE handle;
};
}  // namespace details

Win32FileStream::Win32FileStream(String path, OpenFileFlag flags)
    : path_(std::move(path)), flags_(flags) {
  p_ = new details::Win32FileStreamPrivate();

  DWORD dwDesiredAccess = 0;
  if (flags & OpenFileFlags::Read) {
    dwDesiredAccess |= GENERIC_READ;
  }
  if (flags & OpenFileFlags::Write) {
    dwDesiredAccess |= GENERIC_WRITE;
  }

  DWORD dwCreationDisposition = 0;
  if (flags & OpenFileFlags::Create) {
    if (flags & OpenFileFlags::ThrowOnExist) {
      dwCreationDisposition = CREATE_NEW;
    } else {
      dwCreationDisposition = OPEN_ALWAYS;
    }
  } else {
    dwCreationDisposition = OPEN_EXISTING;
  }

  p_->handle =
      ::CreateFileW(path_.WinCStr(), dwDesiredAccess, 0, nullptr,
                    dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (p_->handle == INVALID_HANDLE_VALUE) {
    throw Win32Error(u"Failed to call CreateFileW.");
  }
}

Win32FileStream::~Win32FileStream() {
  Close();
  delete p_;
}

bool Win32FileStream::CanSeek() { return true; }

Index Win32FileStream::Seek(Index offset, SeekOrigin origin) {
  CheckClosed();

  DWORD dwMoveMethod = 0;

  if (origin == SeekOrigin::Current) {
    dwMoveMethod = FILE_CURRENT;
  } else if (origin == SeekOrigin::End) {
    dwMoveMethod = FILE_END;
  } else {
    dwMoveMethod = FILE_BEGIN;
  }

  LARGE_INTEGER n_offset;
  n_offset.QuadPart = offset;
  LARGE_INTEGER n_new_offset;
  if (!::SetFilePointerEx(p_->handle, n_offset, &n_new_offset, dwMoveMethod)) {
    throw Win32Error(u"Failed to call SetFilePointerEx.");
  }

  return n_new_offset.QuadPart;
}

bool Win32FileStream::CanRead() { return true; }

Index Win32FileStream::Read(std::byte* buffer, Index offset, Index size) {
  CheckClosed();

  DWORD dwRead;
  if (::ReadFile(p_->handle, buffer + offset, size, &dwRead, nullptr) == 0) {
    throw Win32Error(u"Failed to call ReadFile.");
  }
  return dwRead;
}

bool Win32FileStream::CanWrite() { return true; }

Index Win32FileStream::Write(const std::byte* buffer, Index offset,
                             Index size) {
  CheckClosed();

  DWORD dwWritten;
  if (::WriteFile(p_->handle, buffer + offset, size, &dwWritten, nullptr) ==
      0) {
    throw Win32Error(u"Failed to call WriteFile.");
  }

  return dwWritten;
}

void Win32FileStream::Close() {
  if (closed_) return;

  ::CloseHandle(p_->handle);

  closed_ = true;
}

void Win32FileStream::CheckClosed() {
  if (closed_) throw Exception(u"Stream is closed.");
}

}  // namespace cru::platform::win
