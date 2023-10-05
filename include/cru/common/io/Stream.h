#pragma once

#include "../Base.h"

#include "../Exception.h"
#include "../String.h"

#include <cstddef>
#include <vector>

namespace cru::io {
class CRU_BASE_API StreamOperationNotSupportedException : public Exception {
 public:
  explicit StreamOperationNotSupportedException(String operation);

  CRU_DEFAULT_COPY(StreamOperationNotSupportedException)
  CRU_DEFAULT_MOVE(StreamOperationNotSupportedException)

  CRU_DEFAULT_DESTRUCTOR(StreamOperationNotSupportedException)

 public:
  String GetOperation() const { return operation_; }

 public:
  static void CheckSeek(bool seekable);
  static void CheckRead(bool readable);
  static void CheckWrite(bool writable);

 private:
  String operation_;
};

class CRU_BASE_API StreamAlreadyClosedException : public Exception {
 public:
  using Exception::Exception;

  CRU_DEFAULT_COPY(StreamAlreadyClosedException)
  CRU_DEFAULT_MOVE(StreamAlreadyClosedException)

  CRU_DEFAULT_DESTRUCTOR(StreamAlreadyClosedException)

  static void Check(bool closed);
};

class CRU_BASE_API Stream : public Object {
 public:
  enum class SeekOrigin { Current, Begin, End };

  Stream() = default;

  CRU_DELETE_COPY(Stream)
  CRU_DELETE_MOVE(Stream)

  ~Stream() override = default;

 public:
  virtual bool CanSeek() = 0;
  virtual Index Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) = 0;
  virtual Index Tell();
  virtual void Rewind();
  virtual Index GetSize();

  virtual bool CanRead() = 0;
  virtual Index Read(std::byte* buffer, Index offset, Index size) = 0;
  virtual Index Read(std::byte* buffer, Index size);

  virtual bool CanWrite() = 0;
  virtual Index Write(const std::byte* buffer, Index offset, Index size) = 0;
  virtual Index Write(const std::byte* buffer, Index size);
  Index Write(const char* buffer, Index offset, Index size);
  Index Write(const char* buffer, Index size);

  virtual std::vector<std::byte> ReadAll();

  // Utf8 encoding.
  String ReadAllAsString();

  virtual void Flush();

  virtual void Close();
};
}  // namespace cru::io
