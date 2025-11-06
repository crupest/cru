#pragma once
#include "cru/base/platform/win/Base.h"
#include "cru/base/io/Stream.h"

#include <objidlbase.h>

namespace cru::platform::win {
class BridgeComStream : public IStream {
 public:
  explicit BridgeComStream(io::Stream* stream);

  CRU_DELETE_COPY(BridgeComStream)
  CRU_DELETE_MOVE(BridgeComStream)

  ~BridgeComStream();

 public:
  ULONG AddRef() override;
  ULONG Release() override;
  HRESULT QueryInterface(REFIID riid, void** ppvObject) override;

  HRESULT Read(void* pv, ULONG cb, ULONG* pcbRead) override;
  HRESULT Write(const void* pv, ULONG cb, ULONG* pcbWritten) override;
  HRESULT Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
               ULARGE_INTEGER* plibNewPosition) override;
  HRESULT SetSize(ULARGE_INTEGER libNewSize) override;
  HRESULT CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead,
                 ULARGE_INTEGER* pcbWritten) override;
  HRESULT Commit(DWORD grfCommitFlags) override;
  HRESULT Revert() override;
  HRESULT LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                     DWORD dwLockType) override;
  HRESULT UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                       DWORD dwLockType) override;
  HRESULT Stat(STATSTG* pstatstg, DWORD grfStatFlag) override;
  HRESULT Clone(IStream** ppstm) override;

 private:
  io::Stream* stream_;
  ULONG ref_count_;
};
}  // namespace cru::platform::win
