#include "cru/common/io/Stream.hpp"
#include "cru/common/platform/win/BrigdeComStream.hpp"

namespace cru::platform::win {
BridgeComStream::BridgeComStream(io::Stream *stream)
    : stream_(stream), ref_count_(1) {}

BridgeComStream::~BridgeComStream() {}

ULONG BridgeComStream::AddRef() { return ++ref_count_; }

ULONG BridgeComStream::Release() {
  --ref_count_;

  if (ref_count_ == 0) {
    delete this;
    return 0;
  }

  return ref_count_;
}

HRESULT BridgeComStream::QueryInterface(const IID &riid, void **ppvObject) {
  if (riid == IID_IStream) {
    *ppvObject = static_cast<IStream *>(this);
    AddRef();
    return S_OK;
  } else if (riid == IID_ISequentialStream) {
    *ppvObject = static_cast<ISequentialStream *>(this);
    AddRef();
    return S_OK;
  } else if (riid == IID_IUnknown) {
    *ppvObject = static_cast<IUnknown *>(this);
    AddRef();
    return S_OK;
  } else {
    return E_NOINTERFACE;
  }
}

HRESULT BridgeComStream::Read(void *pv, ULONG cb, ULONG *pcbRead) {
  *pcbRead = stream_->Read(static_cast<std::byte *>(pv), cb);
  return S_OK;
}

HRESULT BridgeComStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten) {
  *pcbWritten = stream_->Write(static_cast<const std::byte *>(pv), cb);
  return S_OK;
}

HRESULT BridgeComStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
                              ULARGE_INTEGER *plibNewPosition) {
  io::Stream::SeekOrigin so;

  switch (dwOrigin) {
    case STREAM_SEEK_SET:
      so = io::Stream::SeekOrigin::Begin;
      break;
    case STREAM_SEEK_CUR:
      so = io::Stream::SeekOrigin::Current;
      break;
    case STREAM_SEEK_END:
      so = io::Stream::SeekOrigin::End;
      break;
    default:
      return STG_E_INVALIDFUNCTION;
  };

  plibNewPosition->QuadPart = stream_->Seek(dlibMove.QuadPart, so);
  return S_OK;
}

HRESULT BridgeComStream::SetSize(ULARGE_INTEGER libNewSize) {
  return E_NOTIMPL;
}

HRESULT BridgeComStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb,
                                ULARGE_INTEGER *pcbRead,
                                ULARGE_INTEGER *pcbWritten) {
  return E_NOTIMPL;
}

HRESULT BridgeComStream::Commit(DWORD grfCommitFlags) { return S_OK; }

HRESULT BridgeComStream::Revert() { return S_OK; }

HRESULT BridgeComStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                                    DWORD dwLockType) {
  return S_OK;
}

HRESULT BridgeComStream::UnlockRegion(ULARGE_INTEGER libOffset,
                                      ULARGE_INTEGER cb, DWORD dwLockType) {
  return S_OK;
}

HRESULT BridgeComStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag) {
  return E_NOTIMPL;
}

HRESULT BridgeComStream::Clone(IStream **ppstm) {
  *ppstm = new BridgeComStream(stream_);
  return S_OK;
}

}  // namespace cru::platform::win
