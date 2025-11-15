#include "cru/base/platform/win/Stream.h"

#include "BrigdeComStream.h"
#include "cru/base/StringUtil.h"
#include "cru/base/io/Base.h"
#include "cru/base/io/MemoryStream.h"
#include "cru/base/platform/win/ComAutoInit.h"

#include <Windows.h>
#include <coml2api.h>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <shlwapi.h>
#include <winnt.h>
#include <filesystem>
#include <format>

namespace cru::platform::win {
using namespace cru::io;

namespace {
HANDLE OpenHandle(std::string_view path, OpenFileFlag flags) {
  DWORD access = 0;
  DWORD creation_disposition = 0;
  if (flags & OpenFileFlags::Read) {
    access |= GENERIC_READ;
  } else {
    access |= GENERIC_WRITE;
  }

  if (std::filesystem::exists(path)) {
    creation_disposition =
        flags & io::OpenFileFlags::Truncate ? TRUNCATE_EXISTING : OPEN_EXISTING;
  } else {
    if (!flags.Has(OpenFileFlags::Create)) {
      throw FileNotExistException(std::format("File {} does not exist.", path));
    } else {
      creation_disposition = CREATE_NEW;
    }
  }

  IStream* stream;

  auto handle =
      ::CreateFileW(cru::string::ToUtf16(path).c_str(), access, 0, nullptr,
                    creation_disposition, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (handle == INVALID_HANDLE_VALUE) {
    throw Win32Error("Failed to call CreateFileW.");
  }

  if (flags.Has(OpenFileFlags::Append)) {
    LARGE_INTEGER offset;
    offset.QuadPart = 0;
    CheckWinReturn(::SetFilePointerEx(handle, offset, nullptr, FILE_END));
  }

  return handle;
}
}  // namespace

Win32HandleStream::Win32HandleStream(std::string_view path, OpenFileFlag flags)
    : Win32HandleStream(OpenHandle(path, flags), true, true,
                        flags.Has(OpenFileFlags::Read),
                        flags.Has(OpenFileFlags::Write)) {}

Win32HandleStream::Win32HandleStream(HANDLE handle, bool auto_close,
                                     bool can_seek, bool can_read,
                                     bool can_write)
    : Win32HandleStream(Win32Handle(handle, auto_close), can_seek, can_read,
                        can_write) {}

Win32HandleStream::Win32HandleStream(Win32Handle&& handle, bool can_seek,
                                     bool can_read, bool can_write)
    : Stream(SupportedOperations(can_seek, can_read, can_write)),
      handle_(std::move(handle)) {}

Win32HandleStream::~Win32HandleStream() { DoClose(); }

Index Win32HandleStream::DoSeek(Index offset, SeekOrigin origin) {
  DWORD method = 0;

  if (origin == SeekOrigin::Current) {
    method = FILE_CURRENT;
  } else if (origin == SeekOrigin::End) {
    method = FILE_END;
  } else {
    method = FILE_BEGIN;
  }

  LARGE_INTEGER n_offset, new_pos;
  n_offset.QuadPart = offset;

  CheckWinReturn(::SetFilePointerEx(handle_.Get(), n_offset, &new_pos, method));

  return new_pos.QuadPart;
}

Index Win32HandleStream::DoRead(std::byte* buffer, Index offset, Index size) {
  DWORD real_read;
  auto r = ::ReadFile(handle_.Get(), static_cast<LPVOID>(buffer + offset), size,
                      &real_read, nullptr);
  if (r == FALSE) {
    auto e = ::GetLastError();
    if (e != ERROR_BROKEN_PIPE || e != ERROR_BROKEN_PIPE) {
      throw Win32Error(e, "Failed to call ReadFile.");
    }
  }
  return real_read;
}

Index Win32HandleStream::DoWrite(const std::byte* buffer, Index offset,
                                 Index size) {
  DWORD real_write;
  CheckWinReturn(::WriteFile(handle_.Get(),
                             static_cast<LPCVOID>(buffer + offset), size,
                             &real_write, nullptr));
  return real_write;
}

void Win32HandleStream::DoClose() {
  CRU_STREAM_BEGIN_CLOSE

  handle_ = {};
}

IStream* ToComStream(io::Stream* stream) {
  static ComAutoInit com_auto_init;

  if (auto memory_stream = dynamic_cast<io::MemoryStream*>(stream)) {
    return SHCreateMemStream(
        reinterpret_cast<const BYTE*>(memory_stream->GetBuffer()),
        memory_stream->GetSize());
  } else if (auto file_stream = dynamic_cast<ComStream*>(stream)) {
    return file_stream->GetComStream();
  } else {
    return new BridgeComStream(stream);
  }
}

namespace {
IStream* OpenComStream(std::string_view path, OpenFileFlag flags) {
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
      throw Exception("Stream must be readable or writable.");
    }
  }

  if (flags & io::OpenFileFlags::Truncate) {
    grfMode |= STGM_CREATE;
  }

  IStream* stream;

  CheckHResult(SHCreateStreamOnFileEx(
      cru::string::ToUtf16(path).c_str(), grfMode, FILE_ATTRIBUTE_NORMAL,
      flags & io::OpenFileFlags::Create ? TRUE : FALSE, NULL, &stream));

  return stream;
}
}  // namespace

ComStream::ComStream(std::string_view path, OpenFileFlag flags)
    : ComStream(OpenComStream(path, flags), true, true,
                flags.Has(OpenFileFlags::Read),
                flags.Has(OpenFileFlags::Write)) {}

ComStream::ComStream(IStream* com_stream, bool auto_release, bool can_seek,
                     bool can_read, bool can_write)
    : Stream(SupportedOperations(can_seek, can_read, can_write)),
      stream_(com_stream),
      auto_release_(auto_release) {}

ComStream::~ComStream() { DoClose(); }

Index ComStream::DoSeek(Index offset, SeekOrigin origin) {
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

  CheckHResult(stream_->Seek(n_offset, dwOrigin, &n_new_offset));

  return n_new_offset.QuadPart;
}

Index ComStream::DoRead(std::byte* buffer, Index offset, Index size) {
  ULONG n_read;
  CheckHResult(stream_->Read(buffer + offset, size, &n_read));
  return n_read;
}

Index ComStream::DoWrite(const std::byte* buffer, Index offset, Index size) {
  if (size < 0) {
    throw Exception("Size must be greater than 0.");
  }

  ULONG n_written;
  CheckHResult(stream_->Write(buffer + offset, size, &n_written));

  return n_written;
}

void ComStream::DoClose() {
  CRU_STREAM_BEGIN_CLOSE

  if (stream_ && auto_release_) {
    stream_->Release();
    stream_ = nullptr;
  }
}
}  // namespace cru::platform::win
