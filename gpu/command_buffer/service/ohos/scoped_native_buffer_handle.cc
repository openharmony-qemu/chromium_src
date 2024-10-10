// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/ohos/scoped_native_buffer_handle.h"

#include "base/logging.h"
#include "base/posix/unix_domain_socket.h"

namespace gpu {

ScopedNativeBufferHandle::ScopedNativeBufferHandle() = default;

ScopedNativeBufferHandle::ScopedNativeBufferHandle(
    ScopedNativeBufferHandle&& other) {
  *this = std::move(other);
}

ScopedNativeBufferHandle::~ScopedNativeBufferHandle() {
  LOG(DEBUG) << "NATIVE_BUFFER ~(ScopedNativeBufferHandle)" << this << " buffer " << buffer_;
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
  OHOS::NWeb::OhosAdapterHelper::GetInstance().GetOhosNativeBufferAdapter().AcquireBuffer(buffer);
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
    LOG(DEBUG) << "NATIVE_BUFFER ScopedNativeBufferHandle reset buffer " << buffer_;
    OHOS::NWeb::OhosAdapterHelper::GetInstance().GetOhosNativeBufferAdapter().Release(buffer_);
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
  OHOS::NWeb::OhosAdapterHelper::GetInstance().GetOhosNativeBufferAdapter().AcquireBuffer(buffer_);
  return ScopedNativeBufferHandle(buffer_);
}

ScopedNativeBufferHandle::ScopedNativeBufferHandle(OHOSNativeBuffer buffer)
    : buffer_(buffer) {
    LOG(ERROR) << "NATIVE_BUFFER explicit ScopedNativeBufferHandle" << __PRETTY_FUNCTION__ << "::" << __LINE__ <<"\n";              
    CHECK(buffer);
}

}  // namespace gpu
