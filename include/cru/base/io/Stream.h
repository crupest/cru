#pragma once

#include "../Base.h"
#include "../Guard.h"  // IWYU pragma: keep

#include <atomic>
#include <cstddef>

namespace cru::io {
class Stream;

class CRU_BASE_API StreamException : public Exception {
 public:
  explicit StreamException(Stream* stream, std::string message = "",
                           std::shared_ptr<std::exception> inner = nullptr);

  Stream* GetStream() const { return stream_; }

 private:
  Stream* stream_;
};

class CRU_BASE_API StreamOperationNotSupportedException
    : public StreamException {
 public:
  explicit StreamOperationNotSupportedException(Stream* stream,
                                                std::string operation);

 public:
  std::string GetOperation() const { return operation_; }

 public:
  static void CheckSeek(Stream* stream, bool seekable);
  static void CheckRead(Stream* stream, bool readable);
  static void CheckWrite(Stream* stream, bool writable);

 private:
  std::string operation_;
};

class CRU_BASE_API StreamClosedException : public StreamException {
  using StreamException::StreamException;
};

class CRU_BASE_API StreamIOException : public StreamException {
  using StreamException::StreamException;
};

class CRU_BASE_API Stream : public Object {
 protected:
  struct SupportedOperations {
    std::optional<bool> can_seek;
    std::optional<bool> can_read;
    std::optional<bool> can_write;
  };

 protected:
  explicit Stream(SupportedOperations supported_operations = {});
  Stream(std::optional<bool> can_seek, std::optional<bool> can_read,
         std::optional<bool> can_write);

 public:
  enum class SeekOrigin { Current, Begin, End };
  constexpr static Index kEOF = -1;

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

  bool IsClosed();
  bool Close();

  virtual std::vector<std::byte> ReadToEnd(Index grow_size = 256);
  std::string ReadToEndAsUtf8String();

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
  virtual void DoClose();

  void SetSupportedOperations(SupportedOperations supported_operations);
  void CheckClosed();

 private:
  SupportedOperations supported_operations_;
  std::atomic_bool closed_;
};
}  // namespace cru::io
