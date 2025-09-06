#pragma once

#include "../Base.h"

#include "../Exception.h"
#include "../String.h"

#include <cstddef>

namespace cru::io {
class CRU_BASE_API StreamOperationNotSupportedException : public Exception {
 public:
  explicit StreamOperationNotSupportedException(StringView operation);
  explicit StreamOperationNotSupportedException(std::string operation);

 public:
  [[deprecated("Use GetOperationUtf8 instead.")]]
  String GetOperation() const {
    return String::FromUtf8(operation_);
  }

  std::string GetOperationUtf8() const { return operation_; }

 public:
  static void CheckSeek(bool seekable);
  static void CheckRead(bool readable);
  static void CheckWrite(bool writable);

 private:
  std::string operation_;
};

class CRU_BASE_API StreamClosedException : public Exception {
 public:
  StreamClosedException();

  CRU_DEFAULT_DESTRUCTOR(StreamClosedException)

  static void Check(bool closed);
};

#define CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE \
  void Close() override { DoClose(); }

#define CRU_STREAM_BEGIN_CLOSE \
  if (GetClosed()) return;     \
  CloseGuard close_guard(this);

/**
 * All stream is thread-unsafe by default unless being documented.
 */
class CRU_BASE_API Stream : public Object {
 protected:
  struct SupportedOperations {
    std::optional<bool> can_seek;
    std::optional<bool> can_read;
    std::optional<bool> can_write;
  };

  struct CloseGuard {
    explicit CloseGuard(Stream* stream) : stream(stream) {}
    ~CloseGuard() { stream->SetClosed(true); }
    Stream* stream;
  };

 protected:
  explicit Stream(SupportedOperations supported_operations = {});
  Stream(std::optional<bool> can_seek, std::optional<bool> can_read,
         std::optional<bool> can_write);

 public:
  enum class SeekOrigin { Current, Begin, End };

  CRU_DELETE_COPY(Stream)
  CRU_DELETE_MOVE(Stream)

  ~Stream() override = default;

 public:
  bool CanSeek();
  Index Seek(Index offset, SeekOrigin origin = SeekOrigin::Current);
  Index Tell();
  void Rewind();
  Index GetSize();

  bool CanRead();
  Index Read(std::byte* buffer, Index offset, Index size);
  Index Read(std::byte* buffer, Index size);
  Index Read(char* buffer, Index offset, Index size);
  Index Read(char* buffer, Index size);

  bool CanWrite();
  Index Write(const std::byte* buffer, Index offset, Index size);
  Index Write(const std::byte* buffer, Index size);
  Index Write(const char* buffer, Index offset, Index size);
  Index Write(const char* buffer, Index size);

  void Flush();
  virtual void Close() = 0;

  virtual Buffer ReadToEnd(Index grow_size = 256);

  // Utf8 encoding.
  String ReadToEndAsUtf8String();

 protected:
  virtual bool DoCanSeek();
  virtual bool DoCanRead();
  virtual bool DoCanWrite();
  virtual Index DoSeek(Index offset, SeekOrigin origin);
  virtual Index DoTell();
  virtual void DoRewind();
  virtual Index DoGetSize();
  virtual Index DoRead(std::byte* buffer, Index offset, Index size);
  virtual Index DoWrite(const std::byte* buffer, Index offset, Index size);
  virtual void DoFlush();

  void SetSupportedOperations(SupportedOperations supported_operations) {
    supported_operations_ = std::move(supported_operations);
  }

  bool GetClosed() { return closed_; }
  void SetClosed(bool closed) { closed_ = closed; }
  void CheckClosed() { StreamClosedException::Check(closed_); }

 private:
  std::optional<SupportedOperations> supported_operations_;
  bool closed_;
};
}  // namespace cru::io
