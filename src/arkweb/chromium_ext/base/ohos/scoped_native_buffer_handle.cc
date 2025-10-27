// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/base/ohos/scoped_native_buffer_handle.h"

#include "base/logging.h"
#include "base/posix/unix_domain_socket.h"

namespace gpu {

ScopedNativeBufferHandle::ScopedNativeBufferHandle() = default;

ScopedNativeBufferHandle::ScopedNativeBufferHandle(
    ScopedNativeBufferHandle&& other) {
  *this = std::move(other);
}

ScopedNativeBufferHandle::~ScopedNativeBufferHandle() {
  LOG(DEBUG) << "NATIVE_BUFFER ~(ScopedNativeBufferHandle)" << this
             << " buffer " << buffer_;
  reset();
}

// static
ScopedNativeBufferHandle ScopedNativeBufferHandle::Adopt(
    OHOSNativeBuffer buffer) {
  LOG(DEBUG) << "NATIVE_BUFFER ScopedNativeBufferHandle Adopt.";
  return ScopedNativeBufferHandle(buffer);
}

// static
ScopedNativeBufferHandle ScopedNativeBufferHandle::Create(
    OHOSNativeBuffer buffer) {
  LOG(DEBUG) << "NATIVE_BUFFER Create.";
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosNativeBufferAdapter()
      .AcquireBuffer(buffer);
  return ScopedNativeBufferHandle(buffer);
}

ScopedNativeBufferHandle& ScopedNativeBufferHandle::operator=(
    ScopedNativeBufferHandle&& other) {
  LOG(DEBUG) << "NATIVE_BUFFER = " << buffer_ << ", " << other.buffer_;
  reset();
  std::swap(buffer_, other.buffer_);
  return *this;
}

bool ScopedNativeBufferHandle::is_valid() const {
  return buffer_ != nullptr;
}

OHOSNativeBuffer ScopedNativeBufferHandle::get() const {
  return buffer_;
}

void ScopedNativeBufferHandle::reset() {
  if (buffer_) {
    LOG(DEBUG) << "NATIVE_BUFFER ScopedNativeBufferHandle reset buffer "
               << buffer_;
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetOhosNativeBufferAdapter()
        .Release(buffer_);
    buffer_ = nullptr;
  }
}

OHOSNativeBuffer ScopedNativeBufferHandle::Take() {
  LOG(DEBUG) << "NATIVE_BUFFER Take";
  OHOSNativeBuffer buffer = buffer_;
  buffer_ = nullptr;
  return buffer;
}

ScopedNativeBufferHandle ScopedNativeBufferHandle::Clone() const {
  LOG(DEBUG) << "NATIVE_BUFFER clone " << buffer_;
  DCHECK(buffer_);
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosNativeBufferAdapter()
      .AcquireBuffer(buffer_);
  return ScopedNativeBufferHandle(buffer_);
}

base::ScopedFD ScopedNativeBufferHandle::SerializeAsFileDescriptor() const {
  LOG(ERROR) << "NATIVE_BUFFER" << __PRETTY_FUNCTION__ << "::" << __LINE__
             << "\n";
  DCHECK(is_valid());

  base::ScopedFD reader;
  base::ScopedFD writer;
  if (!CreateSocketPair(&reader, &writer)) {
    LOG(ERROR) << "create socketpair failed.";
    return base::ScopedFD();
  }

  // NOTE: SendHandleToUnixSocket does NOT acquire or retain a reference to the
  // buffer object. The caller is therefore responsible for ensuring that the
  // buffer remains alive through the lifetime of this file descriptor.
  int result = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                   .GetOhosNativeBufferAdapter()
                   .SendHandleToUnixSocket(buffer_, writer.get());
  if (result < 0) {
    LOG(ERROR) << "send Handle To UnixSocket failed.";
    return base::ScopedFD();
  }

  return reader;
}

// static
ScopedNativeBufferHandle
ScopedNativeBufferHandle::DeserializeFromFileDescriptor(base::ScopedFD fd) {
  LOG(ERROR) << "NATIVE_BUFFER" << __PRETTY_FUNCTION__ << "::" << __LINE__
             << "\n";
  DCHECK(fd.is_valid());
  OHOSNativeBuffer buffer = nullptr;

  // NOTE: Upon success, RecvHandleFromUnixSocket acquires a new reference to
  // the OhosNativeBuffer.
  int result = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                   .GetOhosNativeBufferAdapter()
                   .RecvHandleFromUnixSocket(fd.get(), &buffer);
  if (result < 0) {
    LOG(ERROR) << "recv Handle from UnixSocket failed";
    return ScopedNativeBufferHandle();
  }

  return ScopedNativeBufferHandle(buffer);
}

ScopedNativeBufferHandle::ScopedNativeBufferHandle(OHOSNativeBuffer buffer)
    : buffer_(buffer) {
  CHECK(buffer);
}

}  // namespace gpu
