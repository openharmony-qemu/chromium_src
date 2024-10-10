// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_NATIVE_BUFFER_FENCE_SYNC_H
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_NATIVE_BUFFER_FENCE_SYNC_H

#include "gpu/command_buffer/service/ohos/scoped_native_buffer_handle.h"
#include "base/base_export.h"
#include "base/files/scoped_file.h"

namespace gpu {

class BASE_EXPORT ScopedNativeBufferFenceSync {
 public:
  ScopedNativeBufferFenceSync();
  ScopedNativeBufferFenceSync(
      ScopedNativeBufferHandle handle,
      base::ScopedFD fence_fd);
  virtual ~ScopedNativeBufferFenceSync();

  OHOSNativeBuffer buffer() const { return handle_.get(); }
  ScopedNativeBufferHandle TakeBuffer();
  base::ScopedFD TakeFence();

  // Provides fence which is signaled when the reads for this buffer are done
  // and it can be reused. Must only be called once.
  virtual void SetReadFence(base::ScopedFD fence_fd) = 0;

 private:
  ScopedNativeBufferHandle handle_;
  base::ScopedFD fence_fd_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_NATIVE_BUFFER_FENCE_SYNC_H
